QT += core gui widgets network

CONFIG += c++11

TARGET = BrasRobotise
TEMPLATE = app

# Fichiers sources
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    client.cpp \
    serialport.cpp \
    server_http.cpp \
    serveur.cpp \
    camera.cpp

# Fichiers headers
HEADERS += \
    mainwindow.h \
    client.h \
    serialport.h \
    server_http.h \
    serveur.h \
    camera.h

# Include OpenCV
INCLUDEPATH += /usr/include/opencv4
LIBS += -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs
