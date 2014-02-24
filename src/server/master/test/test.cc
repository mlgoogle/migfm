#include<stdio.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<malloc.h>
#include<stddef.h>
#include<linux/types.h>
#include<errno.h>
#include<netinet/in.h>
#include<assert.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/un.h>
#include<fcntl.h>
#include <string>

#define SENDBUFFER(sock,buf,nbytes)\
	int amt = 0;\
	int total = 0;\
	do{\
	amt = nbytes;\
	amt = send(sock,buf,amt,0);\
	buf = (char*)buf+amt;\
	nbytes -= amt;\
	total += amt;\
	}while(amt!=-1&&nbytes>0)\



int main(int agrc,char* argv[]){

	int   sock_;
	int   flags;
	std::string  path = "/root/pj/server/bin/corefile";
	struct sockaddr_un addr;
	if((sock_=socket(AF_UNIX,SOCK_STREAM,0))==-1){
		printf("unix socket create error (%s)\n",strerror(errno));
		return 0;
	}
	if(flags=fcntl(sock_,F_GETFL,0)<0||
		fcntl(sock_,F_SETFL,flags|O_NONBLOCK)<0){
			printf("setting O_NONBLOCK error(%s)\n",strerror(errno));
			close(sock_);
			return 0;
	}
	/*if(lstat(path.c_str(),&tstat)==0){
	if(S_ISSOCK(tstat.st_mode))
	unlink(path.c_str());
	}*/
	setsockopt(sock_,SOL_SOCKET,SO_REUSEADDR,(void*)&flags,sizeof(flags));
	setsockopt(sock_,SOL_SOCKET,SO_KEEPALIVE,(void*)&flags,sizeof(flags));
	setsockopt(sock_,SOL_SOCKET,SO_LINGER,(void*)&flags,sizeof(flags));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path,path.c_str(),path.length());
	if(connect(sock_,(struct sockaddr*)&addr,sizeof(addr))==-1){
		printf("connect error (%s) \n",strerror(errno));
		close(sock_);
		return 0;
	}
	const char* str = "kerry123123";
	int len = strlen(str)+1;
	printf("======%d=======%d======\n",strlen(str),len);
	SENDBUFFER(sock_,str,len);

	return 1;
}