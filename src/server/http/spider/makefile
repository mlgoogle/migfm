
###############################################################################
## 输出文件名
OUTPUT_FILE = spider
## 编译器
COMPILER = g++
COMPILERC = gcc

## 编译选项
## for debug -DROOM_DEBUG_MCACHE
CPP_FLAG = -g -Wall -w -fPIC -c -D_LINUX -DDEBUG -D_DEBUG -D_REENTRANT -D_GNU_SOURCE -DFCGI_STD #-DTEST
## for release
## CPP_FLAG = -O3 -Wall -s -w -fPIC -c -D_LINUX -DNDEBUG -D_REENTRANT -D_GNU_SOURCE -DROOM_LOG_ERROR -DROOM_LOG_MSG -DROOM_CHECK_COLUMN -DROOM_URANDOM_EXIST

## 链接选项
## for debug
LINK_FLAGS = -g  -fPIC
## for release
## LINK_FLAGS = -s -O3 -shared -fPIC

## 头文件
INCLUDE_PATHS = -I../public/ -I /usr/local/libmemcached-1.0.7/include\
		-I/usr/local/lib -I../../third_party/hiredis\
	        -I../../third_party/breakpad/src -I/usr/include/mysql 

## 库
LINK_PATHS =  -L/usr/local/libmemcached-1.0.7/lib/ -L/usr/lib64/mysql
LINK_LIBRARYS = -lmemcached -lmemcachedutil -lrt -lpthread  -liconv -lfcgi++ -lmysqlclient -lexpat -lcurl\
					../lib/libbreakpad_client.a ../lib/libhiredis.a $(LINK_CORE_PATH)

# 编译
COMPILE = $(COMPILER) $(CPP_FLAG) $(INCLUDE_PATHS) -o $@
COMPLIEC = $(COMPILERC) $(CPP_FLAG) $(INCLUDE_PAHTS) -o $@

# 链接
LINK = $(COMPILER) $(LINK_FLAGS) $(LINK_PATHS) $? -o $(OUTPUT_FILE) $(LINK_LIBRARYS)

all: clean output delete-temp-file

output: spider.o http_engine.o http_response.o thread.o\
    	base_thread.o base_threadlock.o base_threadpool.o\
    	mig_log.o dic_serialization.o db_serialization.o base64.o\
	basic_info.o mem_storage_impl.o redis_storage_impl.o\
	storage.o constants.o redis_warrper.o mysql_storage_impl.o\
	basictypes.o config.o dic_serialization.o\
	db_serialization.o xml_serialization.o basic_util.o\
	xml_serialization.o xmlbuilder.o xmlconstants.o xmlelement.o\
	xmlnsstack.o xmlparser.o xmlprinter.o xmppstanzaparser.o qname.o\
	json_reader.o json_value.o json_writer.o
	$(LINK)

clean:
	-rm -f *.o
	-rm -f $(OUTPUT_FILE)

delete-temp-file:
	-rm -f *.o

### 编译
spider.o:
	$(COMPILE) ./spider.cc

http_engine.o:
	$(COMPILE) ./http_engine.cc

http_response.o:
	$(COMPILE) ./http_response.cc

thread.o:
	$(COMPILE) ./thread.cc

base_thread.o:
	gcc -g -Wall -fPIC -c -w -DNODEBUG -D_LINUX -D_REENTRANT -D_GNU_SOURCE -rdynamic ./../public/thread/base_thread.c 

base_threadlock.o:
	gcc -g -Wall -fPIC -c -w -DNODEBUG -D_LINUX -D_REENTRANT -D_GNU_SOURCE -rdynamic ./../public/thread/base_threadlock.c

base_threadpool.o:
	gcc -g -Wall -fPIC -c -w -DNODEBUG -D_LINUX -D_REENTRANT -D_GNU_SOURCE -rdynamic ./../public/thread/base_threadpool.c

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

base64.o:
	$(COMPILE) ./../public/basic/base64.cc

qname.o:
	$(COMPILE) ./../public/xmpp/qname.cc

xmlbuilder.o:
	$(COMPILE) ./../public/xmpp/xmlbuilder.cc

xmlconstants.o:
	$(COMPILE) ./../public/xmpp/xmlconstants.cc

xmlelement.o:
	$(COMPILE) ./../public/xmpp/xmlelement.cc

xmlnsstack.o:
	$(COMPILE) ./../public/xmpp/xmlnsstack.cc

xmlparser.o:
	$(COMPILE) ./../public/xmpp/xmlparser.cc

xmlprinter.o:
	$(COMPILE) ./../public/xmpp/xmlprinter.cc

xmppstanzaparser.o:
	$(COMPILE) ./../public/xmpp/xmppstanzaparser.cc

json_reader.o:
	$(COMPILE) ./../public/json/json_reader.cpp

json_value.o:
	$(COMPILE) ./../public/json/json_value.cpp

json_writer.o:
	$(COMPILE) ./../public/json/json_writer.cpp
