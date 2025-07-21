#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <vector>

int main() {
    // 1. Ouvrir la caméra via OpenCV
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Impossible d'ouvrir la caméra !" << std::endl;
        return 1;
    }

    // Optionnel : forcer 640x480
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    // 2. Créer un socket pour le serveur
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Erreur socket()" << std::endl;
        return 1;
    }

    // Autoriser la réutilisation immédiate du port
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 3. Lier (bind) le socket sur le port 8080
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    if (bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Erreur bind()" << std::endl;
        close(server_fd);
        return 1;
    }

    // 4. Écouter les connexions
    if (listen(server_fd, 1) < 0) {
        std::cerr << "Erreur listen()" << std::endl;
        close(server_fd);
        return 1;
    }

    std::cout << "Serveur en écoute sur http://0.0.0.0:8080" << std::endl;
    std::cout << "(Mono-client, bloquant)" << std::endl;

    // 5. Accepter un client
    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);
    if (client_fd < 0) {
        std::cerr << "Erreur accept()" << std::endl;
        close(server_fd);
        return 1;
    }

    std::cout << "Client connecté !" << std::endl;

    // 6. Envoyer l'en-tête HTTP pour un flux MJPEG
    std::string header =
        "HTTP/1.0 200 OK\r\n"
        "Server: MJPEG-Server\r\n"
        "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n";
    send(client_fd, header.c_str(), header.size(), 0);

    // 7. Boucle : capture OpenCV -> encode JPEG -> envoi MJPEG
    while (true) {
        // Capture
        cv::Mat frame;
        cap >> frame;
        if (frame.empty()) {
            std::cerr << "Frame vide, on arrête." << std::endl;
            break;
        }

        // Encodage JPEG
        std::vector<uchar> buff;
        std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, 80};
        cv::imencode(".jpg", frame, buff, params);

        // Préparer le bloc de données
        // -- boundary
        std::string boundary = "--frame\r\nContent-Type: image/jpeg\r\nContent-Length: ";
        boundary += std::to_string(buff.size());
        boundary += "\r\n\r\n";

        // Envoi HEAD
        if (send(client_fd, boundary.c_str(), boundary.size(), 0) < 0) {
            std::cerr << "Client déconnecté." << std::endl;
            break;
        }
        // Envoi data JPEG
        if (send(client_fd, reinterpret_cast<char*>(buff.data()), buff.size(), 0) < 0) {
            std::cerr << "Client déconnecté." << std::endl;
            break;
        }
        // Envoi \r\n
        std::string end = "\r\n";
        if (send(client_fd, end.c_str(), end.size(), 0) < 0) {
            std::cerr << "Client déconnecté." << std::endl;
            break;
        }

        //  ~ 25 fps
        cv::waitKey(40);
    }

    // Nettoyage
    close(client_fd);
    close(server_fd);
    cap.release();

    return 0;
}
