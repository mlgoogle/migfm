#ifndef  _MFS_ENGINE_H__
#define  _MFS_ENGINE_H__
#include <stdio.h>
#include <string>
#include "tfs_client_api.h"
#include "basic/basictypes.h"

#if defined (_TFS_)
typedef tfs::common::TfsFileStat      FS_STAT;
typedef tfs::common::TfsStatType      FS_MODE;
typedef tfs::common::OptionFlag       FS_OPTION_FLAG;
typedef tfs::common::TfsUnlinkType    FS_UNLINKTYPE;
#endif

namespace filestorage{

enum{
    TFS_TYPE = 0,
};

class MFSEngine{
public:
	static MFSEngine* Create(int32 type);
public:
    virtual int32 Initialize(const char* host) = 0;
    
    virtual int32 OpenFile(const char* file_name,const char* suffix,
                           const int32 flags,const char* key) = 0;
    
    virtual int32 OpenFile(const char* file_name,const char* suffix,
    	                   const char* host,const int32 flags,const char* key) = 0;

    virtual int32 WriteBuffer(const int fd,const void* data,int64 count)=0;
    
    virtual int32 ReadBuffer(const int fd,void* buf,const int64 count)=0;
    
    virtual int32 ReadBuffer(const int fd,void* buf,const int64 count,FS_STAT* file_info) = 0;
    
    virtual int32 GetFileStat(const int fd,FS_STAT* buf,FS_MODE mode) = 0;

    virtual int32 CloseFile(const int fd,char* ret_fs_name,const int32 ret_fs_name_len) =0;

    virtual int32 DeleteFile(int64& file_size,const char* file_name,const char* suffix,
                             const FS_UNLINKTYPE action,const FS_OPTION_FLAG option_flag) = 0;

    virtual int32 DeleteFile(int64& file_size,const char* file_name,const char* suffix,
                             const char* host, const FS_UNLINKTYPE action,
                             const FS_OPTION_FLAG option_flags) = 0;
   
    virtual int64 SaveBuffer(char* ret_fs_name,
                           const int32 ret_fs_name_len,
                           const char* buf,const int64 file_length,
                           const int32 flag,const char* suffix,
                           const char* host,
                           const char* key) = 0;

    virtual int64 SaveFile(char* ret_fs_name,
                           const int32 ret_fs_name_len,
                           const char* local_file,const int32 flag,
                           const char* suffix,const char* host) = 0;

    virtual int32 FetchFile(const char* local_file,
                            const char* file_name,const char* suffix,
                            const char* host) = 0;

    virtual int32 FetchFile(int64& ret_count,char* buf,const int64 count,
                            const char*  file_name,const char*  suffix,
                            const char*  host) = 0; 

};

}
#endif
