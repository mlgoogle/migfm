#include "config/config.h"
#include "errno.h"
#include "log/mig_log.h"
#include "basic/constants.h"
#include "basic/basictypes.h"
namespace config{

static char config[] = "config";
static char database[] = "database";
static char cached[]= "cached";
static char mysql[] = "mysql";
static char mssql[] = "mssql";
static char redis[] = "redis";
static char memcached[] = "memcached";
static char host[] = "host";
static char port[] = "port";
static char user[] = "user";
static char pass[] = "pass";
static char source[] = "name";

static int32 config_flag = 0;
static int32 data_flag = 0;
static int32 cached_flag = 0;
static int32 memcached_flag = 0;

FileConfig*  FileConfig::config_ = NULL;

FileConfig::FileConfig(){

}

FileConfig::~FileConfig(){

}

FileConfig* FileConfig::GetFileConfig(){
    if(config_==NULL){
       config_ = new FileConfig();
    }
    return config_;
}

static void XMLCALL OnConfigStart(void* usrData,const char* name,const char** atts){
    FileConfig* file_config = (FileConfig*)usrData;
    int i = 0;
    std::string shost;
    std::string sport;
    std::string suser;
    std::string spass;
    std::string sname;
    if((strcmp(name,config)==0)&&memcached_flag==0)
        config_flag = 1;
    else if(config_flag&&(strcmp(name,database)==0))
    	data_flag = 1;
    else if(config_flag&&(strcmp(name,cached)==0))
        cached_flag = 1;
    else if(config_flag&&data_flag&&
        ((strcmp(name,mysql)==0)||(strcmp(name,mssql)==0))){
        
        for(i = 0;atts[i]!=0;i+=2){
            if(strcmp(atts[i],host)==0)
                shost.assign(atts[i+1]);
            else if(strcmp(atts[i],port)==0)
                sport.assign(atts[i+1]);
            else if(strcmp(atts[i],user)==0)
                suser.assign(atts[i+1]);
            else if(strcmp(atts[i],pass)==0)
                spass.assign(atts[i+1]);
            else if(strcmp(atts[i],source)==0)
                sname.assign(atts[i+1]); 
        }
        base::ConnAddr addr(shost.c_str(),atoi(sport.c_str()),
            suser.c_str(),spass.c_str(),sname.c_str());
        if(strcmp(name,mysql)==0)
            file_config->mysql_db_list_.push_back(addr);
        MIG_INFO(USER_LEVEL,"config name[%s] ip[%s] port[%d] user[%s] pass[%s] db[%s]",
                 name,shost.c_str(),atoi(sport.c_str()),suser.c_str(),spass.c_str(),
                 sname.c_str());
    }else if(config_flag&&cached_flag&&
    	(strcmp(name,redis))==0||(memcached_flag&&strcmp(name,config)==0)){
        for(i = 0;atts[i]!=0;i+=2){
            if(strcmp(atts[i],host)==0)
                shost.assign(atts[i+1]);
            else if(strcmp(atts[i],port)==0)
                sport.assign(atts[i+1]);
            else if(strcmp(atts[i],user)==0)
                suser.assign(atts[i+1]);
            else if(strcmp(atts[i],pass)==0)
                spass.assign(atts[i+1]);
            else if(strcmp(atts[i],source)==0)
                sname.assign(atts[i+1]); 
        }
        base::ConnAddr addr(shost.c_str(),atoi(sport.c_str()),
            suser.c_str(),spass.c_str(),sname.c_str());
        MIG_INFO(USER_LEVEL,"config name[%s] ip[%s] port[%d]",
                 name,shost.c_str(),atoi(sport.c_str()));
        if(strcmp(name,redis)==0)
            file_config->redis_list_.push_back(addr);
        else
            file_config->mem_list_.push_back(addr);
        	
    }else if(config_flag&&cached_flag&&strcmp(name,memcached)==0)
    	  memcached_flag = 1;
/*	if(strcmp(name,database)==0){//mysql
		for(i=0;atts[i]!=0;i+=2){
		    if(strcmp(atts[i],host)==0)
		    	shost.assign(atts[i+1]);
		    else if(strcmp(atts[i],port)==0)
		    	sport.assign(atts[i+1]);
		    else if(strcmp(atts[i],user)==0)
		    	suser.assign(atts[i+1]);
		    else if(strcmp(atts[i],pass)==0)
		    	spass.assign(atts[i+1]);
		    else if(strcmp(atts[i],source)==0)
		    	sname.assign(atts[i+1]);
		}
		base::ConnAddr addr(shost.c_str(),atoi(sport.c_str()),
					suser.c_str(),spass.c_str(),sname.c_str());
		file_config->db_list_.push_back(addr);
		
	}else if(strcmp(name,redis)==0){//redis
		std::string shost;
		std::string sport;
		for(i=0;atts[i]!=0;i+=2){
			if(strcmp(atts[i],host)==0)
				shost.assign(atts[i+1]);
			else if(strcmp(atts[i],port)==0)
				sport.assign(atts[i+1]);
		}
		base::ConnAddr addr(shost.c_str(),atoi(sport.c_str()));
		file_config->redis_list_.push_back(addr);
		
	}else if(strcmp(name,memcached)==0){//memcached
		flag = 1;
	}else if((strcmp(name,config)==0)&&flag){
		std::string shost;
		std::string sport;
		for(i=0;atts[i]!=0;i+=2){
			if(strcmp(atts[i],host)==0)
				shost.assign(atts[i+1]);
			else if(strcmp(atts[i],port)==0)
				sport.assign(atts[i+1]);
		}
		if((shost.empty()!=true)&&(sport.empty()!=true)){
			base::ConnAddr addr(shost.c_str(),atoi(sport.c_str()));
			file_config->mem_list_.push_back(addr);
		}
	}*/
}

static void XMLCALL OnConfigEnd(void* usrData,const char* name){
    if(strcmp(name,config)==0)
        config_flag = 0;
    else if(strcmp(name,database)==0)
    	data_flag = 0;
    else if(strcmp(name,cached)==0)
        cached_flag = 0;
    else if(strcmp(name,memcached)==0)
    	memcached_flag = 0;
}


bool FileConfig::LoadConfig(std::string& path){
    char* xml=NULL;
	size_t len = 0;
	size_t amt = 0;
	FILE* fp = fopen(path.c_str(),"r");
	if(fp==NULL)
		return false;
    fseek(fp,0,SEEK_END);
	len = ftell(fp);
	fseek(fp,0,SEEK_SET);
	if(len<=0){
	    MIG_ERROR(USER_LEVEL,"%s error\n",path.c_str());
		fclose(fp);
		return false;
	}
	xml = new char[len+1];
	if(xml==NULL){
	    MIG_ERROR(USER_LEVEL,"alloc error %d",errno);
		fclose(fp);
		return false;
	}
	do{
		amt = fread(xml+amt,1,len,fp);
		len-=amt;
	}while(len!=0);
	XML_Parser parser = XML_ParserCreate(NULL);
	XML_SetUserData(parser,this);
	XML_SetElementHandler(parser,OnConfigStart,OnConfigEnd);

    if(XML_Parse(parser,xml,amt,1)==XML_STATUS_ERROR){
        MIG_ERROR(USER_LEVEL,"XML Parser error:%s at line %llu",
					XML_ErrorString(XML_GetErrorCode(parser)),
					XML_GetCurrentLineNumber(parser));
		XML_ParserFree(parser);
    if(xml){delete xml;xml=NULL;}
		return false;
	}
	XML_ParserFree(parser);
    if(xml){delete xml;xml=NULL;}
	return true;
}

}
