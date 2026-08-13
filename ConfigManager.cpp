#include "ConfigManager.h"
#include "LanguageManager.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

ConfigManager* ConfigManager::m_instance = nullptr;

ConfigManager::ConfigManager(QObject *parent)
    : QObject(parent)
{
}

ConfigManager* ConfigManager::getInstance()
{
    if (m_instance == nullptr) {
        m_instance = new ConfigManager();
    }
    return m_instance;
}

QString ConfigManager::defaultConfigFilePath() const
{
    QString appDir = QCoreApplication::applicationDirPath();
    return QDir(appDir).filePath("config.json");
}

QString ConfigManager::configFilePath() const
{
    return defaultConfigFilePath();
}

static ProxyMode intToProxyMode(int v, ProxyMode defVal = ProxyMode::ProxyServer)
{
    if (v == 0) return ProxyMode::ProxyServer;
    if (v == 1) return ProxyMode::ProxyClient;
    return defVal;
}

bool ConfigManager::loadConfig(ProxyConfig &outCfg)
{
    QString path = defaultConfigFilePath();
    QFile file(path);
    if (!file.exists()) {
        // 文件不存在：写入默认值并返回 true（默认配置合法）
        emit logMessage(LTR("log_cfg_not_found").arg(path));
        ProxyConfig def;
        def.mode = ProxyMode::ProxyServer;
        def.tcpHost = "127.0.0.1";
        def.tcpPort = 9000;
        def.zddsSendDomain = "TCPProxySendDomain";
        def.zddsRecvDomain = "TCPProxyRecvDomain";
        def.zddsSendTopic = "TcpToZdds";
        def.zddsRecvTopic = "ZddsToTcp";
        outCfg = def;
        m_lastCfg = def;
        saveConfig(def);
        return true;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit logMessage(LTR("log_cfg_open_fail").arg(path).arg(file.errorString()));
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError) {
        emit logMessage(LTR("log_cfg_parse_fail").arg(err.errorString()).arg(err.offset));
        return false;
    }
    if (!doc.isObject()) {
        emit logMessage(LTR("log_cfg_not_object"));
        return false;
    }
    QJsonObject obj = doc.object();

    ProxyConfig cfg;
    cfg.mode = intToProxyMode(obj.value("mode").toInt(0));
    cfg.tcpHost = obj.value("tcpHost").toString("127.0.0.1").trimmed();
    cfg.tcpPort = obj.value("tcpPort").toInt(9000);
    cfg.zddsSendDomain = obj.value("zddsSendDomain").toString("TCPProxySendDomain").trimmed();
    cfg.zddsRecvDomain = obj.value("zddsRecvDomain").toString("TCPProxyRecvDomain").trimmed();
    cfg.zddsSendTopic = obj.value("zddsSendTopic").toString("TcpToZdds").trimmed();
    cfg.zddsRecvTopic = obj.value("zddsRecvTopic").toString("ZddsToTcp").trimmed();

    // 修正端口
    if (cfg.tcpPort <= 0 || cfg.tcpPort > 65535) cfg.tcpPort = 9000;

    outCfg = cfg;
    m_lastCfg = cfg;
    emit logMessage(LTR("log_cfg_loaded").arg(path));
    return true;
}

bool ConfigManager::saveConfig(const ProxyConfig &cfg)
{
    QJsonObject obj;
    obj.insert("mode", (int)cfg.mode);
    obj.insert("tcpHost", cfg.tcpHost);
    obj.insert("tcpPort", cfg.tcpPort);
    obj.insert("zddsSendDomain", cfg.zddsSendDomain);
    obj.insert("zddsRecvDomain", cfg.zddsRecvDomain);
    obj.insert("zddsSendTopic", cfg.zddsSendTopic);
    obj.insert("zddsRecvTopic", cfg.zddsRecvTopic);

    QJsonDocument doc(obj);
    QString path = defaultConfigFilePath();

    // 确保目录存在
    QString dirPath = QFileInfo(path).absolutePath();
    QDir().mkpath(dirPath);

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        emit logMessage(LTR("log_cfg_write_fail").arg(path).arg(file.errorString()));
        return false;
    }
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    m_lastCfg = cfg;
    emit logMessage(LTR("log_cfg_saved").arg(path));
    return true;
}
