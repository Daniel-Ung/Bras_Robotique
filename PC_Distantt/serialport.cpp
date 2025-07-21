#include "serialport.h"
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <QDebug>

//========= Connexion Serveur Distant ==========//

SerialPort::SerialPort(const QString &portName) : portName(portName), fd(-1) {}

SerialPort::~SerialPort() {
    closePort();
}

//========= Ouverture du port série ==========//

bool SerialPort::openPort() {
    fd = open(portName.toStdString().c_str(), O_RDWR | O_NOCTTY);
    if (fd == -1) {
        qDebug() << "[SerialPort] ❌ Erreur ouverture port série :" << portName;
        return false;
    }

    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        qDebug() << "[SerialPort] ❌ Erreur configuration série.";
        return false;
    }

    //========= Configuration du port série ==========//

    cfsetospeed(&tty, B9600); // Vitesse de transmission (9600 bauds)
    cfsetispeed(&tty, B9600);

    tty.c_cflag |= (CLOCAL | CREAD);   // Activation du mode local et lecture
    tty.c_cflag &= ~CSIZE;             // Masque la taille des données
    tty.c_cflag |= CS8;                // 8 bits de données
    tty.c_cflag &= ~PARENB;            // Pas de parité
    tty.c_cflag &= ~CSTOPB;            // 1 bit de stop
    tty.c_cflag &= ~CRTSCTS;           // Pas de contrôle de flux matériel

    tcsetattr(fd, TCSANOW, &tty);      // Application de la configuration

    qDebug() << "[SerialPort] ✅ Port série ouvert :" << portName;
    return true;
}

//========= Fermeture du port série ==========//

void SerialPort::closePort() {
    if (fd != -1) {
        close(fd);  // Fermeture du port
        fd = -1;    // Réinitialisation du descripteur de fichier
    }
}

//========= Écriture de données sur le port série ==========//

bool SerialPort::writeData(const QString &data) {
    if (fd != -1) {
        QByteArray bytes = data.toUtf8(); // Conversion de la chaîne en bytes
        write(fd, bytes.constData(), bytes.size()); // Envoi des données
        return true;
    }
    return false;
}
