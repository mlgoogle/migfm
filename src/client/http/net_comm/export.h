// Dao1 Technology
// Copyright Dao1 Pty, Ltd.
// All Rights Reserved. Commercial in confidence.

// WARNING: This computer program is protected by copyright law and
// international treaties. Unauthorized use, reproduction or distribution of
// this program, or any portion of this program, may result in the imposition
// of civil and criminal penalties as provided by law.

#ifndef _HTTP_NET_COMM_EXPORT_H__
#define _HTTP_NET_COMM_EXPORT_H__
#pragma once

#include "net_comm/config_macros.h"

#if defined (PE_AS_STATIC_LIBS)
#  if !defined (PE_HAS_DLL)
#    define PE_HAS_DLL 0
#  endif
#else
#  if !defined (PE_HAS_DLL)
#    define PE_HAS_DLL 1
#  endif
#endif

#if defined (PE_HAS_DLL)
#  if (PE_HAS_DLL==1)
#    if defined (PE_BUILD_DLL)
#      define PE_EXPORT PE_EXPORT_FLAG
#    else
#      define PE_EXPORT PE_IMPORT_FLAG
#    endif
#  else
#    define PE_EXPORT
#  endif /* PE_HAS_DLL==1 */
#else
#  define PE_EXPORT
#endif /* PE_HAS_DLL */

#endif /* BASE_EXPORT_H__ */
