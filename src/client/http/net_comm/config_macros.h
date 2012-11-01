// Dao1 Technology
// Copyright Dao1 Pty, Ltd.
// All Rights Reserved. Commercial in confidence.

// WARNING: This computer program is protected by copyright law and
// international treaties. Unauthorized use, reproduction or distribution of
// this program, or any portion of this program, may result in the imposition
// of civil and criminal penalties as provided by law.

#ifndef BASE_CONFIG_MACROS_H__
#define BASE_CONFIG_MACROS_H__
#pragma once

#if defined (__linux__)
#include "net_comm/config_posix.h"
#elif defined (WIN32)
#include "net_comm/config_win32.h"
#else
#error Unsupported operating system.
#endif

#endif
