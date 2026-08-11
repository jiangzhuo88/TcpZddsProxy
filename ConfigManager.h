#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>
#include <QString>
#include "TcpProxyCore.h"

class ConfigManager : public QObject
{
    Q_OBJECT
public:
    static ConfigManager* getInstance();

    // 获取配置文件路径（默认：可执行文件同目录/config.json）
    QString configFilePath() const;

    // 加载配置；文件不存在或解析失败时返回false，并填入默认值
    bool loadConfig(ProxyConfig &outCfg);

    // 保存配置
    bool saveConfig(const ProxyConfig &cfg);

    // 设置/获取 最近一次加载或保存的配置（方便UI读取默认值）
    ProxyConfig lastConfig() const { return m_lastCfg; }

signals:
    void logMessage(const QString &msg);

private:
    explicit ConfigManager(QObject *parent = nullptr);
    static ConfigManager* m_instance;
    ProxyConfig m_lastCfg;

    QString defaultConfigFilePath() const;
};

#endif // CONFIGMANAGER_H
