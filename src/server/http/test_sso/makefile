
###############################################################################
## 输出文件名
OUTPUT_FILE = ./test_sso

## 编译器
COMPIlER = g++
COMPILERC = gcc
## 编译选项
## for debug -DROOM_DEBUG_MCACHE
CPP_FLAG = -g -Wall -w -fPIC -c -D_LINUX -DDEBUG -D_DEBUG -D_REENTRANT -D_GNU_SOURCE -DROOM_LOG_ERROR -DROOM_LOG_MSG -DROOM_CHECK_COLUMN -DROOM_URANDOM_EXIST
## for release
# CPP_FLAG = -O3 -Wall -s -w  -c -D_LINUX -DNDEBUG -D_REENTRANT -D_GNU_SOURCE

## 链接选项
## for debug
LINK_FLAGS = -g 
## for release
## LINK_FLAGS = -s -O3 -shared -fPIC

## 头文件
INCLUDE_PATHS = -I./ -I/usr/local/include/ -I/usr/include/glib-2.0/ -I/usr/include/xmlsec1/ \
                -I/usr/include/libxml2/ -I/usr/lib64/glib-2.0/include/ -I../public/ \
                -I/usr/local/libmemcached-1.0.7/include -I/root/third_party/hiredis\
		-I/usr/include/mysql/

## 库
LINK_PATHS = -L./ -L/usr/local/libmemcached-1.0.7/lib/ -L./usr/lib64/mysql/
LINK_LIBRARYS = -lpthread -llasso -lmemcached -lmemcachedutil -lexpat  ../lib/libmysqlclient.a ../lib/libhiredis.a

##3rd
THIRD_PATH = ../
# 编译
COMPILE = $(COMPIlER) $(CPP_FLAG) $(INCLUDE_PATHS) -o $@

# 链接
LINK = $(COMPIlER) $(LINK_FLAGS) $(LINK_PATHS) $? -o $(OUTPUT_FILE) $(LINK_LIBRARYS)

all: clean output delete-temp-file
output:	test_sso.o mig_log.o dic_serialization.o db_serialization.o\
	basic_info.o mem_storage_impl.o mysql_storage_impl.o\
        storage.o constants.o redis_warrper.o redis_storage_impl.o\
        basictypes.o config.o xml_serialization.o basic_util.o json_writer.o\
        json_value.o json_reader.o 
	$(LINK)

clean:
	-rm -f *.o
	-rm -f $(OUTPUT_FILE)

delete-temp-file:
	-rm -f *.o

### 编译

test_sso.o:
	$(COMPILE) ./test_sso.cc

mig_log.o:
	$(COMPILE) ./../public/log/mig_log.cc

dic_serialization.o:
	$(COMPILE) ./../public/storage/dic_serialization.cc

db_serialization.o:
	$(COMPILE) ./../public/storage/db_serialization.cc

basic_info.o:
	$(COMPILE) ./../public/basic/basic_info.cc

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

config.o:
	$(COMPILE) ./../public/config/config.cc

xml_serialization.o:
	$(COMPILE) ./../public/xmpp/xml_serialization.cc

basic_util.o:
	$(COMPILE) ./../public/basic/basic_util.cc

json_reader.o:
	$(COMPILE) ./../public/json/json_reader.cpp

json_value.o:
	$(COMPILE) ./../public/json/json_value.cpp

json_writer.o:
	$(COMPILE) ./../public/json/json_writer.cpp
