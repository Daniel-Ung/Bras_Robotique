// camera.cpp
#include "camera.h"
#include <QVBoxLayout>
#include <QImage>
#include <QPixmap>
#include <QDebug>

//========= Constructeur de la classe Camera ==========//

Camera::Camera(QWidget *parent) : QWidget(parent) {
    // Création d'un label pour afficher l'image de la caméra
    label = new QLabel(this);
    label->setFixedSize(640, 480);  // Taille fixe du label pour afficher la vidéo
    label->setStyleSheet("background: black"); // Fond noir pour le label

    // Mise en place d'un layout vertical pour le label

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(label);
    setLayout(layout);

    // Ouverture de la caméra par défaut (index 0)

    cap.open(0);
    if (!cap.isOpened()) {
        label->setText("❌ Caméra non détectée"); // Affichage d'un message d'erreur si la caméra n'est pas détectée
        return;
    }

    // Configuration de la résolution de la vidéo (640x480)
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    // Création d'un timer pour mettre à jour l'image affichée toutes les 25ms (~30fps)
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Camera::updateFrame);
    timer->start(25);  // Mise à jour de l'image toutes les 25ms (~30fps)
}

//========= Destructeur de la classe Camera ==========//

Camera::~Camera() {
    // Libération des ressources de la caméra si elle est ouverte
    if (cap.isOpened())
        cap.release();
}

//========= Mise à jour du frame vidéo ==========//

void Camera::updateFrame() {
    cv::Mat frame;
    cap >> frame;  // Capture d'une frame de la caméra
    if (frame.empty()) return; // Si la frame est vide, on ne fait rien

    // Conversion de la frame en QImage pour l'afficher dans le QLabel
    QImage image = matToQImage(frame);
    if (!image.isNull())
        label->setPixmap(QPixmap::fromImage(image)); // Affichage de l'image convertie dans le QLabel
}

//========= Conversion d'une frame OpenCV (cv::Mat) en QImage ==========//

QImage Camera::matToQImage(const cv::Mat &frame) {
    cv::Mat rgb;
    // Conversion de l'image de format BGR (utilisé par OpenCV) en format RGB (utilisé par Qt)
    cv::cvtColor(frame, rgb, cv::COLOR_BGR2RGB);

    // Création d'un QImage à partir de la donnée de l'image
    return QImage(rgb.data, rgb.cols, rgb.rows, static_cast<int>(rgb.step), QImage::Format_RGB888).copy();
}

//========= Getter pour le label vidéo ==========//

QLabel* Camera::getVideoLabel() {
    return label;
}

//========= Getter pour l'objet VideoCapture de la caméra ==========//

cv::VideoCapture* Camera::getCapture() {
    return &cap;
}
