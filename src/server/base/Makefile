
###############################################################################
## 输出文件名
OUTPUT_FILE = ./../lib/mybase.a

## 编译器
COMPIlER = g++
COMPILERC = gcc
AR = ar cr

## 编译选项
## for debug -DROOM_DEBUG_MCACHE
CPP_FLAG = -g -Wall -w -fPIC -c -D_LINUX -DDEBUG -D_DEBUG -D_DB_POOL_ -D_DIC_POOL_ -D_REENTRANT -D_GNU_SOURCE -DUSR_LOG_ERROR -DUSR_LOG_MSG
## for release
#CPP_FLAG = -O3 -Wall -s -w -fPIC -c -D_LINUX -DNDEBUG -D_DB_POOL_ -D_DIC_POOL_ -D_REENTRANT -D_GNU_SOURCE -DUSR_LOG_ERROR -DUSR_LOG_MSG

## 链接选项
## for debug
LINK_FLAGS = 
## for release
## LINK_FLAGS = -s -O3 -shared -fPIC

## 头文件
INCLUDE_PATHS = -I./ -I ../ -I/usr/local/libmemcached-1.0.7/include/ -I../core -I../public/ -I/usr/local/include\
				 -I/usr/include/mysql -I./../../third_party/hiredis  -I../base

## 库
LINK_PATHS = -L./ -L/usr/local/libmemcached-1.0.7/lib/  -L/usr/lib64/mysql
LINK_LIBRARYS = -lmemcached -lmemcachedutil  -lexpat -lrt -lpthread -ldl -liconv -lmysqlclient -lcurl\
				../lib/libbreakpad_client.a ../lib/libhiredis.a

##3rd
THIRD_PATH = ../public

# 编译
COMPILE = $(COMPIlER) $(CPP_FLAG) $(INCLUDE_PATHS) -o $@

# 链接
LINK = $(AR)  $(OUTPUT_FILE) $? 

all: clean output delete-temp-file
output:	base_db_mysql_auto.o basic_lbs_info.o lbs_baidu_connector.o lbs_connector.o\
	lbs_db_comm.o lbs_logic_unit.o logic_comm.o api_errno.o\
	http_api.o base_thread_handler.o base_thread_lock.o caiyun_weather_engine.o\
	weather_engine.o weather_logic_unit.o\
	basic_push_info.o push_baidu_connector.o push_connector.o push_db_comm.o push_logic_unit.o\
	storage.o mem_storage_impl.o mysql_storage_impl.o redis_storage_impl.o\
	redis_warrper.o http_method.o http_method.o\
	json_reader.o json_writer.o json_value.o\
	constants.o basic_info.o basictypes.o basic_util.o
	$(LINK)

clean:
	-rm -f *.o
	-rm -f $(OUTPUT_FILE)

delete-temp-file:
	-rm -f *.o

### 编译

base_db_mysql_auto.o:
	$(COMPILE) ./db/base_db_mysql_auto.cc

basic_lbs_info.o:
	$(COMPILE) ./lbs/basic_lbs_info.cc

lbs_baidu_connector.o:
	$(COMPILE) ./lbs/lbs_baidu_connector.cc

lbs_connector.o:
	$(COMPILE) ./lbs/lbs_connector.cc
	
lbs_db_comm.o:
	$(COMPILE) ./lbs/lbs_db_comm.cc
	
lbs_logic_unit.o:
	$(COMPILE) ./lbs/lbs_logic_unit.cc
	
logic_comm.o:
	$(COMPILE) ./logic/logic_comm.cc
	
api_errno.o:
	$(COMPILE) ./net/api_errno.cc
	
http_api.o:
	$(COMPILE) ./net/http_api.cc

base_thread_handler.o:
	$(COMPILE) ./thread/base_thread_handler.cc

base_thread_lock.o:
	$(COMPILE) ./thread/base_thread_lock.cc
	

caiyun_weather_engine.o:
	$(COMPILE) ./weather/caiyun_weather_engine.cc

weather_engine.o:
	$(COMPILE) ./weather/weather_engine.cc

weather_logic_unit.o:
	$(COMPILE) ./weather/weather_logic_unit.cc
	
	
basic_push_info.o:
	$(COMPILE) ./pushmsg/basic_push_info.cc

push_baidu_connector.o:
	$(COMPILE) ./pushmsg/push_baidu_connector.cc

push_connector.o:
	$(COMPILE) ./pushmsg/push_connector.cc

push_db_comm.o:
	$(COMPILE) ./pushmsg/push_db_comm.cc
	
push_logic_unit.o:
	$(COMPILE) ./pushmsg/push_logic_unit.cc
	
##基础
basic_info.o:
	$(COMPILE) $(THIRD_PATH)/basic/basic_info.cc
	
basictypes.o:
	$(COMPILE) $(THIRD_PATH)/basic/basictypes.cc
	
basic_util.o:
	$(COMPILE) $(THIRD_PATH)/basic/basic_util.cc
	
constants.o:
	$(COMPILE) $(THIRD_PATH)/basic/constants.cc
	
##操作
http_method.o:
	$(COMPILE) $(THIRD_PATH)/http/http_method.cc
	
##协议层
http_packet.o:
	$(COMPILE) $(THIRD_PATH)/protocol/http_method.cc
	
##数据库存储
mem_storage_impl.o:
	$(COMPILE) $(THIRD_PATH)/storage/mem_storage_impl.cc

mysql_storage_impl.o:
	$(COMPILE) $(THIRD_PATH)/storage/mysql_storage_impl.cc

mssql_storage_impl.o:
	$(COMPILE) $(THIRD_PATH)/storage/mssql_storage_impl.cc
	
redis_storage_impl.o:
	$(COMPILE) $(THIRD_PATH)/storage/redis_storage_impl.cc
	
redis_warrper.o:
	$(COMPILERC) -std=c99 -pedantic -c $(INCLUDE_PATHS) -O3 -fPIC  -Wall -W -Wstrict-prototypes -Wwrite-strings -g -ggdb $(THIRD_PATH)/storage/redis_warrper.c

storage.o:
	$(COMPILE) $(THIRD_PATH)/storage/storage.cc
	
##JSON
json_reader.o:
	$(COMPILE) $(THIRD_PATH)/json/json_reader.cpp

json_value.o:
	$(COMPILE) $(THIRD_PATH)/json/json_value.cpp

json_writer.o:
	$(COMPILE) $(THIRD_PATH)/json/json_writer.cpp
	
#日志
mig_log.o:
	$(COMPILE) $(THIRD_PATH)/log/mig_log.cc
	


