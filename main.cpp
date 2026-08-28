#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("TCP-ZDDS Proxy");
    a.setWindowIcon(QIcon(":/Proxy.png"));
    MainWindow w;
    w.show();

    return a.exec();
}
