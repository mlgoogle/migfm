#include<stdio.h>`
#include<stdlib.h>
#include<string.h>
#include<dlfcn.h>
#include "common.h"
#include "linuxlist.h"
#include "plugins.h"
#include "native_library.h"
#include "log/mig_log.h"

struct plugin_data{
   
    PLUGIN_DATA_ID;

    PLUGIN_DATA_NAME;

    PLUGIN_DATA_VERSION;

    PLUGIN_DATA_PROVIDER;
};

typedef enum {
    
    PLUGIN_FUNC_UNSET,
    PLUGIN_FUNC_HANDLER_INIT,
    PLUGIN_FUNC_HANDLER_CLEAN_UP,
    PLUGIN_FUNC_HANDLER_READ,
    PLUGIN_FUNC_HANDLER_READ_SRV,
    PLUGIN_FUNC_CONNECTION,
    PLUGIN_FUNC_CONNECTION_CLOSE,
    PLUGIN_FUNC_CLOCK_HANDLER,
    PLUGIN_FUNC_CONNECTION_SRV,
    PLUGIN_FUNC_CONNECTION_CLOSE_SRV,
    PLUGIN_FUNC_HANDLER_READ_OTHER,
    PLUGIN_FUNC_TIME_INIT,
    PLUGIN_FUNC_TIME_MSG,
    PLUGIN_FUNC_SIZEOF
}plugin_t;


static struct plugin*  plugin_init()
{
    struct plugin* p;
    p = (struct plugin*)calloc(1,sizeof(*p));
#define CLEAN(x)\
    p->x = buffer_init();

    CLEAN(id);
    CLEAN(name);
    CLEAN(version);
    CLEAN(provider);
#undef CLEAN
    return p;
}


static void plugin_free(struct plugin *p)
{
    if(p->lib)
        unload_native_library(p->lib);
#define CLEAN(x)\
    buffer_free(p->x)
    CLEAN(id);
    CLEAN(name);
    CLEAN(version);
    CLEAN(provider);
#undef CLEAN
    if(p){
        free(p);
        p = NULL;
    }
}

#define PLUGIN_TO_SLOT(x,y)\
    handler_t plugins_call_##y(struct server* srv,void *pd) {\
        struct plugin** slot;\
        int j;\
        struct plugin* p;\
        handler_t r;\
        if(!srv->plugins_slot) return HANDLER_GO_ON;\
        slot = ((struct plugin***)srv->plugins_slot)[x];\
        if(!slot) return HANDLER_GO_ON;\
        for(j=0;j<srv->plugins.used&&slot[j];j++){\
            p=slot[j];\
            switch(r=(p->y(srv,pd))){\
                case HANDLER_GO_ON:\
                    break;\
                case HANDLER_FINISHED:\
                case HANDLER_COMEBACK:\
                case HANDLER_WAIT_FOR_EVENT:\
                case HANDLER_WAIT_FOR_FD:\
                case HANDLER_ERROR:\
                    return r;\
                default:\
                	MIG_ERROR(USER_LEVEL,"%s unkown state:%d\n",p->name->ptr,r);\
            }\
        }\
        return HANDLER_GO_ON;\
     }

PLUGIN_TO_SLOT(PLUGIN_FUNC_HANDLER_CLEAN_UP,clean_up);
#undef PLUGIN_TO_SLOT


handler_t plugins_call_handler_init_time(struct server *srv){
     struct plugin** slot;
     int j;
     struct plugin* p;
     handler_t r;
     if(!srv->plugins_slot) return HANDLER_GO_ON;
     slot = ((struct plugin***)srv->plugins_slot)[PLUGIN_FUNC_TIME_INIT];
     if(!slot) return HANDLER_GO_ON;
     for(j=0;j<srv->plugins.used&&slot[j];j++){
         p=slot[j];
         switch(r=(p->handler_init_time(srv))){
            case HANDLER_GO_ON:
                break;
            case HANDLER_FINISHED:
            case HANDLER_COMEBACK:
            case HANDLER_WAIT_FOR_EVENT:
            case HANDLER_WAIT_FOR_FD:
            case HANDLER_ERROR:
                return r;
            default:
                MIG_ERROR(USER_LEVEL,"%s unkown state:%d\n",p->name->ptr,r);\
         }
     }
    return HANDLER_GO_ON;
}  

// #define PLUGIN_TO_SLOT(x,y)\
//     handler_t plugins_call_##y(struct server* srv) {\
//         struct plugin** slot;\
//         int j;\
//         struct plugin* p;\
//         handler_t r;\
//         if(!srv->plugins_slot) return HANDLER_GO_ON;\
//         slot = ((struct plugin***)srv->plugins_slot)[x];\
//         if(!slot) return HANDLER_GO_ON;\
//         for(j=0;j<srv->plugins.used&&slot[j];j++){\
//             p=slot[j];\
//             switch(r=(p->y(srv))){\
//                case HANDLER_GO_ON:\
//                    break;\
//                case HANDLER_FINISHED:\
//                case HANDLER_COMEBACK:\
//                case HANDLER_WAIT_FOR_EVENT:\
//                case HANDLER_WAIT_FOR_FD:\
//                case HANDLER_ERROR:\
//                    return r;\
//                default:\
//                    MIG_ERROR(USER_LEVEL,"%s unkown state:%d\n",p->name->ptr,r);\
//             }\
//         }\
//        return HANDLER_GO_ON;\
//     }  
// PLUGIN_TO_SLOT(PLUGIN_FUNC_TIME_INIT,handler_init_time);
// #undef PLUGIN_TO_SLOT
    
#define PLUGIN_TO_SLOT(x,y)\
    handler_t plugins_call_##y(struct server* srv,int fd,void *pd,int len) {\
        struct plugin** slot;\
        int j;\
        struct plugin* p;\
        handler_t r;\
        if(!srv->plugins_slot) return HANDLER_GO_ON;\
        slot = ((struct plugin***)srv->plugins_slot)[x];\
        if(!slot) return HANDLER_GO_ON;\
        for(j=0;j<srv->plugins.used&&slot[j];j++){\
            p=slot[j];\
            switch(r=(p->y(srv,fd,pd,len))){\
               case HANDLER_GO_ON:\
                   break;\
               case HANDLER_FINISHED:\
               case HANDLER_COMEBACK:\
               case HANDLER_WAIT_FOR_EVENT:\
               case HANDLER_WAIT_FOR_FD:\
               case HANDLER_ERROR:\
                   return r;\
               default:\
                   MIG_ERROR(USER_LEVEL,"%s unkown state:%d\n",p->name->ptr,r);\
            }\
        }\
       return HANDLER_GO_ON;\
    }

PLUGIN_TO_SLOT(PLUGIN_FUNC_CONNECTION,connection);
PLUGIN_TO_SLOT(PLUGIN_FUNC_HANDLER_READ,handler_read);
PLUGIN_TO_SLOT(PLUGIN_FUNC_CONNECTION_SRV, connection_srv);
PLUGIN_TO_SLOT(PLUGIN_FUNC_HANDLER_READ_SRV,handler_read_srv);
PLUGIN_TO_SLOT(PLUGIN_FUNC_HANDLER_READ_OTHER,handler_read_other);
PLUGIN_TO_SLOT(PLUGIN_FUNC_TIME_INIT,init_time);
#undef PLUGIN_TO_SLOT


#define PLUGIN_TO_SLOT(x,y)\
    handler_t plugins_call_##y(struct server* srv,int fd) {\
        struct plugin** slot;\
        int j;\
        struct plugin* p;\
        handler_t r;\
        if(!srv->plugins_slot) return HANDLER_GO_ON;\
        slot = ((struct plugin***)srv->plugins_slot)[x];\
        if(!slot) return HANDLER_GO_ON;\
        for(j=0;j<srv->plugins.used&&slot[j];j++){\
            p=slot[j];\
            switch(r=(p->y(srv,fd))){\
               case HANDLER_GO_ON:\
                   break;\
               case HANDLER_FINISHED:\
               case HANDLER_COMEBACK:\
               case HANDLER_WAIT_FOR_EVENT:\
               case HANDLER_WAIT_FOR_FD:\
               case HANDLER_ERROR:\
                   return r;\
               default:\
                   MIG_ERROR(USER_LEVEL,"%s unkown state:%d\n",p->name->ptr,r);\
            }\
        }\
       return HANDLER_GO_ON;\
    }
 
PLUGIN_TO_SLOT(PLUGIN_FUNC_CONNECTION_CLOSE,connection_close);
PLUGIN_TO_SLOT(PLUGIN_FUNC_CONNECTION_CLOSE_SRV,connection_close_srv);
#undef PLUGIN_TO_SLOT

#define PLUGIN_TO_SLOT(x,y)\
	handler_t plugins_call_##y(struct server* srv,char* id,int opcode,int time){\
		struct plugin** slot;\
		int j;\
		struct plugin* p;\
		handler_t r;\
		if(!srv->plugins_slot) return HANDLER_GO_ON;\
		slot = ((struct plugin***)srv->plugins_slot)[x];\
		if(!slot) return HANDLER_GO_ON;\
		for(j=0;j<srv->plugins.used&&slot[j];j++){\
			p = slot[j];\
			if((strcmp(p->id->ptr,id)==0)){\
				switch(r=(p->y(srv,id,opcode,time))){\
					case HANDLER_GO_ON:\
						break;\
					case HANDLER_FINISHED:\
					case HANDLER_COMEBACK:\
					case HANDLER_WAIT_FOR_EVENT:\
					case HANDLER_WAIT_FOR_FD:\
					case HANDLER_ERROR:\
						return r;\
					default:\
						MIG_ERROR(USER_LEVEL,"%s unkown state:%d\n",p->name->ptr,r);\
				}\
			}\
		}\
			return HANDLER_GO_ON;\
		}
		
PLUGIN_TO_SLOT(PLUGIN_FUNC_TIME_MSG,time_msg);
#undef PLUGIN_TO_SLOT


#define PLUGIN_TO_SLOT(x,y)\
    handler_t plugins_call_##y(struct server* srv,void* pd) {\
        struct plugin** slot;\
        int j;\
        struct plugin* p;\
        handler_t r;\
        if(!srv->plugins_slot) return HANDLER_GO_ON;\
        slot = ((struct plugin***)srv->plugins_slot)[x];\
        if(!slot) return HANDLER_GO_ON;\
        for(j=0;j<srv->plugins.used&&slot[j];j++){\
            p=slot[j];\
            switch(r=(p->y(srv,pd))){\
               case HANDLER_GO_ON:\
                   break;\
               case HANDLER_FINISHED:\
               case HANDLER_COMEBACK:\
               case HANDLER_WAIT_FOR_EVENT:\
               case HANDLER_WAIT_FOR_FD:\
               case HANDLER_ERROR:\
                   return r;\
               default:\
                   MIG_ERROR(USER_LEVEL,"%s unkown state:%d\n",p->name->ptr,r);\
            }\
        }\
       return HANDLER_GO_ON;\
    }  

#undef PLUGIN_TO_SLOT

static int plugins_register(struct server* srv,struct plugin* p)
{
    struct plugin** ps;
    
    if(srv->plugins.size == 0){
    
        srv->plugins.size = 4;
        srv->plugins.ptr = (char*)malloc(srv->plugins.size*sizeof(*p));
        srv->plugins.used = 0;

    }else if(srv->plugins.size == srv->plugins.used){ 
        
        srv->plugins.size+=4;
        srv->plugins.ptr = (char*)realloc(srv->plugins.ptr,srv->plugins.size*
                               sizeof(*p));
    }
    
    ps = (struct plugin**)srv->plugins.ptr;
    ps[srv->plugins.used++] = p;
    
    return 1; 
}


struct plugin *get_pluginst_info(struct server *srv,const char* id)
{
    int j;
    struct plugin *p;
    struct plugin**ps;
    ps = (struct plugin**)srv->plugins.ptr;
    for(j=0;j<srv->plugins.used;j++){
        p = ps[j];
        if((strcmp(p->id->ptr,id)==0)){
            return p;    
        }
    }
    return NULL;
}

int plugins_load(struct server* srv) 
{

    struct plugin* p;
    int (*init)(struct plugin* pl);
    struct list_head *tmp;
    struct plugin_desc *pl_desc;
    list_for_each(tmp,&srv->plugins_des){

        pl_desc = list_entry(tmp,struct plugin_desc,list);
        if(pl_desc&&(!(strcmp(pl_desc->id->ptr,"core")==0))){
            p = plugin_init();
            
            p->lib = load_native_library(pl_desc->path->ptr);
            //p->lib = dlopen("/home/kerry/webchat/bin/plugins/diskio/diskio.so",RTLD_LAZY);
            if(p->lib==NULL){
       
              MIG_ERROR(USER_LEVEL,"dlopen()failed for:[%s] %s",
                         pl_desc->id->ptr,dlerror());
              plugin_free(p);
              return -1;
            }
            
            buffer_copy_string(srv->tmp_buf,pl_desc->id->ptr);
            buffer_append_string_len(srv->tmp_buf,CONST_STR_LEN("_plugin_init"));
            init =(int(*)(struct plugin*))get_function_pointer(p->lib,srv->tmp_buf->ptr);
            
            if(init == NULL){
                MIG_ERROR(USER_LEVEL,"unable find %s in %s",srv->tmp_buf->ptr,pl_desc->id->ptr);
                plugin_free(p);
                return -1;
            }

            if((*init)(p)){

                MIG_ERROR(USER_LEVEL,"%s plugin init failed",p->id);
                plugin_free(p);
                return -1;
            }
            plugins_register(srv,p);
        }
    }
    return 0;
}


void plugins_free(struct server* srv) 
{
    size_t i;
    plugins_call_clean_up(srv,NULL);
    
    for(i=0;i<srv->plugins.used;i++){
        struct plugin *p = ((struct plugin **)srv->plugins.ptr)[i];
        plugin_free(p);
    }

    for(i=0;srv->plugins_slot && i<PLUGIN_FUNC_SIZEOF;i++){

        struct plugin **slot = ((struct plugin***)(srv->plugins_slot))[i];
        if(slot) {free(slot);slot=NULL;}
    }
    free(srv->plugins_slot);
    srv->plugins_slot = NULL;

    free(srv->plugins.ptr);
    srv->plugins.ptr = NULL;
    srv->plugins.used = 0;
}


handler_t plugins_call_init(struct server* srv){

    struct plugin** ps;
    struct plugin** slot;
    int i,j;
    struct plugin* p;
    struct list_head* tmp;
    struct plugin_desc *pl_desc;
    ps = (struct plugin**)srv->plugins.ptr;
    
    srv->plugins_slot = calloc(PLUGIN_FUNC_SIZEOF,sizeof(ps)); 

    for(i=0;i<srv->plugins.used;i++) {
        
        p = ps[i];
     
#define PLUGIN_TO_SLOT(x,y)\
    if(p->y){\
       slot =((struct plugin***)srv->plugins_slot)[x];\
       if(!slot){\
           slot = (struct plugin**)calloc(srv->plugins.used,sizeof(*slot));\
           ((struct plugin***)srv->plugins_slot)[x] = slot;\
       }\
      for(j=0;j<srv->plugins.used;j++) {\
          if(slot[j]) continue;\
          slot[j] = p;\
          break;\
      }\
    }

    PLUGIN_TO_SLOT(PLUGIN_FUNC_HANDLER_INIT,init);
    PLUGIN_TO_SLOT(PLUGIN_FUNC_HANDLER_CLEAN_UP,clean_up);
    PLUGIN_TO_SLOT(PLUGIN_FUNC_HANDLER_READ,handler_read);
    PLUGIN_TO_SLOT(PLUGIN_FUNC_HANDLER_READ_SRV,handler_read_srv);
    PLUGIN_TO_SLOT(PLUGIN_FUNC_CONNECTION,connection);
    PLUGIN_TO_SLOT(PLUGIN_FUNC_CONNECTION_CLOSE,connection_close);
    PLUGIN_TO_SLOT(PLUGIN_FUNC_CONNECTION_SRV,connection_srv);
    PLUGIN_TO_SLOT(PLUGIN_FUNC_CONNECTION_CLOSE_SRV,connection_close_srv);
    PLUGIN_TO_SLOT(PLUGIN_FUNC_HANDLER_READ_OTHER,handler_read_other);
    PLUGIN_TO_SLOT(PLUGIN_FUNC_TIME_MSG,time_msg);
    PLUGIN_TO_SLOT(PLUGIN_FUNC_TIME_INIT,handler_init_time);
#undef PLUGIN_TO_SLOT

        if(p->init) {
            
            if(NULL==(p->data=p->init())){
                
               //SINA_ERROR(SYSTEM_LEVEL,"plugin-init failed for module %s",p->id);
               return HANDLER_ERROR; 
            }
            list_for_each(tmp,&srv->plugins_des){
                pl_desc = list_entry(tmp,struct plugin_desc,list);
                if(strcmp(pl_desc->id->ptr,"core")==0)
                    continue; 
                if(pl_desc){
                    /*if((buffer_is_equal(pl_desc->id,
                                        ((struct plugin_data*)(p->data))->id)&&
                         buffer_is_equal(pl_desc->version,
                                        ((struct plugin_data*)p->data)->version))){
                        SINA_ERROR(SYSTEM_LEVEL,"plugin-version or plugin-id"
                                                 "dosen't match for %s",p->id);
                        assert(1);
                        return -1;
                    }*/
					if((strcmp(pl_desc->id->ptr,((struct plugin_data*)(p->data))->id)==0)&&
					   (strcmp(pl_desc->version->ptr,((struct plugin_data*)p->data)->version)==0)){
#define COPY_PLUGIN(x)\
    buffer_copy_string(p->x,pl_desc->x->ptr);

                      COPY_PLUGIN(id);
                      COPY_PLUGIN(name);
                      COPY_PLUGIN(version);
                      COPY_PLUGIN(provider);
#undef COPY_PLUGIN
                      MIG_ERROR(USER_LEVEL,"the plugin id[%s] name[%s] "
                                "version[%s] provider[%s]",p->id->ptr,p->name->ptr,
                                p->version->ptr,p->provider->ptr);
					  break;
					}
                }
            }
        } else{
      
            p->data = NULL;
        }
    }

    return HANDLER_GO_ON;
}

