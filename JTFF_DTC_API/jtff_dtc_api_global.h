#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(JTFF_DTC_API_LIB)
#  define JTFF_DTC_API_EXPORT Q_DECL_EXPORT
# else
#  define JTFF_DTC_API_EXPORT Q_DECL_IMPORT
# endif
#else
# define JTFF_DTC_API_EXPORT
#endif
