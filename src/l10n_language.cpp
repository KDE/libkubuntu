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

QString Language::ubuntuPackgeForKdeCode(const QString &kdeCode)
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

QString Language::kdeCodeForKdePackage(const QString &kdePkg)
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

class LanguagePrivate
{
public:
    LanguagePrivate(Language *q, const QString language = QString());

    /** Helper to clean up after a transaction ended. */
    void transactionCleanup();

    /** Slot handling QApt transactions ending. */
    void transactionFinished(int exitStatus);

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

    QScopedPointer<QApt::Backend> backend;
    QSet<QApt::Package *> missingPackages;
    QApt::Transaction *transaction;

private:
    LanguagePrivate() : q_ptr(nullptr) {}
    Q_DISABLE_COPY(LanguagePrivate)
};

LanguagePrivate::LanguagePrivate(Language *q, const QString language)
    : q_ptr(q)
    , kdeLanguage(language)
    , ubuntuLanguage(q->ubuntuPackgeForKdeCode(kdeLanguage))
    , backend(new QApt::Backend)
    , transaction(nullptr)
{
    if (kdeLanguage.isEmpty()) {
        KSharedConfigPtr config = KSharedConfig::openConfig("kdeglobals", KConfig::CascadeConfig);
        KConfigGroup settings = KConfigGroup(config, "Locale");

        kdeLanguage = settings.readEntry("Language", QString::fromLatin1("en_US"));
        if (kdeLanguage.contains(QChar(':')))
            kdeLanguage = kdeLanguage.split(QChar(':')).at(0);

        ubuntuLanguage = q->ubuntuPackgeForKdeCode(kdeLanguage);
    }

    qDebug() << kdeLanguage << ubuntuLanguage;

#warning backend not reloading, backend not opening xapian etc.etc.
    backend->init();
}

void LanguagePrivate::transactionCleanup()
{
    // Reload cache to reset pending changes, this way error'd installs will not
    // keep repeating for any subsequent attempts.
    backend->reloadCache();
    transaction = nullptr;
}

void LanguagePrivate::transactionFinished(int exitStatus)
{
    Q_Q(Language);
    if (!transaction)
        return;

    transactionCleanup();

    switch (static_cast<QApt::ExitStatus>(exitStatus)) {
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
    if (package && !package->isInstalled() && missingPackages.contains(package))
        missingPackages.insert(package);
}

void LanguagePrivate::possiblyAddMissingPrefixPackage(const QString &prefix)
{
    possiblyAddMissingPackage(prefix % kdeLanguage);
    possiblyAddMissingPackage(prefix % ubuntuLanguage);
}

Language::Language(QObject *parent)
    : Language(QString(), parent)
{}

Language::Language(const QString language, QObject *parent)
    : QObject(parent)
    , d_ptr(new LanguagePrivate(this, language))
{
}

Language::~Language()
{
}

QString Language::languageCode() const
{
    Q_D(const Language);
    return d->kdeLanguage;
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
            this, SLOT(transactionFinished(int)));
    qDebug() << "start";
    d->transaction->run();
}

} // namespace Kubuntu

#include "l10n_language.moc"
