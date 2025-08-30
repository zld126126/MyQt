QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mythreadcamera.cpp

HEADERS += \
    mythreadcamera.h

FORMS += \
    mythreadcamera.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# -------------------------- 关键配置 --------------------------
# 1. 动态库路径（指定libcamera.so所在目录）
LIBS += -L./camera_lib -lcamera  # -L：库路径，-lcamera：链接libcamera.so
# 2. 设置rpath，确保运行时能找到动态库
unix: LIBS += -Wl,-rpath=$$PWD/camera_lib
# 3. 头文件路径（指定C代码头文件所在目录）
INCLUDEPATH += ./camera_lib
# 4. 确保动态库可被加载（Linux环境下的可选配置）
unix: QMAKE_POST_LINK += cp ./camera_lib/libcamera.so $$OUT_PWD
