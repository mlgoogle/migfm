#include"parser_xml.h"
#include<stdio.h>
#include<stdlib.h>
#include<expat.h>
#include<string.h>
#include<dirent.h>
#include "common.h"
#include"linuxlist.h"
#include "log/mig_log.h"

#define XML_FMT_INT_MOD "1"

const char config_file_name[] = "config.xml";
const char config[] = "config";
const char srv_config[] = "srv_config";
const char srv_config_bindhost[] = "bindhost";
const char srv_config_port[] = "port";
const char srv_config_path[] = "path";
const char log_config[] = "log_config";
const char log_usr_file[] = "usr_file";
const char log_sys_file[] = "sys_file";
const char log_err_file[] = "error_file";
const char app_config[] = "app_config";
const char app_config_max_worker[] = "max_worker";
const char app_config_max_fds[] = "max_fds";
const char app_config_max_conns[] = "max_conns";
const char other_config[] = "other_config";
const char other_config_modules_dir[] = "modules_dir"; 
const char plugin_filename[]="plugin";
const char plugin_root[] = "plugin";
const char plugin_id[]="id";
const char plugin_name[]="name";
const char plugin_version[]="version";
const char plugin_provider[]="provider-name";
const char remote_config[]="remote_config";
const char remote_host[]="remote_host";
const char remote_port[]="remote_port";

static int xml_config_flag = 0;
static int ncount_connect = 0;

static void XMLCALL end_config_element(void *userData,const char *name){

    struct server *srv = (struct server*)userData;
    if(strcmp(name,remote_config)==0){
        srv->ncount_connect = ncount_connect;
        ncount_connect = 0;
        xml_config_flag = 0;
    }
}

static void XMLCALL start_config_element(void *userData,
                                         const char *name,
                                         const char **atts){
    int i;
    struct server *srv = (struct server*)userData;
#define SET_SRV_CONFIG(NAME,MEMBER)\
    if(strcmp(NAME,atts[i])==0){\
        buffer_copy_string(srv->MEMBER,atts[i+1]);\
    }
    
    if(strcmp(name,srv_config)==0){
        for(i=0;atts[i]!=0;i+=2){
            SET_SRV_CONFIG(srv_config_bindhost,srv_conf.bindhost);
#if defined (NET_WORK)
            SET_SRV_CONFIG(srv_config_port,srv_conf.port);
#endif
#if defined (PROCESS_WORK)
			SET_SRV_CONFIG(srv_config_path,srv_conf.process_path);
#endif
        }
	return;
    }
    
    else if(strcmp(name,log_config)==0){
        for(i=0;atts[i]!=0;i+=2){
            SET_SRV_CONFIG(log_usr_file,srv_conf.usr_file);
            SET_SRV_CONFIG(log_sys_file,srv_conf.sys_file);
            SET_SRV_CONFIG(log_err_file,srv_conf.error_file);
        }
	return;
    }
    
    else if(strcmp(name,app_config)==0){
        for(i=0;atts[i]!=0;i+=2){
            SET_SRV_CONFIG(app_config_max_worker,srv_conf.max_worker);
            SET_SRV_CONFIG(app_config_max_fds,srv_conf.max_fds);
            SET_SRV_CONFIG(app_config_max_conns,srv_conf.max_conns);
        }
	return;
    }

    else if(strcmp(name,other_config)==0){
        for(i=0;atts[i]!=0;i+=2){
            SET_SRV_CONFIG(other_config_modules_dir,srv_conf.modules_dir);
        }
	return;
    }
 
    else if(strcmp(name,remote_config)==0){
         xml_config_flag = atoi(atts[1]); 
    }
    else if(strcmp(name,config)==0&&xml_config_flag==1){
        struct connect_config* cont_conf = (struct connect_config*)malloc(sizeof(struct connect_config));
        if(cont_conf==NULL)
            return;
		cont_conf->bindremote = buffer_init();
        buffer_copy_string(cont_conf->bindremote,atts[1]);
        cont_conf->port = buffer_init();	
        buffer_copy_string(cont_conf->port,atts[3]);
        list_add_tail(&(cont_conf->list),&srv->srv_conf.remotes);
        srv->ncount_connect++;
	return;
    }
#undef SET_SRV_CONFIG
}

static void XMLCALL start_plugin_element(void *userData,
                                         const char *name,
                                         const char **atts)
{
    int i;
    struct plugin_desc* p = (struct plugin_desc*)userData;
    
    if(strcmp(name,plugin_root)==0){
     
        for(i=0;atts[i]!=0;i+=2){
     
            if(strcmp(plugin_id,atts[i])==0)
                buffer_copy_string(p->id,atts[i+1]);
            else if(strcmp(plugin_name,atts[i])==0)
                buffer_copy_string(p->name,atts[i+1]);
            else if(strcmp(plugin_version,atts[i])==0)
                buffer_copy_string(p->version,atts[i+1]);
            else if(strcmp(plugin_provider,atts[i])==0)
                buffer_copy_string(p->provider,atts[i+1]);
        }
    }
    else{
        MIG_INFO(USER_LEVEL,"the name is vailed");
    }
}


int config_read(struct server *srv)
{
    FILE  *conf_file;
    int len;
    int size;
    char *buf;

    XML_Parser parser = XML_ParserCreate(NULL);    
    XML_SetUserData(parser,srv);
    XML_SetElementHandler(parser,start_config_element,NULL);
    
    conf_file = fopen("./plugins/core/config","rb");

    if(!conf_file){

      fprintf(stderr,"config valid\n");
      return -1;
    }

    size = 0;
    fseek(conf_file,0,SEEK_END);
    size = ftell(conf_file);
    fseek(conf_file,0,SEEK_SET);
    if(size<0){

        fprintf(stderr,"xml error\n");
        fclose(conf_file);
        return -1;
    }

    buf = (char*)malloc(size+1);
    len = (int)fread(buf,1,size,conf_file);   
    if(XML_Parse(parser,buf,len,1)==XML_STATUS_ERROR){
   
        if(buf) {free(buf);buf = NULL;} 
        return -1;
    }

    XML_ParserFree(parser);
    fclose(conf_file);
    if(buf){
        free(buf);
        buf = NULL;
    }
    return 0;
}

static int should_skip(const char* filepath)
{

    char* l =".";
    char* p;
    
    p = strstr(filepath,l);
    if(p)
        return 1;
    else
        return 0;
}



static int get_file_path(struct server *srv)
{

    struct dirent dent_buf;
    struct dirent *dent;
    struct plugin_xml_path  *node_path;
    int ret;
   
    DIR *dir = opendir(srv->srv_conf.modules_dir->ptr);
   
    if(!dir){
    
        MIG_ERROR(USER_LEVEL,"the moudule path vaild");
        return -1;
    }

    while(readdir_r(dir,&dent_buf,&dent)==0&&(dent)){
    
        if(should_skip(dent->d_name))
            continue;
        node_path = (struct plugin_xml_path*)calloc(1,sizeof(*node_path));
        node_path->filepath = buffer_init();
        buffer_copy_string(node_path->filepath,srv->srv_conf.modules_dir->ptr);
        buffer_append_string(node_path->filepath,"/");
        buffer_append_string(node_path->filepath,dent_buf.d_name);
        
        ret = stat(node_path->filepath->ptr,&node_path->stat); 
                if(ret<0){
            buffer_free(node_path->filepath);
            free(node_path);
            node_path = NULL;
            continue;
        }
     
        if(S_ISDIR(node_path->stat.st_mode)){
           
            list_add_tail(&(node_path->list),&srv->plugins_xml_path);
        }
        else{
            
            buffer_free(node_path->filepath);
            free(node_path);
            node_path = NULL;
        }
    }
    
    closedir(dir); 
    return 0; 
}

static void init_desc(struct plugin_desc* pl_desc)
{
#define DESC_INIT(x)\
    pl_desc->x = buffer_init();
    
    DESC_INIT(id);
    DESC_INIT(name);
    DESC_INIT(version);
    DESC_INIT(provider);
    DESC_INIT(path);
#undef DESC_INIT
}

static void deinit_desc(struct plugin_desc* pl_desc){

#define DESC_DEINIT(x)\
    buffer_free(pl_desc->x);
    
    DESC_DEINIT(id);
    DESC_DEINIT(name);
    DESC_DEINIT(version);
    DESC_DEINIT(provider);
    DESC_DEINIT(path);
#undef DESC_DEINIT
    if(pl_desc){free(pl_desc);pl_desc=NULL;}
	
}

static struct plugin_desc*  parser_xml_plugin(struct server *srv,
                                              struct buffer *path)
{
    FILE *plugin_file;
    int len;
    int size;
    char *buf;
    struct plugin_desc *pl_desc;
    struct buffer *all_path_plugin;
    pl_desc = (struct plugin_desc*)malloc(sizeof(*pl_desc));
    
    init_desc(pl_desc); 

    all_path_plugin = buffer_init();
    buffer_copy_string(all_path_plugin,path->ptr);
    buffer_append_string(all_path_plugin,"/");
    buffer_append_string(all_path_plugin,plugin_filename);

    XML_Parser parser = XML_ParserCreate(NULL);
    XML_SetUserData(parser,pl_desc);
    XML_SetElementHandler(parser,start_plugin_element,NULL);
    
    plugin_file = fopen(all_path_plugin->ptr,"rb");
    if(!plugin_file){
        MIG_INFO(USER_LEVEL,"the %s xml vaild",path->ptr);
        free(pl_desc);
        pl_desc = NULL;
        return NULL;
    }
   
    size = 0;
    fseek(plugin_file,0,SEEK_END);
    size = ftell(plugin_file);
    fseek(plugin_file,0,SEEK_SET);
    if(size<0){
        MIG_INFO(USER_LEVEL,"the file[%s] not xml",path->ptr);
        fclose(plugin_file);
        free(pl_desc);
        pl_desc = NULL;
        return NULL;
    }

    buf = (char*)malloc(size+1);
    len = (int)fread(buf,1,size,plugin_file);
     
    if(XML_Parse(parser,buf,len,1)==XML_STATUS_ERROR){
        
        MIG_INFO(USER_LEVEL,"%s at line %"XML_FMT_INT_MOD"u\n",
                   XML_ErrorString(XML_GetErrorCode(parser)),
                   XML_GetCurrentLineNumber(parser));
        fclose(plugin_file);
        free(pl_desc);
        pl_desc = NULL;
        return NULL;
    }

    XML_ParserFree(parser);
    fclose(plugin_file);
    buffer_free(all_path_plugin);
    //so path
    if(pl_desc!=NULL){
        buffer_copy_string(pl_desc->path,path->ptr);
        buffer_append_string(pl_desc->path,"/");
        buffer_append_string(pl_desc->path,pl_desc->id->ptr);
        buffer_append_string(pl_desc->path,".so");
    }
    if(buf) {free(buf);buf=NULL;}
    return pl_desc;
}

static int check_more_plugin(struct plugin_desc *pl_desc,struct list_head* head)
{
    struct plugin_desc *tmp_desc;
    struct list_head* tmp;
    list_for_each(tmp,head){
 
        tmp_desc = list_entry(tmp,struct plugin_desc,list);
        if(buffer_is_equal(tmp_desc->id,pl_desc->id)){
#define CLEAN(x)\
    buffer_free(pl_desc->x);
        CLEAN(id);
        CLEAN(name);
        CLEAN(version);
        CLEAN(provider);
        CLEAN(path);
#undef CLEAN
            return 0;
        }
    }
    return 1;
}
int get_plugin_info(struct server *srv)
{
    int ret;
    struct list_head *tmp;
    struct plugin_xml_path  *node_path;
    struct plugin_desc* pl_desc;
    ret = get_file_path(srv);
    if(ret!=0)
        return ret;
    
    list_for_each(tmp,&srv->plugins_xml_path){

        node_path = list_entry(tmp,struct plugin_xml_path,list);
        pl_desc = parser_xml_plugin(srv,node_path->filepath);
        if(pl_desc!=NULL){
            // check cannot load plugin more than once
            if(check_more_plugin(pl_desc,&srv->plugins_des)){ 
                list_add_tail(&(pl_desc->list),&srv->plugins_des);
            }else{
              MIG_INFO(USER_LEVEL,"Cannot load plugin %s"
                    " more than once",pl_desc->id->ptr);
              return -1;
            }
            
        }
    }

    return 0;
   
}
