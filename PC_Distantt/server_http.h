#ifndef SERVERHTTP_H
#define SERVERHTTP_H

#include <QObject>
#include <opencv2/opencv.hpp>
#include <thread>

class ServerHttp : public QObject {
    Q_OBJECT

public:
    explicit ServerHttp(cv::VideoCapture* cap, QObject *parent = nullptr);
    ~ServerHttp();

    bool start(int port);

private:
    void handleConnection();

    int server_fd;
    cv::VideoCapture* m_cap;
    std::thread serverThread;
    bool running = false;
};

#endif // SERVERHTTP_H
