#include "core.h"
#include "common.h"
#include "parser_xml.h"
#include "plugins.h"
#include "thread.h"
#include "network.h"
#include "clock_task.h"
#include "buffer.h"
#include "log/mig_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <locale.h>
#include <dirent.h>
#include <string.h>
#include <signal.h>
#include <sys/resource.h>
//#include "client/linux/handler/exception_handler.h"

static struct server *srvt = NULL;

/*
static bool DumpCallBack(const char* dump_path,const char* minidump_id,
						 void* contect,bool succeeded){

	printf("Dump path:%s/%s.dump\n",dump_path,minidump_id);

	return succeeded;
}
*/
static struct server* server_init(void){

	struct server  *srv; 
	srv = (struct server*)calloc(1,sizeof(*srv));    
	assert(srv);

#define CLEAN(x)\
	srv->x = buffer_init();

	CLEAN(srv_conf.bindhost);
	CLEAN(srv_conf.error_file);
	CLEAN(srv_conf.usr_file);
	CLEAN(srv_conf.sys_file);
	CLEAN(srv_conf.username);
	CLEAN(srv_conf.modules_dir);
	CLEAN(srv_conf.max_worker);
	CLEAN(srv_conf.max_fds);
	CLEAN(srv_conf.max_conns);
#if defined (NET_WORK)
	CLEAN(srv_conf.port);
#endif
#if defined (PROCESS_WORK)
	CLEAN(srv_conf.process_path);
#endif
	CLEAN(module_path);
	CLEAN(tmp_buf);
#undef CLEAN

	INIT_LIST_HEAD(&srv->plugins_xml_path);
	INIT_LIST_HEAD(&srv->plugins_des);
	INIT_LIST_HEAD(&srv->srv_conf.remotes);

	srv->connect_pool = NULL;
	srv->ncount_connect = 0;
	srv->state = SERVER_NO_START;

	return srv;
}

static void server_free(struct server *srv)
{
	struct list_head *tmp;
	struct list_head  *n;
	struct plugin_xml_path *pl_path;
	struct plugin_desc  *pl_desc;
	struct connect_config *cont_conf;
#define CLEAN(x)\
	if(srv->x!=NULL)\
	buffer_free(srv->x);\
	srv->x=NULL;

	CLEAN(srv_conf.bindhost);
	CLEAN(srv_conf.error_file);
	CLEAN(srv_conf.usr_file);
	CLEAN(srv_conf.sys_file);
	CLEAN(srv_conf.username);
	CLEAN(srv_conf.modules_dir); 
	CLEAN(srv_conf.max_worker);
	CLEAN(srv_conf.max_fds);
	CLEAN(srv_conf.max_conns);
#if defined (NET_WORK)
	CLEAN(srv_conf.port);
#endif
#if defined (PROCESS_WORK)
	CLEAN(srv_conf.process_path);
#endif
	CLEAN(module_path);
	CLEAN(tmp_buf);
#undef CLEAN
	//clean list
	list_for_each_safe(tmp,n,&srv->plugins_xml_path){
		pl_path = list_entry(tmp,struct plugin_xml_path,list);
		if(pl_path){
			list_del(&pl_path->list);
			buffer_free(pl_path->filepath);
			free(pl_path);
			pl_path = NULL;
		}
	}

	//clean remote
	list_for_each_safe(tmp,n,&srv->srv_conf.remotes){
		cont_conf = list_entry(tmp,struct connect_config,list);
		if(cont_conf){
			list_del(&cont_conf->list);
			buffer_free(cont_conf->port);
			buffer_free(cont_conf->bindremote);
			free(cont_conf);
			cont_conf = NULL;
		}	
	}

	list_for_each_safe(tmp,n,&srv->plugins_des){
		pl_desc = list_entry(tmp,struct plugin_desc,list);
#define CLEAN(x)\
	buffer_free(pl_desc->x);\
	pl_desc->x = NULL;

		if(pl_desc){
			list_del(&pl_desc->list);
			CLEAN(id);
			CLEAN(name);
			CLEAN(version);
			CLEAN(provider);
			CLEAN(path);
			free(pl_desc);
			pl_desc = NULL;
		}
	}
#undef CLEAN
	free(srv);
	srv=NULL;
}


static void sig_term (int sig)
{

	// shutdown
	MIG_INFO(USER_LEVEL,"shutdown");
	srvt->state = __sync_lock_test_and_set(&srvt->state,0);
	if(srvt->state==SERVER_NO_START){
		MIG_INFO(USER_LEVEL,"quit");
		return;
	}
	srvt->state = SERVER_NO_START;
	network_stop(srvt);
	MIG_INFO(USER_LEVEL,"networkstop");
	stop_threadpool(srvt);
	MIG_INFO(USER_LEVEL,"stop_threadpool");
	plugins_free(srvt);
	MIG_INFO(USER_LEVEL,"plugins_free");
	server_free(srvt);
	exit(0);
}

static void sig_hup (int sig){
	// config
}

static void set_signals (void){
	struct sigaction action;

	sigemptyset (&action.sa_mask);
	action.sa_flags = 0;
	action.sa_handler = sig_term;

#ifdef SIGTERM
	sigaction (SIGTERM, &action, NULL);
#endif

#ifdef SIGINT
	sigaction (SIGINT, &action, NULL);
#endif

	action.sa_handler = sig_hup;

#ifdef SIGHUP
	sigaction (SIGHUP, &action, NULL);
#endif

#ifdef SIGPIPE
	signal (SIGPIPE, SIG_IGN);
#endif

#ifdef SIGCHLD
	signal (SIGCHLD, SIG_IGN);
#endif

#ifdef SIGUSR1
#endif

#ifdef SIGUSR2
#endif

}

static int open_dev_null(int fd){
	int tmpfd;
	close(fd);

	tmpfd = open("/dev/null",O_RDWR);
	if(tmpfd!=-1 && tmpfd!=fd){

		dup2(tmpfd,fd);
		close(tmpfd);
	}

	return (tmpfd!=-1)?0:-1;
}


static int set_ulimit(){
	struct rlimit rlim;
	if (srvt->srv_conf.max_fds!=0){
		struct rlimit rlim_new;
		if (getrlimit(RLIMIT_CORE,&rlim)==0){
			rlim_new.rlim_cur = rlim_new.rlim_max = RLIM_INFINITY;
			if (setrlimit(RLIMIT_CORE,&rlim_new)!=0){
				rlim_new.rlim_cur = rlim_new.rlim_max = rlim.rlim_max;
				(void)setrlimit(RLIMIT_CORE,&rlim_new);
			}

		}

		if ((getrlimit(RLIMIT_CORE,&rlim)!=0)||rlim.rlim_cur==0){
			MIG_ERROR(USER_LEVEL,"failed to ensure corefile creation");
			return 0;
		}
	}

	if (srvt->srv_conf.max_conns!=0){
		if (getrlimit(RLIMIT_NOFILE, &rlim) != 0) {
			MIG_ERROR(USER_LEVEL, "failed to getrlimit number of files");
			return 0;
		} else {
			rlim.rlim_cur = atol(srvt->srv_conf.max_conns->ptr);
			rlim.rlim_max = atol(srvt->srv_conf.max_conns->ptr);
			if (setrlimit(RLIMIT_NOFILE, &rlim) != 0) {
				MIG_ERROR(SYSTEM_LEVEL, "failed to set rlimit for open files."
					      "Try starting as root or requesting smaller maxconns value");
				return 0;
			}
		}
	}
	return 1;

}

__attribute__((visibility("default")))
int core_main(int agrc,char* argv[]){
    
	if ((srvt=server_init())==NULL){
	    MIG_ERROR(USER_LEVEL,"server_init error[%s]",strerror(errno));
		return -1;
	}

	set_signals();

	open_dev_null(STDIN_FILENO);
	open_dev_null(STDOUT_FILENO);

	if (config_read(srvt)<0){
		MIG_ERROR(USER_LEVEL,"xml parser error\n");
		goto rel_srv;
	}
	srvt->state = SERVER_RD_CONF;

	set_ulimit();

	if (get_plugin_info(srvt)<0){
		MIG_ERROR(SYSTEM_LEVEL,"get plugins xml error");
		goto rel_plugins;
	}

	srvt->state = SERVER_RD_PLUGINS;
	MIG_INFO(USER_LEVEL,"get plugins xml success");

	if (plugins_load(srvt)){
		MIG_ERROR(USER_LEVEL,"loading plugins finally faield");
		goto rel_plugins;
	}

	srvt->state = SERVER_LOAD_PLUGINS;
	MIG_INFO(USER_LEVEL,"load plugins success");


	if (HANDLER_GO_ON!=plugins_call_init(srvt)){
		MIG_ERROR(USER_LEVEL,"initialization of plugins failed.Going down");
		goto rel_plugins;
	}

	srvt->state = SERVER_INIT_PLUGINS;
	MIG_INFO(USER_LEVEL,"Init Plugins Success");

	if (init_threadpool(srvt)<0){
		MIG_ERROR(USER_LEVEL,"initialization of threadpool error");
		goto rel_plugins;
	}

	///test threadpool

	srvt->state = SERVER_INIT_TP;
	MIG_INFO(USER_LEVEL,"init threadpool success");



	if (init_clock(srvt)<0){
		MIG_ERROR(USER_LEVEL,"initialization of clock error");
		goto rel_plugins;
	}

	srvt->state = SERVER_INIT_CLOCK;
	MIG_INFO(USER_LEVEL,"init clock success");


	if (network_init(srvt)<0){
		MIG_ERROR(USER_LEVEL,"initialization of network init error");
		goto rel_net;
	}


	srvt->state = SERVER_INIT_NET;
	MIG_INFO(USER_LEVEL,"register event success");

	if (network_register_fdevents(srvt)){
		MIG_ERROR(USER_LEVEL,"register network event error");
		goto rel_net;
	}

    if(create_connects(srvt)<0){
    	MIG_ERROR(USER_LEVEL,"initalization of connect");
    }


	if (network_start(srvt)<0){
		MIG_ERROR(USER_LEVEL,"start network error");
		goto rel_netstop;
	}

	/*clean_up*/
rel_netstop:
	network_stop(srvt);
rel_net:
	stop_threadpool(srvt);
rel_plugins:
	plugins_free(srvt);
rel_srv: 
	server_free(srvt);
	return 0; 
}
