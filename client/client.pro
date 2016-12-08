TEMPLATE = app
TARGET = client
INCLUDEPATH += . \
 Show \
 Connect \
 Decode \
 Accept \
 lib \
 /usr/local/include
HEADERS += define.h Accept/accept.h Decode/decode.h Show/processImage.h
SOURCES += main.cpp Accept/accept.cpp Decode/decode.cpp Show/processImage.cpp
LIBS += lib/libjrtp.a \
 lib/libjthread.a \
lib/libavformat.a\
lib/libswscale.a\
lib/libavcodec.a\
lib/libavutil.a

CONFIG += c++11
