#include "serveur.h"
#include "serialport.h"
#include <QDebug>

//========= Constructeur du serveur ==========//

Serveur::Serveur(SerialPort *serialPort, QObject *parent)
    : QObject(parent), serial(serialPort)
{
    // Création du serveur TCP
    tcpServer = new QTcpServer(this);

    // Connexion du signal de nouvelle connexion à la fonction onNewConnection
    connect(tcpServer, &QTcpServer::newConnection, this, &Serveur::onNewConnection);
}

//========= Démarrage du serveur TCP ==========//

bool Serveur::startServer(quint16 port)
{
    // Démarrage du serveur pour écouter sur n'importe quelle adresse et le port spécifié
    return tcpServer->listen(QHostAddress::Any, port);
}

//========= Gestion de la nouvelle connexion client ==========//

void Serveur::onNewConnection()
{
    // Récupération du socket du client connecté
    QTcpSocket *clientSocket = tcpServer->nextPendingConnection();
    clients.append(clientSocket); // Ajout du client à la liste des clients

    // Connexion des signaux de lecture et de déconnexion du client
    connect(clientSocket, &QTcpSocket::readyRead, this, &Serveur::onReadyRead);
    connect(clientSocket, &QTcpSocket::disconnected, this, &Serveur::onClientDisconnected);

    // Affichage de l'adresse du client connecté
    qDebug() << "[Serveur] Nouveau client connecté:" << clientSocket->peerAddress().toString();
}

//========= Gestion des données reçues du client ==========//

void Serveur::onReadyRead()
{
    // Récupération du socket du client qui a envoyé les données
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) return; // Vérification si le socket est valide

    // Lecture des données envoyées par le client
    QByteArray data = clientSocket->readAll();
    QString message = QString::fromUtf8(data); // Conversion des données en message texte
    qDebug() << "[Serveur] Message reçu:" << message;

    emit messageRecu(message); // Émission du signal messageRecu

}

//========= Gestion de la déconnexion d'un client ==========//

void Serveur::onClientDisconnected()
{
    // Récupération du socket du client qui s'est déconnecté
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) return; // Vérification si le socket est valide

    // Retrait du client de la liste des clients et suppression de la mémoire du socket
    clients.removeAll(clientSocket);
    clientSocket->deleteLater();
}
