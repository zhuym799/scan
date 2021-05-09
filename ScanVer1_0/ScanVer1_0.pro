QT       += sql
QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = ScanVer1_0
TEMPLATE = app
RC_ICONS=app.ico
DEFINES += QT_DEPRECATED_WARNINGS


CONFIG += c++11
SOURCES += \
        main.cpp \
        mainwindow.cpp \
    logindialog.cpp \
    DBUtil/dbutil.cpp \
    qscanglobal.cpp \
    scannersdk/dtwaincore.cpp \
    scannersdk/scannerconfig.cpp \
    scannersdk/scanner.cpp \
    scannersdk/scanner_operation.cpp \
    scannersdk/scannerconfig_sane.cpp \
    scannerform.cpp \
    handleform.cpp \
    highform.cpp \
    pdfform.cpp \
    filedirectoryform.cpp

HEADERS += \
    mainwindow.h \
    logindialog.h \
    DBUtil/dbutil.h \
    qscanglobal.h \
    scannersdk/dtwaincore.h \
    scannersdk/scannerconfig.h \
    scannersdk/scanner.h \
    scannersdk/scanner_operation.h \
    scannersdk/scannerconfig_sane.h \
    scannerform.h \
    handleform.h \
    highform.h \
    pdfform.h \
    filedirectoryform.h



FORMS += \
    mainwindow.ui \
    logindialog.ui \
    scannersdk/scannerconfig.ui \
    scannersdk/scannerconfig_sane.ui \
    scannerform.ui \
    handleform.ui \
    highform.ui \
    pdfform.ui \
    filedirectoryform.ui

contains(QT_ARCH,x86_64){
       DEFINES += QT_WIN
        #openssl
        win32: LIBS += -L$$PWD/libs/ -lcrypto
        #opencv
        win32: LIBS += -L$$PWD/libs/ -lopencv_world401.dll
}



INCLUDEPATH += $$PWD/thirdparty/
INCLUDEPATH += $$PWD/thirdparty/openssl/include
DEPENDPATH += $$PWD/thirdparty/openssl/include

INCLUDEPATH += $$PWD/thirdparty/opencv-4.0.1/

INCLUDEPATH += $$PWD/scannersdk/sane/include
DEPENDPATH += $$PWD/scannersdk/sane/include

DESTDIR= ../ScanVer_bin

RESOURCES += \
    qrc.qrc


VERSION = 1.0.0
# 0x0004 表示 简体中文
RC_LANG = 0x0004
# 公司名
QMAKE_TARGET_COMPANY = 923637194@qq.com
# 产品名称
QMAKE_TARGET_PRODUCT =Document scanning management system
# 详细描述
QMAKE_TARGET_DESCRIPTION = Document scanning management system VERSION
# 版权
QMAKE_TARGET_COPYRIGHT = copyright 2021-2050 923637194@qq.com All Rights

