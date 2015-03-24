
###############################################################################
## 编译器
COMPILER = g++
COMPILERC = gcc

## 编译选项
## for debug -DROOM_DEBUG_MCACHE
CPP_FLAG = -g -Wall -w -fPIC -c -D_LINUX -DDEBUG -D_DEBUG -D_REENTRANT -D_GNU_SOURCE -D__DEBUG_CORE__ -DFCGI_STD  #-DCHECK_HEADER-DTEST
RCPP_FLAG = -g -Wall -w -fPIC -c -D_LINUX -DDEBUG -D_DEBUG -D_REENTRANT -D_GNU_SOURCE -DFCGI_STD #-DTEST
## for release
##CPP_FLAG = -O3 -Wall -s -w -fPIC -c -D_LINUX -DNDEBUG -D_REENTRANT -D_GNU_SOURCE -DFCGI_STD

## 链接选项
## for debug
LINK_FLAGS = -g  -fPIC
## for release
## LINK_FLAGS = -s -O3 -shared -fPIC

LIB_DIR = ./../../library
BASE_DIR = ./../pub
## 头文件
INCLUDE_PATHS = -I$(LIB_DIR)/public/ -I /usr/local/libmemcached-1.0.7/include\
		-I/usr/local/lib -I$(LIB_DIR)/third_party/hiredis -I$(BASE_DIR)/\
		-I$(LIB_DIR)/third_party/breakpad/src

## 库
LINK_PATHS =  -L/usr/local/libmemcached-1.0.7/lib/ -L/usr/lib64/mysql
LINK_LIBRARYS = -lmemcached -lmemcachedutil -lrt -lpthread  -liconv -lfcgi++ -lmysqlclient -lexpat -lcurl\
					./../lib/libbreakpad_client.a ./../lib/libhiredis.a $(LINK_CORE_PATH)

# 编译
COMPILE = $(COMPILER) $(CPP_FLAG) $(INCLUDE_PATHS) -o $@
RCOMPILE = $(COMPILER) $(RCPP_FLAG) $(INCLUDE_PATHS) -o $@

# 链接
LINK = $(COMPILER) $(LINK_FLAGS) $(LINK_PATHS) $? -o $(OUTPUT_FILE) $(LINK_LIBRARYS)
LINKALL = $(COMPILER) $(LINK_FLAGS) $(LINK_PATHS) $?  $(LINK_LIBRARYS)











POBJ =  mig_log.o core_interface.o
OUTPUT_FILE = /var/www/cgi-bin/miyo
#############################################################################################################################
all: clean proc delete-temp-file

proc: socproc chatproc musicproc userproc

######################userproc###############################################
USERPROC_PATH = /user
OTJERPROC_VERSION = /v1
userproc:quicklogin.fcgi bdbindpush.fcgi thirdlogin.fcgi loginrecord.fcgi update.fcgi

quicklogin.fcgi: quicklogin.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(USERPROC_PATH)$(OTJERPROC_VERSION)/quicklogin.fcgi

thirdlogin.fcgi: thirdlogin.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(USERPROC_PATH)$(OTJERPROC_VERSION)/thirdlogin.fcgi
	
bdbindpush.fcgi: bindpush.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(USERPROC_PATH)$(OTJERPROC_VERSION)/bdbindpush.fcgi
	
loginrecord.fcgi: loginrecord.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(USERPROC_PATH)$(OTJERPROC_VERSION)/loginrecord.fcgi
	
update.fcgi: update.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(USERPROC_PATH)$(OTJERPROC_VERSION)/update.fcgi
### 编译

quicklogin.o:
	$(COMPILE) .$(USERPROC_PATH)$(OTJERPROC_VERSION)/quicklogin/quicklogin.cc
	
bindpush.o:
	$(COMPILE) .$(USERPROC_PATH)$(OTJERPROC_VERSION)/bindpush/bindpush.cc

thirdlogin.o:
	$(COMPILE) .$(USERPROC_PATH)$(OTJERPROC_VERSION)/thirdlogin/thirdlogin.cc
	
loginrecord.o:
	$(COMPILE) .$(USERPROC_PATH)$(OTJERPROC_VERSION)/loginrecord/loginrecord.cc

update.o:
	$(COMPILE) .$(USERPROC_PATH)$(OTJERPROC_VERSION)/update/update.cc
	
######################musicproc###############################################
MUSICPROC_PATH = /music
OTJERPROC_VERSION = /v1
musicproc:getcltsongs.fcgi getdimension.fcgi getdimensions.fcgi collectsong.fcgi delcltsong.fcgi

getcltsongs.fcgi: getcltsongs.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(MUSICPROC_PATH)$(OTJERPROC_VERSION)/getcltsongs.fcgi

getdimension.fcgi: getdimension.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(MUSICPROC_PATH)$(OTJERPROC_VERSION)/getdimension.fcgi
	
getdimensions.fcgi: getdimensions.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(MUSICPROC_PATH)$(OTJERPROC_VERSION)/getdimensions.fcgi

collectsong.fcgi: collectsong.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(MUSICPROC_PATH)$(OTJERPROC_VERSION)/collectsong.fcgi
	
delcltsong.fcgi: delcltsong.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(MUSICPROC_PATH)$(OTJERPROC_VERSION)/delcltsong.fcgi

### 编译

getcltsongs.o:
	$(COMPILE) .$(MUSICPROC_PATH)$(OTJERPROC_VERSION)/getcltsongs/getcltsongs.cc

getdimension.o:
	$(COMPILE) .$(MUSICPROC_PATH)$(OTJERPROC_VERSION)/getdimension/getdimension.cc
	
getdimensions.o:
	$(COMPILE) .$(MUSICPROC_PATH)$(OTJERPROC_VERSION)/getdimensions/getdimensions.cc

collectsong.o:
	$(COMPILE) .$(MUSICPROC_PATH)$(OTJERPROC_VERSION)/collectsong/collectsong.cc
	
delcltsong.o:
	$(COMPILE) .$(MUSICPROC_PATH)$(OTJERPROC_VERSION)/delcltsong/delcltsong.cc
clean:
	-rm -f *.o
	-rm -f $(OUTPUT_FILE)./*.fcgi

delete-temp-file:
	-rm -f *.o
	
	
######################socproc###############################################
SOCPROC_PATH = /soc
OTJERPROC_VERSION = /v1
socproc:getbarragecomm.fcgi getlocation.fcgi sayhello.fcgi presentsong.fcgi

getbarragecomm.fcgi: getbarragecomm.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(SOCPROC_PATH)$(OTJERPROC_VERSION)/getbarragecomm.fcgi

getlocation.fcgi: getlocation.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(SOCPROC_PATH)$(OTJERPROC_VERSION)/getlocation.fcgi

sayhello.fcgi: sayhello.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(SOCPROC_PATH)$(OTJERPROC_VERSION)/sayhello.fcgi
	
presentsong.fcgi: presentsong.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(SOCPROC_PATH)$(OTJERPROC_VERSION)/presentsong.fcgi
### 编译

getbarragecomm.o:
	$(COMPILE) .$(SOCPROC_PATH)$(OTJERPROC_VERSION)/getbarragecomm/getbarragecomm.cc

getlocation.o:
	$(COMPILE) .$(SOCPROC_PATH)$(OTJERPROC_VERSION)/getlocation/getlocation.cc
	
sayhello.o:
	$(COMPILE) .$(SOCPROC_PATH)$(OTJERPROC_VERSION)/sayhello/sayhello.cc
	
presentsong.o:
	$(COMPILE) .$(SOCPROC_PATH)$(OTJERPROC_VERSION)/presentsong/presentsong.cc
	

clean:
	-rm -f *.o
	-rm -f $(OUTPUT_FILE)./*.fcgi

delete-temp-file:
	-rm -f *.o
	
######################chatproc###############################################
CHATPROC_PATH = /chat
OTJERPROC_VERSION = /v1
chatproc:groupmessage.fcgi alonemessage.fcgi getsc.fcgi

groupmessage.fcgi: groupmessage.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(CHATPROC_PATH)$(OTJERPROC_VERSION)/groupmessage.fcgi
	
alonemessage.fcgi: alonemessage.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(CHATPROC_PATH)$(OTJERPROC_VERSION)/alonemessage.fcgi

getsc.fcgi: getsc.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(CHATPROC_PATH)$(OTJERPROC_VERSION)/getsc.fcgi

### 编译

groupmessage.o:
	$(COMPILE) .$(CHATPROC_PATH)$(OTJERPROC_VERSION)/groupmessage/groupmessage.cc
	

alonemessage.o:
	$(COMPILE) .$(CHATPROC_PATH)$(OTJERPROC_VERSION)/alonemessage/alonemessage.cc

getsc.o:
	$(COMPILE) .$(CHATPROC_PATH)$(OTJERPROC_VERSION)/getsc/getsc.cc
	

clean:
	-rm -f *.o
	-rm -f $(OUTPUT_FILE)./*.fcgi

delete-temp-file:
	-rm -f *.o




############public###############################
mig_log.o:
	$(COMPILE) $(LIB_DIR)/public/log/mig_log.cc

core_interface.o:
	$(COMPILE) $(LIB_DIR)/public/comm/core_interface.cc
