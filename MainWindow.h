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
#include "TcpProxyCore.h"

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

private:
    void setupUi();
    void loadConfigFromFile();     // 启动时从配置文件加载
    void applyConfigToUi(const ProxyConfig &cfg);  // 把配置填充到UI控件
    ProxyConfig collectConfigFromUi();
    void updateUiEditableState(bool running);

    // UI 组件
    QComboBox *m_modeCombo;

    // TCP配置
    QLabel *m_tcpHostLabel;
    QLineEdit *m_tcpHostEdit;
    QSpinBox *m_tcpPortSpin;

    // ZDDS配置
    QLineEdit *m_zddsDomainEdit;
    QLineEdit *m_zddsSendTopicEdit;
    QLineEdit *m_zddsRecvTopicEdit;

    // 控制按钮
    QPushButton *m_startBtn;
    QPushButton *m_clearLogBtn;

    // 状态显示
    QLabel *m_modeValueLabel;
    QLabel *m_statusValueLabel;
    QLabel *m_tcpRxValueLabel;
    QLabel *m_tcpTxValueLabel;
    QLabel *m_zddsRxValueLabel;
    QLabel *m_zddsTxValueLabel;

    // 日志
    QTextEdit *m_logEdit;

    // 核心模块
    TcpProxyCore *m_proxyCore;

    // 统计刷新定时器
    QTimer *m_statsTimer;
};

#endif // MAINWINDOW_H
