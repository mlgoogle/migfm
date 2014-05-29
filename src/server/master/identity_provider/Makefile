
###############################################################################
## 输出文件名
OUTPUT_FILE = ./../../bin/plugins/identity_provider/identity_provider.so

## 编译器
COMPIlER = g++
COMPILERC = gcc
## 编译选项
## for debug -DROOM_DEBUG_MCACHE
CPP_FLAG = -g -Wall -w -fPIC -c -D_LINUX -DDEBUG -D_DEBUG -D_REENTRANT -D_GNU_SOURCE -DROOM_LOG_ERROR -DROOM_LOG_MSG
## for release
## CPP_FLAG = -O3 -Wall -s -w -fPIC -c -D_LINUX -DNDEBUG -D_REENTRANT -D_GNU_SOURCE -DROOM_LOG_ERROR -DROOM_LOG_MSG

## 链接选项
## for debug
LINK_FLAGS = -g -shared -fPIC
## for release
## LINK_FLAGS = -s -O3 -shared -fPIC

## 头文件
INCLUDE_PATHS = -I./ -I/usr/local/libmemcached-1.0.7/include/ -I../core -I../../public/ -I/usr/local/include\
				 -I/usr/include/mysql -I./../../../third_party/hiredis  -I/usr/include/glib-2.0/\
				 -I/usr/include/glib-2.0/glib/ -I/usr/include/xmlsec1/ -I/usr/include/libxml2/ -I/usr/lib64/glib-2.0/include/

## 库
LINK_PATHS = -L./ -L/usr/local/libmemcached-1.0.7/lib/  -L/usr/lib64/mysql
LINK_LIBRARYS = -lmemcached -lmemcachedutil  -lexpat -lrt -lpthread -ldl -liconv -lmysqlclient\
				../../lib/libbreakpad_client.a ../../lib/libhiredis.a

##3rd
THIRD_PATH = ../../public
# 编译
COMPILE = $(COMPIlER) $(CPP_FLAG) $(INCLUDE_PATHS) -o $@

# 链接
LINK = $(COMPIlER) $(LINK_FLAGS) $(LINK_PATHS) $? -o $(OUTPUT_FILE) $(LINK_LIBRARYS)

all: clean output delete-temp-file
output:	logic_comm.o identity_provider.o thread_handler.o thread_lock.o\
	identity_provider_engine.o identity_provider_init.o db_comm.o dic_comm.o\
	basic_info.o http_packet.o basictypes.o config.o \
	mig_log.o mem_storage_impl.o mysql_storage_impl.o redis_storage_impl.o\
	redis_warrper.o storage.o constants.o basic_util.o
	$(LINK)

clean:
	-rm -f *.o
	-rm -f $(OUTPUT_FILE)

delete-temp-file:
	-rm -f *.o

### 编译
logic_comm.o:
	$(COMPILE) ./logic_comm.cc

identity_provider.o:
	$(COMPILE) ./identity_provider.cc

identity_provider_engine.o:
	$(COMPILE) ./identity_provider_engine.cc

identity_provider_init.o:
	$(COMPILE) ./identity_provider_init.cc

dic_comm.o:
	$(COMPILE) ./dic_comm.cc
	
db_comm.o:
	$(COMPILE) ./db_comm.cc
thread_lock.o:
	$(COMPILE) ./thread_lock.cc

thread_handler.o:
	$(COMPILE) ./thread_handler.cc
basic_info.o:
	$(COMPILE) ./../../public/basic/basic_info.cc

http_packet.o:
	$(COMPILE) ./../../public/basic/http_packet.cc

basictypes.o:
	$(COMPILE) ./../../public/basic/basictypes.cc

config.o:
	$(COMPILE) ./../../public/config/config.cc

mig_log.o:
	$(COMPILE) ./../../public/log/mig_log.cc

mem_storage_impl.o:
	$(COMPILE) ./../../public/storage/mem_storage_impl.cc

mysql_storage_impl.o:
	$(COMPILE) ./../../public/storage/mysql_storage_impl.cc

redis_storage_impl.o:
	$(COMPILE) ./../../public/storage/redis_storage_impl.cc

redis_warrper.o:
	$(COMPILERC) -std=c99 -pedantic -c $(INCLUDE_PATHS) -O3 -fPIC  -Wall -W -Wstrict-prototypes -Wwrite-strings -g -ggdb ./../../public/storage/redis_warrper.c

storage.o:
	$(COMPILE) ./../../public/storage/storage.cc

constants.o:
	$(COMPILE) ./../../public/basic/constants.cc

basic_util.o:
	$(COMPILE) ./../../public/basic/basic_util.cc
