#-------------------------------------------------
#
# Project created by QtCreator 2016-03-25T00:16:21
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = "LightBurden"
TEMPLATE = app


SOURCES += main.cpp\
    engine_Doom64EX.cpp \
    engine_turok2.cpp \
        rocketlauncher2.cpp \
    abstractmodels.cpp \
    dndfilesystemlistview.cpp \
    hyp_commonfunc.cpp \
    configs.cpp \
    enginesetup.cpp \
    engine_turok.cpp \
    engine_dosdoom.cpp \
    engine_ZDoom.cpp \
    commandlinedialog.cpp

HEADERS  += rocketlauncher2.h \
    abstractmodels.h \
    dndfilesystemlistview.h \
    hyp_commonfunc.h \
    configs.h \
    commandlinedialog.h

FORMS    += rocketlauncher2.ui \
    OLDrocketlauncher2.ui \
    commandlinedialog.ui

RESOURCES += \
    resources.qrc

CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

RC_ICONS = Light_Burden_logo.ico

CONFIG += c++11
CONFIG += static

VERSION = 0.1.0.2
QMAKE_TARGET_COMPANY = DM23
QMAKE_TARGET_PRODUCT = Light Burden 0.6
QMAKE_TARGET_DESCRIPTION = Light Burden 0.6
QMAKE_TARGET_COPYRIGHT = Doommarine23 - Extended from Hypnotoad's Rocket Launcher 2 & Paynworth's Custom Edition
