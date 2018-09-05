QT += gui

CONFIG += c++11 console
CONFIG -= app_bundle

linux-g++*: QMAKE_CFLAGS += -std=c99

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    application.cpp \
    config.cpp \
    pdfwriter.cpp \
    file.cpp \
    zopfli/src/zopfli/blocksplitter.c \
    zopfli/src/zopfli/cache.c \
    zopfli/src/zopfli/deflate.c \
    zopfli/src/zopfli/gzip_container.c \
    zopfli/src/zopfli/hash.c \
    zopfli/src/zopfli/katajainen.c \
    zopfli/src/zopfli/lz77.c \
    zopfli/src/zopfli/squeeze.c \
    zopfli/src/zopfli/tree.c \
    zopfli/src/zopfli/util.c \
    zopfli/src/zopfli/zlib_container.c \
    zopfli/src/zopfli/zopfli_lib.c \
    libimagequant/blur.c \
    libimagequant/kmeans.c \
    libimagequant/libimagequant.c \
    libimagequant/mediancut.c \
    libimagequant/mempool.c \
    libimagequant/nearest.c \
    libimagequant/pam.c \
    imageoptimizer.cpp \
    image.cpp

HEADERS += \
    application.h \
    config.h \
    pdfwriter.h \
    file.h \
    zopfli/src/zopfli/blocksplitter.h \
    zopfli/src/zopfli/cache.h \
    zopfli/src/zopfli/deflate.h \
    zopfli/src/zopfli/gzip_container.h \
    zopfli/src/zopfli/hash.h \
    zopfli/src/zopfli/katajainen.h \
    zopfli/src/zopfli/lz77.h \
    zopfli/src/zopfli/squeeze.h \
    zopfli/src/zopfli/symbols.h \
    zopfli/src/zopfli/tree.h \
    zopfli/src/zopfli/util.h \
    zopfli/src/zopfli/zlib_container.h \
    zopfli/src/zopfli/zopfli.h \
    libimagequant/blur.h \
    libimagequant/kmeans.h \
    libimagequant/libimagequant.h \
    libimagequant/mediancut.h \
    libimagequant/mempool.h \
    libimagequant/nearest.h \
    libimagequant/pam.h \
    imageoptimizer.h \
    image.h
