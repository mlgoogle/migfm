#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"native_library.h"
#include "log/mig_log.h"

namespace basic{
libhandle load_native_library(const char* library_path){
    
    libhandle dl = dlopen(library_path,RTLD_LAZY);
    if(!dl){
        //printf("dlopen failed where trying to open error: %s (on [%d] line"
          //     " in [%s]file)\n",dlerror(),__LINE__,__FILE__);
    	MIG_ERROR(USER_LEVEL,"dlopen failed where trying to open error: %s (on [%d] line in [%s]file)\n",dlerror(),__LINE__,__FILE__);
        return NULL;
    }
    
    return dl;
}

void unload_native_library(libhandle library){
    
    int ret = dlclose(library);
    if(ret<0){
        printf("dlclose failed: %s on [%d] line in [%s] file\n"
            ,dlerror(),__LINE__,__FILE__);
        
    }
}


void* get_function_pointer(libhandle library,
                           const char* name){
    return dlsym(library,name);
}

}
