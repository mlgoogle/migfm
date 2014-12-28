#ifndef MIG_FM_PUBLIC_BASIC_MD5_H__
#define MIG_FM_PUBLIC_BASIC_MD5_H__
#include <string>
#include <cstring>

/* Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All

rights reserved.

License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD5 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.

License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.
 */




 #ifndef md5_INCLUDED
 #  define md5_INCLUDED

 /*
  * This code has some adaptations for the Ghostscript environment, but it
  * will compile and run correctly in any environment with 8-bit chars and
  * 32-bit ints.  Specifically, it assumes that if the following are
  * defined, they have the same meaning as in Ghostscript: P1, P2, P3,
  * ARCH_IS_BIG_ENDIAN.
  */

 typedef unsigned char md5_byte_t; /* 8-bit byte */
 typedef unsigned int md5_word_t; /* 32-bit word */

 /* Define the state of the MD5 Algorithm. */
 typedef struct md5_state_s {
     md5_word_t count[2];    /* message length in bits, lsw first */
     md5_word_t abcd[4];     /* digest buffer */
     md5_byte_t buf[64];     /* accumulate block */
 } md5_state_t;

 #ifdef __cplusplus
 extern "C"
 {
 #endif

 /* Initialize the algorithm. */
 #ifdef P1
 void md5_init(P1(md5_state_t *pms));
 #else
 void md5_init(md5_state_t *pms);
 #endif

 /* Append a string to the message. */
 #ifdef P3
 void md5_append(P3(md5_state_t *pms, const md5_byte_t *data, int nbytes));
 #else
 void md5_append(md5_state_t *pms, const md5_byte_t *data, int nbytes);
 #endif

 /* Finish the message and return the digest. */
 #ifdef P2
 void md5_finish(P2(md5_state_t *pms, md5_byte_t digest[16]));
 #else
 void md5_finish(md5_state_t *pms, md5_byte_t digest[16]);
 #endif

 void md5_passwd(char *oldpasswd, char *md5_passwd);

 #ifdef __cplusplus
 }  /* end extern "C" */
 #endif

#endif

namespace base{

class MD5Sum
{
public:
	MD5Sum();
	MD5Sum(const std::string& sSource);
	MD5Sum(const unsigned char* pachSource, unsigned long nLen);

	std::string Calculate(const std::string& sSource);
	std::string Calculate(const unsigned char* pachSource, unsigned long nLen);

	std::string GetHash() const;
	const unsigned char* GetRawHash() const { return m_rawHash; }

private:
	std::string			m_sHash;
	unsigned char	m_rawHash[16];
};

}
#endif
