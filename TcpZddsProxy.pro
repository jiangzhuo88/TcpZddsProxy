QT += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TcpZddsProxy
TEMPLATE = app
CONFIG += c++11
DESTDIR = $$PWD/bin/

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/ZDDS/include

# ZDDS 库（参考 ProtocolSimulator 项目结构）
LIBS += -L$$PWD/ZDDS/lib -lZDDSd
LIBS += -L$$PWD/ZDDS/lib -lzmq

INCLUDEPATH += $$PWD/src/ZDDS/include
LIBS += -L$$PWD/src/ZDDS/lib -lZDDSd
LIBS += -L$$PWD/src/ZDDS/lib -lzmq

system(cp -r "$$PWD/src/ZDDS/lib/lib*.so*" "$$DESTDIR")

# Qt Linguist 翻译文件
TRANSLATIONS += translations/tcpzddsproxy_en.ts

# 嵌入翻译资源（.qm 编译后文件）
RESOURCES += translations.qrc

SOURCES += \
    main.cpp \
    ZDDSManager.cpp \
    TcpProxyCore.cpp \
    ConfigManager.cpp \
    LanguageManager.cpp \
    MainWindow.cpp

HEADERS += \
    ZDDSManager.h \
    TcpProxyCore.h \
    ConfigManager.h \
    LanguageManager.h \
    MainWindow.h

# 翻译源文件
DISTFILES += \
    translations/tcpzddsproxy_en.ts
