#include "file_util.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <libgen.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

namespace file{

FILE* OpenFile(const std::string& filename,
	             const char* mode){
    return OpenFile(FilePath(filename),mode);
}

FILE* OpenFile(const FilePath& filename,
               const char* mode){
    return fopen(filename.value().c_str(),mode);               	
}

int ReadFile(const FilePath& filename,char* data,int size){
    int fd = open(filename.value().c_str(),O_RDONLY);
    if(fd<0)
        return -1;
    ssize_t bytes_read = HANDLER_EINTR(read(fd,data,size));
    if(int ret = HANDLER_EINTR(close(fd))<0)
        return ret;
    return bytes_read;
}

int WriteFile(const FilePath& filename,const char* data,int size){
    int ret = 0;
	  //int fd = creat(filename.value().c_str(),0666);
	  int fd = open(filename.value().c_str(),O_CREAT|O_APPEND|O_WRONLY,S_IRWXU|S_IRWXG|S_IRWXO);
    if(fd<0)
        return -1;
    int bytes_written = WriteFileDescriptor(fd,data,size);
    //if((ret =HANDLER_EINTR(close(fd)))<0);
        //return ret;
    ret = close(fd);
    return bytes_written;
}

int WriteFileDescriptor(const int fd,const char* data,int size){ //不能一次性完全写入情况
    ssize_t bytes_written_total = 0;
    for(ssize_t bytes_written_partial = 0;
        bytes_written_partial<size;
        bytes_written_total+=bytes_written_partial){
        bytes_written_partial = 
            HANDLER_EINTR(write(fd,data+bytes_written_total,
                               size - bytes_written_total));
        if(bytes_written_partial<0)
             return -1;	
    }   	
    
    return bytes_written_total;
}

bool CloseFile(const FilePath& filename){
    FILE* file = fopen(filename.value().c_str(),O_RDONLY);
    if(file==NULL)
        return true;
    return fclose(file)==0;
}

bool TruncateFile(FILE* file){
    if(file==NULL)
        return false;
    long current_offset = ftell(file);
    if(current_offset==-1)
        return false;
    int fd = fileno(file);
    if(ftruncate(fd,current_offset)!=0)
        return false;
    return true;
}

bool GetCurrentDirectory(FilePath* path){
    char system_buffer[PATH_MAX] = "";
    if(!getcwd(system_buffer,sizeof(system_buffer)))
        return false;
    *path = FilePath(system_buffer);
    return true;
}

bool SetCurrentDirectory(const FilePath& path){
    int ret =  chdir(path.value().c_str());
    return !ret;
}

bool CreateDirectory(const FilePath& full_path){
	std::vector<FilePath> subpaths;

	FilePath last_path = full_path;
	subpaths.push_back(full_path);

	for(FilePath path = full_path.DirName();
			path.value()!=last_path.value();
			path = path.DirName()){
	
	    subpaths.push_back(path);
		last_path = path;
	}

	for(std::vector<FilePath>::reverse_iterator i = subpaths.rbegin();
			i != subpaths.rend();++i){
	    if(DirectoryExists(*i))
			continue;
		//if(mkdir(i->value().c_str(),0700)==0)
		  if(mkdir(i->value().c_str(),S_IRWXU|S_IXOTH)==0)
			continue;

		if(!DirectoryExists(*i))
			return false;
	}
	return true;
}

bool DirectoryExists(const FilePath& path){
    struct stat file_info;
	if(stat(path.value().c_str(),&file_info)==0)
		return S_ISDIR(file_info.st_mode);
	return false;
}

}