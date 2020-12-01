projectdir = /home/goflag/Dev/Lab01-Networking/Lab01_CS494_Racing_Arena

CONFIG += qt
QT += widgets
QT += core
QT += network
QT += gui
INCLUDEPATH += $${projectdir}/include
SOURCES += $${projectdir}/main_server.cpp
HEADERS += $${projectdir}/include/server.h
SOURCES += $${projectdir}/defs/server/*.cpp
DESTDIR = $${projectdir} 
TARGET = server