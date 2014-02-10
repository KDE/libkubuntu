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

#include "l10n_language.h"

#include <KConfigGroup>
#include <KGlobal>
#include <KLocale>
#include <KProtocolManager>

#include <LibQApt/Backend>
#include <LibQApt/Transaction>

#include <QDebug>
#include <QFile>
#include <QStringBuilder>
#include <QStringList>

#include "l10n_languagecollection.h"
#include "l10n_languagecollection_p.h"

namespace Kubuntu {

#warning global static in library...
static const char *s_languageCodeMap[][3] = {
    // kdepkgcode       kdecode         ubuntupkgcode
    { "engb",           "en_GB",        "en"            },
    { "ca-valencia",    "ca@valencia",  "ca"            },
    { "ptbr",           "pt_BR",        "pt"            },
    { "zhcn",           "zh_CN",        "zh-hans"       },
    { "zhtw",           "zh_TW",        "zh-hant"       },
    { 0 }
};

QString Language::ubuntuPackageCodeForKdeCode(const QString &kdeCode)
{
    QString ubuntuPkg = kdeCode;
    for (int i = 0; s_languageCodeMap[i][0]; ++i) {
        if (kdeCode == QLatin1String(s_languageCodeMap[i][1])) {
            QString kdePackageCode = QLatin1String(s_languageCodeMap[i][0]);
            if (s_languageCodeMap[i][2] && (s_languageCodeMap[i][2] != kdePackageCode))
                ubuntuPkg = QLatin1String(s_languageCodeMap[i][2]);
            break;
        }
    }
    return ubuntuPkg;
}

QString Language::kdeLanguageCodeForKdePackageCode(const QString &kdePkg)
{
    QString kdeCode = kdePkg;
    for (int i = 0; s_languageCodeMap[i][0]; ++i) {
        if (kdePkg == QLatin1String(s_languageCodeMap[i][0])) {
            kdeCode = QLatin1String(s_languageCodeMap[i][1]);
            break;
        }
    }
    return kdeCode;
}

QString Language::kdePackageCodeForKdeLanguageCode(const QString &kdeCode)
{
    QString kdePkg = kdeCode;
    for (int i = 0; s_languageCodeMap[i][0]; ++i) {
        if (kdeCode == QLatin1String(s_languageCodeMap[i][1])) {
            kdePkg = QLatin1String(s_languageCodeMap[i][0]);
            break;
        }
    }
    return kdePkg;
}

class LanguagePrivate
{
public:
    LanguagePrivate(Language *q,
                    const QString language = QString(),
                    LanguageCollection *collection = 0);
    ~LanguagePrivate();

    /** Helper to clean up after a transaction ended. */
    void transactionCleanup();

    /** Slot handling QApt transaction errors. */
    void transactionError();

    /** Slot handling QApt transactions ending. */
    void transactionFinished(QApt::ExitStatus exitStatus);

    /**
     * Checks if a package by the name of pkgName exists and if it is not
     * installed and not already in the missingPackages set it will be added.
     *
     * \param pkgName the name of the package to possibly append
     * \see possiblyAddMissingPrefixPackage
     */
    void possiblyAddMissingPackage(const QString &pkgName);

    /**
     * Checks if prefix + kdeLanguage and prefix + ubuntuLanguage are packages
     * and whether they are installed. If they are packages and not installed
     * they will be added to missingPackages.
     *
     * \param prefix package prefix, language values are appended to form a package name
     * \see possiblyAddMissingPackage
     */
    void possiblyAddMissingPrefixPackage(const QString &prefix);

    Language *const q_ptr;
    Q_DECLARE_PUBLIC(Language)

    QString kdeLanguage;
    QString ubuntuLanguage;

    LanguageCollection *collection;
    QApt::Backend *backend;
    QSet<QApt::Package *> missingPackages;
    QApt::Transaction *transaction;

private:
    LanguagePrivate() : q_ptr(nullptr) { Q_ASSERT(q_ptr); }
    Q_DISABLE_COPY(LanguagePrivate)
};

LanguagePrivate::LanguagePrivate(Language *q,
                                 const QString language,
                                 LanguageCollection *collection)
    : q_ptr(q)
    , kdeLanguage(language)
    , ubuntuLanguage(q->ubuntuPackageCodeForKdeCode(kdeLanguage))
    , collection(collection)
    , backend()
    , transaction(nullptr)
{
    // Init backend.
    if (!collection) {
        backend = new QApt::Backend;
        backend->init();
    } else {
        // Collection is our parent, so it's no problem that we hold a ptr here.
        backend = &collection->d_ptr->backend;
        // Backend assumed to be initalized/updated by the user of the collection.
    }

    // Init languages.
    if (kdeLanguage.isEmpty()) {
        KSharedConfigPtr config = KSharedConfig::openConfig("kdeglobals", KConfig::CascadeConfig);
        KConfigGroup settings = KConfigGroup(config, "Locale");

        kdeLanguage = settings.readEntry("Language", QString::fromLatin1("en_US"));
        if (kdeLanguage.contains(QChar(':')))
            kdeLanguage = kdeLanguage.split(QChar(':')).at(0);

        ubuntuLanguage = q->ubuntuPackageCodeForKdeCode(kdeLanguage);
    }

//    qDebug() << kdeLanguage << ubuntuLanguage;
}

LanguagePrivate::~LanguagePrivate()
{
    if (!collection) // Created our own backend.
        delete backend;
}

void LanguagePrivate::transactionCleanup()
{
    transaction = nullptr;
    // Reload cache to reset pending changes, this way error'd installs will not
    // keep repeating for any subsequent attempts.
    backend->reloadCache();
}

void LanguagePrivate::transactionError()
{
    Q_Q(Language);
    if (!transaction)
        return;

    transactionCleanup();

    qDebug() << Q_FUNC_INFO;
    emit q->supportCompletionFailed();
}

void LanguagePrivate::transactionFinished(QApt::ExitStatus exitStatus)
{
    Q_Q(Language);
    if (!transaction)
        return;

    transactionCleanup();

    qDebug() << Q_FUNC_INFO << exitStatus;

    switch (exitStatus) {
    case QApt::ExitSuccess:
        emit q->supportComplete();
        break;
    case QApt::ExitCancelled:
    case QApt::ExitFailed:
    case QApt::ExitPreviousFailed:
    case QApt::ExitUnfinished:
        emit q->supportCompletionFailed();
        break;
    }
}

void LanguagePrivate::possiblyAddMissingPackage(const QString &pkgName)
{
    QApt::Package *package = 0;
    package = backend->package(pkgName);
    if (package && !package->isInstalled() && !missingPackages.contains(package))
        missingPackages.insert(package);
}

void LanguagePrivate::possiblyAddMissingPrefixPackage(const QString &prefix)
{
    possiblyAddMissingPackage(prefix % kdeLanguage);
    possiblyAddMissingPackage(prefix % ubuntuLanguage);
}

Language::Language()
{
}

Language::Language(const QString language, QObject *parent)
    : QObject(parent)
    , d_ptr(new LanguagePrivate(this, language, qobject_cast<LanguageCollection *>(parent)))
{
}

Language::~Language()
{
}

QString Language::kdeLanguageCode() const
{
    Q_D(const Language);
    return d->kdeLanguage;
}

QString Language::kdePackageCode() const
{
    Q_D(const Language);
    // This is not cached because this is the only use we have internally and
    // the function ought not be called that often (mostly once on init and
    // once if the package is being installed, though even latter is unlikely).
    return kdePackageCodeForKdeLanguageCode(d->kdeLanguage);
}

QString Language::ubuntuPackageCode() const
{
    Q_D(const Language);
    return d->ubuntuLanguage;
}

QString Language::systemLanguageCode() const
{
    Q_D(const Language);
    QString code = d->kdeLanguage;
    // Strip all random nonesense away.
    code = code.split(QChar('@')).at(0).split(QChar('_')).at(0);
    return code;
}

bool Language::isSupportComplete()
{
    Q_D(Language);

    if (!d->missingPackages.isEmpty())
        return false;

    QFile file("/usr/share/language-selector/data/pkg_depends");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return true; // Must assume support is complete if we can't read the dep file :S
    }

    // List of valid column identifiers
    QStringList columns;
    columns << QLatin1String("tr") << QLatin1String("wa")
            << QLatin1String("fn") << QLatin1String("in");
    while (!file.atEnd()) {
        QString line = file.readLine();
        QStringList pkgDepends = line.split(':');

        if (pkgDepends.isEmpty())
            continue;

        // Check it's a valid depends line.
        if (!columns.contains(pkgDepends.at(0)))
            continue;

        // Check if next value is for all langs or for this one specifically.
        if (pkgDepends.at(1).isEmpty() || pkgDepends.at(1) == d->ubuntuLanguage) {
            //if it is always to be installed, go for it
            if (pkgDepends.at(2).isEmpty()) {
                QString prefix = pkgDepends.at(3);
                prefix.chop(1);

                d->possiblyAddMissingPrefixPackage(prefix);
            } else {
                //if it is only if another package is installed check that
                QApt::Package *dependency = d->backend->package(pkgDepends.at(2));
                if (dependency && !dependency->installedVersion().isEmpty()) {
                    QString prefix = pkgDepends.at(3);
                    prefix.chop(1);

                    // There are per-language packages such as kde-l10n-xx and meta ones such as chromium-l10n.
                    // Former needs concat whereas latter needs as-is usage
                    if (prefix.right(1) == QChar('-')) { // Per-language
                        d->possiblyAddMissingPrefixPackage(prefix);
                    } else { // Meta
                        d->possiblyAddMissingPackage(prefix);
                    }
                }
            }
        }
    }
    if (d->missingPackages.isEmpty())
        return true;
    return false;
}

void Language::completeSupport()
{
    Q_D(Language);

    if (d->missingPackages.isEmpty())
        return;

    foreach (QApt::Package *p, d->missingPackages)
        qDebug() << "installing" << p->name();
    d->backend->markPackages(d->missingPackages.toList(), QApt::Package::ToInstall);
    d->transaction = d->backend->commitChanges();

    // Provide proxy/locale to the transaction
    if (KProtocolManager::proxyType() == KProtocolManager::ManualProxy)
        d->transaction->setProxy(KProtocolManager::proxyFor("http"));

    d->transaction->setLocale(QLatin1String(setlocale(LC_MESSAGES, 0)));

    connect(d->transaction, SIGNAL(progressChanged(int)),
            this, SIGNAL(supportCompletionProgress(int)));
    connect(d->transaction, SIGNAL(finished(QApt::ExitStatus)),
            this, SLOT(transactionFinished(QApt::ExitStatus)));
    connect(d->transaction, SIGNAL(errorOccurred(QApt::ErrorCode)),
            this, SLOT(transactionError()));
    qDebug() << "start";
    d->transaction->run();
}

} // namespace Kubuntu

#include "l10n_language.moc"
