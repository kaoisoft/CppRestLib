QT -= gui

TEMPLATE = lib
DEFINES += CPPRESTLIB_LIBRARY

CONFIG += c++11

INCLUDEPATH += /usr/local/include

LIBS += -L/usr/local/lib
LIBS += -lcrypto -lssl -lpthread
LIBS += -llog4cxx

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    CppRestLib.cpp \
	RestRequest.cpp \
	RestResponse.cpp \
	RestServer.cpp \
	SecureRestServer.cpp \
	Socket.cpp \
	SslSocket.cpp \
	StringUtils.cpp

HEADERS += \
    CppRestLib_global.h \
    CppRestLib.h \
    RestRequest.h \
    RestResponse.h \
    RestServer.h \
    SecureRestServer.h \
    Socket.h \
    SslSocket.h \
    StringUtils.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

QMAKE_POST_LINK += cp $$PWD/*.h ${HOME}/include/CppRestLib;
QMAKE_POST_LINK += cp $${OUT_PWD}/*.so* ${HOME}/lib;

DISTFILES += \
	CODE_OF_CONDUCT.md \
	CONTRIBUTING.md \
	LICENSE.md \
	README.md \
	TODO.txt \
	log4cxx_config.xml
