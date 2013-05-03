#ifndef _TFS_ENGINE_IMPL_H__
#define _TFS_ENGINE_IMPL_H__
#include <stdio.h>
#include <stdlib.h>
#include "fs.h"

namespace filestorage{

class TFSEnginImpl:public MFSEngine{

public:
    TFSEnginImpl(void);
    ~TFSEnginImpl(void);

    int32 Initialize(const char* host);
 
    int32 OpenFile(const char* file_name,const char* suffix,
                   const int32 flags,const char* key = NULL);
    
    int32 OpenFile(const char* file_name,const char* suffix,
    	           const char* host,const int32 flags,const char* key = NULL);

    int32 WriteBuffer(const int fd,const void* data,int64 count);

    int32 ReadBuffer(const int fd,void* buf,const int64 count);
    
    int32 ReadBuffer(const int fd,void* buf,const int64 count,FS_STAT* file_info);
     
    int32 GetFileStat(const int fd,FS_STAT* buf,FS_MODE mode=tfs::common::NORMAL_STAT);
   
    int32 CloseFile(const int fd,char* ret_fs_name,const int32 ret_fs_name_len=0);

    int32 DeleteFile(int64& file_size,const char* file_name,const char* suffix,
                     const FS_UNLINKTYPE action = tfs::common::DELETE,
                     const FS_OPTION_FLAG option_flag = tfs::common::TFS_FILE_DEFAULT_OPTION);

    int32 DeleteFile(int64& file_size,const char* file_name,const char* suffix,
                     const char* host, const FS_UNLINKTYPE action = tfs::common::DELETE,
                     const FS_OPTION_FLAG option_flags = tfs::common::TFS_FILE_DEFAULT_OPTION);
   
    int64 SaveBuffer(char* ret_fs_name,const int32 ret_fs_name_len,
                   const char* buf,const int64 file_length,const int32 flag,
                   const char* suffix = NULL,const char* host = NULL,const char* key = NULL);

    int64 SaveFile(char* ret_fs_name,
                   const int32 ret_fs_name_len,
                   const char* local_file,const int32 flag,
                   const char* suffix,const char* host);


    int32 FetchFile(const char* local_file,const char* file_name,
                    const char* suffix=NULL,const char* host=NULL);

    int32 FetchFile(int64& ret_count,char* buf,const int64 count,
                    const char*  file_name,const char*  suffix = NULL,
                    const char*  host = NULL); 


private:
    tfs::client::TfsClient*                client_;
};

}


#endif
