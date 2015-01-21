#ifndef NATIVE_LIBRARY_H__
#define NATIVE_LIBRARY_H__
#include<stdio.h>
#include<stdlib.h>
#include<dlfcn.h>

//define type
typedef void* libhandle;

libhandle load_native_library(const char* library_path);

void unload_native_library(libhandle library);

void* get_function_pointer(libhandle library,const char* name);

#endif
