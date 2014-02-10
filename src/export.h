/*
  Copyright (C) 2014 Harald Sitter <apachelogger@ubuntu.com>

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

#ifndef EXPORT_H
#define EXPORT_H

#include <QtCore/QtGlobal>

/**
 * \file export.h
 * Library export macros to handle symbol import/export as well as marking
 * functions as deprecated.
 */

#ifndef KUBUNTU_EXPORT
# if defined Q_WS_WIN
#  ifdef MAKE_KUBUNTU_LIB /* We are building this library */
#   define KUBUNTU_EXPORT Q_DECL_EXPORT
#  else /* We are using this library */
#   define KUBUNTU_EXPORT Q_DECL_IMPORT
#  endif
# else /* UNIX */
#  ifdef MAKE_KUBUNTU_LIB /* We are building this library */
#   define KUBUNTU_EXPORT Q_DECL_EXPORT
#  else /* We are using this library */
#   define KUBUNTU_EXPORT Q_DECL_IMPORT
#  endif
# endif
#endif

#ifndef KUBUNTU_DEPRECATED
# define KUBUNTU_DEPRECATED Q_DECL_DEPRECATED
#endif

#ifndef KUBUNTU_EXPORT_DEPRECATED
# define KUBUNTU_EXPORT_DEPRECATED Q_DECL_DEPRECATED KUBUNTU_EXPORT
#endif

#endif // EXPORT_H
