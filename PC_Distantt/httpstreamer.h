#ifndef HTTPSTREAMER_H
#define HTTPSTREAMER_H

#include <QTcpServer>

// Déclare la classe Camera
class Camera;

/**
 * @brief HttpStreamer
 * Un mini-serveur HTTP qui diffuse le flux vidéo MJPEG
 * en se basant sur la méthode grabFrame() de ta classe Camera.
 */
class HttpStreamer : public QTcpServer {
    Q_OBJECT

public:
    /**
     * @param cam Un pointeur vers la classe Camera (pour récupérer les images)
     * @param parent Le parent QObject, si besoin
     */
    explicit HttpStreamer(Camera *cam, QObject *parent = nullptr);

    /**
     * Lance l’écoute sur le port donné.
     * @param port Le port TCP pour le serveur HTTP
     * @return true si le serveur écoute correctement, false sinon
     */
    bool startServer(quint16 port);

protected:
    /**
     * Cette méthode est appelée à chaque nouvelle connexion TCP.
     * On y gère l’envoi en boucle du flux MJPEG.
     */
    void incomingConnection(qintptr socketDescriptor) override;

private:
    Camera *m_camera;
};

#endif // HTTPSTREAMER_H
