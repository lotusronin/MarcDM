######################################################################
# Automatically generated by qmake (3.0) Tue Jul 8 01:32:11 2014
######################################################################

TEMPLATE = app
TARGET = "../../marcdm"
INCLUDEPATH += .

# Input
SOURCES += main.cpp window.cpp server.cpp session.cpp settings.cpp auth.cpp
QT += widgets
CONFIG += c++11
LIBS += -lxcb -lpam -lpam_misc

OBJECTS_DIR = build
