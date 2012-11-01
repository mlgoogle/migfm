// Dao1 Technology
// Copyright Dao1 Pty, Ltd.
// All Rights Reserved. Commercial in confidence.

// WARNING: This computer program is protected by copyright law and
// international treaties. Unauthorized use, reproduction or distribution of
// this program, or any portion of this program, may result in the imposition
// of civil and criminal penalties as provided by law.

#ifndef _HTTP_NET_COMM_CONFIG_WIN32_H__
#define _HTTP_NET_COMM_CONFIG_WIN32_H__
#pragma once

#ifndef BASE_CONFIG_MACROS_H__
#error Do not #include this file directly. #include "base/config_macros.h" \
    instead
#endif

#define PE_EXPORT_FLAG __declspec (dllexport)
#define PE_IMPORT_FLAG __declspec (dllimport)

#endif
