QT       += core gui serialport widgets network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    ./protocol/protocol.cpp \
   ./protocol/slave2backend.cpp \  # 明确列出
   ./protocol/packetbase.cpp \      # 如果存在
    mainwindow.cpp \
    newdevice.cpp \
    slavedevice.cpp \
    slavepage.cpp \
    udpmanager.cpp

HEADERS += \
    ./protocol/*.h\
    mainwindow.h \
    newdevice.h \
    slavedevice.h \
    slavepage.h \
    udpmanager.h

FORMS += \
    mainwindow.ui \
    newdevice.ui


# QXlsx代码应用于Qt项目中
QXLSX_PARENTPATH=./QXlsx/         # ./当前工程目录路径 指定QXlsx
QXLSX_HEADERPATH=./QXlsx/header/  # ./当前工程目录路径 指定header文件
QXLSX_SOURCEPATH=./QXlsx/source/  # ./当前工程目录路径 指定source文件
include(./QXlsx/QXlsx.pri)        # ./当前工程目录路径 库的项目包含文件（.pri 文件


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
