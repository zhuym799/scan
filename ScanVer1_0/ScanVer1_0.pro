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
    qscanglobal.cpp

HEADERS += \
        mainwindow.h \
    logindialog.h \
    DBUtil/dbutil.h \
    qscanglobal.h

FORMS += \
        mainwindow.ui \
    logindialog.ui


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

