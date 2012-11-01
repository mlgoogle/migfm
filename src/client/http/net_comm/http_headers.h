// Dao1 Technology
// Copyright Dao1 Pty, Ltd.
// All Rights Reserved. Commercial in confidence.

// WARNING: This computer program is protected by copyright law and
// international treaties. Unauthorized use, reproduction or distribution of
// this program, or any portion of this program, may result in the imposition
// of civil and criminal penalties as provided by law.

#ifndef _NET_COMM_HTTP_HEADERS_H__
#define _NET_COMM_HTTP_HEADERS_H__
#pragma once

#include "net_comm/export.h"

namespace net {

	// A collection of HTTP header names.
	// @see <a href="http://rfc-ref.org/RFC-TEXTS/2616/">Hypertext Transfer
	// Protocol -- HTTP/1.1 (RFC 1616)</a>

	class HttpHeaders {
	public:
		// kContentEncoding always is "Content-Encoding"
		PE_EXPORT static const char kContentEncoding[];

		// kContentLanguage always is "Content-Language"
		PE_EXPORT static const char kContentLanguage[];

		// kContentLength always is "Content-Length"
		PE_EXPORT static const char kContentLength[];

		// kContentLocation always is "Content-Location"
		PE_EXPORT static const char kContentLocation[];

		// kContentDisposition always is "Content-Disposition"
		PE_EXPORT static const char kContentDisposition[];

		// kContentMD5 always is "Content-MD5"
		PE_EXPORT static const char kContentMD5[];

		// kContentType always is "Content-type"
		PE_EXPORT static const char kContentType[];

		// kLastModified always is "Last-Modified"
		PE_EXPORT static const char kLastModified[];

		// kLocation always is "Location"
		PE_EXPORT static const char kLocation[];

		// kLocationAlias alway is "location"
		PE_EXPORT static const char kLocationAlias[];
	};

}

#endif
