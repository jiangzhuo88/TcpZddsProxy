#ifndef LANGUAGEMANAGER_H
#define LANGUAGEMANAGER_H

#include <QObject>
#include <QJsonObject>
#include <QString>
#include <QStringList>

class LanguageManager : public QObject
{
    Q_OBJECT
public:
    enum Language {
        Chinese  = 0,
        English  = 1
    };
    Q_ENUM(Language)

    static LanguageManager* instance();

    // Set current language and reload translations
    void setLanguage(Language lang);
    Language currentLanguage() const { return m_currentLang; }

    // Translate a key; returns key itself if not found
    QString translate(const QString &key) const;

    // Get display name for a language
    static QString languageDisplayName(Language lang);

    // Get list of all supported languages
    static QList<Language> supportedLanguages();

signals:
    void languageChanged();

private:
    explicit LanguageManager(QObject *parent = nullptr);
    void loadTranslations(Language lang);
    QString translationsFilePath(Language lang) const;

    Language m_currentLang = Chinese;
    QJsonObject m_translations;
};

// Convenience macro for shorter call sites
#define LTR(key) LanguageManager::instance()->translate(key)

#endif // LANGUAGEMANAGER_H
