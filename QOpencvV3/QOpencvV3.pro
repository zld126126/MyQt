QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

# linux设置 OpenCV 路径
OPENCV_INSTALL_PATH = /opt/opencv

# linux包含头文件路径
INCLUDEPATH += $$OPENCV_INSTALL_PATH/include
INCLUDEPATH += $$OPENCV_INSTALL_PATH/include/opencv2

# linux添加 OpenCV 库文件路径
LIBS += -L$$OPENCV_INSTALL_PATH/lib

# linux链接需要的 OpenCV 库（根据项目实际使用的模块调整）
LIBS += -lopencv_core \
        -lopencv_imgproc \
        -lopencv_highgui \
        -lopencv_imgcodecs \
        -lopencv_videoio

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
