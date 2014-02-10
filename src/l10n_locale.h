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

#ifndef KUBUNTU_LOCALE_H
#define KUBUNTU_LOCALE_H

#include "export.h"

#include <QList>
#include <QScopedPointer>

namespace Kubuntu {

class Language;

class LocalePrivate;

/**
 * \brief The System Locale
 *
 * This is a convenience wrapper to translate a Kubuntu/KDE Language to
 * a system locale value.
 */
class KUBUNTU_EXPORT Locale
{
public:
    /**
     * Constructs a default Locale. The Locale's values are based on KDE's
     * localization configuration.
     */
    Locale();

    /**
     * Constructs a Locale from a predefined set of languages and a country.
     * The LOCALE is built from the first language in the list and the country.
     * The LANGUAGES list is built according to the passed languages list.
     *
     * \code
     * QList<..> list << "en" << "de" << "fr";
     * Locale(list, "US").systemLocaleString(); // en_US.UTF-8
     * Locale(list, "US").systemLanguagesString(); // en:de:fr:en
     * \endcode
     *
     * \warning Owernship of the Language instances moves to the Locale, once
     *          the constructor returns you should throw away your references as
     *          to avoid parenting/scope problems.
     *
     * \param languages list of valid Language instances.
     * \param country the country for this locale.
     */
    Locale(const QList<Language *> &languages, const QString &country);

    /**
     * Overload constructor, internally constructs Language objects from the
     * passed kdeLanguageCodes. This constructor should be avoided whenever you
     * actually want to do more than just know the LOCALE of a random set of
     * languages as constructing Languages outside a LanguageCollection is
     * substantially more costly.
     *
     * \see Locale(const QList<Language *> &languages, const QString &country);
     * \param kdeLanguageCodes list of valid KDE language codes.
     * \param country the country for this locale.
     */
    Locale(const QList<QString> &kdeLanguageCodes, const QString &country);

    /** Destructor. */
    ~Locale();

    /** \returns the system locale string (e.g. ca_ES.UTF-8@valencia) */
    QString systemLocaleString() const;

    /** \returns a list of all system languages; always ends with english */
    QList<QString> systemLanguages() const;

    /** \returns the system languages string (e.g. de:fr:en); always ends with en */
    QString systemLanguagesString() const;

    /**
     * \brief Writes a set of locale exports to a file.
     * This will write sh style system locale exports to a file.
     *
     * Exported values are:
     *  - LANG=systemLocaleString()
     *  - LANGUAGE=systemLanguages().join(':').append(":en")
     *  - LC_{NUMERIC,TIME,MONETARY,...}=systemLocaleString()
     *
     * \param filePath the absolute file path of the file to write. parent
     *                 directories will be created if not present.
     * \return \c true on success, \c false if file could not be opened.
     */
    bool writeToFile(const QString &filePath);

private:
    const QScopedPointer<LocalePrivate> d_ptr;
    Q_DECLARE_PRIVATE(Locale)
};

} // namespace Kubuntu

#endif // KUBUNTU_LOCALE_H
