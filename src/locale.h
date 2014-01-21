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
