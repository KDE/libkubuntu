#ifndef EXPORT_H
#define EXPORT_H

#include <QtCore/QtGlobal>

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
