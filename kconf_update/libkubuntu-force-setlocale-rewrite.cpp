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

#include <QDebug>
#include <QFile>
#include <QString>
#include <QStringBuilder>

#include <KGlobal>
#include <KStandardDirs>

#include <l10n_locale.h>

int main(int argc, char *argv[])
{
    const QString envPath = KGlobal::dirs()->localkdedir() % QLatin1String("/env");
    const QString envFile = envPath % QLatin1String("/setlocale.sh");
    qDebug() << envFile;
    if (QFile::exists(envFile)) {
        // Only rewrite the file iff it already exists, otherwise a user may have
        // removed it and we restore it which doesn't seem particularly nice.
        Kubuntu::Locale locale;
        locale.writeToFile(envFile);
    }
    return 0;
}
