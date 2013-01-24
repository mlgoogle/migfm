
###############################################################################
## 输出文件名
OUTPUT_FILE = idp.fcgi
## 编译器
COMPILER = g++
COMPILERC = gcc

## 编译选项
## for debug -DROOM_DEBUG_MCACHE
CPP_FLAG = -g -Wall -w -fPIC -c -D_LINUX -DDEBUG -DMIG_SSO -D_DEBUG -D_REENTRANT -D_GNU_SOURCE -DFCGI_STD #-DTEST
## for release
## CPP_FLAG = -O3 -Wall -s -w -fPIC -c -D_LINUX -DNDEBUG -D_REENTRANT -D_GNU_SOURCE -DROOM_LOG_ERROR -DROOM_LOG_MSG -DROOM_CHECK_COLUMN -DROOM_URANDOM_EXIST

## 链接选项
## for debug
LINK_FLAGS = -g  -fPIC
## for release
## LINK_FLAGS = -s -O3 -shared -fPIC

## 头文件
INCLUDE_PATHS = -I../public/ -I /usr/local/libmemcached-1.0.7/include\
		-I/usr/local/lib -I/root/third_party/hiredis\
	        -I/root/third_party/breakpad/src/  -I/usr/include/mysql\
                -I/usr/include/glib-2.0/ -I/usr/include/xmlsec1/ \
                -I/usr/include/libxml2/ -I/usr/lib64/glib-2.0/include/

## 库
LINK_PATHS =  -L/usr/local/libmemcached-1.0.7/lib/ -L/usr/lib64/mysql
LINK_LIBRARYS = -lmemcached -lmemcachedutil -lrt -lpthread  -liconv -lfcgi++ -lmysqlclient -lexpat -llasso\
					../lib/libbreakpad_client.a ../lib/libhiredis.a $(LINK_CORE_PATH)

# 编译
COMPILE = $(COMPILER) $(CPP_FLAG) $(INCLUDE_PATHS) -o $@

# 链接
LINK = $(COMPILER) $(LINK_FLAGS) $(LINK_PATHS) $? -o $(OUTPUT_FILE) $(LINK_LIBRARYS)

all: clean output delete-temp-file

output: mig_idp.o mig_log.o id_provider.o config.o \
	basic_info.o dic_serialization.o db_serialization.o\
	mem_storage_impl.o mysql_storage_impl.o\
	storage.o constants.o redis_warrper.o redis_storage_impl.o\
	basictypes.o 
	$(LINK)

clean:
	-rm -f *.o
	-rm -f $(OUTPUT_FILE)

delete-temp-file:
	-rm -f *.o

### 编译
mig_idp.o:
	$(COMPILE) ./mig_idp.cc

id_provider.o:
	$(COMPILE) ./id_provider.cc

mig_log.o:
	$(COMPILE) ./../public/log/mig_log.cc

config.o:
	$(COMPILE) ./../public/config/config.cc

basic_info.o:
	$(COMPILE) ./../public/basic/basic_info.cc


dic_serialization.o:
	$(COMPILE) ./../public/storage/dic_serialization.cc

db_serialization.o:
	$(COMPILE) ./../public/storage/db_serialization.cc

mem_storage_impl.o:
	$(COMPILE) ./../public/storage/mem_storage_impl.cc

mysql_storage_impl.o:
	$(COMPILE) ./../public/storage/mysql_storage_impl.cc

storage.o:
	$(COMPILE) ./../public/storage/storage.cc

constants.o:
	$(COMPILE) ./../public/basic/constants.cc

redis_warrper.o:
	$(COMPILERC) -std=c99 -pedantic -c $(INCLUDE_PATHS) -O3 -fPIC  -Wall -W -Wstrict-prototypes -Wwrite-strings -g -ggdb ./../public/storage/redis_warrper.c

redis_storage_impl.o:
	$(COMPILE) ./../public/storage/redis_storage_impl.cc

basictypes.o:
	$(COMPILE) ./../public/basic/basictypes.cc
