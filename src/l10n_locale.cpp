#include "l10n_locale.h"

#include "l10n_language.h"

#include <KConfigGroup>
#include <KSharedConfig>

#include <QDebug>

namespace Kubuntu {

typedef QList<Language *> LanguagePtrList;

class LocalePrivate
{
public:
    LocalePrivate();
    ~LocalePrivate();

    void init(LanguagePtrList _languages, QString _country);

    LanguagePtrList languages;

    QString country;
    QString encoding;
    QString variant;
};

LocalePrivate::LocalePrivate()
    : encoding(QLatin1String("UTF-8"))
{
}

LocalePrivate::~LocalePrivate()
{
    qDeleteAll(languages);
}

void LocalePrivate::init(LanguagePtrList _languages, QString _country)
{
    languages = _languages;
    country = _country;

    // Parse additional stuff out of the main language's kde code.
    QString mainLanguage = _languages.at(0)->languageCode();
    qDebug() << mainLanguage;
    // Get variant.
    if (mainLanguage.contains(QChar('@'))) {
        QStringList components = mainLanguage.split(QChar('@'));
        if (!components.isEmpty()) {
            variant = components.takeLast();
            mainLanguage = components.join(QChar('@'));
        }
    }
    // Get possibly implicit country to override manually defined one.
    // For example zh_CN must be zh_CN because that is a different language from
    // zh_TW.
    // en_US is exepcted from this rule as 'en' is the uniform base language
    // and makes sense for everything.
    if (mainLanguage.contains(QChar('_')) && !mainLanguage.startsWith(QLatin1String("en_US"))) {
        QStringList components = mainLanguage.split(QChar('_'));
        if (!components.isEmpty()) {
            country = components.takeLast();
            mainLanguage = components.join(QChar('_'));
        }
    }
}

Locale::Locale()
    : d_ptr(new LocalePrivate)
{
    Q_D(Locale);

    KSharedConfigPtr config = KSharedConfig::openConfig("kdeglobals", KConfig::CascadeConfig);
    KConfigGroup settings = KConfigGroup(config, "Locale");

    LanguagePtrList languages;
    QString languageSetting = settings.readEntry("Language", QString::fromLatin1("en_US"));
    if (languageSetting.contains(QChar(':'))) {
        QStringList languageCodes = languageSetting.split(QChar(':'));
        foreach (const QString &languageCode, languageCodes)
            languages.append(new Language(languageCode));
    } else {
        languages.append(new Language(languageSetting));
    }
    QString country = settings.readEntry("Country", QString::fromLatin1("C"));

    d->init(languages, country);
}

Locale::Locale(const QList<Language *> &languages, const QString &country)
    : d_ptr(new LocalePrivate)
{
    Q_D(Locale);
    d->init(languages, country);
}

Locale::~Locale()
{
}

QString Locale::systemLocaleString() const
{
    Q_D(const Locale);
    QString locale = d->languages.at(0)->systemLanguageCode();

    if (!d->country.isEmpty())
        locale.append(QString("_%1").arg(d->country.toUpper()));

    if (!d->encoding.isEmpty())
        locale.append(QString(".%1").arg(d->encoding));
    else // Encoding must not ever not be set as otherwise ISO nonsense comes up.
        locale.append(QLatin1String(".UTF-8"));

    if (!d->variant.isEmpty())
        locale.append(QString("@%1").arg(d->variant));

    return locale;
}

QList<QString> Locale::systemLanguages() const
{
    Q_D(const Locale);
    QStringList list;
    foreach (Language *language, d->languages) {
        if (list.isEmpty() || list.last() != language->systemLanguageCode())
            list.append(language->systemLanguageCode());
    }
    // Must always end with en.
    if (list.isEmpty() || list.last() != QLatin1String("en"))
        list.append("en");
    return list;
}

QString Locale::systemLanguagesString() const
{
    return QStringList(systemLanguages()).join(QChar(':'));
}

} // namespace Kubuntu
