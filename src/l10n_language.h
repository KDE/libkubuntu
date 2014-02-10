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

#ifndef KUBUNTU_LANGUAGE_H
#define KUBUNTU_LANGUAGE_H

#include "export.h"

#include <QObject>
#include <QScopedPointer>
#include <QString>

namespace Kubuntu {

class LanguagePrivate;

/**
 * Language is the internal representation and management unit for KDE languages.
 * Instances are created from a KDE language code and allow the installation
 * of system wide dependencies required to allow for a complete localized
 * user experience.
 *
 * Whenever possible Languages should be obtained from a LanguageCollection
 * rather than getting manually constructed.
 */
class KUBUNTU_EXPORT Language : public QObject
{
    Q_OBJECT
public:
    /** Constructs an instance with a language set.
     *
     * \param language the KDE language code to use
     * \param parent parent of the object, can be a LanguageCollection in which
     *        case the internal QApt backend will be shared with the collection.
     */
    explicit Language(const QString kdeLanguageCode, QObject *parent = 0);

    /** Destruct this thing! */
    ~Language();

    /** \returns the KDE language code (e.g. ca@valencia) */
    QString kdeLanguageCode() const;

    /** \returns the KDE package code for this language */
    QString kdePackageCode() const;

    /** \returns Ubuntu package code for this language */
    QString ubuntuPackageCode() const;

    /** \returns the system language code (e.g. ca) */
    QString systemLanguageCode() const;

    /**
     * \returns \c true when all packages for this language are installed or
     * the actual state can not be detected.
     */
    bool isSupportComplete();

    /** Installs all missing packages for languages; async \see supportComplete */
    void completeSupport();

    /** \returns the Ubuntu package code (zh-hant) for a KDE l10n code (zh_TW) */
    static QString ubuntuPackageCodeForKdeCode(const QString &kdeLanguageCode);

    /** \returns the KDE l10n code (zh_TW) for a KDE package code (zhtw) */
    static QString kdeLanguageCodeForKdePackageCode(const QString &kdePkg);

    /** \returns the KDE package code (zhtw) for the KDE package code (zh_TW) */
    static QString kdePackageCodeForKdeLanguageCode(const QString &kdeLanguageCode);

signals:
    /** Emitted once completeSupport has finished. \see completeSupport */
    void supportComplete();

    /** Emitted if an error is encounted during support completion \see completeSupport */
    void supportCompletionFailed();

    /**
     * Emitted during support completion.
     *
     * \param progress Progress between 0 and 100
     */
    void supportCompletionProgress(int progress);

private:
    // Prevent construction of definitely invalid Language instances.
    Language();

    const QScopedPointer<LanguagePrivate> d_ptr;
    Q_DECLARE_PRIVATE(Language)
    Q_PRIVATE_SLOT(d_func(),void transactionFinished(QApt::ExitStatus))
    Q_PRIVATE_SLOT(d_func(),void transactionError())
};

} // namespace Kubuntu

#endif // KUBUNTU_LANGUAGE_H
