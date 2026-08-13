#ifndef TCPPROXYCORE_H
#define TCPPROXYCORE_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QByteArray>
#include <QDateTime>

class ZDDSManager;

enum class ProxyMode {
    ProxyServer = 0,   // 代理服务端：监听TCP端口，接收真实客户端连接，通过ZDDS转发
    ProxyClient = 1    // 代理客户端：连接真实服务端，通过ZDDS转发
};

// TCP链路状态
enum class TcpLinkStatus {
    Stopped       = 0,   // 未启动
    Listening     = 1,   // 服务端模式：正在监听
    Connecting    = 2,   // 客户端模式：正在连接
    Connected     = 3,   // 已连接
    Disconnected  = 4,   // 已断开
    Error         = 5    // 错误
};

// 配置结构
struct ProxyConfig {
    ProxyMode mode = ProxyMode::ProxyServer;

    // TCP配置
    QString tcpHost;      // ProxyClient模式：真实服务端地址；ProxyServer模式：监听绑定地址（空=0.0.0.0所有地址）
    int tcpPort = 0;      // ProxyClient模式：真实服务端端口；ProxyServer模式：本地监听端口

    // ZDDS配置
    QString zddsSendDomain;           // ZDDS发送域名称
    QString zddsRecvDomain;           // ZDDS接收域名称
    QString zddsSendTopic;        // ZDDS发送主题（TCP->ZDDS方向）
    QString zddsRecvTopic;        // ZDDS接收主题（ZDDS->TCP方向）

    // 语言设置（0=中文, 1=英文）
    int language = 1;

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

    // === 状态查询 ===
    TcpLinkStatus tcpLinkStatus() const;
    bool isZddsSubscribed() const { return m_zddsSubscribed; }
    int clientCount() const { return m_clientSockets.size(); }
    QString tcpPeerInfo() const;   // 当前TCP连接的对端信息

    // 数据流活跃状态（最近5秒内有数据则为活跃）
    bool isTcpToZddsActive() const;
    bool isZddsToTcpActive() const;

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
    TcpLinkStatus m_tcpStatus = TcpLinkStatus::Stopped;

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

    // 数据流活跃时间戳
    QDateTime m_lastTcpToZddsTime;
    QDateTime m_lastZddsToTcpTime;

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
