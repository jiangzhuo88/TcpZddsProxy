#ifndef LANGUAGEMANAGER_H
#define LANGUAGEMANAGER_H

#include <QObject>
#include <QTranslator>

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

    void setLanguage(Language lang);
    Language currentLanguage() const { return m_currentLang; }

    static QString languageDisplayName(Language lang);
    static QList<Language> supportedLanguages();

signals:
    void languageChanged();

private:
    explicit LanguageManager(QObject *parent = nullptr);

    QTranslator *m_translator = nullptr;
    Language m_currentLang = Chinese;
};

#endif // LANGUAGEMANAGER_H
