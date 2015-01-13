
###############################################################################
## 输出文件名
OUTPUT_FILE = ./../../bin/robot/plugins/robot_manager/robot_manager.so

## 编译器
COMPIlER = g++
COMPILERC = gcc


LIB_DIR = ./../../../library
USER_DIR = ../../pub
CUSTOM_THIRD_PATH = $(LIB_DIR)/third_party

## 编译选项
## for debug -DROOM_DEBUG_MCACHE
CPP_FLAG = -g -Wall -w -fPIC -c -D_LINUX -DDEBUG -D_DEBUG -D_DB_POOL_ -D_DIC_POOL_ -DNET_WORK -D_REENTRANT -D_GNU_SOURCE
## for release
## CPP_FLAG = -O3 -Wall -s -w -fPIC -c -D_LINUX -DNDEBUG -D_DB_POOL_ -D_DIC_POOL_ -D_REENTRANT -D_GNU_SOURCE -DUSR_LOG_ERROR -DUSR_LOG_MSG

## 链接选项
## for debug
LINK_FLAGS = -g -shared -fPIC
## for release
## LINK_FLAGS = -s -O3 -shared -fPIC

## 头文件
#INCLUDE_PATHS = -I./ -I./../ -I$(LIB_DIR)/core -I$(LIB_DIR)/public/ -I$(LIB_DIR)/third_party/hiredis/ \
	-I/usr/include/mysql -I/usr/local/include/libmemcached-1.0 \
	-I$(USER_DIR) -I$(CUSTOM_THIRD_PATH)/hiredis -I$(CUSTOM_THIRD_PATH)\

INCLUDE_PATHS = -I/usr/local/libmemcached-1.0.7/include \
				-I$(LIB_DIR)/core \
				-I$(THIRD_PATH)\
				-I/usr/local/include\
				-I/usr/include/mysql\
				-I$(USER_DIR)\
				-I$(CUSTOM_THIRD_PATH)/hiredis\
				-I$(CUSTOM_THIRD_PATH)\
				-I$(LIB_DIR)/base\
				-I./../\
## 库
LINK_PATHS = -L./ -L/usr/lib64/mysql -L/usr/local/lib
LINK_LIBRARYS = -lcurl -lmysqlclient -lmemcached -lmemcachedutil \
	-lexpat -lrt -lpthread -ldl -liconv \
	../../lib/libbreakpad_client.a ../../lib/libhiredis.a\
	$(CUSTOM_THIRD_PATH)/dmg_fp/lib/dmg_fp.a $(LIB_DIR)/base/lib/mybase.a\
	

##3rd
THIRD_PATH = $(LIB_DIR)/public/

# 编译
COMPILE = $(COMPIlER) $(CPP_FLAG) $(INCLUDE_PATHS) -o $@

# 链接
LINK = $(COMPIlER) $(LINK_FLAGS) $(LINK_PATHS) $? -o $(OUTPUT_FILE) $(LINK_LIBRARYS)

all: clean output delete-temp-file
output:	robot_cache_manager.o robot_manager.o prize_rate.o robot_basic_info.o  robot_connection.o robot_song_mgr.o util_mgr.o scheduler_mgr.o protocol.o \
		robot_init.o http_method.o robot_weather_mgr.o db_comm.o dic_comm.o basictypes.o basic_util.o logic_unit.o logic_comm.o\
		radom_in.o data_packet.o constants.o mig_log.o redis_storage_impl.o mem_storage_impl.o mysql_storage_impl.o\
		redis_warrper.o storage.o json_reader.o json_writer.o json_value.o config.o basic_info.o md5sum.o  base64.o
	$(LINK)

clean:
	-rm -f *.o
	-rm -f $(OUTPUT_FILE)

delete-temp-file:
	-rm -f *.o

### 编译

robot_init.o:
	$(COMPILE) ./robot_init.cc

robot_manager.o:
	$(COMPILE) ./robot_manager.cc

robot_connection.o:
	$(COMPILE) ./robot_connection.cc
	
robot_song_mgr.o:
	$(COMPILE) ./robot_song_mgr.cc
	
robot_weather_mgr.o:
	$(COMPILE) ./robot_weather_mgr.cc

util_mgr.o:
	$(COMPILE) ./util_mgr.cc
	
scheduler_mgr.o:
	$(COMPILE) ./scheduler_mgr.cc

db_comm.o:
	$(COMPILE) ./db_comm.cc
	
robot_cache_manager.o:
	$(COMPILE) ./robot_cache_manager.cc
	
http_method.o:
	$(COMPILE) ./http_method.cc
	
robot_basic_info.o:
	$(COMPILE) ./robot_basic_info.cc
	
dic_comm.o:
	$(COMPILE) ./dic_comm.cc

http_response.o:
	$(COMPILE) ./http_response.cc

file_mgr.o:
	$(COMPILE) ./file_mgr.cc

im_mgr.o:
	$(COMPILE) ./im_mgr.cc

prize_rate.o:
	$(COMPILE) ./prize_rate.cc
	
logic_comm.o:
	$(COMPILE) ./../base/logic_comm.cc
	
logic_unit.o:
	$(COMPILE) ./logic_unit.cc

user_connection.o:
	$(COMPILE) ./user_connection.cc

error_code.o:
	$(COMPILE) ./../base/error_code.cc

protocol.o:
	$(COMPILE) ./../base/protocol.cc

basictypes.o:
	$(COMPILE) $(THIRD_PATH)/basic/basictypes.cc

basic_util.o:
	$(COMPILE) $(THIRD_PATH)/basic/basic_util.cc

radom_in.o:
	$(COMPILE) $(THIRD_PATH)/basic/radom_in.cc

md5sum.o:
	$(COMPILE) $(THIRD_PATH)/basic/md5sum.cc

data_packet.o:
	$(COMPILE) $(THIRD_PATH)/protocol/data_packet.cc

constants.o:
	$(COMPILE) $(THIRD_PATH)/basic/constants.cc

mig_log.o:
	$(COMPILE) $(THIRD_PATH)/log/mig_log.cc

mem_storage_impl.o:
	$(COMPILE) $(THIRD_PATH)/storage/mem_storage_impl.cc

mysql_storage_impl.o:
	$(COMPILE) $(THIRD_PATH)/storage/mysql_storage_impl.cc

redis_storage_impl.o:
	$(COMPILE) $(THIRD_PATH)/storage/redis_storage_impl.cc
	
config.o:
	$(COMPILE) $(THIRD_PATH)/config/config.cc
	
basic_info.o:
	$(COMPILE) $(THIRD_PATH)/basic/basic_info.cc

base64.o:
	$(COMPILE) $(THIRD_PATH)/basic/base64.cc
	
redis_warrper.o:
	$(COMPILERC) -std=c99 -pedantic -c $(INCLUDE_PATHS) -O3 -fPIC  -Wall -W -Wstrict-prototypes -Wwrite-strings -g -ggdb $(THIRD_PATH)/storage/redis_warrper.c

storage.o:
	$(COMPILE) $(THIRD_PATH)/storage/storage.cc
	
json_reader.o:
	$(COMPILE) $(THIRD_PATH)/json/json_reader.cpp

json_value.o:
	$(COMPILE) $(THIRD_PATH)/json/json_value.cpp

json_writer.o:
	$(COMPILE) $(THIRD_PATH)/json/json_writer.cpp

