#include "TcpProxyCore.h"
#include "ZDDSManager.h"
#include "LanguageManager.h"
#include <QHostAddress>

TcpProxyCore::TcpProxyCore(QObject *parent)
    : QObject(parent)
    , m_tcpServer(nullptr)
    , m_serverSocket(nullptr)
{
}

TcpProxyCore::~TcpProxyCore()
{
    stop();
}

void TcpProxyCore::setConfig(const ProxyConfig &cfg)
{
    if (m_running) {
        emit logMessage(LTR("log_warn_running"));
        return;
    }
    m_cfg = cfg;
}

void TcpProxyCore::resetStats()
{
    m_tcpRxBytes = 0;
    m_tcpTxBytes = 0;
    m_zddsRxBytes = 0;
    m_zddsTxBytes = 0;
    m_lastTcpToZddsTime = QDateTime();
    m_lastZddsToTcpTime = QDateTime();
}

QString TcpProxyCore::toHex(const QByteArray &data, int maxShow)
{
    if (data.size() <= maxShow) {
        return QString::fromLatin1(data.toHex(' '));
    }
    QByteArray head = data.left(maxShow);
    return QString::fromLatin1(head.toHex(' ')) + LTR("log_hex_truncated").arg(data.size());
}

bool TcpProxyCore::isRunning() const
{
    return m_running;
}

bool TcpProxyCore::isConnected() const
{
    if (m_cfg.mode == ProxyMode::ProxyServer)
        return m_running;  // 服务端模式：监听即算连接
    return m_running && m_serverSocketConnected;  // 客户端模式：需TCP已连接
}

TcpLinkStatus TcpProxyCore::tcpLinkStatus() const
{
    return m_tcpStatus;
}

QString TcpProxyCore::tcpPeerInfo() const
{
    if (m_cfg.mode == ProxyMode::ProxyClient && m_serverSocket && m_serverSocketConnected) {
        return QString("%1:%2").arg(m_cfg.tcpHost).arg(m_cfg.tcpPort);
    }
    if (m_cfg.mode == ProxyMode::ProxyServer && !m_clientSockets.isEmpty()) {
        auto *sock = m_clientSockets.first();
        return QString("%1:%2").arg(sock->peerAddress().toString()).arg(sock->peerPort());
    }
    return QString();
}

bool TcpProxyCore::isTcpToZddsActive() const
{
    if (m_lastTcpToZddsTime.isNull()) return false;
    return m_lastTcpToZddsTime.msecsTo(QDateTime::currentDateTime()) < 5000;
}

bool TcpProxyCore::isZddsToTcpActive() const
{
    if (m_lastZddsToTcpTime.isNull()) return false;
    return m_lastZddsToTcpTime.msecsTo(QDateTime::currentDateTime()) < 5000;
}

bool TcpProxyCore::start()
{
    if (m_running) {
        emit logMessage(LTR("log_warn_already_running"));
        return false;
    }
    if (!m_cfg.isValid()) {
        emit logMessage(LTR("log_err_invalid_config"));
        return false;
    }

    resetStats();
    ZDDSManager::getInstance()->initialize();

    // 订阅ZDDS接收主题（ZDDS -> TCP方向）
    if (!m_zddsSubscribed) {
        auto recvCb = [this](const char* data, size_t len) {
            forwardZddsToTcp(data, len);
        };
        ZDDSManager::getInstance()->subscribe(
            m_cfg.zddsRecvDomain.toUtf8().constData(),
            m_cfg.zddsRecvTopic.toUtf8().constData(),
            recvCb);
        m_zddsSubscribed = true;
    }

    bool ok = false;
    if (m_cfg.mode == ProxyMode::ProxyServer) {
        // === 代理服务端：监听端口，等待真实客户端 ===
        m_tcpServer = new QTcpServer(this);
        connect(m_tcpServer, &QTcpServer::newConnection, this, &TcpProxyCore::onNewClientConnection);
        if (!m_tcpServer->listen(QHostAddress::Any, m_cfg.tcpPort)) {
            m_tcpStatus = TcpLinkStatus::Error;
            emit logMessage(LTR("log_server_listen_failed").arg(m_cfg.tcpPort).arg(m_tcpServer->errorString()));
            delete m_tcpServer;
            m_tcpServer = nullptr;
            emit stateChanged();
            return false;
        }
        m_tcpStatus = TcpLinkStatus::Listening;
        emit logMessage(LTR("log_server_started").arg(m_cfg.tcpPort));
        emit logMessage(LTR("log_config_info").arg(m_cfg.zddsSendDomain).arg(m_cfg.zddsRecvDomain).arg(m_cfg.zddsSendTopic).arg(m_cfg.zddsRecvTopic));
        ok = true;
    } else {
        // === 代理客户端：连接真实服务端 ===
        m_tcpStatus = TcpLinkStatus::Connecting;
        m_serverSocket = new QTcpSocket(this);
        m_serverSocketConnected = false;
        connect(m_serverSocket, &QTcpSocket::connected, this, &TcpProxyCore::onServerSocketConnected);
        connect(m_serverSocket, &QTcpSocket::disconnected, this, &TcpProxyCore::onServerSocketDisconnected);
        connect(m_serverSocket, &QTcpSocket::readyRead, this, &TcpProxyCore::onServerSocketReadyRead);
        connect(m_serverSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
                this, &TcpProxyCore::onServerSocketError);
        emit logMessage(LTR("log_client_connecting").arg(m_cfg.tcpHost).arg(m_cfg.tcpPort));
        m_serverSocket->connectToHost(m_cfg.tcpHost, m_cfg.tcpPort);
        // 标记为运行中（连接中），由 connected/error 信号维护具体状态
        ok = true;
        emit logMessage(LTR("log_config_info").arg(m_cfg.zddsSendDomain).arg(m_cfg.zddsRecvDomain).arg(m_cfg.zddsSendTopic).arg(m_cfg.zddsRecvTopic));
    }

    m_running = ok;
    emit stateChanged();
    return ok;
}

void TcpProxyCore::stop()
{
    if (!m_running && !m_tcpServer && !m_serverSocket) return;

    // 取消订阅ZDDS
    if (m_zddsSubscribed) {
        ZDDSManager::getInstance()->unsubscribe(
            m_cfg.zddsRecvDomain.toUtf8().constData(),
            m_cfg.zddsRecvTopic.toUtf8().constData());
        m_zddsSubscribed = false;
    }

    // 关闭代理服务端
    if (m_tcpServer) {
        for (auto *sock : m_clientSockets) {
            QString info = sock->peerAddress().toString() + ":" + QString::number(sock->peerPort());
            sock->close();
            sock->deleteLater();
            emit logMessage(LTR("log_client_disconnect").arg(info));
        }
        m_clientSockets.clear();
        m_tcpServer->close();
        delete m_tcpServer;
        m_tcpServer = nullptr;
        emit logMessage(LTR("log_server_stopped"));
    }

    // 关闭代理客户端
    if (m_serverSocket) {
        m_serverSocket->close();
        m_serverSocket->deleteLater();
        m_serverSocket = nullptr;
        m_serverSocketConnected = false;
        emit logMessage(LTR("log_client_stopped"));
    }

    m_running = false;
    m_tcpStatus = TcpLinkStatus::Stopped;
    emit stateChanged();
}

// ============================================================
// ProxyServer模式 - 真实客户端连接处理
// ============================================================
void TcpProxyCore::onNewClientConnection()
{
    while (m_tcpServer->hasPendingConnections()) {
        QTcpSocket *sock = m_tcpServer->nextPendingConnection();
        connect(sock, &QTcpSocket::readyRead, this, &TcpProxyCore::onClientSocketReadyRead);
        connect(sock, &QTcpSocket::disconnected, this, &TcpProxyCore::onClientSocketDisconnected);
        connect(sock, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
                this, &TcpProxyCore::onClientSocketError);
        m_clientSockets.append(sock);
        QString info = sock->peerAddress().toString() + ":" + QString::number(sock->peerPort());
        emit logMessage(LTR("log_client_connected_in").arg(info).arg(m_clientSockets.size()));
        if (m_tcpStatus != TcpLinkStatus::Connected) {
            m_tcpStatus = TcpLinkStatus::Connected;
        }
        emit stateChanged();
    }
}

void TcpProxyCore::onClientSocketReadyRead()
{
    QTcpSocket *sock = qobject_cast<QTcpSocket*>(sender());
    if (!sock) return;
    QByteArray data = sock->readAll();
    m_tcpRxBytes += data.size();
    QString info = sock->peerAddress().toString() + ":" + QString::number(sock->peerPort());
    emit logMessage(LTR("log_tcp_rx_client").arg(info).arg(data.size()).arg(toHex(data)));
    forwardTcpToZdds(data, info);
}

void TcpProxyCore::onClientSocketDisconnected()
{
    QTcpSocket *sock = qobject_cast<QTcpSocket*>(sender());
    if (!sock) return;
    QString info = sock->peerAddress().toString() + ":" + QString::number(sock->peerPort());
    m_clientSockets.removeOne(sock);
    sock->deleteLater();
    emit logMessage(LTR("log_client_disconnected_in").arg(info).arg(m_clientSockets.size()));
    if (m_clientSockets.isEmpty()) {
        m_tcpStatus = TcpLinkStatus::Listening;
    }
    emit stateChanged();
}

void TcpProxyCore::onClientSocketError(QAbstractSocket::SocketError err)
{
    QTcpSocket *sock = qobject_cast<QTcpSocket*>(sender());
    if (!sock) return;
    QString info = sock->peerAddress().toString() + ":" + QString::number(sock->peerPort());
    emit logMessage(LTR("log_client_error").arg(info).arg(sock->errorString()));
    Q_UNUSED(err);
}

// ============================================================
// ProxyClient模式 - 连接真实服务端
// ============================================================
void TcpProxyCore::onServerSocketConnected()
{
    m_serverSocketConnected = true;
    m_tcpStatus = TcpLinkStatus::Connected;
    emit logMessage(LTR("log_client_connected").arg(m_cfg.tcpHost).arg(m_cfg.tcpPort));
    emit stateChanged();
}

void TcpProxyCore::onServerSocketDisconnected()
{
    m_serverSocketConnected = false;
    m_tcpStatus = TcpLinkStatus::Disconnected;
    emit logMessage(LTR("log_client_disconnected"));
    // 不改 m_running，仍允许重连或重新启动；但状态标签反映实际连接状态
    emit stateChanged();
}

void TcpProxyCore::onServerSocketReadyRead()
{
    if (!m_serverSocket) return;
    QByteArray data = m_serverSocket->readAll();
    m_tcpRxBytes += data.size();
    QString info = m_serverSocket->peerAddress().toString() + ":" + QString::number(m_serverSocket->peerPort());
    emit logMessage(LTR("log_tcp_rx_server").arg(info).arg(data.size()).arg(toHex(data)));
    forwardTcpToZdds(data, info);
}

void TcpProxyCore::onServerSocketError(QAbstractSocket::SocketError err)
{
    if (!m_serverSocket) return;
    emit logMessage(LTR("log_client_connect_error").arg(m_serverSocket->errorString()));
    // 连接失败时修正状态
    if (!m_serverSocketConnected) {
        m_tcpStatus = TcpLinkStatus::Error;
        m_running = false;
        emit logMessage(LTR("log_client_connect_failed"));
        emit stateChanged();
    }
    Q_UNUSED(err);
}

// ============================================================
// 双向转发
// ============================================================
void TcpProxyCore::forwardTcpToZdds(const QByteArray &data, const QString &peerInfo)
{
    if (data.isEmpty()) return;
    ZDDSManager::getInstance()->publish(
        m_cfg.zddsSendDomain.toUtf8().constData(),
        m_cfg.zddsSendTopic.toUtf8().constData(),
        data);
    m_zddsTxBytes += data.size();
    m_lastTcpToZddsTime = QDateTime::currentDateTime();
    emit logMessage(LTR("log_zdds_tx").arg(m_cfg.zddsSendDomain).arg(m_cfg.zddsSendTopic)
                        .arg(data.size()).arg(peerInfo));
    emit stateChanged();
}

void TcpProxyCore::forwardZddsToTcp(const char *data, size_t len)
{
    if (data == nullptr || len == 0) return;
    // ZDDS回调在其内部线程执行，不能直接操作QTcpSocket
    // 拷贝数据后通过 QMetaObject::invokeMethod 切换到主线程执行
    QByteArray bytes(data, (int)len);
    QMetaObject::invokeMethod(this, "doForwardZddsToTcp",
                              Qt::QueuedConnection,
                              Q_ARG(QByteArray, bytes));
}

void TcpProxyCore::doForwardZddsToTcp(const QByteArray &bytes)
{
    // 此方法在主线程（Qt事件循环）中执行
    size_t len = (size_t)bytes.size();
    m_zddsRxBytes += len;
    m_lastZddsToTcpTime = QDateTime::currentDateTime();
    emit logMessage(LTR("log_zdds_rx").arg(m_cfg.zddsRecvDomain).arg(m_cfg.zddsRecvTopic)
                        .arg(len).arg(toHex(bytes)));

    if (m_cfg.mode == ProxyMode::ProxyServer) {
        // 代理服务端：ZDDS数据 -> 转发给所有真实客户端
        if (m_clientSockets.isEmpty()) {
            emit logMessage(LTR("log_warn_no_clients"));
        } else {
            for (auto *sock : m_clientSockets) {
                qint64 written = sock->write(bytes);
                if (written > 0) {
                    m_tcpTxBytes += written;
                    QString info = sock->peerAddress().toString() + ":" + QString::number(sock->peerPort());
                    emit logMessage(LTR("log_tcp_tx_client").arg(info).arg(written));
                }
            }
        }
    } else {
        // 代理客户端：ZDDS数据 -> 转发给真实服务端
        if (m_serverSocket && m_serverSocket->isOpen()) {
            qint64 written = m_serverSocket->write(bytes);
            if (written > 0) {
                m_tcpTxBytes += written;
                QString info = m_serverSocket->peerAddress().toString() + ":" + QString::number(m_serverSocket->peerPort());
                emit logMessage(LTR("log_tcp_tx_server").arg(info).arg(written));
            }
        } else {
            emit logMessage(LTR("log_warn_no_server"));
        }
    }
    emit stateChanged();
}
