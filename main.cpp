#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("TCP-ZDDS Proxy");
    a.setApplicationDisplayName("TCP-ZDDS 双向代理软件");

    MainWindow w;
    w.show();

    return a.exec();
}
