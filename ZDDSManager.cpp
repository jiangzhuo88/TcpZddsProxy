#include "ZDDSManager.h"

ZDDSManager* ZDDSManager::m_instance = nullptr;

ZDDSManager::ZDDSManager(QObject *parent)
    : QObject(parent)
    , m_zddsInterface(nullptr)
{
}

ZDDSManager::~ZDDSManager()
{
    shutdown();
}

ZDDSManager* ZDDSManager::getInstance()
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    if (m_instance == nullptr) {
        m_instance = new ZDDSManager();
    }
    return m_instance;
}

ZDDSInterface* ZDDSManager::getInterfaceInstance()
{
    if (m_zddsInterface == nullptr) {
        m_zddsInterface = ZDDSInterface::createZDDSInterface();
    }
    return m_zddsInterface;
}

void ZDDSManager::initialize()
{
    if (m_zddsInterface == nullptr) {
        m_zddsInterface = getInterfaceInstance();
    }
    if (!m_zddsStarted) {
        m_zddsInterface->regRecvCallbackFunc(staticOnRecvData, this);
        m_zddsInterface->regZDDSStartSuccessNotify(staticOnStartSuccess, this);
        bool ok = m_zddsInterface->startZDDS();
        m_zddsStarted = true;
        if (ok) {
            emit logMessage(QString("[ZDDS] 启动成功"));
        } else {
            emit logMessage(QString("[ZDDS] 启动失败"));
        }
    }
}

void ZDDSManager::shutdown()
{
    if (m_zddsInterface) {
        ZDDSInterface::releaseZDDSInterface(m_zddsInterface);
        m_zddsInterface = nullptr;
        emit logMessage(QString("[ZDDS] 已释放"));
    }
    m_callbackMap.clear();
}

void ZDDSManager::subscribe(const char* domainName, const char* topicName, RecvCallback callback)
{
    if (m_zddsInterface == nullptr) return;
    m_zddsInterface->subMessage(domainName, topicName);
    {
        QMutexLocker locker(&m_callbackMutex);
        m_callbackMap[domainName][topicName].push_back(callback);
    }
    emit logMessage(QString("[ZDDS] 订阅 %1/%2").arg(domainName).arg(topicName));
}

void ZDDSManager::unsubscribe(const char* domainName, const char* topicName)
{
    if (m_zddsInterface == nullptr) return;
    m_zddsInterface->unSubMessage(domainName, topicName);
    {
        QMutexLocker locker(&m_callbackMutex);
        m_callbackMap[domainName].erase(topicName);
    }
    emit logMessage(QString("[ZDDS] 取消订阅 %1/%2").arg(domainName).arg(topicName));
}

void ZDDSManager::publish(const char* domainName, const char* topicName, const char* data, size_t len)
{
    if (m_zddsInterface == nullptr) return;
    m_zddsInterface->asySendMessage(domainName, topicName, data, len);
}

void ZDDSManager::publish(const char* domainName, const char* topicName, const QByteArray &data)
{
    publish(domainName, topicName, data.constData(), (size_t)data.size());
}

void ZDDSManager::ZDDS_CALLBACK staticOnRecvData(const char* domainName, const char* topicName,
                                                  const char* data, size_t len, void* context)
{
    ZDDSManager* self = static_cast<ZDDSManager*>(context);
    if (!self || !data) return;
    // 拷贝回调列表到局部变量，缩小锁的临界区
    std::list<RecvCallback> callbacks;
    {
        QMutexLocker locker(&self->m_callbackMutex);
        auto itDomain = self->m_callbackMap.find(domainName);
        if (itDomain == self->m_callbackMap.end()) return;
        auto itTopic = itDomain->second.find(topicName);
        if (itTopic == itDomain->second.end()) return;
        callbacks = itTopic->second;
    }
    for (auto &cb : callbacks) {
        cb(data, len);
    }
}

void ZDDSManager::ZDDS_CALLBACK staticOnStartSuccess(void* userContext)
{
    ZDDSManager* self = static_cast<ZDDSManager*>(userContext);
    if (self) {
        emit self->zddsStarted();
        emit self->logMessage(QString("[ZDDS] 启动成功通知回调"));
    }
}
