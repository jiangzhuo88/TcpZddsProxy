#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTimer>
#include <QList>
#include <QCheckBox>
#include "TcpProxyCore.h"
#include "LanguageManager.h"

// Status indicator dot colors
namespace StatusColor {
    const QString Green  = "#27ae60";   // OK / Connected / Active
    const QString Yellow = "#f39c12";   // Connecting / Starting / Idle
    const QString Red    = "#e74c3c";   // Error / Disconnected / Failed
    const QString Gray   = "#95a5a6";   // Not Started / N/A
}

class StatusIndicator : public QWidget
{
    Q_OBJECT
public:
    explicit StatusIndicator(QWidget *parent = nullptr);
    void setColor(const QString &color);
    void setText(const QString &text);
    void setDetail(const QString &detail);
    void setName(const QString &name);
    QString labelText() const;

private:
    QLabel *m_dotLabel;
    QLabel *m_nameLabel;
    QLabel *m_valueLabel;
    QLabel *m_detailLabel;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onToggleStart();
    void onProxyModeChanged(int index);
    void onStateChanged();
    void appendLog(const QString &msg);
    void onClearLog();
    void onUpdateStats();
    void onSaveConfig();
    void onZddsStatusChanged();

private:
    void setupUi();
    void loadConfigFromFile();
    void applyConfigToUi(const ProxyConfig &cfg);
    ProxyConfig collectConfigFromUi();
    void updateUiEditableState(bool running);
    void retranslateUi();
    void updateStatusPanel();

    // 创建状态指示器行
    StatusIndicator* createStatusRow(QGridLayout *layout, int row);

    // UI 组件
    QComboBox *m_modeCombo;

    // 配置区 labels (for retranslation)
    QLabel *m_modeLabel;
    QLabel *m_tcpHostLabel;
    QLabel *m_tcpPortLabel;
    QLabel *m_zddsSendDomainLabel;
    QLabel *m_zddsRecvDomainLabel;
    QLabel *m_zddsSendTopicLabel;
    QLabel *m_zddsRecvTopicLabel;

    // 自动重连配置
    QCheckBox *m_autoReconnectCheck;
    QLabel *m_reconnectIntervalLabel;
    QSpinBox *m_reconnectIntervalSpin;

    // TCP配置
    QLineEdit *m_tcpHostEdit;
    QSpinBox *m_tcpPortSpin;

    // ZDDS配置
    QLineEdit *m_zddsSendDomainEdit;
    QLineEdit *m_zddsRecvDomainEdit;
    QLineEdit *m_zddsSendTopicEdit;
    QLineEdit *m_zddsRecvTopicEdit;

    // 控制按钮
    QPushButton *m_startBtn;
    QPushButton *m_clearLogBtn;
    QPushButton *m_saveBtn;

    // 状态区
    QGroupBox *m_statusBox;
    QLabel *m_modeValueLabel;
    QLabel *m_statusValueLabel;
    QLabel *m_tcpRxValueLabel;
    QLabel *m_tcpTxValueLabel;
    QLabel *m_zddsRxValueLabel;
    QLabel *m_zddsTxValueLabel;

    // 状态标签 (for retranslation)
    QLabel *m_modeTextLabel;
    QLabel *m_runStatusLabel;
    QLabel *m_tcpRxLabel;
    QLabel *m_tcpTxLabel;
    QLabel *m_zddsRxLabel;
    QLabel *m_zddsTxLabel;
    QGroupBox *m_configBox;
    QGroupBox *m_logBox;

    // 状态面板指示器
    StatusIndicator *m_zddsStatusInd;
    StatusIndicator *m_zddsSubInd;
    StatusIndicator *m_tcpLinkInd;
    StatusIndicator *m_clientsInd;
    StatusIndicator *m_flowTcpZddsInd;
    StatusIndicator *m_flowZddsTcpInd;

    // 日志
    QTextEdit *m_logEdit;

    // 核心模块
    TcpProxyCore *m_proxyCore;

    // 统计刷新定时器
    QTimer *m_statsTimer;
};

#endif // MAINWINDOW_H
