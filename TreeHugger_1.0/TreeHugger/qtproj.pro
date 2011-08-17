######################################################################
# Automatically generated by qmake (2.01a) Thu May 26 21:41:31 2011
######################################################################

QT        += opengl
#QT       -= gui

CONFIG += console                      #für Konsolenanwendungen
CONFIG += qt warn_on debug_and_release #Debugging-Informationen und Warnungen werden ausgegeben

TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .

INCLUDEPATH += ../include/ /usr/include/ /usr/local/include
LIBS = -L/usr/lib/ -L/usr/local/lib -L../lib -lGL -lGLEW -lIL

DESTDIR = bin  

# Input
HEADERS += shader.h \
    openGLQtContext.h

SOURCES += main.cpp \
    openGLQtContext.cpp \
    shader.cpp


# install
#target.path = opengl
#sources.files = $$SOURCES \
 #   $$HEADERS \
  #  $$RESOURCES \

#INSTALLS += target \
 #   sources
 
#win32:CONFIG(release, debug|release): LIBS += C:\Windows\System32 -lGLEW
#else:win32:CONFIG(debug, debug|release): LIBS += C:\Windows\System32 -lGLEWd

#unix: LIBS += /usr/lib/ -lGLEW

#INCLUDEPATH += /usr/include
#DEPENDPATH += /usr/include

#win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../usr/lib/GLEW.lib
#else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../usr/lib/GLEWd.lib
#unix:!symbian: PRE_TARGETDEPS += $$PWD/../../../../../../usr/lib/libGLEW.a
