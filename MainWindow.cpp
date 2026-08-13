#include "MainWindow.h"
#include "ZDDSManager.h"
#include "ConfigManager.h"
#include "LanguageManager.h"
#include <QDateTime>
#include <QLabel>
#include <QMessageBox>
#include <QCloseEvent>
#include <QPushButton>
#include <QTextCursor>
#include <QFrame>

static QString formatBytes(quint64 bytes)
{
    if (bytes < 1024) return QString::number(bytes) + " B";
    if (bytes < 1024 * 1024) return QString::number(bytes / 1024.0, 'f', 2) + " KB";
    if (bytes < 1024ULL * 1024 * 1024) return QString::number(bytes / (1024.0 * 1024), 'f', 2) + " MB";
    return QString::number(bytes / (1024.0 * 1024 * 1024), 'f', 2) + " GB";
}

// ============================================================
// StatusIndicator
// ============================================================
StatusIndicator::StatusIndicator(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setSpacing(8);
    layout->setContentsMargins(0, 0, 0, 0);

    m_dotLabel = new QLabel(this);
    m_dotLabel->setFixedSize(16, 16);
    m_dotLabel->setStyleSheet(
        QString("QLabel{background-color:%1;border-radius:8px;}").arg(StatusColor::Gray));

    m_nameLabel = new QLabel(this);
    m_nameLabel->setStyleSheet("font-weight:bold;min-width:140px;");

    m_valueLabel = new QLabel(this);
    m_valueLabel->setStyleSheet("font-weight:bold;min-width:100px;");

    m_detailLabel = new QLabel(this);
    m_detailLabel->setStyleSheet("color:#7f8c8d;");

    layout->addWidget(m_dotLabel);
    layout->addWidget(m_nameLabel);
    layout->addWidget(m_valueLabel);
    layout->addWidget(m_detailLabel, 1);
}

void StatusIndicator::setColor(const QString &color)
{
    m_dotLabel->setStyleSheet(
        QString("QLabel{background-color:%1;border-radius:8px;}").arg(color));
}

void StatusIndicator::setText(const QString &text)
{
    m_valueLabel->setText(text);
}

void StatusIndicator::setDetail(const QString &detail)
{
    m_detailLabel->setText(detail);
}

void StatusIndicator::setName(const QString &name)
{
    m_nameLabel->setText(name);
}

QString StatusIndicator::labelText() const
{
    return m_nameLabel->text();
}

// ============================================================
// MainWindow
// ============================================================
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
    connect(ZDDSManager::getInstance(), &ZDDSManager::statusChanged, this, &MainWindow::onZddsStatusChanged);
    connect(ConfigManager::getInstance(), &ConfigManager::logMessage, this, &MainWindow::appendLog);
    connect(LanguageManager::instance(), &LanguageManager::languageChanged, this, &MainWindow::retranslateUi);

    m_statsTimer->setInterval(1000);
    connect(m_statsTimer, &QTimer::timeout, this, &MainWindow::onUpdateStats);
    m_statsTimer->start();

    onStateChanged();
    onUpdateStats();
    retranslateUi();
}

MainWindow::~MainWindow()
{
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
    resize(960, 780);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(8);
    mainLayout->setContentsMargins(12, 12, 12, 12);

    // ============ 顶部：配置区 ============
    m_configBox = new QGroupBox(centralWidget);
    QGridLayout *cfgLayout = new QGridLayout(m_configBox);
    cfgLayout->setSpacing(8);
    cfgLayout->setContentsMargins(12, 16, 12, 12);

    int row = 0;
    m_langLabel = new QLabel(m_configBox);
    cfgLayout->addWidget(m_langLabel, row, 0);
    m_langCombo = new QComboBox(m_configBox);
    m_langCombo->addItem(LanguageManager::languageDisplayName(LanguageManager::Chinese), (int)LanguageManager::Chinese);
    m_langCombo->addItem(LanguageManager::languageDisplayName(LanguageManager::English), (int)LanguageManager::English);
    cfgLayout->addWidget(m_langCombo, row, 1);
    connect(m_langCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onLanguageChanged);
    row++;

    m_modeLabel = new QLabel(m_configBox);
    cfgLayout->addWidget(m_modeLabel, row, 0);
    m_modeCombo = new QComboBox(m_configBox);
    cfgLayout->addWidget(m_modeCombo, row, 1, 1, 3);
    connect(m_modeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onProxyModeChanged);
    row++;

    m_tcpHostLabel = new QLabel(m_configBox);
    cfgLayout->addWidget(m_tcpHostLabel, row, 0);
    m_tcpHostEdit = new QLineEdit(m_configBox);
    cfgLayout->addWidget(m_tcpHostEdit, row, 1);
    row++;

    m_tcpPortLabel = new QLabel(m_configBox);
    cfgLayout->addWidget(m_tcpPortLabel, row, 0);
    m_tcpPortSpin = new QSpinBox(m_configBox);
    m_tcpPortSpin->setRange(1, 65535);
    m_tcpPortSpin->setValue(9000);
    cfgLayout->addWidget(m_tcpPortSpin, row, 1);
    row++;

    m_zddsSendDomainLabel = new QLabel(m_configBox);
    cfgLayout->addWidget(m_zddsSendDomainLabel, row, 0);
    m_zddsSendDomainEdit = new QLineEdit(m_configBox);
    cfgLayout->addWidget(m_zddsSendDomainEdit, row, 1);
    m_zddsSendTopicLabel = new QLabel(m_configBox);
    cfgLayout->addWidget(m_zddsSendTopicLabel, row, 2);
    m_zddsSendTopicEdit = new QLineEdit(m_configBox);
    cfgLayout->addWidget(m_zddsSendTopicEdit, row, 3);
    row++;

    m_zddsRecvDomainLabel = new QLabel(m_configBox);
    cfgLayout->addWidget(m_zddsRecvDomainLabel, row, 0);
    m_zddsRecvDomainEdit = new QLineEdit(m_configBox);
    cfgLayout->addWidget(m_zddsRecvDomainEdit, row, 1);
    m_zddsRecvTopicLabel = new QLabel(m_configBox);
    cfgLayout->addWidget(m_zddsRecvTopicLabel, row, 2);
    m_zddsRecvTopicEdit = new QLineEdit(m_configBox);
    cfgLayout->addWidget(m_zddsRecvTopicEdit, row, 3);
    row++;

    QHBoxLayout *btnLayout = new QHBoxLayout();
    m_saveBtn = new QPushButton(m_configBox);
    m_saveBtn->setMinimumHeight(34);
    m_saveBtn->setStyleSheet("QPushButton{padding:6px 18px;}");
    connect(m_saveBtn, &QPushButton::clicked, this, &MainWindow::onSaveConfig);
    btnLayout->addWidget(m_saveBtn);
    btnLayout->addStretch();
    m_startBtn = new QPushButton(m_configBox);
    m_startBtn->setMinimumHeight(34);
    m_startBtn->setStyleSheet("QPushButton{font-weight:bold;padding:6px 24px;}");
    connect(m_startBtn, &QPushButton::clicked, this, &MainWindow::onToggleStart);
    btnLayout->addWidget(m_startBtn);
    cfgLayout->addLayout(btnLayout, row, 0, 1, 4);

    mainLayout->addWidget(m_configBox);

    // ============ 状态面板区 ============
    m_statusBox = new QGroupBox(centralWidget);
    QGridLayout *stLayout = new QGridLayout(m_statusBox);
    stLayout->setSpacing(6);
    stLayout->setContentsMargins(12, 16, 12, 12);

    int srow = 0;
    m_zddsStatusInd = createStatusRow(stLayout, srow); srow++;
    m_zddsSubInd = createStatusRow(stLayout, srow); srow++;
    m_tcpLinkInd = createStatusRow(stLayout, srow); srow++;
    m_clientsInd = createStatusRow(stLayout, srow); srow++;

    QFrame *line = new QFrame(m_statusBox);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    stLayout->addWidget(line, srow, 0, 1, 4);
    srow++;

    m_flowTcpZddsInd = createStatusRow(stLayout, srow); srow++;
    m_flowZddsTcpInd = createStatusRow(stLayout, srow); srow++;

    mainLayout->addWidget(m_statusBox);

    // ============ 数据统计区 ============
    QGroupBox *statsBox = new QGroupBox(centralWidget);
    QGridLayout *statsLayout = new QGridLayout(statsBox);
    statsLayout->setSpacing(8);
    statsLayout->setContentsMargins(12, 16, 12, 12);

    m_modeTextLabel = new QLabel(statsBox);
    statsLayout->addWidget(m_modeTextLabel, 0, 0);
    m_modeValueLabel = new QLabel("-", statsBox);
    m_modeValueLabel->setStyleSheet("font-weight:bold;");
    statsLayout->addWidget(m_modeValueLabel, 0, 1);

    m_runStatusLabel = new QLabel(statsBox);
    statsLayout->addWidget(m_runStatusLabel, 0, 2);
    m_statusValueLabel = new QLabel("-", statsBox);
    m_statusValueLabel->setStyleSheet("font-weight:bold;color:gray;");
    statsLayout->addWidget(m_statusValueLabel, 0, 3);

    m_tcpRxLabel = new QLabel(statsBox);
    statsLayout->addWidget(m_tcpRxLabel, 1, 0);
    m_tcpRxValueLabel = new QLabel("0", statsBox);
    statsLayout->addWidget(m_tcpRxValueLabel, 1, 1);

    m_tcpTxLabel = new QLabel(statsBox);
    statsLayout->addWidget(m_tcpTxLabel, 1, 2);
    m_tcpTxValueLabel = new QLabel("0", statsBox);
    statsLayout->addWidget(m_tcpTxValueLabel, 1, 3);

    m_zddsRxLabel = new QLabel(statsBox);
    statsLayout->addWidget(m_zddsRxLabel, 2, 0);
    m_zddsRxValueLabel = new QLabel("0", statsBox);
    statsLayout->addWidget(m_zddsRxValueLabel, 2, 1);

    m_zddsTxLabel = new QLabel(statsBox);
    statsLayout->addWidget(m_zddsTxLabel, 2, 2);
    m_zddsTxValueLabel = new QLabel("0", statsBox);
    statsLayout->addWidget(m_zddsTxValueLabel, 2, 3);

    mainLayout->addWidget(statsBox);

    // ============ 底部：日志区 ============
    m_logBox = new QGroupBox(centralWidget);
    QVBoxLayout *logLayout = new QVBoxLayout(m_logBox);
    logLayout->setContentsMargins(12, 16, 12, 12);

    QHBoxLayout *logHeader = new QHBoxLayout();
    logHeader->addStretch();
    m_clearLogBtn = new QPushButton(m_logBox);
    connect(m_clearLogBtn, &QPushButton::clicked, this, &MainWindow::onClearLog);
    logHeader->addWidget(m_clearLogBtn);
    logLayout->addLayout(logHeader);

    m_logEdit = new QTextEdit(m_logBox);
    m_logEdit->setReadOnly(true);
    m_logEdit->setStyleSheet("QTextEdit{font-family:'Consolas','Courier New',monospace;font-size:12px;}");
    logLayout->addWidget(m_logEdit);

    mainLayout->addWidget(m_logBox, 1);

    onProxyModeChanged(m_modeCombo->currentIndex());
}

StatusIndicator* MainWindow::createStatusRow(QGridLayout *layout, int row)
{
    StatusIndicator *ind = new StatusIndicator(m_statusBox);
    layout->addWidget(ind, row, 0, 1, 4);
    return ind;
}

void MainWindow::loadConfigFromFile()
{
    ProxyConfig cfg;
    bool ok = ConfigManager::getInstance()->loadConfig(cfg);
    if (!ok) {
        cfg.mode = ProxyMode::ProxyServer;
        cfg.tcpHost = "127.0.0.1";
        cfg.tcpPort = 9000;
        cfg.zddsSendDomain = "TCPProxySendDomain";
        cfg.zddsRecvDomain = "TCPProxyRecvDomain";
        cfg.zddsSendTopic = "TcpToZdds";
        cfg.zddsRecvTopic = "ZddsToTcp";
    }
    applyConfigToUi(cfg);
}

void MainWindow::applyConfigToUi(const ProxyConfig &cfg)
{
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
}

void MainWindow::onSaveConfig()
{
    if (m_proxyCore->isRunning()) {
        QMessageBox::information(this, tr("提示"), tr("请先停止代理后再保存配置"));
        return;
    }
    ProxyConfig cfg = collectConfigFromUi();
    if (!cfg.isValid()) {
        QMessageBox::warning(this, tr("错误"),
            tr("请检查配置：\n"
               "1. TCP端口必须在 1-65535 之间\n"
               "2. 代理客户端模式必须填写真实服务端地址\n"
               "3. ZDDS域名称、发送主题、接收主题均不能为空"));
        return;
    }
    bool ok = ConfigManager::getInstance()->saveConfig(cfg);
    if (ok) {
        QMessageBox::information(this, tr("保存成功"),
            tr("配置已保存到：\n%1").arg(ConfigManager::getInstance()->configFilePath()));
    } else {
        QMessageBox::warning(this, tr("保存失败"), tr("请检查日志获取详细错误信息"));
    }
}

void MainWindow::onProxyModeChanged(int index)
{
    ProxyMode mode = (ProxyMode)m_modeCombo->itemData(index).toInt();
    bool isClientMode = (mode == ProxyMode::ProxyClient);
    m_tcpHostLabel->setEnabled(isClientMode);
    m_tcpHostEdit->setEnabled(isClientMode);
    if (isClientMode) {
        m_tcpHostLabel->setText(tr("真实服务端地址:"));
    } else {
        m_tcpHostLabel->setText(tr("(服务端模式监听所有地址)"));
    }
}

void MainWindow::onLanguageChanged(int index)
{
    LanguageManager::Language lang = (LanguageManager::Language)m_langCombo->itemData(index).toInt();
    LanguageManager::instance()->setLanguage(lang);
    // retranslateUi() will be triggered by languageChanged signal
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
    return cfg;
}

void MainWindow::updateUiEditableState(bool running)
{
    m_modeCombo->setEnabled(!running);
    m_tcpHostEdit->setEnabled(!running && (m_modeCombo->currentData().toInt() == (int)ProxyMode::ProxyClient));
    m_tcpHostLabel->setEnabled(!running && (m_modeCombo->currentData().toInt() == (int)ProxyMode::ProxyClient));
    m_tcpPortSpin->setEnabled(!running);
    m_zddsSendDomainEdit->setEnabled(!running);
    m_zddsRecvDomainEdit->setEnabled(!running);
    m_zddsSendTopicEdit->setEnabled(!running);
    m_zddsRecvTopicEdit->setEnabled(!running);

    if (running) {
        m_startBtn->setText(tr("停止代理"));
        m_startBtn->setStyleSheet("QPushButton{font-weight:bold;padding:6px 24px;background-color:#e74c3c;color:white;border:none;border-radius:4px;}"
                                  "QPushButton:hover{background-color:#c0392b;}");
    } else {
        m_startBtn->setText(tr("启动代理"));
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
            QMessageBox::warning(this, tr("错误"),
                tr("请检查配置：\n"
                   "1. TCP端口必须在 1-65535 之间\n"
                   "2. 代理客户端模式必须填写真实服务端地址\n"
                   "3. ZDDS域名称、发送主题、接收主题均不能为空"));
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

    if (mode == ProxyMode::ProxyServer) {
        m_modeValueLabel->setText(tr("代理服务端"));
    } else {
        m_modeValueLabel->setText(tr("代理客户端"));
    }

    if (!running) {
        m_statusValueLabel->setText(tr("已停止"));
        m_statusValueLabel->setStyleSheet("font-weight:bold;color:#e74c3c;");
    } else if (connected) {
        m_statusValueLabel->setText(tr("运行中"));
        m_statusValueLabel->setStyleSheet("font-weight:bold;color:#27ae60;");
    } else {
        m_statusValueLabel->setText(tr("连接中..."));
        m_statusValueLabel->setStyleSheet("font-weight:bold;color:#f39c12;");
    }

    updateStatusPanel();
    onUpdateStats();
}

void MainWindow::onZddsStatusChanged()
{
    updateStatusPanel();
}

void MainWindow::updateStatusPanel()
{
    // 1. ZDDS Status
    ZDDSManager::ZddsStatus zddsStatus = ZDDSManager::getInstance()->status();
    switch (zddsStatus) {
    case ZDDSManager::ZddsStatus::NotStarted:
        m_zddsStatusInd->setColor(StatusColor::Gray);
        m_zddsStatusInd->setText(tr("未启动"));
        m_zddsStatusInd->setDetail("");
        break;
    case ZDDSManager::ZddsStatus::Starting:
        m_zddsStatusInd->setColor(StatusColor::Yellow);
        m_zddsStatusInd->setText(tr("启动中..."));
        m_zddsStatusInd->setDetail("");
        break;
    case ZDDSManager::ZddsStatus::Started:
        m_zddsStatusInd->setColor(StatusColor::Green);
        m_zddsStatusInd->setText(tr("已接入"));
        m_zddsStatusInd->setDetail("");
        break;
    case ZDDSManager::ZddsStatus::Failed:
        m_zddsStatusInd->setColor(StatusColor::Red);
        m_zddsStatusInd->setText(tr("启动失败"));
        m_zddsStatusInd->setDetail("");
        break;
    }

    // 2. ZDDS Subscription
    if (m_proxyCore->isZddsSubscribed()) {
        m_zddsSubInd->setColor(StatusColor::Green);
        m_zddsSubInd->setText(tr("已订阅"));
        const ProxyConfig &cfg = m_proxyCore->config();
        m_zddsSubInd->setDetail(QString("%1/%2").arg(cfg.zddsRecvDomain).arg(cfg.zddsRecvTopic));
    } else {
        m_zddsSubInd->setColor(StatusColor::Gray);
        m_zddsSubInd->setText(tr("未订阅"));
        m_zddsSubInd->setDetail("");
    }

    // 3. TCP Link
    TcpLinkStatus tcpStatus = m_proxyCore->tcpLinkStatus();
    const ProxyConfig &cfg = m_proxyCore->config();
    switch (tcpStatus) {
    case TcpLinkStatus::Stopped:
        m_tcpLinkInd->setColor(StatusColor::Gray);
        m_tcpLinkInd->setText(tr("未启动"));
        m_tcpLinkInd->setDetail("");
        break;
    case TcpLinkStatus::Listening:
        m_tcpLinkInd->setColor(StatusColor::Green);
        m_tcpLinkInd->setText(tr("监听中 (端口 %1)").arg(cfg.tcpPort));
        m_tcpLinkInd->setDetail("");
        break;
    case TcpLinkStatus::Connecting:
        m_tcpLinkInd->setColor(StatusColor::Yellow);
        m_tcpLinkInd->setText(tr("连接中 (%1:%2)").arg(cfg.tcpHost).arg(cfg.tcpPort));
        m_tcpLinkInd->setDetail("");
        break;
    case TcpLinkStatus::Connected:
        m_tcpLinkInd->setColor(StatusColor::Green);
        m_tcpLinkInd->setText(tr("已连接 (%1:%2)").arg(cfg.tcpHost).arg(cfg.tcpPort));
        m_tcpLinkInd->setDetail(m_proxyCore->tcpPeerInfo());
        break;
    case TcpLinkStatus::Disconnected:
        m_tcpLinkInd->setColor(StatusColor::Red);
        m_tcpLinkInd->setText(tr("已断开"));
        m_tcpLinkInd->setDetail("");
        break;
    case TcpLinkStatus::Error:
        m_tcpLinkInd->setColor(StatusColor::Red);
        m_tcpLinkInd->setText(tr("错误"));
        m_tcpLinkInd->setDetail("");
        break;
    }

    // 4. Connected Clients
    int clientCount = m_proxyCore->clientCount();
    if (m_proxyCore->isRunning() && cfg.mode == ProxyMode::ProxyServer) {
        if (clientCount > 0) {
            m_clientsInd->setColor(StatusColor::Green);
            m_clientsInd->setText(tr("%1 个客户端").arg(clientCount));
            m_clientsInd->setDetail(m_proxyCore->tcpPeerInfo());
        } else {
            m_clientsInd->setColor(StatusColor::Yellow);
            m_clientsInd->setText(tr("无客户端"));
            m_clientsInd->setDetail("");
        }
    } else {
        m_clientsInd->setColor(StatusColor::Gray);
        m_clientsInd->setText("-");
        m_clientsInd->setDetail("");
    }

    // 5. Data Flow TCP->ZDDS
    if (m_proxyCore->isTcpToZddsActive()) {
        m_flowTcpZddsInd->setColor(StatusColor::Green);
        m_flowTcpZddsInd->setText(tr("活跃 (%1)").arg(formatBytes(m_proxyCore->zddsTxBytes())));
        m_flowTcpZddsInd->setDetail(QString("%1/%2").arg(cfg.zddsSendDomain).arg(cfg.zddsSendTopic));
    } else {
        m_flowTcpZddsInd->setColor(StatusColor::Gray);
        m_flowTcpZddsInd->setText(tr("空闲"));
        m_flowTcpZddsInd->setDetail("");
    }

    // 6. Data Flow ZDDS->TCP
    if (m_proxyCore->isZddsToTcpActive()) {
        m_flowZddsTcpInd->setColor(StatusColor::Green);
        m_flowZddsTcpInd->setText(tr("活跃 (%1)").arg(formatBytes(m_proxyCore->zddsRxBytes())));
        m_flowZddsTcpInd->setDetail(QString("%1/%2").arg(cfg.zddsRecvDomain).arg(cfg.zddsRecvTopic));
    } else {
        m_flowZddsTcpInd->setColor(StatusColor::Gray);
        m_flowZddsTcpInd->setText(tr("空闲"));
        m_flowZddsTcpInd->setDetail("");
    }
}

void MainWindow::onUpdateStats()
{
    m_tcpRxValueLabel->setText(formatBytes(m_proxyCore->tcpRxBytes()));
    m_tcpTxValueLabel->setText(formatBytes(m_proxyCore->tcpTxBytes()));
    m_zddsRxValueLabel->setText(formatBytes(m_proxyCore->zddsRxBytes()));
    m_zddsTxValueLabel->setText(formatBytes(m_proxyCore->zddsTxBytes()));
    updateStatusPanel();
}

void MainWindow::appendLog(const QString &msg)
{
    QString ts = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    QString line = QString("[%1] %2").arg(ts).arg(msg);
    m_logEdit->append(line);
    QTextCursor cursor = m_logEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    m_logEdit->setTextCursor(cursor);
}

void MainWindow::onClearLog()
{
    m_logEdit->clear();
}

void MainWindow::retranslateUi()
{
    setWindowTitle(tr("TCP-ZDDS 双向代理软件"));

    // Config box
    m_configBox->setTitle(tr("配置"));
    m_langLabel->setText(tr("语言:"));
    m_modeLabel->setText(tr("代理模式:"));
    m_tcpPortLabel->setText(tr("TCP端口:"));
    m_zddsSendDomainLabel->setText(tr("ZDDS发送域名称:"));
    m_zddsRecvDomainLabel->setText(tr("ZDDS接收域名称:"));
    m_zddsSendTopicLabel->setText(tr("ZDDS发送主题(TCP→ZDDS):"));
    m_zddsRecvTopicLabel->setText(tr("ZDDS接收主题(ZDDS→TCP):"));
    m_tcpHostEdit->setPlaceholderText(tr("例如: 192.168.1.100 或 127.0.0.1"));
    m_zddsSendDomainEdit->setPlaceholderText(tr("例如: DomainTCPProxy"));
    m_zddsRecvDomainEdit->setPlaceholderText(tr("例如: DomainTCPProxy"));
    m_zddsSendTopicEdit->setPlaceholderText(tr("例如: TopicTcpToZdds"));
    m_zddsRecvTopicEdit->setPlaceholderText(tr("例如: TopicZddsToTcp"));
    m_saveBtn->setText(tr("保存配置"));
    m_clearLogBtn->setText(tr("清空日志"));

    // Mode combo items
    int curIdx = m_modeCombo->currentIndex();
    m_modeCombo->blockSignals(true);
    m_modeCombo->clear();
    m_modeCombo->addItem(tr("代理服务端 (监听端口，接收真实客户端)"), (int)ProxyMode::ProxyServer);
    m_modeCombo->addItem(tr("代理客户端 (连接真实服务端)"), (int)ProxyMode::ProxyClient);
    m_modeCombo->setCurrentIndex(curIdx);
    m_modeCombo->blockSignals(false);

    onProxyModeChanged(m_modeCombo->currentIndex());

    // Status box
    m_statusBox->setTitle(tr("连接状态总览"));

    // Status indicators name labels
    m_zddsStatusInd->setName(tr("ZDDS 状态"));
    m_zddsSubInd->setName(tr("ZDDS 订阅"));
    m_tcpLinkInd->setName(tr("TCP 链路"));
    m_clientsInd->setName(tr("已连接客户端"));
    m_flowTcpZddsInd->setName(tr("数据流 TCP→ZDDS"));
    m_flowZddsTcpInd->setName(tr("数据流 ZDDS→TCP"));

    // Stats labels
    m_modeTextLabel->setText(tr("当前模式") + ":");
    m_runStatusLabel->setText(tr("运行状态") + ":");
    m_tcpRxLabel->setText(tr("TCP接收字节:"));
    m_tcpTxLabel->setText(tr("TCP发送字节:"));
    m_zddsRxLabel->setText(tr("ZDDS接收字节:"));
    m_zddsTxLabel->setText(tr("ZDDS发送字节:"));

    // Log box
    m_logBox->setTitle(tr("日志"));

    // Update start button text
    updateUiEditableState(m_proxyCore->isRunning());

    // Refresh status panel with translated strings
    onStateChanged();

    // Update language combo display names (native names, not translated)
    int langIdx = m_langCombo->currentIndex();
    m_langCombo->blockSignals(true);
    m_langCombo->setItemText(0, LanguageManager::languageDisplayName(LanguageManager::Chinese));
    m_langCombo->setItemText(1, LanguageManager::languageDisplayName(LanguageManager::English));
    m_langCombo->setCurrentIndex(langIdx);
    m_langCombo->blockSignals(false);
}
