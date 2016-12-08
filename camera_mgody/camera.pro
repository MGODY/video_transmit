TEMPLATE = app
TARGET =
DEPENDPATH += . \
 Camera_device \
 H264 \
 Qt \
 Transfer
INCLUDEPATH += . \
 Qt \
 Camera_device \
 H264 \
 Transfer \
 include
HEADERS += def.h \
 Camera_device/videodevice.h \
 H264/h264.h \
 Qt/processImage.h \
 Transfer/transfer.h \
 H264/nal_struct.h
SOURCES += main.cpp \
 Camera_device/videodevice.cpp \
 H264/h264.cpp \
 Qt/processImage.cpp \
 Transfer/transfer.cpp
 
LIBS += lib/libx264.a -ldl lib/libjrtp.a lib/libjthread.a
CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++0x
