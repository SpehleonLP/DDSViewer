QT       += core gui openglwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

LIBS += -lGLEW -lGL -lGLU -ldrm -lz

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/Shaders/defaultvaos.cpp \
    src/Shaders/glprogram.cpp \
    src/Shaders/textureshaders.cpp \
    src/Shaders/transparencyshader.cpp \
    src/document.cpp \
    src/glviewwidget.cpp \
    src/main.cpp \
    src/mainwindow.cpp

HEADERS += \
    src/Shaders/defaultvaos.h \
    src/Shaders/glprogram.h \
    src/Shaders/textureshaders.h \
    src/Shaders/transparencyshader.h \
    src/counted_ptr.hpp \
    src/dds_header.hpp \
    src/document.h \
    src/enums.hpp \
    src/format_table.hpp \
    src/glviewwidget.h \
    src/mainwindow.h

FORMS += \
    src/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
