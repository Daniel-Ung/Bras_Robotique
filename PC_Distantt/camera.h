// camera.h
#ifndef CAMERA_H
#define CAMERA_H

#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <opencv2/opencv.hpp>

class Camera : public QWidget {
    Q_OBJECT

public:
    explicit Camera(QWidget *parent = nullptr);
    ~Camera();
    QLabel* getVideoLabel();
    cv::VideoCapture* getCapture();

private slots:
    void updateFrame();

private:
    cv::VideoCapture cap;
    QTimer *timer;
    QLabel *label;
    QImage matToQImage(const cv::Mat &frame);
};

#endif // CAMERA_H
