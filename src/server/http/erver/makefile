
###############################################################################
## 输出文件名
OUTPUT_FILE = erver.fcgi
## 编译器
COMPILER = g++
COMPILERC = gcc

## 编译选项
## for debug -DROOM_DEBUG_MCACHE
CPP_FLAG = -g -Wall -w -fPIC -c -D_LINUX -DDEBUG -D_DEBUG -D_REENTRANT -D_GNU_SOURCE
## for release
## CPP_FLAG = -O3 -Wall -s -w -fPIC -c -D_LINUX -DNDEBUG -D_REENTRANT -D_GNU_SOURCE -DROOM_LOG_ERROR -DROOM_LOG_MSG -DROOM_CHECK_COLUMN -DROOM_URANDOM_EXIST

## 链接选项
## for debug
LINK_FLAGS = -g  -fPIC
## for release
## LINK_FLAGS = -s -O3 -shared -fPIC

## 头文件
INCLUDE_PATHS = -I../public -I /usr/local/libmemcached-1.0.7/include/ -I/usr/local/lib -I../../third_party/hiredis/ -I../../third_party/breakpad/src -I/usr/include/mysql 

## 库
LINK_PATHS =  -L/usr/local/libmemcached-1.0.7/lib/ -L/usr/lib64/mysql
LINK_LIBRARYS = -lmemcached -lmemcachedutil -lrt -lpthread  -liconv -lfcgi -lmysqlclient -lexpat\
					libbreakpad_client.a libhiredis.a $(LINK_CORE_PATH)

# 编译
COMPILE = $(COMPILER) $(CPP_FLAG) $(INCLUDE_PATHS) -o $@

# 链接
LINK = $(COMPILER) $(LINK_FLAGS) $(LINK_PATHS) $? -o $(OUTPUT_FILE) $(LINK_LIBRARYS)

all: clean output delete-temp-file

output:	erver.o mig_log.o conn_addr.o storage.o constants.o mysql_storage_impl.o\
		mysql_opertion.o basictypes.o config.o redis_warrper.o redis_storage_impl.o\
		mem_storage_impl.o
	$(LINK)

clean:
	-rm -f *.o
	-rm -f $(OUTPUT_FILE)

delete-temp-file:
	-rm -f *.o

### 编译
erver.o:
	$(COMPILE) ./erver.cc

mig_log.o:
	$(COMPILE) ./../public/mig_log.cc

conn_addr.o:
	$(COMPILE) ./../public/conn_addr.cc

mem_storage_impl.o:
	$(COMPILE) ./../public/mem_storage_impl.cc

mysql_storage_impl.o:
	$(COMPILE) ./../public/mysql_storage_impl.cc

storage.o:
	$(COMPILE) ./../public/storage.cc

constants.o:
	$(COMPILE) ./../public/constants.cc

mysql_opertion.o:
	$(COMPILE) ./../public/mysql_opertion.cc

redis_warrper.o:
	$(COMPILERC) -std=c99 -pedantic -c $(INCLUDE_PATHS) -O3 -fPIC  -Wall -W -Wstrict-prototypes -Wwrite-strings -g -ggdb ./../public/redis_warrper.c

redis_storage_impl.o:
	$(COMPILE) ./../public/redis_storage_impl.cc

basictypes.o:
	$(COMPILE) ./../public/basictypes.cc

config.o:
	$(COMPILE) ./../public/config.cc
