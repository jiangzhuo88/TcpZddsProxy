#include "MainWindow.h"
#include "ZDDSManager.h"
#include "ConfigManager.h"
#include <QDateTime>
#include <QLabel>
#include <QMessageBox>
#include <QCloseEvent>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_proxyCore(new TcpProxyCore(this))
    , m_statsTimer(new QTimer(this))
{
    setupUi();
    loadConfigFromFile();

    connect(m_proxyCore, &TcpProxyCore::logMessage, this, &MainWindow::appendLog);
    connect(m_proxyCore, &TcpProxyCore::stateChanged, this, &MainWindow::onStateChanged);
    connect(ZDDSManager::getInstance(), &ZDDSManager::logMessage, this, &MainWindow::appendLog);
    connect(ConfigManager::getInstance(), &ConfigManager::logMessage, this, &MainWindow::appendLog);

    m_statsTimer->setInterval(1000);
    connect(m_statsTimer, &QTimer::timeout, this, &MainWindow::onUpdateStats);
    m_statsTimer->start();

    onStateChanged();
    onUpdateStats();
}

MainWindow::~MainWindow()
{
    // 关闭窗口前先保存当前配置
    if (!m_proxyCore->isRunning()) {
        ProxyConfig cfg = collectConfigFromUi();
        if (cfg.isValid()) {
            ConfigManager::getInstance()->saveConfig(cfg);
        }
    }
    if (m_proxyCore->isRunning()) {
        m_proxyCore->stop();
    }
}

void MainWindow::setupUi()
{
    setWindowTitle("TCP-ZDDS 双向代理软件");
    resize(900, 680);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(12, 12, 12, 12);

    // ============ 顶部：配置区 ============
    QGroupBox *configBox = new QGroupBox("配置", centralWidget);
    QGridLayout *cfgLayout = new QGridLayout(configBox);
    cfgLayout->setSpacing(8);
    cfgLayout->setContentsMargins(12, 16, 12, 12);

    int row = 0;
    // 代理模式
    cfgLayout->addWidget(new QLabel("代理模式:"), row, 0);
    m_modeCombo = new QComboBox(configBox);
    m_modeCombo->addItem("代理服务端 (监听端口，接收真实客户端)", (int)ProxyMode::ProxyServer);
    m_modeCombo->addItem("代理客户端 (连接真实服务端)", (int)ProxyMode::ProxyClient);
    cfgLayout->addWidget(m_modeCombo, row, 1, 1, 3);
    connect(m_modeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onProxyModeChanged);
    row++;

    // TCP主机地址（代理客户端模式才需要）
    m_tcpHostLabel = new QLabel("真实服务端地址:", configBox);
    cfgLayout->addWidget(m_tcpHostLabel, row, 0);
    m_tcpHostEdit = new QLineEdit(configBox);
    m_tcpHostEdit->setPlaceholderText("例如: 192.168.1.100 或 127.0.0.1");
    cfgLayout->addWidget(m_tcpHostEdit, row, 1);
    row++;

    // TCP端口
    cfgLayout->addWidget(new QLabel("TCP端口:"), row, 0);
    m_tcpPortSpin = new QSpinBox(configBox);
    m_tcpPortSpin->setRange(1, 65535);
    m_tcpPortSpin->setValue(9000);
    cfgLayout->addWidget(m_tcpPortSpin, row, 1);
    row++;

    // SendZDDS域
    cfgLayout->addWidget(new QLabel("ZDDS发送域名称:"), row, 0);
    m_zddsSendDomainEdit = new QLineEdit(configBox);
    m_zddsSendDomainEdit->setPlaceholderText("例如: DomainTCPProxy");
    cfgLayout->addWidget(m_zddsSendDomainEdit, row, 1);


    // ZDDS发送主题
    cfgLayout->addWidget(new QLabel("ZDDS发送主题(TCP→ZDDS):"), row, 2);
    m_zddsSendTopicEdit = new QLineEdit(configBox);
    m_zddsSendTopicEdit->setPlaceholderText("例如: TopicTcpToZdds");
    cfgLayout->addWidget(m_zddsSendTopicEdit, row, 3);
    row++;

    // ZDDS域
    cfgLayout->addWidget(new QLabel("ZDDS接收域名称:"), row, 0);
    m_zddsRecvDomainEdit = new QLineEdit(configBox);
    m_zddsRecvDomainEdit->setPlaceholderText("例如: DomainTCPProxy");
    cfgLayout->addWidget(m_zddsRecvDomainEdit, row, 1);

    // ZDDS接收主题
    cfgLayout->addWidget(new QLabel("ZDDS接收主题(ZDDS→TCP):"), row, 2);
    m_zddsRecvTopicEdit = new QLineEdit(configBox);
    m_zddsRecvTopicEdit->setPlaceholderText("例如: TopicZddsToTcp");
    cfgLayout->addWidget(m_zddsRecvTopicEdit, row, 3);
    row++;

    // 自动重连配置（仅代理客户端模式有效）
    m_autoReconnectCheck = new QCheckBox("自动重连", configBox);
    cfgLayout->addWidget(m_autoReconnectCheck, row, 0);
    cfgLayout->addWidget(new QLabel("重连间隔(秒):"), row, 2);
    m_reconnectIntervalSpin = new QSpinBox(configBox);
    m_reconnectIntervalSpin->setRange(1, 300);
    m_reconnectIntervalSpin->setValue(5);
    cfgLayout->addWidget(m_reconnectIntervalSpin, row, 3);
    connect(m_autoReconnectCheck, &QCheckBox::toggled, m_reconnectIntervalSpin, &QSpinBox::setEnabled);
    row++;

    // 控制按钮
    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *saveBtn = new QPushButton("保存配置", configBox);
    saveBtn->setMinimumHeight(34);
    saveBtn->setStyleSheet("QPushButton{padding:6px 18px;}");
    connect(saveBtn, &QPushButton::clicked, this, &MainWindow::onSaveConfig);
    btnLayout->addWidget(saveBtn);
    btnLayout->addStretch();
    m_startBtn = new QPushButton("启动代理", configBox);
    m_startBtn->setMinimumHeight(34);
    m_startBtn->setStyleSheet("QPushButton{font-weight:bold;padding:6px 24px;}");
    connect(m_startBtn, &QPushButton::clicked, this, &MainWindow::onToggleStart);
    btnLayout->addWidget(m_startBtn);
    cfgLayout->addLayout(btnLayout, row, 0, 1, 4);

    mainLayout->addWidget(configBox);

    // ============ 中部：状态区 ============
    QGroupBox *statusBox = new QGroupBox("当前状态", centralWidget);
    QGridLayout *stLayout = new QGridLayout(statusBox);
    stLayout->setSpacing(8);
    stLayout->setContentsMargins(12, 16, 12, 12);

    stLayout->addWidget(new QLabel("当前模式:"), 0, 0);
    m_modeValueLabel = new QLabel("-", statusBox);
    m_modeValueLabel->setStyleSheet("font-weight:bold;");
    stLayout->addWidget(m_modeValueLabel, 0, 1);

    stLayout->addWidget(new QLabel("运行状态:"), 0, 2);
    m_statusValueLabel = new QLabel("未启动", statusBox);
    m_statusValueLabel->setStyleSheet("font-weight:bold;color:gray;");
    stLayout->addWidget(m_statusValueLabel, 0, 3);

    stLayout->addWidget(new QLabel("TCP接收字节:"), 1, 0);
    m_tcpRxValueLabel = new QLabel("0", statusBox);
    stLayout->addWidget(m_tcpRxValueLabel, 1, 1);

    stLayout->addWidget(new QLabel("TCP发送字节:"), 1, 2);
    m_tcpTxValueLabel = new QLabel("0", statusBox);
    stLayout->addWidget(m_tcpTxValueLabel, 1, 3);

    stLayout->addWidget(new QLabel("ZDDS接收字节:"), 2, 0);
    m_zddsRxValueLabel = new QLabel("0", statusBox);
    stLayout->addWidget(m_zddsRxValueLabel, 2, 1);

    stLayout->addWidget(new QLabel("ZDDS发送字节:"), 2, 2);
    m_zddsTxValueLabel = new QLabel("0", statusBox);
    stLayout->addWidget(m_zddsTxValueLabel, 2, 3);

    mainLayout->addWidget(statusBox);

    // ============ 底部：日志区 ============
    QGroupBox *logBox = new QGroupBox("日志", centralWidget);
    QVBoxLayout *logLayout = new QVBoxLayout(logBox);
    logLayout->setContentsMargins(12, 16, 12, 12);

    QHBoxLayout *logHeader = new QHBoxLayout();
    logHeader->addStretch();
    m_clearLogBtn = new QPushButton("清空日志", logBox);
    connect(m_clearLogBtn, &QPushButton::clicked, this, &MainWindow::onClearLog);
    logHeader->addWidget(m_clearLogBtn);
    logLayout->addLayout(logHeader);

    m_logEdit = new QTextEdit(logBox);
    m_logEdit->setReadOnly(true);
    m_logEdit->setStyleSheet("QTextEdit{font-family:'Consolas','Courier New',monospace;font-size:12px;}");
    logLayout->addWidget(m_logEdit);

    mainLayout->addWidget(logBox, 1);

    // 初始化模式切换的UI可见性
    onProxyModeChanged(m_modeCombo->currentIndex());
}

void MainWindow::loadConfigFromFile()
{
    ProxyConfig cfg;
    bool ok = ConfigManager::getInstance()->loadConfig(cfg);
    if (!ok) {
        // 加载失败：使用内置默认
        cfg.mode = ProxyMode::ProxyServer;
        cfg.tcpHost = "127.0.0.1";
        cfg.tcpPort = 9000;
        cfg.zddsSendDomain = "TCPProxySendDomain";
        cfg.zddsRecvDomain = "TCPProxyRecvDomain";
        cfg.zddsSendTopic = "TcpToZdds";
        cfg.zddsRecvTopic = "ZddsToTcp";
        cfg.autoReconnect = false;
        cfg.reconnectInterval = 5;
    }
    applyConfigToUi(cfg);
}

void MainWindow::applyConfigToUi(const ProxyConfig &cfg)
{
    // 选择模式
    int idx = (cfg.mode == ProxyMode::ProxyClient) ? 1 : 0;
    if (m_modeCombo->currentIndex() != idx) {
        m_modeCombo->setCurrentIndex(idx);
    }
    m_tcpHostEdit->setText(cfg.tcpHost);
    m_tcpPortSpin->setValue(cfg.tcpPort);
    m_zddsSendDomainEdit->setText(cfg.zddsSendDomain);
    m_zddsRecvDomainEdit->setText(cfg.zddsRecvDomain);
    m_zddsSendTopicEdit->setText(cfg.zddsSendTopic);
    m_zddsRecvTopicEdit->setText(cfg.zddsRecvTopic);
    m_autoReconnectCheck->setChecked(cfg.autoReconnect);
    m_reconnectIntervalSpin->setValue(cfg.reconnectInterval);
}

void MainWindow::onSaveConfig()
{
    if (m_proxyCore->isRunning()) {
        QMessageBox::information(this, "提示", "请先停止代理后再保存配置");
        return;
    }
    ProxyConfig cfg = collectConfigFromUi();
    if (!cfg.isValid()) {
        QMessageBox::warning(this, "配置错误",
            "请检查配置：\n"
            "1. TCP端口必须在 1-65535 之间\n"
            "2. 代理客户端模式必须填写真实服务端地址\n"
            "3. ZDDS域名称、发送主题、接收主题均不能为空");
        return;
    }
    bool ok = ConfigManager::getInstance()->saveConfig(cfg);
    if (ok) {
        QMessageBox::information(this, "保存成功",
            QString("配置已保存到：\n%1").arg(ConfigManager::getInstance()->configFilePath()));
    } else {
        QMessageBox::warning(this, "保存失败", "请检查日志获取详细错误信息");
    }
}

void MainWindow::onProxyModeChanged(int index)
{
    ProxyMode mode = (ProxyMode)m_modeCombo->itemData(index).toInt();
    bool isClientMode = (mode == ProxyMode::ProxyClient);
    m_tcpHostLabel->setEnabled(isClientMode);
    m_tcpHostEdit->setEnabled(isClientMode);
    m_autoReconnectCheck->setEnabled(isClientMode);
    m_reconnectIntervalSpin->setEnabled(isClientMode && m_autoReconnectCheck->isChecked());
    if (isClientMode) {
        m_tcpHostLabel->setText("真实服务端地址:");
    } else {
        m_tcpHostLabel->setText("(服务端模式监听所有地址)");
    }
}

ProxyConfig MainWindow::collectConfigFromUi()
{
    ProxyConfig cfg;
    cfg.mode = (ProxyMode)m_modeCombo->currentData().toInt();
    cfg.tcpHost = m_tcpHostEdit->text().trimmed();
    cfg.tcpPort = m_tcpPortSpin->value();
    cfg.zddsSendDomain = m_zddsSendDomainEdit->text().trimmed();
    cfg.zddsRecvDomain = m_zddsRecvDomainEdit->text().trimmed();
    cfg.zddsSendTopic = m_zddsSendTopicEdit->text().trimmed();
    cfg.zddsRecvTopic = m_zddsRecvTopicEdit->text().trimmed();
    cfg.autoReconnect = m_autoReconnectCheck->isChecked();
    cfg.reconnectInterval = m_reconnectIntervalSpin->value();
    return cfg;
}

void MainWindow::updateUiEditableState(bool running)
{
    bool isClientMode = (m_modeCombo->currentData().toInt() == (int)ProxyMode::ProxyClient);
    m_modeCombo->setEnabled(!running);
    m_tcpHostEdit->setEnabled(!running && isClientMode);
    m_tcpHostLabel->setEnabled(!running && isClientMode);
    m_tcpPortSpin->setEnabled(!running);
    m_zddsSendDomainEdit->setEnabled(!running);
    m_zddsRecvDomainEdit->setEnabled(!running);
    m_zddsSendTopicEdit->setEnabled(!running);
    m_zddsRecvTopicEdit->setEnabled(!running);
    m_autoReconnectCheck->setEnabled(!running && isClientMode);
    m_reconnectIntervalSpin->setEnabled(!running && isClientMode && m_autoReconnectCheck->isChecked());

    if (running) {
        m_startBtn->setText("停止代理");
        m_startBtn->setStyleSheet("QPushButton{font-weight:bold;padding:6px 24px;background-color:#e74c3c;color:white;border:none;border-radius:4px;}"
                                  "QPushButton:hover{background-color:#c0392b;}");
    } else {
        m_startBtn->setText("启动代理");
        m_startBtn->setStyleSheet("QPushButton{font-weight:bold;padding:6px 24px;background-color:#27ae60;color:white;border:none;border-radius:4px;}"
                                  "QPushButton:hover{background-color:#1e8449;}");
    }
}

void MainWindow::onToggleStart()
{
    if (m_proxyCore->isRunning()) {
        m_proxyCore->stop();
    } else {
        ProxyConfig cfg = collectConfigFromUi();
        if (!cfg.isValid()) {
            QMessageBox::warning(this, "配置错误",
                "请检查配置：\n"
                "1. TCP端口必须在 1-65535 之间\n"
                "2. 代理客户端模式必须填写真实服务端地址\n"
                "3. ZDDS域名称、发送主题、接收主题均不能为空");
            return;
        }
        m_proxyCore->setConfig(cfg);
        m_proxyCore->start();
    }
}

void MainWindow::onStateChanged()
{
    bool running = m_proxyCore->isRunning();
    bool connected = m_proxyCore->isConnected();
    ProxyMode mode = m_proxyCore->currentMode();

    updateUiEditableState(running);

    // 更新模式标签
    if (mode == ProxyMode::ProxyServer) {
        m_modeValueLabel->setText("代理服务端");
    } else {
        m_modeValueLabel->setText("代理客户端");
    }

    // 更新状态标签
    if (!running) {
        m_statusValueLabel->setText("已停止");
        m_statusValueLabel->setStyleSheet("font-weight:bold;color:#e74c3c;");
    } else if (connected) {
        m_statusValueLabel->setText("运行中");
        m_statusValueLabel->setStyleSheet("font-weight:bold;color:#27ae60;");
    } else if (m_proxyCore->isReconnecting()) {
        // 代理客户端模式：正在自动重连
        m_statusValueLabel->setText("重连中...");
        m_statusValueLabel->setStyleSheet("font-weight:bold;color:#f39c12;");
    } else {
        // 代理客户端模式：已启动但TCP尚未连接
        m_statusValueLabel->setText("连接中...");
        m_statusValueLabel->setStyleSheet("font-weight:bold;color:#f39c12;");
    }

    onUpdateStats();
}

static QString formatBytes(quint64 bytes)
{
    if (bytes < 1024) return QString::number(bytes) + " B";
    if (bytes < 1024 * 1024) return QString::number(bytes / 1024.0, 'f', 2) + " KB";
    if (bytes < 1024ULL * 1024 * 1024) return QString::number(bytes / (1024.0 * 1024), 'f', 2) + " MB";
    return QString::number(bytes / (1024.0 * 1024 * 1024), 'f', 2) + " GB";
}

void MainWindow::onUpdateStats()
{
    m_tcpRxValueLabel->setText(formatBytes(m_proxyCore->tcpRxBytes()));
    m_tcpTxValueLabel->setText(formatBytes(m_proxyCore->tcpTxBytes()));
    m_zddsRxValueLabel->setText(formatBytes(m_proxyCore->zddsRxBytes()));
    m_zddsTxValueLabel->setText(formatBytes(m_proxyCore->zddsTxBytes()));
}

void MainWindow::appendLog(const QString &msg)
{
    QString ts = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    QString line = QString("[%1] %2").arg(ts).arg(msg);
    m_logEdit->append(line);
    // 自动滚动到底部
    QTextCursor cursor = m_logEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    m_logEdit->setTextCursor(cursor);
}

void MainWindow::onClearLog()
{
    m_logEdit->clear();
}
