#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QString>

class SerialPort {
public:
    SerialPort(const QString &portName);
    ~SerialPort();

    bool openPort();
    void closePort();
    bool writeData(const QString &data);

private:
    QString portName;
    int fd;
};

#endif // SERIALPORT_H
