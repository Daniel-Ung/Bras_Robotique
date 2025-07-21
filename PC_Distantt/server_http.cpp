#include "server_http.h"
#include <iostream>
#include <vector>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
using namespace std;

//========= Constructeur du serveur HTTP ==========//

ServerHttp::ServerHttp(cv::VideoCapture* cap, QObject *parent)
    : QObject(parent), m_cap(cap), server_fd(-1), running(false) {
}

//========= Destructeur du serveur HTTP ==========//

ServerHttp::~ServerHttp() {
    running = false; // Arrêt du serveur
    if (serverThread.joinable())
        serverThread.join(); // Attente de la fin du thread du serveur

    if (server_fd >= 0)
        close(server_fd); // Fermeture de la socket du serveur
}

//========= Démarrage du serveur HTTP ==========//
bool ServerHttp::start(int port) {
    // Création d'une socket pour le serveur
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        cerr << "[ServerHttp] Erreur socket()" << endl;
        return false;
    }

    // Configuration des options de la socket

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Configuration de l'adresse du serveur

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    // Lier la socket à l'adresse spécifiée
    if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        cerr << "[ServerHttp] Erreur bind()" << endl;
        close(server_fd);
        return false;
    }

    // Mise en écoute des connexions entrantes

    if (listen(server_fd, 1) < 0) {
        cerr << "[ServerHttp] Erreur listen()" << endl;
        close(server_fd);
        return false;
    }

    cout << "[ServerHttp] En écoute sur http://192.168.0.31:" << port << endl;

    // Démarrage du thread pour gérer la connexion
    running = true;
    serverThread = std::thread(&ServerHttp::handleConnection, this);

    return true;
}

//========= Gestion de la connexion du client ==========//

void ServerHttp::handleConnection() {
    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Acceptation de la connexion client
    int client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);
    if (client_fd < 0) {
        cerr << "[ServerHttp] Erreur accept()" << endl;
        return;
    }

    cout << "[ServerHttp] Client connecté !" << endl;

    // Envoi de l'en-tête HTTP pour la réponse multipart

    string header =
        "HTTP/1.0 200 OK\r\n"
        "Server: MJPEG-QtStreamer\r\n"
        "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n";
    send(client_fd, header.c_str(), header.size(), 0);

    // Boucle d'envoi des images au client

    while (running) {
        cv::Mat frame;
        *m_cap >> frame; // Capture d'une frame

        if (frame.empty()) {
            cerr << "[ServerHttp] Frame vide." << endl;
            break; // Si la frame est vide, on arrête la boucle
        }

        vector<uchar> buffer;
        vector<int> params = {cv::IMWRITE_JPEG_QUALITY, 80}; // Paramètres de compression JPEG
        cv::imencode(".jpg", frame, buffer, params); // Encodage de la frame en JPEG

        // Construction de la partie de l'en-tête contenant l'image
        string boundary = "--frame\r\nContent-Type: image/jpeg\r\nContent-Length: ";
        boundary += to_string(buffer.size()) + "\r\n\r\n";

        // Envoi de l'image au client
        if (send(client_fd, boundary.c_str(), boundary.size(), 0) < 0) break;
        if (send(client_fd, reinterpret_cast<char*>(buffer.data()), buffer.size(), 0) < 0) break;
        if (send(client_fd, "\r\n", 2, 0) < 0) break;

        cv::waitKey(30); // Attente avant de capturer la prochaine image
    }

    cout << "[ServerHttp] Déconnexion client." << endl;
    close(client_fd); // Fermeture de la connexion avec le client
}
