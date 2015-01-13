
###############################################################################
## 输出文件名
OUTPUT_FILE = ./../../bin/hfcrsrv/plugins/music_manager/music_manager.so

## 编译器
COMPIlER = g++
COMPILERC = gcc

LIB_DIR = ./../../../library
USER_DIR = ../../pub
CUSTOM_THIRD_PATH = $(LIB_DIR)/third_party

## 编译选项
## for debug -DROOM_DEBUG_MCACHE
CPP_FLAG = -g -Wall -w -fPIC -c -D_LINUX -DDEBUG -D_DEBUG -D_DB_POOL_ -D_DIC_POOL_ -DPROCESS_WORK -D_REENTRANT -D_GNU_SOURCE -DROOM_LOG_ERROR -DROOM_LOG_MSG
## for release
##CPP_FLAG = -O3 -Wall -s -w -fPIC -c -D_LINUX -DNDEBUG -D_DB_POOL_ -D_DIC_POOL_ -D-DPROCESS_WORK  -D_REENTRANT -D_GNU_SOURCE -DROOM_LOG_ERROR -DROOM_LOG_MSG

## 链接选项
## for debug
LINK_FLAGS = -g -shared -fPIC
## for release
## LINK_FLAGS = -s -O3 -shared -fPIC

## 头文件
INCLUDE_PATHS = -I/usr/local/libmemcached-1.0.7/include \
				-I$(LIB_DIR)/core \
				-I$(THIRD_PATH)\
				-I/usr/local/include\
				-I/usr/include/mysql\
				-I$(USER_DIR)\
				-I$(CUSTOM_THIRD_PATH)/hiredis\
				-I$(CUSTOM_THIRD_PATH)\
				-I$(LIB_DIR)/base\
				-I../../robot/\
				-I$(LIB_DIR)/public\
				-I./../\


## 库
LINK_PATHS = -L./ -L/usr/lib64/mysql -L/usr/local/lib
LINK_LIBRARYS = -lcurl -lmysqlclient -lmemcached -lmemcachedutil \
	-lexpat -lrt -lpthread -ldl -liconv \
	../../lib/libbreakpad_client.a ../../lib/libhiredis.a ../../lib/robot_interface.a $(CUSTOM_THIRD_PATH)/dmg_fp/lib/dmg_fp.a

##3rd
THIRD_PATH = $(LIB_DIR)/public/
# 编译
COMPILE = $(COMPIlER) $(CPP_FLAG) $(INCLUDE_PATHS) -o $@

# 链接
LINK = $(COMPIlER) $(LINK_FLAGS) $(LINK_PATHS) $? -o $(OUTPUT_FILE) $(LINK_LIBRARYS)

all: clean output delete-temp-file
output:	dic_comm.o logic_comm.o thread_handler.o music_mgr_engine.o \
	music_mgr_init.o music_cache_mgr.o get_song.o http_response.o thread_lock.o db_comm.o\
	basic_info.o http_packet.o basictypes.o config.o base64.o radom_in.o\
	mig_log.o mem_storage_impl.o mysql_storage_impl.o redis_storage_impl.o\
	redis_warrper.o storage.o constants.o basic_util.o errno_comm.o json_reader.o\
	json_value.o json_writer.o music_recording.o file_path.o file_util.o\
	string_serialization.o algorithm_base.o
	$(LINK)

clean:
	-rm -f *.o
	-rm -f $(OUTPUT_FILE)

delete-temp-file:
	-rm -f *.o

### 编译
db_comm.o:
	$(COMPILE) ./db_comm.cc

dic_comm.o:
	$(COMPILE) ./dic_comm.cc

logic_comm.o:
	$(COMPILE) ./logic_comm.cc

thread_handler.o:
	$(COMPILE) ./thread_handler.cc

music_mgr_engine.o:
	$(COMPILE) ./music_mgr_engine.cc

music_mgr_init.o:
	$(COMPILE) ./music_mgr_init.cc

music_recording.o:
	$(COMPILE) ./music_recording.cc

get_song.o:
	$(COMPILE) ./get_song.cc

http_response.o:
	$(COMPILE) ./http_response.cc

music_cache_mgr.o:
	$(COMPILE) ./music_cache_mgr.cc

thread_lock.o:
	$(COMPILE) ./thread_lock.cc

basic_info.o:
	$(COMPILE) $(THIRD_PATH)/basic/basic_info.cc

http_packet.o:
	$(COMPILE) $(THIRD_PATH)/protocol/http_packet.cc

radom_in.o:
	$(COMPILE) $(THIRD_PATH)/basic/radom_in.cc

basictypes.o:
	$(COMPILE) $(THIRD_PATH)/basic/basictypes.cc

base64.o:
	$(COMPILE) $(THIRD_PATH)/basic/base64.cc

config.o:
	$(COMPILE) $(THIRD_PATH)/config/config.cc

mig_log.o:
	$(COMPILE) $(THIRD_PATH)/log/mig_log.cc

file_path.o:
	$(COMPILE) $(THIRD_PATH)/file/file_path.cc
	
file_util.o:
	$(COMPILE) $(THIRD_PATH)/file/file_util.cc

string_serialization.o:
	$(COMPILE) $(THIRD_PATH)/file/string_serialization.cc
	
mem_storage_impl.o:
	$(COMPILE) $(THIRD_PATH)/storage/mem_storage_impl.cc

mysql_storage_impl.o:
	$(COMPILE) $(THIRD_PATH)/storage/mysql_storage_impl.cc

redis_storage_impl.o:
	$(COMPILE) $(THIRD_PATH)/storage/redis_storage_impl.cc

redis_warrper.o:
	$(COMPILERC) -std=c99 -pedantic -c $(INCLUDE_PATHS) -O3 -fPIC  -Wall -W -Wstrict-prototypes -Wwrite-strings -g -ggdb $(THIRD_PATH)/storage/redis_warrper.c

storage.o:
	$(COMPILE) $(THIRD_PATH)/storage/storage.cc

algorithm_base.o:
	$(COMPILE) $(THIRD_PATH)/algorithm/algorithm_base.cc

constants.o:
	$(COMPILE) $(THIRD_PATH)/basic/constants.cc

basic_util.o:
	$(COMPILE) $(THIRD_PATH)/basic/basic_util.cc

errno_comm.o:
	$(COMPILE) $(THIRD_PATH)/basic/errno_comm.cc

json_reader.o:
	$(COMPILE) $(THIRD_PATH)/json/json_reader.cpp

json_value.o:
	$(COMPILE) $(THIRD_PATH)/json/json_value.cpp

json_writer.o:
	$(COMPILE) $(THIRD_PATH)/json/json_writer.cpp

