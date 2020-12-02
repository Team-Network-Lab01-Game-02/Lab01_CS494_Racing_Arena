projectdir = /home/goflag/Dev/Lab01-Networking/Lab01_CS494_Racing_Arena

CONFIG += qt
QT += widgets
QT += core
QT += network
QT += gui
INCLUDEPATH += $${projectdir}/include
HEADERS += $${projectdir}/include/client.h
SOURCES += $${projectdir}/main_client.cpp
SOURCES += $${projectdir}/defs/client/*.cpp
DESTDIR = $${projectdir} 
TARGET = client

