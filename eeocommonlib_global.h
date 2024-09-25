#pragma once

#include <QtCore/qglobal.h>

//#ifndef BUILD_STATIC
# if defined(EEOCOMMONLIB_LIBRARY)
#  define EEOCOMMONLIB_EXPORT Q_DECL_EXPORT
# else
#  define EEOCOMMONLIB_EXPORT Q_DECL_IMPORT
# endif
//#else
//# define EEOCOMMONLIB_EXPORT
//#endif
