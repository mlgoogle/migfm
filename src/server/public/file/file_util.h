#ifndef _MIG_FM_PUBLIC_BASIC_FILE_UTIL_H__
#define _MIG_FM_PUBLIC_BASIC_FILE_UTIL_H__

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include "file_path.h"

#define HANDLER_EINTR(x) ({\
    typeof(x) __eintr_result__;\
    do{\
        __eintr_result__ = x;\
    }while(__eintr_result__==-1&&errno==EINTR);\
    __eintr_result__;\
})

namespace file{
int WriteFileDescriptor(const int fd,const char* data,int size);

FILE * OpenFile(const std::string& filename, const char* mode);

FILE* OpenFile(const FilePath& filename,const char* mode);

bool CloseFile(const FilePath& filename);

bool TruncateFile(FILE* file);

int ReadFile(const FilePath& filename,char* data,int size);

int WriteFile(const FilePath& filename,const char* data,int size);

bool GetCurrentDirectory(FilePath* path);

bool SetCurrentDirectory(const FilePath& path);

bool CreateDirectory(const FilePath& path);

bool DirectoryExists(const FilePath& path);

}
#endif
