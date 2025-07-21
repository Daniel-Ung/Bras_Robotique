#include "mainwindow.h"
#include "serialport.h"
#include "serveur.h"
#include "camera.h"
#include "server_http.h"
#include "client.h"

#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QStatusBar>
#include <QMessageBox>
#include <QDebug>
#include <QThread>
#include <QTcpSocket>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), mySerial(nullptr), server(nullptr), pinceOuverte(false), client(new Client(this)) {

    //========= Interface Principale ==========//
    QWidget *central = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    QGridLayout *grid = new QGridLayout();
    QHBoxLayout *wordLayout = new QHBoxLayout();

    //========= Port Série (Arduino) ==========//
    mySerial = new SerialPort("/dev/ttyACM0"); // ← adapte ce chemin si besoin

    if (!mySerial->openPort()) {
        QMessageBox::warning(this, "Erreur", "Impossible d'ouvrir le port série.");
    } else {
        qDebug() << "[MainWindow] Port série ouvert avec succès.";
    }

    //========= Serveur TCP (tablette) ==========//
    server = new Serveur(nullptr, this);
    if (server->startServer(12345)) {
        statusBar()->showMessage("Serveur TCP en écoute sur le port 12345");
    } else {
        statusBar()->showMessage("Erreur lors du lancement du serveur TCP");
    }

    //========= Caméra + MJPEG HTTP Server ==========//
    Camera *cam = new Camera();
    mainLayout->addWidget(cam->getVideoLabel());

    ServerHttp *httpServer = new ServerHttp(cam->getCapture(), this);
    if (httpServer->start(8080)) {
        qDebug() << "[MainWindow] MJPEG en ligne sur http://<IP>:8080";
    } else {
        qDebug() << "[MainWindow] Erreur serveur HTTP";
    }

    //========= Boutons ==========//

    btnUp = new QPushButton("↑");
    btnDown = new QPushButton("↓");
    btnLeft = new QPushButton("←");
    btnRight = new QPushButton("→");
    btnStop = new QPushButton("STOP");
    AVBbtnUp = new QPushButton("AVB ↑");
    AVBbtnDown = new QPushButton("AVB ↓");
    PincebtnLeft = new QPushButton("Pince ←");
    PincebtnRight = new QPushButton("Pince →");
    PinceOuvrirBtn = new QPushButton("Ouvrir Pince");
    PinceFermerBtn = new QPushButton("Fermer Pince");
    poignethaut = new QPushButton("Poignet ↑");
    poignetbas = new QPushButton("Poignet ↓");
    resetButton = new QPushButton("RESET");
    commandInput = new QLineEdit();
    quartG = new QPushButton("QuartG");
    quartD = new QPushButton("QuartD");
    attrape = new QPushButton("attrape");
    remonte = new QPushButton("remonte");
    sendButton = new QPushButton("Envoyer Mot");

    //========= Placement Grille ==========//
    grid->addWidget(btnUp, 0, 1);
    grid->addWidget(btnLeft, 1, 0);
    grid->addWidget(btnStop, 1, 1);
    grid->addWidget(btnRight, 1, 2);
    grid->addWidget(btnDown, 2, 1);
    grid->addWidget(AVBbtnUp, 3, 0);
    grid->addWidget(AVBbtnDown, 3, 2);
    grid->addWidget(PincebtnLeft, 4, 0);
    grid->addWidget(PincebtnRight, 4, 2);
    grid->addWidget(PinceOuvrirBtn, 5, 0);
    grid->addWidget(resetButton, 5, 1);
    grid->addWidget(PinceFermerBtn, 5, 2);
    grid->addWidget(poignethaut, 6, 0);
    grid->addWidget(poignetbas, 6, 2);
    grid->addWidget(quartG, 0, 0);
    grid->addWidget(quartD, 0, 2);
    grid->addWidget(attrape, 3, 1);
    grid->addWidget(remonte, 4, 1);

    wordLayout->addWidget(commandInput);
    wordLayout->addWidget(sendButton);

    mainLayout->addLayout(grid);
    mainLayout->addLayout(wordLayout);
    setCentralWidget(central);

    //========= Connexion Serveur Distant ==========//
    client->connectToServer("192.168.0.134", 12345);

    connect(sendButton, &QPushButton::clicked, this, [=]() {

        QString mot = commandInput->text().trimmed();
        if (!mot.isEmpty()) {
            client->sendMessage(mot);
            if (mySerial) mySerial->writeData(mot);
            statusBar()->showMessage("Envoyé : " + mot);
        } else {
            statusBar()->showMessage("Entrez une commande texte.");
        }
    });

    connect(resetButton, &QPushButton::clicked, this, [=]() {
        client->sendMessage("I");
        if (mySerial) mySerial->writeData("I");
        statusBar()->showMessage("Réinitialisation (I)");
    });

    //========= Connexions ==========//
    auto connectButton = [&](QPushButton* button, const QString &code, const QString &label) {
        connect(button, &QPushButton::clicked, this, [=]() {
            client->sendMessage(code);
            if (mySerial) mySerial->writeData(code);
            statusBar()->showMessage(label + " (" + code + ")");
        });
    };

    connectButton(btnUp, "E", "Bras monter");
    connectButton(btnDown, "e", "Bras descendre");
    connectButton(btnLeft, "b", "Base gauche");
    connectButton(btnRight, "B", "Base droite");
    connectButton(AVBbtnUp, "c", "Avant-bras monter");
    connectButton(AVBbtnDown, "C", "Avant-bras descendre");
    connectButton(PincebtnLeft, "r", "Pince rotation gauche");
    connectButton(PincebtnRight, "R", "Pince rotation droite");
    connectButton(PinceOuvrirBtn, "O", "Pince ouverte");
    connectButton(PinceFermerBtn, "o", "Pince fermée");
    connectButton(btnStop, "S", "STOP");
    connectButton(poignethaut, "M", "Poignet monter");
    connectButton(poignetbas, "m", "Poignet descendre");
    connectButton(quartD, "ddddddddddd", "1/4 à droite");
    connectButton(quartG, "bbbbbbbbbbb", "1/4 à gauche");
    connectButton(attrape, "CCCCeeeooommm", "attrape l'objet");
    connectButton(remonte, "ccccEEEOOOMMM", "remonte l'objet");

    //========= Relais tablette vers serveur distant ==========//
    connect(server, &Serveur::messageRecu, this, [=](const QString &message) {
        client->sendMessage(message);
        if (mySerial) mySerial->writeData(message);
        statusBar()->showMessage("Message relayé depuis tablette : " + message);
    });
}

MainWindow::~MainWindow() {
    if (mySerial) {
        mySerial->closePort();
        delete mySerial;
    }
}
