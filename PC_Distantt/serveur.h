#ifndef SERVEUR_H
#define SERVEUR_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QObject>
#include <QList>

class SerialPort;

class Serveur : public QObject
{
    Q_OBJECT

public:
    explicit Serveur(SerialPort *serialPort, QObject *parent = nullptr);
    bool startServer(quint16 port);

signals:
    void messageRecu(const QString &message);

private slots:
    void onNewConnection();
    void onReadyRead();
    void onClientDisconnected();

private:
    QTcpServer *tcpServer;
    QList<QTcpSocket*> clients;
    SerialPort *serial;
};

#endif // SERVEUR_H
