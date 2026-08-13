#include "LanguageManager.h"
#include <QCoreApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QFileInfo>
#include <QDir>

LanguageManager* LanguageManager::instance()
{
    static LanguageManager inst;
    return &inst;
}

LanguageManager::LanguageManager(QObject *parent)
    : QObject(parent)
{
    loadTranslations(m_currentLang);
}

QString LanguageManager::translationsFilePath(Language lang) const
{
    QString appDir = QCoreApplication::applicationDirPath();
    QString subDir = (lang == Chinese) ? "translations/translations_zh.json"
                                       : "translations/translations_en.json";
    return QDir(appDir).filePath(subDir);
}

void LanguageManager::loadTranslations(Language lang)
{
    // Try application directory first, then source directory (for development)
    QString path = translationsFilePath(lang);
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // Fallback: go up from bin/ to project root, then look for translations/
        QDir dir(QCoreApplication::applicationDirPath());
        dir.cdUp();  // go up from bin/ to project root
        path = dir.filePath("translations/translations_" +
                      QString(lang == Chinese ? "zh" : "en") + ".json");
        file.setFileName(path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            m_translations = QJsonObject();
            return;
        }
    }

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &err);
    file.close();

    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        m_translations = QJsonObject();
        return;
    }
    m_translations = doc.object();
}

void LanguageManager::setLanguage(Language lang)
{
    if (lang == m_currentLang) return;
    m_currentLang = lang;
    loadTranslations(lang);
    emit languageChanged();
}

QString LanguageManager::translate(const QString &key) const
{
    if (m_translations.contains(key)) {
        return m_translations.value(key).toString();
    }
    return key;
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
