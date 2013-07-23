###############################################################################
## 输出文件名
OUTPUT_FILE = ../../bin/plugins/lbs/lbs.so

## 编译器
COMPILERC = gcc
COMPIlER = g++

## 编译选项
## for debug -DROOM_DEBUG_MCACHE
CPP_FLAG = -g -Wall -w -fPIC -c -D_LINUX -DDEBUG -D_DEBUG -D_REENTRANT -D_GNU_SOURCE
## for release
## CPP_FLAG = -O3 -Wall -s -w -fPIC -c -D_LINUX -DNDEBUG -D_REENTRANT -D_GNU_SOURCE -DROOM_LOG_ERROR -DROOM_LOG_MSG -DROOM_CHECK_COLUMN -DROOM_URANDOM_EXIST

## 链接选项
## for debug
LINK_FLAGS = -g -s -shared -fPIC
## for release
## LINK_FLAGS = -s -O3 -shared -fPIC

## 头文件
INCLUDE_PATHS = -I./ -I../core -I../../public/ -I../../../third_party/hiredis/ \
	-I/usr/include/mysql -I/usr/local/include/libmemcached-1.0

## 库
LINK_PATHS = -L./ -L/usr/lib64/mysql -L/usr/local/lib
LINK_LIBRARYS = -lcurl -lmysqlclient -lmemcached -lmemcachedutil libhiredis.a \
	-lexpat -lrt -lpthread -ldl -liconv \
	../../lib/libbreakpad_client.a ../../lib/libhiredis.a

##3rd
THIRD_PATH = 
# 编译
COMPILE = $(COMPIlER) $(CPP_FLAG) $(INCLUDE_PATHS) -o $@

# 链接
LINK = $(COMPIlER) $(LINK_FLAGS) $(LINK_PATHS) $? -o $(OUTPUT_FILE) $(LINK_LIBRARYS)

all: clean output delete-temp-file
output:	main.o mig_log.o http_response.o baidu_lbs_connector.o redis_connector.o \
	json_reader.o json_value.o json_writer.o lbs_logic.o \
	storage.o redis_storage_impl.o redis_warrper.o basic_info.o constants.o basic_util.o \
	dic_comm.o logic_comm.o thread_handler.o http_packet.o basictypes.o config.o
	$(LINK)

clean:
	-rm -f *.o
	-rm -f $(OUTPUT_FILE)

delete-temp-file:
	-rm -f *.o

### 编译

main.o:
	$(COMPILE) ./main.cc

mig_log.o:
	$(COMPILE) ../../public/log/mig_log.cc

lbs_logic.o:
	$(COMPILE) ./lbs_logic.cc

http_response.o:
	$(COMPILE) ./http_response.cc

baidu_lbs_connector.o:
	$(COMPILE) ./baidu_lbs_connector.cc

redis_connector.o:
	$(COMPILE) ./redis_connector.cc

json_reader.o:
	$(COMPILE) ../../public/json/json_reader.cpp

json_value.o:
	$(COMPILE) ../../public/json/json_value.cpp

json_writer.o:
	$(COMPILE) ../../public/json/json_writer.cpp

basic_info.o:
	$(COMPILE) ../../public/basic/basic_info.cc
	
storage.o:
	$(COMPILE) ../../public/storage/storage.cc

redis_storage_impl.o:
	$(COMPILE) ../../public/storage/redis_storage_impl.cc

dic_comm.o:
	$(COMPILE) ./dic_comm.cc

logic_comm.o:
	$(COMPILE) ./logic_comm.cc

thread_handler.o:
	$(COMPILE) ./thread_handler.cc

http_packet.o:
	$(COMPILE) ./../../public/basic/http_packet.cc

basictypes.o:
	$(COMPILE) ./../../public/basic/basictypes.cc

config.o:
	$(COMPILE) ./../../public/config/config.cc

constants.o:
	$(COMPILE) ./../../public/basic/constants.cc

basic_util.o:
	$(COMPILE) ./../../public/basic/basic_util.cc

redis_warrper.o:
	$(COMPILERC) -std=c99 -pedantic -c $(INCLUDE_PATHS) -O3 -fPIC  -Wall -W -Wstrict-prototypes -Wwrite-strings -g -ggdb ../../public/storage/redis_warrper.c
