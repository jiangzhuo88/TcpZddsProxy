#include "LanguageManager.h"
#include <QCoreApplication>
#include <QDir>

LanguageManager* LanguageManager::instance()
{
    static LanguageManager inst;
    return &inst;
}

LanguageManager::LanguageManager(QObject *parent)
    : QObject(parent)
{
}

void LanguageManager::setLanguage(Language lang)
{
    if (lang == m_currentLang) return;

    // 移除旧的翻译器
    if (m_translator) {
        qApp->removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }

    // 英文模式：加载 .qm 翻译文件
    if (lang == English) {
        m_translator = new QTranslator();
        // 优先从 Qt 资源系统加载
        bool loaded = m_translator->load(":/translations/tcpzddsproxy_en.qm");
        if (!loaded) {
            // 回退：从可执行文件同目录加载
            QString appDir = QCoreApplication::applicationDirPath();
            loaded = m_translator->load("tcpzddsproxy_en.qm", appDir + "/translations");
        }
        if (loaded) {
            qApp->installTranslator(m_translator);
        }
    }
    // 中文模式：不加载翻译器，直接使用源字符串

    m_currentLang = lang;
    emit languageChanged();
}

QString LanguageManager::languageDisplayName(Language lang)
{
    switch (lang) {
    case Chinese:  return QString::fromUtf8("中文");
    case English:  return "English";
    }
    return "Unknown";
}

QList<LanguageManager::Language> LanguageManager::supportedLanguages()
{
    return { Chinese, English };
}
