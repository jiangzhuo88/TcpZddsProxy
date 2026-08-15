<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="en_US">
<context>
    <name>ConfigManager</name>
    <message>
        <location filename="../ConfigManager.cpp" line="48"/>
        <source>[配置] 配置文件不存在，将使用默认值: %1</source>
        <translation>[Config] Config file not found, using defaults: %1</translation>
    </message>
    <message>
        <location filename="../ConfigManager.cpp" line="67"/>
        <source>[配置] 打开失败: %1 (%2)</source>
        <translation>[Config] Open failed: %1 (%2)</translation>
    </message>
    <message>
        <location filename="../ConfigManager.cpp" line="77"/>
        <source>[配置] JSON解析失败: %1 (offset:%2)</source>
        <translation>[Config] JSON parse error: %1 (offset:%2)</translation>
    </message>
    <message>
        <location filename="../ConfigManager.cpp" line="81"/>
        <source>[配置] JSON顶层必须是对象</source>
        <translation>[Config] JSON top-level must be an object</translation>
    </message>
    <message>
        <location filename="../ConfigManager.cpp" line="104"/>
        <source>[配置] 已加载配置文件: %1</source>
        <translation>[Config] Loaded config file: %1</translation>
    </message>
    <message>
        <location filename="../ConfigManager.cpp" line="131"/>
        <source>[配置] 写入失败: %1 (%2)</source>
        <translation>[Config] Write failed: %1 (%2)</translation>
    </message>
    <message>
        <location filename="../ConfigManager.cpp" line="137"/>
        <source>[配置] 已保存配置文件: %1</source>
        <translation>[Config] Saved config file: %1</translation>
    </message>
</context>
<context>
    <name>MainWindow</name>
    <message>
        <location filename="../MainWindow.cpp" line="343"/>
        <source>提示</source>
        <translation>Information</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="343"/>
        <source>请先停止代理后再保存配置</source>
        <translation>Please stop the proxy before saving configuration</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="348"/>
        <location filename="../MainWindow.cpp" line="436"/>
        <location filename="../MainWindow.cpp" line="560"/>
        <source>错误</source>
        <translation>Error</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="349"/>
        <location filename="../MainWindow.cpp" line="437"/>
        <source>请检查配置：
1. TCP端口必须在 1-65535 之间
2. 代理客户端模式必须填写真实服务端地址
3. ZDDS域名称、发送主题、接收主题均不能为空</source>
        <translation>Please check configuration:
1. TCP port must be 1-65535
2. Proxy Client mode requires real server address
3. ZDDS domain, send topic, recv topic cannot be empty</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="357"/>
        <source>保存成功</source>
        <translation>Save Success</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="358"/>
        <source>配置已保存到：
%1</source>
        <translation>Configuration saved to:
%1</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="360"/>
        <source>保存失败</source>
        <translation>Save Failed</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="360"/>
        <source>请检查日志获取详细错误信息</source>
        <translation>Please check log for error details</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="373"/>
        <source>真实服务端地址:</source>
        <translation>Real Server Address:</translation>
    </message>
    <message>
        <source>(服务端模式监听所有地址)</source>
        <translation type="vanished">(Server mode listens on all addresses)</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="419"/>
        <source>停止代理</source>
        <translation>Stop Proxy</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="423"/>
        <source>启动代理</source>
        <translation>Start Proxy</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="457"/>
        <source>代理服务端</source>
        <translation>Proxy Server</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="459"/>
        <source>代理客户端</source>
        <translation>Proxy Client</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="463"/>
        <source>已停止</source>
        <translation>Stopped</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="466"/>
        <source>运行中</source>
        <translation>Running</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="474"/>
        <source>连接中...</source>
        <translation>Connecting...</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="470"/>
        <source>重连中...</source>
        <translation>Reconnecting...</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="494"/>
        <location filename="../MainWindow.cpp" line="532"/>
        <source>未启动</source>
        <translation>Not Started</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="499"/>
        <source>启动中...</source>
        <translation>Starting...</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="504"/>
        <source>已接入</source>
        <translation>Connected</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="509"/>
        <source>启动失败</source>
        <translation>Start Failed</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="517"/>
        <source>已订阅</source>
        <translation>Subscribed</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="522"/>
        <source>未订阅</source>
        <translation>Not Subscribed</translation>
    </message>
    <message>
        <source>监听中 (端口 %1)</source>
        <translation type="vanished">Listening (port %1)</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="545"/>
        <source>连接中 (%1:%2)</source>
        <translation>Connecting (%1:%2)</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="550"/>
        <source>已连接 (%1:%2)</source>
        <translation>Connected (%1:%2)</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="555"/>
        <source>已断开</source>
        <translation>Disconnected</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="570"/>
        <source>%1 个客户端</source>
        <translation>%1 client(s)</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="574"/>
        <source>无客户端</source>
        <translation>No Clients</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="586"/>
        <location filename="../MainWindow.cpp" line="597"/>
        <source>活跃 (%1)</source>
        <translation>Active (%1)</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="590"/>
        <location filename="../MainWindow.cpp" line="601"/>
        <source>空闲</source>
        <translation>Idle</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="632"/>
        <source>TCP-ZDDS 双向代理软件</source>
        <translation>TCP-ZDDS Bidirectional Proxy</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="635"/>
        <source>配置</source>
        <translation>Configuration</translation>
    </message>
    <message>
        <source>语言:</source>
        <translation type="vanished">Language:</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="186"/>
        <source>自动重连</source>
        <translation>Auto Reconnected</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="188"/>
        <source>重连间隔(秒):</source>
        <translation>Reconnect Range:</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="376"/>
        <source>监听地址:</source>
        <translation>Listen Address:</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="539"/>
        <source>监听中 (%1:%2)</source>
        <translation>Listening (%1:%2)</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="636"/>
        <source>代理模式:</source>
        <translation>Proxy Mode:</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="637"/>
        <source>TCP端口:</source>
        <translation>TCP Port:</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="638"/>
        <source>ZDDS发送域名称:</source>
        <translation>ZDDS Send Domain:</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="639"/>
        <source>ZDDS接收域名称:</source>
        <translation>ZDDS Recv Domain:</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="640"/>
        <source>ZDDS发送主题(TCP→ZDDS):</source>
        <translation>ZDDS Send Topic (TCP-&gt;ZDDS):</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="641"/>
        <source>ZDDS接收主题(ZDDS→TCP):</source>
        <translation>ZDDS Recv Topic (ZDDS-&gt;TCP):</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="642"/>
        <source>例如: 192.168.1.100 或 127.0.0.1</source>
        <translation>e.g. 192.168.1.100 or 127.0.0.1</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="643"/>
        <location filename="../MainWindow.cpp" line="644"/>
        <source>例如: DomainTCPProxy</source>
        <translation>e.g. DomainTCPProxy</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="645"/>
        <source>例如: TopicTcpToZdds</source>
        <translation>e.g. TopicTcpToZdds</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="646"/>
        <source>例如: TopicZddsToTcp</source>
        <translation>e.g. TopicZddsToTcp</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="647"/>
        <source>保存配置</source>
        <translation>Save Config</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="648"/>
        <source>清空日志</source>
        <translation>Clear Log</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="654"/>
        <source>代理服务端 (监听端口，接收真实客户端)</source>
        <translation>Proxy Server (Listen port, accept real client)</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="655"/>
        <source>代理客户端 (连接真实服务端)</source>
        <translation>Proxy Client (Connect to real server)</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="662"/>
        <source>连接状态总览</source>
        <translation>Connection Status Overview</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="665"/>
        <source>ZDDS 状态</source>
        <translation>ZDDS Status</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="666"/>
        <source>ZDDS 订阅</source>
        <translation>ZDDS Subscription</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="667"/>
        <source>TCP 链路</source>
        <translation>TCP Link</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="668"/>
        <source>已连接客户端</source>
        <translation>Connected Clients</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="669"/>
        <source>数据流 TCP→ZDDS</source>
        <translation>Data Flow TCP-&gt;ZDDS</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="670"/>
        <source>数据流 ZDDS→TCP</source>
        <translation>Data Flow ZDDS-&gt;TCP</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="673"/>
        <source>当前模式</source>
        <translation>Mode</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="674"/>
        <source>运行状态</source>
        <translation>Run Status</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="675"/>
        <source>TCP接收字节:</source>
        <translation>TCP RX Bytes:</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="676"/>
        <source>TCP发送字节:</source>
        <translation>TCP TX Bytes:</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="677"/>
        <source>ZDDS接收字节:</source>
        <translation>ZDDS RX Bytes:</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="678"/>
        <source>ZDDS发送字节:</source>
        <translation>ZDDS TX Bytes:</translation>
    </message>
    <message>
        <location filename="../MainWindow.cpp" line="681"/>
        <source>日志</source>
        <translation>Log</translation>
    </message>
</context>
<context>
    <name>TcpProxyCore</name>
    <message>
        <location filename="../TcpProxyCore.cpp" line="23"/>
        <source>[警告] 运行中无法修改配置，请先停止</source>
        <translation>[Warning] Cannot modify config while running, stop first</translation>
    </message>
    <message>
        <location filename="../TcpProxyCore.cpp" line="45"/>
        <source> ...(共%1字节)</source>
        <translation> ...(total %1 bytes)</translation>
    </message>
    <message>
        <location filename="../TcpProxyCore.cpp" line="98"/>
        <source>[警告] 代理已在运行中</source>
        <translation>[Warning] Proxy is already running</translation>
    </message>
    <message>
        <location filename="../TcpProxyCore.cpp" line="102"/>
        <source>[错误] 配置无效，请检查参数</source>
        <translation>[Error] Invalid configuration, please check parameters</translation>
    </message>
    <message>
        <location filename="../TcpProxyCore.cpp" line="130"/>
        <source>[错误] 代理服务端监听端口 %1 失败: %2</source>
        <translation>[Error] Proxy server listen on port %1 failed: %2</translation>
    </message>
    <message>
        <source>[代理服务端] 已启动，监听端口 %1</source>
        <translation type="vanished">[Proxy Server] Started, listening on port %1</translation>
    </message>
    <message>
        <location filename="../TcpProxyCore.cpp" line="139"/>
        <source>[代理服务端] 已启动，监听 %1:%2</source>
        <translation>[Proxy Server] Started, listening on %1:%2</translation>
    </message>
    <message>
        <location filename="../TcpProxyCore.cpp" line="140"/>
        <location filename="../TcpProxyCore.cpp" line="158"/>
        <source>[配置] TCP-&gt;ZDDS: %1/%3, ZDDS-&gt;TCP: %2/%4</source>
        <translation>[Config] TCP-&gt;ZDDS: %1/%3, ZDDS-&gt;TCP: %2/%4</translation>
    </message>
    <message>
        <location filename="../TcpProxyCore.cpp" line="153"/>
        <source>[代理客户端] 正在连接真实服务端 %1:%2 ...</source>
        <translation>[Proxy Client] Connecting to real server %1:%2 ...</translation>
    </message>
    <message>
        <location filename="../TcpProxyCore.cpp" line="190"/>
        <source>[断开] 客户端 %1</source>
        <translation>[Disconnect] Client %1</translation>
    </message>
    <message>
        <location filename="../TcpProxyCore.cpp" line="196"/>
        <source>[代理服务端] 已停止</source>
        <translation>[Proxy Server] Stopped</translation>
    </message>
    <message>
        <location filename="../TcpProxyCore.cpp" line="205"/>
        <source>[代理客户端] 已停止</source>
        <translation>[Proxy Client] Stopped</translation>
    </message>
    <message>
        <location filename="../TcpProxyCore.cpp" line="226"/>
        <source>[连接] 真实客户端已接入: %1 (当前客户端数:%2)</source>
        <translation>[Connect] Real client connected: %1 (clients:%2)</translation>
    </message>
    <message>
        <location filename="../TcpProxyCore.cpp" line="241"/>
        <source>[TCP收&lt;-客户端 %1] %2字节: %3</source>
        <translation>[TCP RX&lt;-Client %1] %2 bytes: %3</translation>
    </message>
    <message>
        <location filename="../TcpProxyCore.cpp" line="252"/>
        <source>[断开] 真实客户端 %1 已断开 (剩余客户端数:%2)</source>
        <translation>[Disconnect] Real client %1 disconnected (remaining:%2)</translation>
    </message>
    <message>
        <location filename="../TcpProxyCore.cpp" line="265"/>
        <source>[错误] 客户端 %1 错误: %2</source>
        <translation>[Error] Client %1 error: %2</translation>
    </message>
    <message>
        <location filename="../TcpProxyCore.cpp" line="276"/>
        <source>[代理客户端] 已连接真实服务端 %1:%2</source>
        <translation>[Proxy Client] Connected to real server %1:%2</translation>
    </message>
    <message>
        <location filename="../TcpProxyCore.cpp" line="285"/>
        <source>[代理客户端] 与真实服务端断开连接</source>
        <translation>[Proxy Client] Disconnected from real server</translation>
    </message>
    <message>
        <location filename="../TcpProxyCore.cpp" line="291"/>
        <source>[代理客户端] 将在 %1 秒后尝试自动重连...</source>
        <translation>[Proxy Client] Try auto reconnnect after %1 seconed ...</translation>
    </message>
    <message>
        <location filename="../TcpProxyCore.cpp" line="302"/>
        <source>[TCP收&lt;-服务端 %1] %2字节: %3</source>
        <translation>[TCP RX&lt;-Server %1] %2 bytes: %3</translation>
    </message>
    <message>
        <location filename="../TcpProxyCore.cpp" line="309"/>
        <source>[错误] 服务端连接错误: %1</source>
        <translation>[Error] Server connection error: %1</translation>
    </message>
    <message>
        <location filename="../TcpProxyCore.cpp" line="315"/>
        <source>[代理客户端] 连接失败，将在 %1 秒后重连...</source>
        <translation>[Proxy Client] Connect fiald,Try auto reconnnect after %1 seconed ...</translation>
    </message>
    <message>
        <location filename="../TcpProxyCore.cpp" line="321"/>
        <source>[代理客户端] 连接真实服务端失败，代理已停止</source>
        <translation>[Proxy Client] Failed to connect to real server, proxy stopped</translation>
    </message>
    <message>
        <location filename="../TcpProxyCore.cpp" line="333"/>
        <source>[代理客户端] 正在尝试重连 %1:%2 ...</source>
        <translation> [Proxy Client] Try reconnnect  %1:%2 ...</translation>
    </message>
    <message>
        <location filename="../TcpProxyCore.cpp" line="348"/>
        <source>[ZDDS发-&gt;] %1/%2 发送 %3字节 (来源:%4)</source>
        <translation>[ZDDS TX-&gt;] %1/%2 sent %3 bytes (from:%4)</translation>
    </message>
    <message>
        <location filename="../TcpProxyCore.cpp" line="371"/>
        <source>[ZDDS收&lt;-] %1/%2 收到 %3字节: %4</source>
        <translation>[ZDDS RX&lt;-] %1/%2 received %3 bytes: %4</translation>
    </message>
    <message>
        <location filename="../TcpProxyCore.cpp" line="378"/>
        <source>[警告] 无真实客户端连接，ZDDS数据未转发到TCP</source>
        <translation>[Warning] No real client connected, ZDDS data not forwarded to TCP</translation>
    </message>
    <message>
        <location filename="../TcpProxyCore.cpp" line="385"/>
        <source>[TCP发-&gt;客户端 %1] %2字节</source>
        <translation>[TCP TX-&gt;Client %1] %2 bytes</translation>
    </message>
    <message>
        <location filename="../TcpProxyCore.cpp" line="396"/>
        <source>[TCP发-&gt;服务端 %1] %2字节</source>
        <translation>[TCP TX-&gt;Server %1] %2 bytes</translation>
    </message>
    <message>
        <location filename="../TcpProxyCore.cpp" line="399"/>
        <source>[警告] 真实服务端未连接，ZDDS数据未转发到TCP</source>
        <translation>[Warning] Real server not connected, ZDDS data not forwarded to TCP</translation>
    </message>
</context>
<context>
    <name>ZDDSManager</name>
    <message>
        <location filename="../ZDDSManager.cpp" line="49"/>
        <source>[ZDDS] 启动成功</source>
        <translation>[ZDDS] Started successfully</translation>
    </message>
    <message>
        <location filename="../ZDDSManager.cpp" line="53"/>
        <source>[ZDDS] 启动失败</source>
        <translation>[ZDDS] Failed to start</translation>
    </message>
    <message>
        <location filename="../ZDDSManager.cpp" line="66"/>
        <source>[ZDDS] 已释放</source>
        <translation>[ZDDS] Released</translation>
    </message>
    <message>
        <location filename="../ZDDSManager.cpp" line="79"/>
        <source>[ZDDS] 订阅 %1/%2</source>
        <translation>[ZDDS] Subscribe %1/%2</translation>
    </message>
    <message>
        <location filename="../ZDDSManager.cpp" line="90"/>
        <source>[ZDDS] 取消订阅 %1/%2</source>
        <translation>[ZDDS] Unsubscribe %1/%2</translation>
    </message>
    <message>
        <location filename="../ZDDSManager.cpp" line="141"/>
        <source>[ZDDS] 启动成功通知回调</source>
        <translation>[ZDDS] Start success notification callback</translation>
    </message>
</context>
</TS>
