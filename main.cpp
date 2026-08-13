#include <QApplication>
#include "MainWindow.h"
#include "LanguageManager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("TCP-ZDDS Proxy");
    a.setApplicationDisplayName(LTR("app_display_name"));

    MainWindow w;
    w.show();

    return a.exec();
}
