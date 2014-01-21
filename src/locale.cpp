#include "locale.h"

#include "language.h"

#include <KConfigGroup>
#include <KSharedConfig>

#include <QDebug>

namespace Kubuntu {

typedef QList<Language *> LanguagePtrList;

class LocalePrivate
{
public:
    LocalePrivate();

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

void LocalePrivate::init(LanguagePtrList _languages, QString _country)
{
    languages = _languages;
    country = _country;
    const QString mainLanguage = _languages.at(0)->languageCode();
    qDebug() << mainLanguage;
    if (mainLanguage.contains(QChar('@'))) {
        QStringList components = mainLanguage.split(QChar('@'));
        variant = components.at(1);
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
    if (list.isEmpty() || list.last() != QLatin1String("en"))
        list.append("en");
    return list;
}

QString Locale::systemLanguagesString() const
{
    return QStringList(systemLanguages()).join(QChar(':'));
}

} // namespace Kubuntu
