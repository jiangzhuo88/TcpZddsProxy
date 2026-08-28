#include <QApplication>
#include <QLocalSocket>
#include <QLocalServer>
#include <QMessageBox>
#include "MainWindow.h"
#include "version.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("TCP-ZDDS Proxy");
    a.setWindowIcon(QIcon(":/Proxy.png"));

    // 单进程模式：尝试连接已存在的实例
    const QString serverName = "TcpZddsProxy_SingleInstance";
    QLocalSocket socket;
    socket.connectToServer(serverName);
    if (socket.waitForConnected(500)) {
        // 已有实例在运行，退出
        QMessageBox::information(nullptr, QObject::tr("提示"),
            QObject::tr("程序已在运行中，不允许重复启动。"));
        return 0;
    }

    // 创建本地服务占位（防止重复启动）
    QLocalServer::removeServer(serverName);
    QLocalServer localServer;
    localServer.listen(serverName);
    QObject::connect(&localServer, &QLocalServer::newConnection, []() {
        // 接受连接但不处理，仅用于占位
        QLocalServer *server = qobject_cast<QLocalServer*>(sender());
        if (server) {
            QLocalSocket *client = server->nextPendingConnection();
            if (client) client->deleteLater();
        }
    });

    MainWindow w;
    w.show();

    int ret = a.exec();

    // 清理本地服务
    localServer.close();
    QLocalServer::removeServer(serverName);
    return ret;
}
