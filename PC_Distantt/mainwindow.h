#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include "serialport.h"
#include "serveur.h"
#include "client.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    SerialPort *mySerial;
    Serveur *server;
    Client *client;
    bool pinceOuverte;

    QPushButton *btnUp;
    QPushButton *btnDown;
    QPushButton *btnLeft;
    QPushButton *btnRight;
    QPushButton *btnStop;
    QPushButton *AVBbtnUp;
    QPushButton *AVBbtnDown;
    QPushButton *PincebtnLeft;
    QPushButton *PincebtnRight;
    QPushButton *PinceOuvrirBtn;
    QPushButton *PinceFermerBtn;
    QPushButton *poignethaut;
    QPushButton *poignetbas;
    QPushButton *quartG;
    QPushButton *remonte;
    QPushButton *attrape;
    QPushButton *quartD;
    QPushButton *resetButton;
    QPushButton *sendButton;
    QLineEdit *commandInput;
};

#endif // MAINWINDOW_H
