#ifndef TCPPROXYCORE_H
#define TCPPROXYCORE_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QByteArray>

class ZDDSManager;

enum class ProxyMode {
    ProxyServer = 0,   // 代理服务端：监听TCP端口，接收真实客户端连接，通过ZDDS转发
    ProxyClient = 1    // 代理客户端：连接真实服务端，通过ZDDS转发
};

// 配置结构
struct ProxyConfig {
    ProxyMode mode = ProxyMode::ProxyServer;

    // TCP配置
    QString tcpHost;      // ProxyClient模式下：真实服务端地址；ProxyServer模式下忽略（监听Any）
    int tcpPort = 0;      // ProxyClient模式下：真实服务端端口；ProxyServer模式下：本地监听端口

    // ZDDS配置
    QString zddsSendDomain;           // ZDDS发送域名称
    QString zddsRecvDomain;           // ZDDS接收域名称
    QString zddsSendTopic;        // ZDDS发送主题（TCP->ZDDS方向）
    QString zddsRecvTopic;        // ZDDS接收主题（ZDDS->TCP方向）

    bool isValid() const {
        if (zddsSendDomain.isEmpty() || zddsRecvDomain.isEmpty() || zddsSendTopic.isEmpty() || zddsRecvTopic.isEmpty())
            return false;
        if (tcpPort <= 0 || tcpPort > 65535) return false;
        if (mode == ProxyMode::ProxyClient && tcpHost.isEmpty()) return false;
        return true;
    }
};

class TcpProxyCore : public QObject
{
    Q_OBJECT
public:
    explicit TcpProxyCore(QObject *parent = nullptr);
    ~TcpProxyCore();

    void setConfig(const ProxyConfig &cfg);
    const ProxyConfig& config() const { return m_cfg; }

    bool start();
    void stop();
    bool isRunning() const;
    bool isConnected() const;  // TCP实际连接是否建立（代理客户端模式有意义）
    ProxyMode currentMode() const { return m_cfg.mode; }

    // 统计
    quint64 tcpRxBytes() const { return m_tcpRxBytes; }
    quint64 tcpTxBytes() const { return m_tcpTxBytes; }
    quint64 zddsRxBytes() const { return m_zddsRxBytes; }
    quint64 zddsTxBytes() const { return m_zddsTxBytes; }

signals:
    void logMessage(const QString &msg);
    void stateChanged();

private slots:
    // === ProxyServer模式相关 ===
    void onNewClientConnection();
    void onClientSocketReadyRead();
    void onClientSocketDisconnected();
    void onClientSocketError(QAbstractSocket::SocketError err);

    // === ProxyClient模式相关 ===
    void onServerSocketConnected();
    void onServerSocketDisconnected();
    void onServerSocketReadyRead();
    void onServerSocketError(QAbstractSocket::SocketError err);

    // 在主线程中实际执行 ZDDS->TCP 转发（由ZDDS回调线程通过invokeMethod触发）
    void doForwardZddsToTcp(const QByteArray &bytes);

private:
    ProxyConfig m_cfg;
    bool m_running = false;

    // === ProxyServer模式 ===
    QTcpServer *m_tcpServer = nullptr;
    QVector<QTcpSocket*> m_clientSockets;   // 真实客户端连接列表

    // === ProxyClient模式 ===
    QTcpSocket *m_serverSocket = nullptr;   // 连接真实服务端的socket
    bool m_serverSocketConnected = false;   // 真实服务端是否已连接成功

    // 统计
    quint64 m_tcpRxBytes = 0;
    quint64 m_tcpTxBytes = 0;
    quint64 m_zddsRxBytes = 0;
    quint64 m_zddsTxBytes = 0;

    // ZDDS回调注册ID
    bool m_zddsSubscribed = false;

    // === 通用方法 ===
    // TCP收到数据 -> 发到ZDDS
    void forwardTcpToZdds(const QByteArray &data, const QString &peerInfo);
    // ZDDS收到数据 -> 发到TCP（ZDDS回调线程调用，仅做线程切换）
    void forwardZddsToTcp(const char* data, size_t len);

    void resetStats();

    // 工具
    QString toHex(const QByteArray &data, int maxShow = 64);
};

#endif // TCPPROXYCORE_H
