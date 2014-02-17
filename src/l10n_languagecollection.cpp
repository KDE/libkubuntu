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

#include "l10n_languagecollection.h"
#include "l10n_languagecollection_p.h"

#include <QSet>

#include "l10n_language.h"

namespace Kubuntu {

LanguageCollectionPrivate::LanguageCollectionPrivate()
{
}

LanguageCollection::LanguageCollection(QObject *parent)
    : QObject(parent)
    , d_ptr(new LanguageCollectionPrivate)
{
    Q_D(LanguageCollection);
    d->backend.init();
    connect(&d->backend, SIGNAL(xapianUpdateProgress(int)),
            this, SIGNAL(updateProgress(int)));
    connect(&d->backend, SIGNAL(xapianUpdateFinished()),
            this, SIGNAL(updated()));
}

LanguageCollection::~LanguageCollection()
{
}

bool LanguageCollection::isUpdated()
{
    Q_D(LanguageCollection);
    if (d->backend.openXapianIndex() && d->backend.xapianIndexNeedsUpdate())
        return false;
    return true;
}

void LanguageCollection::update()
{
    Q_D(LanguageCollection);
    if (isUpdated())
        d->backend.updateXapianIndex();
    else
        emit updated();
}

QSet<Language *> LanguageCollection::languages()
{
    Q_D(LanguageCollection);

    // Make sure the xapian cache is open at this point.
    d->backend.openXapianIndex();

    const QString queryString = QLatin1String("kde-l10n-");
    const int queryStringLength = queryString.size();

    QSet<Language *> languages;
    foreach (const QApt::Package *package, d->backend.search(queryString)) {
        const QString packageName = QString(package->name());
        if (packageName.startsWith(QLatin1String("kde-l10n-"))) {
            QString languageCode = packageName.mid(queryStringLength);

            // It is more convenient to translate to code here rather than inside
            // the Language ctor, as we like to utilize init lists in there.
            // Also there ought not be a use case to construct a Language
            // from a Package outside the collection.
            Language *language = new Language(Language::kdeLanguageCodeForKdePackageCode(languageCode), this);
            languages.insert(language);
        }
    }

    return languages;
}

} // namespace Kubuntu

#include "l10n_languagecollection.moc"
