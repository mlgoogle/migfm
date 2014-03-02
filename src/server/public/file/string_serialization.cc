#include "string_serialization.h"
#include "basic/basictypes.h"
#include <stdarg.h>
#include <cstdarg>

namespace serial{

bool MemSerialition(void* data,int len,...){
   
	va_list ap;
	char* databuf = (char*)data+sizeof(int);
	int datalen = len - sizeof(int);
	int32 field_type = 0;
	int32 field_int32 = 0;
	long long field_int64 = 0;
	int32 buflen = 0;
	char *buffer = NULL;
	int32 offset = 0;

	if (data==NULL || len<0)
		return false;
	if (len==0)
		return true;

	//×Ü³¤
	*(int32*)(data) = len;
	va_start (ap,datalen);

	do {
		field_type = va_arg(ap,int);
		if (field_type==0){
			va_end(ap);
			return true;
		}

		switch (field_type){
			case PACK_INT32:
				field_int32 = va_arg(ap,int);
				*(int*)(databuf + offset) = field_int32;
				offset += PACK_INT32;
				break;
			case PACK_INT64:
				field_int64 = va_arg (ap, long long);
				*(long long *)(databuf + offset) = field_int64;
				offset += PACK_INT64;
				break;

			case PACK_BUFLEN:
				buflen = va_arg (ap, int);
				break;

			case PACK_BUFFER:
				if (buflen <= 0)
					break;
				buffer = va_arg (ap, char *);
				//strncpy ((databuf + offset), buffer, buflen);
				memcpy ((databuf + offset), buffer, buflen);
				offset = offset + buflen;
				buflen = 0;
				break;

			default:
				va_end (ap);
				return false;
		}
	} while (offset < len);
	va_end (ap);
	return true;
}

}