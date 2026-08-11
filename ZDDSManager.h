#ifndef ZDDSMANAGER_H
#define ZDDSMANAGER_H

#include <QObject>
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

signals:
    void logMessage(const QString &msg);
    void zddsStarted();

private:
    explicit ZDDSManager(QObject *parent = nullptr);
    ~ZDDSManager();
    static ZDDSManager* m_instance;

    ZDDSInterface* m_zddsInterface = nullptr;
    std::map<std::string, std::map<std::string, std::list<RecvCallback>>> m_callbackMap;

    // 静态回调转发
    static void ZDDS_CALLBACK staticOnRecvData(const char* domainName, const char* topicName,
                                               const char* data, size_t len, void* context);
    static void ZDDS_CALLBACK staticOnStartSuccess(void* userContext);
};

#endif // ZDDSMANAGER_H
