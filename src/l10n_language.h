#ifndef KUBUNTU_LANGUAGE_H
#define KUBUNTU_LANGUAGE_H

#include "export.h"

#include <QObject>
#include <QScopedPointer>
#include <QString>

namespace Kubuntu {

class LanguagePrivate;
class KUBUNTU_EXPORT Language : public QObject
{
    Q_OBJECT
public:
    Language(QObject *parent = 0);
    Language(const QString language, QObject *parent = 0);
    ~Language();

    /** \returns the KDE language code (e.g. ca@valencia) */
    QString languageCode() const;

    /** \returns the system language code (e.g. ca) */
    QString systemLanguageCode() const;

    /** \returns \c true when all packages for this language are installed */
    bool isSupportComplete();

    /** Installs all missing packages for languages; async \see supportComplete */
    void completeSupport();

    /** \returns the Ubuntu package code (zh-hant) for a KDE l10n code (zh_TW) */
    static QString ubuntuPackgeForKdeCode(const QString &kdeCode);

    /** \returns the KDE l10n code (zh_TW) for a KDE package code (zhtw) */
    static QString kdeCodeForKdePackage(const QString &kdePkg);

signals:
    /** Emitted once completeSupport has finished. \see completeSupport */
    void supportComplete();

    /**
     * Emitted during support completion.
     *
     * \param progress Progress between 0 and 100
     */
    void supportCompletionProgress(int progress);

private:
    const QScopedPointer<LanguagePrivate> d_ptr;

    Q_DECLARE_PRIVATE(Language)
    Q_PRIVATE_SLOT(d_func(),void transactionFinished())
    Q_PRIVATE_SLOT(d_func(),void transactionError())
};

} // namespace Kubuntu

#endif // KUBUNTU_LANGUAGE_H
