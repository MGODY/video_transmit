TEMPLATE = app
TARGET = client
INCLUDEPATH += . \
 Show \
 Connect \
 Decode \
 Accept \
 lib \
 include

HEADERS += define.h Accept/accept.h Decode/decode.h Show/processImage.h
SOURCES += main.cpp Accept/accept.cpp Decode/decode.cpp Show/processImage.cpp
FORMS += ui/client.ui
LIBS += lib/libjrtp.a \
 lib/libjthread.a \
lib/libavformat.a\
lib/libswscale.a\
lib/libavcodec.a\
lib/libavutil.a

CONFIG += c++11
