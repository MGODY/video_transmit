TEMPLATE = app
TARGET = Server
DEPENDPATH += . src lib include
INCLUDEPATH += . include

HEADERS += def.h \
 include/rtpt.h \
 include/tcpserver.h \
 include/mesegtype.h
 
SOURCES+= main.cpp \
 	src/RtpT.cpp \
 	src/TcpServer.cpp 
LIBS +=  -ldl  lib/libjrtp.a  lib/libjthread.a

CONFIG += c++11
