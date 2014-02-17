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

#ifndef KUBUNTU_L10N_LANGUAGECOLLECTION_H
#define KUBUNTU_L10N_LANGUAGECOLLECTION_H

#include "export.h"

#include <QObject>

namespace Kubuntu {

class Language;

class LanguageCollectionPrivate;

/**
 * \brief A LanguageCollection is a provider of all available Languages (packages).
 *
 * LanguageCollection is a wrapper around QApt querying all avilable language
 * packages and creating a set of Language objects for them.
 * This class is useful to batch query of all languages and should be preferred
 * over creating Language instances directly as it results in a more centralized
 * QApt handling.
 *
 * Additionally it takes care of parenting the Language instances and thus provides
 * a convenience management interface.
 */
class KUBUNTU_EXPORT LanguageCollection : public QObject
{
    Q_OBJECT
    friend class LanguagePrivate;
public:
    /**
     * \brief Creates new collection.
     *
     * This internally initializes the QApt backend in a blocking manner, which
     * means that this class potentially blocks the GUI thread while opening
     * the APT cache.
     *
     * \param parent QObject parent.
     */
    LanguageCollection(QObject *parent = 0);

    /** EXTERMINATE */
    ~LanguageCollection();

    /** \returns \c true if the collection cache is up-to-date. */
    bool isUpdated();

    /**
     * Updates the collection cache. Should be called before using languages()
     * to ensure that the language list is the most current set of available ones.
     * This function is async.
     * \see updated
     */
    void update();

    /** \returns a set of available languages (based on available packages) */
    QSet<Language *> languages();

signals:
    /** Emitted when the cache update progress changes \see update */
    void updateProgress(int progress);

    /** Emitted when the cache update is finished \see update */
    void updated();

private:
    const QScopedPointer<LanguageCollectionPrivate> d_ptr;
    Q_DECLARE_PRIVATE(LanguageCollection)
};

} // namespace Kubuntu

#endif // KUBUNTU_L10N_LANGUAGECOLLECTION_H
