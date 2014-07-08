/*
  Copyright (C) 2014 Harald Sitter <apachelogger@kubuntu.org>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) version 3, or any
  later version accepted by the membership of KDE e.V. (or its
  successor approved by the membership of KDE e.V.), which shall
  act as a proxy defined in Section 6 of version 3 of the license.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "l10n_locale.h"

#include "l10n_language.h"

#include <KConfigGroup>
#include <KSharedConfig>

#include <QDebug>
#include <QDir>
#include <QFileInfo>

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
    QString mainLanguage = _languages.at(0)->kdeLanguageCode();
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
    // en_US is excepted from this rule as 'en' is the uniform base language
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
    foreach (Language *lang, languages) {
        // We do manual life time control because Locale is no QObject.
        lang->setParent(nullptr);
    }
    d->init(languages, country);
}

Locale::Locale(const QList<QString> &kdeLanguageCodes, const QString &country)
    : d_ptr(new LocalePrivate)
{
    Q_D(Locale);

    qDebug() << kdeLanguageCodes;
    LanguagePtrList languages;
    foreach (const QString &languageCode, kdeLanguageCodes) {
        languages.append(new Language(languageCode));
    }

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

// NOTE: not public as there is no external use for this right now
static bool isLocaleStringValid(const QString &locale)
{
    QProcess process;
    process.start(QLatin1String("locale"), QStringList() << QLatin1String("-a"));
    bool finished = process.waitForFinished(30 * 1000); // If locale takes more than 30 secs something is very wrong
    if (!finished || process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        return false;
    }

    // We need to split the encoding from the locale as there's different
    // formats floating around as supported (e.g. utf-8 vs. utf8)
    // Also we only support utf-8 anyway, so it doesn't matter much as we assume
    // it is always supported on Kubuntu systems.
    const QString lowerLocale = locale.toLower().split(QChar('.')).first();
    while (process.canReadLine()) {
        QString line = QString(process.readLine()).remove(QChar('\n'));
        line = line.split(QChar('.')).first();
        line = line.toLower();
        if (line == lowerLocale) {
            return true;
        }
    }

    // If locale -a didn't give a perfect match for the defined locale we must
    // assume it is invalid.
    return false;
}

// TODO: we likely should introduce GUI backing for only partially applying
//       the configured settings. Then again, when someone is using a
//       non-standard locale they likely won't notice or care about random
//       localization inconsistencies.
bool Locale::writeToFile(const QString &filePath)
{
    QDir dir = QFileInfo(filePath).absoluteDir();
    dir.mkpath(dir.absolutePath());

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Couldn't open file for writing:" << filePath;
        return false;
    }

    QTextStream stream(&file);

    // Due to the fact that KDE country and language are independent settings we
    // can construct an entirely invalid locale such as en_AT if the user wishes
    // to use english with Austrian format settings. We cannot accurately
    // translate this to a glib locale, so we must quite simply discard any
    // and all attempts at getting the format defining variables right and
    // can only set LANGUAGE in this case. As language either contains a 
    // (hopefully) valid locale as used by KDE (e.g. en_GB, zh_TW ...) or
    // language-only identifiers (e.g. de).
    // NOTE: LANGUAGE can contain any old nonsense as it has a built-in fallback
    //       logic and we always force en as final option explicitly.
    qDebug() << QString("export LANGUAGE=%1").arg(systemLanguagesString());
    stream << QString("export LANGUAGE=%1").arg(systemLanguagesString()) << endl;
    if (isLocaleStringValid(systemLocaleString())) {
        qDebug() << QString("export LANG=%1").arg(systemLocaleString());
        stream << QString("export LANG=%1").arg(systemLocaleString()) << endl;
        static QStringList lcVariables;
        if (lcVariables.isEmpty()) {
            lcVariables << QLatin1String("LC_NUMERIC")
                        << QLatin1String("LC_TIME")
                        << QLatin1String("LC_MONETARY")
                        << QLatin1String("LC_PAPER")
                        << QLatin1String("LC_IDENTIFICATION")
                        << QLatin1String("LC_NAME")
                        << QLatin1String("LC_ADDRESS")
                        << QLatin1String("LC_TELEPHONE")
                        << QLatin1String("LC_MEASUREMENT");
        }
        foreach (const QString &variable, lcVariables) {
            qDebug() << QString("export %1=%2").arg(variable, systemLocaleString());
            stream << QString("export %1=%2").arg(variable, systemLocaleString()) << endl;
        }
    }

    file.close();
    return true;
}

} // namespace Kubuntu
