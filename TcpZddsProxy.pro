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

# 运行时拷贝库文件到输出目录
unix {
    system(mkdir -p $$DESTDIR)
    system(cp -f "$$PWD/ZDDS/lib/libZDDSd.so" "$$DESTDIR/" 2>/dev/null) || true
    system(cp -f "$$PWD/ZDDS/lib/libZDDSd.so.1" "$$DESTDIR/" 2>/dev/null) || true
    system(cp -f "$$PWD/ZDDS/lib/libZDDSd.so.1.2" "$$DESTDIR/" 2>/dev/null) || true
    system(cp -f "$$PWD/ZDDS/lib/libzmq.so" "$$DESTDIR/" 2>/dev/null) || true
    system(cp -f "$$PWD/ZDDS/lib/libzmq.so.5" "$$DESTDIR/" 2>/dev/null) || true
}

SOURCES += \
    main.cpp \
    ZDDSManager.cpp \
    TcpProxyCore.cpp \
    ConfigManager.cpp \
    MainWindow.cpp

HEADERS += \
    ZDDSManager.h \
    TcpProxyCore.h \
    ConfigManager.h \
    MainWindow.h
