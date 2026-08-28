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
//主题域结构定义------------------------------
typedef struct _PLT_DMHEAD
{
    uint32_t		unMsgCode;				//数据指令编码	unMsgCode		无符号4字节整数	系统统一规划定义。
    uint32_t		unMsgLen;				//数据包总长度	unMsgLen		无符号4字节整数	报文头和数据内容的总长度，取值范围[20, 8192]。
    uint32_t		unSrcID;				//源地址		unSrcID			无符号4字节整数	报文发送方唯一标识ID。软件地址编码=计算机ID*65536+软件配置项ID*256+软件编号
    uint32_t		unDestID;				//目的地址	unDestID		无符号4字节整数	软件地址编码=计算机ID*65536+软件配置项ID*256+软件编号
    //1、0：即计算机ID、软件配置项ID、软件编号均为0无特定接收方
    //2、计算机ID不等于0，软件配置项ID、软件编号均为0：特定席位上软件接收
    //3、计算机ID不等于0，软件配置项ID不等于0，软件编号为0：特定席位上特定配置项软件接收；
    //4、计算机ID不等于0、软件配置项ID不等于0、软件编号不等于0：特定席位上特定配置项的特定软件进程接收；

    uint32_t		unMsgOrder;				//数据流水编号	unMsgOrder		无符号4字节整数	发起方对发送数据的流水编号。

    _PLT_DMHEAD()
    {
        memset(this, 0,sizeof(*this));
        unMsgCode = 0x3008;
    }
}stDMHead;
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
    void publish(const char* domainName, const char* topicName,quint32 unMsgCode,const char* data, size_t len);
    void publish(const char* domainName, const char* topicName,quint32 unMsgCode, const QByteArray &data);

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
