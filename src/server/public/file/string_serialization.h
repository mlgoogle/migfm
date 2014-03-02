#ifndef _MIG_FM_PUBLIC_BASIC_STRING_SERIALIZATION_H__
#define _MIG_FM_PUBLIC_BASIC_STRING_SERIALIZATION_H__

namespace serial{

#define PACK_INT32      4
#define PACK_INT64      8
#define PACK_BUFLEN     -1
#define PACK_BUFFER     -2

#ifdef USERIDX_IS_64BIT
#define PACK_USERIDX    PACK_INT64
#else
#define PACK_USERIDX    PACK_INT32
#endif

bool MemSerialition(void* data,int len,...); 

}
#endif