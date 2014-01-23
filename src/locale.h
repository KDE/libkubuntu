#ifndef KUBUNTU_LOCALE_H
#define KUBUNTU_LOCALE_H

#include "export.h"

#include <QList>
#include <QScopedPointer>

namespace Kubuntu {

class Language;

class LocalePrivate;
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
     *
     * \warning Owernship of the Language instances moves to the Locale, once
     *          the constructor returns you should throw away your references as
     *          to avoid parenting/scope problems.
     *
     * \param languages list of valid Language instances.
     * \param country the country for this locale.
     */
    Locale(const QList<Language *> &languages, const QString &country);

    /** Destructor. */
    ~Locale();

    /** \returns the system locale string (e.g. ca_ES.UTF-8@valencia) */
    QString systemLocaleString() const;

    /** \returns a list of all system languages; always ends with english */
    QList<QString> systemLanguages() const;

    /** \returns the system languages string (e.g. de:fr:en); always ends with en */
    QString systemLanguagesString() const;

#warning todo
//    bool isValid() const;

private:
    const QScopedPointer<LocalePrivate> d_ptr;
    Q_DECLARE_PRIVATE(Locale)
};

} // namespace Kubuntu

#endif // KUBUNTU_LOCALE_H
