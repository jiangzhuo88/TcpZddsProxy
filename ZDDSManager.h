#ifndef ZDDSMANAGER_H
#define ZDDSMANAGER_H

#include <QObject>
#include <QMutex>
#include <functional>
#include <map>
#include <list>
#include <string>
#include <stddef.h>
#include "zdds.h"

class ZDDSManager : public QObject
{
    Q_OBJECT
public:
    enum class ZddsStatus {
        NotStarted   = 0,   // 未启动
        Starting     = 1,   // 启动中
        Started      = 2,   // 已启动
        Failed       = 3    // 启动失败
    };
    Q_ENUM(ZddsStatus)

    typedef std::function<void(const char*, size_t)> RecvCallback;

    static ZDDSManager* getInstance();
    ZDDSInterface* getInterfaceInstance();

    void initialize();
    void shutdown();

    // 订阅主题并注册回调
    void subscribe(const char* domainName, const char* topicName, RecvCallback callback);

    // 取消订阅
    void unsubscribe(const char* domainName, const char* topicName);

    // 发布消息
    void publish(const char* domainName, const char* topicName, const char* data, size_t len);
    void publish(const char* domainName, const char* topicName, const QByteArray &data);

    // 状态查询
    ZddsStatus status() const { return m_zddsStatus; }
    bool isStarted() const { return m_zddsStatus == ZddsStatus::Started; }
    int subscribedCount() const;

signals:
    void logMessage(const QString &msg);
    void zddsStarted();
    void statusChanged();

private:
    explicit ZDDSManager(QObject *parent = nullptr);
    ~ZDDSManager();
    static ZDDSManager* m_instance;

    ZDDSInterface* m_zddsInterface = nullptr;
    std::map<std::string, std::map<std::string, std::list<RecvCallback>>> m_callbackMap;
    QMutex m_callbackMutex;   // 保护 m_callbackMap 的跨线程读写
    bool m_zddsStarted = false;  // 避免重复 startZDDS
    ZddsStatus m_zddsStatus = ZddsStatus::NotStarted;

    // 静态回调转发
    static void staticOnRecvData(const char* domainName, const char* topicName,
                                               const char* data, size_t len, void* context);
    static void staticOnStartSuccess(void* userContext);
};

#endif // ZDDSMANAGER_H
