#include "client.h"
#include <QHostAddress>
#include <QDebug>

//========= Constructeur du client ==========//

Client::Client(QObject *parent)
    : QObject(parent), socket(new QTcpSocket(this))
{
    // Connexion du signal readyRead à la fonction onReadyRead pour traiter les données reçues
    connect(socket, &QTcpSocket::readyRead, this, &Client::onReadyRead);
}

//========= Connexion au serveur ==========//

void Client::connectToServer(const QString &host, quint16 port)
{
    // Tentative de connexion au serveur à l'adresse et au port spécifiés

    qDebug() << "[Client] Tentative de connexion à" << host << ":" << port;
    socket->connectToHost(QHostAddress(host), port);

    // Attente de la connexion, avec un délai d'attente de 3 secondes

    if (!socket->waitForConnected(3000)) {
        // Si la connexion échoue, on affiche un message d'erreur
        qWarning() << "Erreur de connexion au serveur:" << socket->errorString();
    } else {
        // Si la connexion réussit, on affiche un message de succès

        qDebug() << "[Client] Connecté au serveur";
    }
}

//========= Envoi d'un message au serveur ==========//

void Client::sendMessage(const QString &message)
{
    // Affichage du message envoyé

    qDebug() << "[Client] Envoi du message:" << message;

    // Vérification si le client est connecté au serveur avant d'envoyer le message

    if (socket->state() == QAbstractSocket::ConnectedState) {
        QByteArray data = message.toUtf8(); // Conversion du message en bytes (UTF-8)
        socket->write(data); // Envoi du message au serveur
    }
}

//========= Lecture des données reçues du serveur ==========//

void Client::onReadyRead()
{
    // Lecture et traitement de toutes les données reçues

    while (socket->bytesAvailable()) {
        // Conversion des données reçues en chaîne de caractères et affichage du message

        QString message = QString::fromUtf8(socket->readAll());
        qDebug() << "[Client] Message reçu:" << message;

        // Émission du signal messageReceived pour notifier les autres composants du message reçu

        emit messageReceived(message);
    }
}
