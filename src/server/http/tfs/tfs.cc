#include "tfs.h"
#include <stdio.h>
#include <stdlib.h>

namespace filestorage{

TFSEnginImpl::TFSEnginImpl(){

    client_ = tfs::client::TfsClient::Instance(); 

}


int32 TFSEnginImpl::Initialize(const char* host){
    int32 result = 0;
    result = client_->initialize(host);
    if(result==tfs::common::TFS_SUCCESS)
        return 1;
    return 0;
}


int32 TFSEnginImpl::OpenFile(const char* file_name,const char* suffix,
                             const int32 flags,const char* key){
    int32 result = 0;
    result = client_->open(file_name,suffix,flags,key);
    if(result>0)
        return 1;
    return 0;
}



int32 TFSEnginImpl::OpenFile(const char* file_name,const char* suffix,
    	                     const char* host,const int32 flags,
    	                     const char* key){
    int32 result = 0;
    result = client_->open(file_name,suffix,host,flags,key);
    if(result>0)
        return 1;
    return 0;
}


int32 TFSEnginImpl::WriteBuffer(const int fd,const void* data,int64 count){
    int32 result = 0;
    result = client_->write(fd,data,count);
    if(result>0)
        return result;
   return 0;
}

int32 TFSEnginImpl::ReadBuffer(const int fd,void* buf,const int64 count){
    int32 result = 0;
    result = client_->read(fd,buf,count);
    if(result>0)
        return result;
    return 0;
   
}

int TFSEnginImpl::ReadBuffer(const int fd,void* buf,const int64 count,FS_STAT* file_info){
    int32 result = 0;
    result = client_->readv2(fd,buf,count,file_info);
    if(result>0)
        return result;
    return 0;
}


int32 TFSEnginImpl::GetFileStat(const int fd,FS_STAT* buf,FS_MODE mode){
    int32 result = 0;
    result = client_->fstat(fd,buf,mode);
    if(result!=tfs::common::TFS_SUCCESS)
       return 0;
    return 1;
     
         
}


int32 TFSEnginImpl::CloseFile(const int fd,char* ret_fs_name,const int32 ret_fs_name_len){
    int32 result = 0;
    result = client_->close(fd,ret_fs_name,ret_fs_name_len);
    if(result!=tfs::common::TFS_SUCCESS)
        return 0;
    return 1; 
}

int32 TFSEnginImpl::DeleteFile(int64& file_size,const char* file_name,const char* suffix,
                               const FS_UNLINKTYPE action,const FS_OPTION_FLAG option_flag){

    int32 result = 0;
    result = client_->unlink(file_size,file_name,suffix,action,option_flag);
    if(result!=tfs::common::TFS_SUCCESS)
        return 0;
    return 1;
}


int32 TFSEnginImpl::DeleteFile(int64& file_size,const char* file_name,const char* suffix,
                               const char* host, const FS_UNLINKTYPE action,const FS_OPTION_FLAG option_flags){
     int32 result = 0;
     result = client_->unlink(file_size,file_name,suffix,host,action,option_flags);
     if(result!=tfs::common::TFS_SUCCESS)
         return 0;
     return 1;
}

int64 TFSEnginImpl::SaveBuffer(char* ret_fs_name,const int32 ret_fs_name_len,
                             const char* buf,const int64 file_length,const int32 flag,
                             const char* suffix,const char* host ,const char* key){

    int32 result = 0;
    result = client_->save_buf(ret_fs_name,ret_fs_name_len,buf,file_length,
                              flag,suffix,host,key);

    if(result>0)
        return result;
    return 0;

}

int64 TFSEnginImpl::SaveFile(char* ret_fs_name,const int32 ret_fs_name_len,
                             const char* local_file,const int32 flag,
                             const char* suffix,const char* host){

    int64 result = 0;
    result = client_->save_file(ret_fs_name,ret_fs_name_len,local_file,flag,
                               suffix,host);
  
    return result;
}


int32 TFSEnginImpl::FetchFile(const char* local_file,const char* file_name,
                              const char* suffix,const char* host){
    int32 result = 0;
    result= client_->fetch_file(local_file,file_name,
                                suffix,host);
    if(result!=tfs::common::TFS_SUCCESS)
        return 0;
    return 1;
}

int32 TFSEnginImpl::FetchFile(int64& ret_count,char* buf,const int64 count,
                              const char*  file_name,const char*  suffix,
                              const char*  host){
    int32 result = 0;
    result = client_->fetch_file(ret_count,buf,count,file_name,
                                suffix,host);
    if(result!=tfs::common::TFS_SUCCESS)
        return 0;
    return 1;
}

}
