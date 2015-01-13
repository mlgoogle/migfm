
###############################################################################
## 输出文件名
OUTPUT_FILE = ./../../bin/hfcrsrv/plugins/sociality_manager/sociality_manager.so

## 编译器
COMPIlER = g++
COMPILERC = gcc

LIB_DIR = ./../../../library
USER_DIR = ../../pub
CUSTOM_THIRD_PATH = $(LIB_DIR)/third_party

## 编译选项
## for debug -DROOM_DEBUG_MCACHE
CPP_FLAG = -g -Wall -w -fPIC -c -D_LINUX -DDEBUG -D_DEBUG -D_DB_POOL_ -D_DIC_POOL_ -D_REENTRANT -D_GNU_SOURCE -DROOM_LOG_ERROR -DROOM_LOG_MSG -DPROCESS_WORK
## for release
## CPP_FLAG = -O3 -Wall -s -w -fPIC -c -D_LINUX -DNDEBUG -D_DB_POOL_ -D_DIC_POOL_ -D_REENTRANT -D_GNU_SOURCE -DROOM_LOG_ERROR -DROOM_LOG_MSG -DPROCESS_WORK

## 链接选项
## for debug
LINK_FLAGS = -g -shared -fPIC
## for release
## LINK_FLAGS = -s -O3 -shared -fPIC

## 头文件
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
				-I./../\

## 库
LINK_PATHS = -L./ -L/usr/local/libmemcached-1.0.7/lib/  -L/usr/lib64/mysql
LINK_LIBRARYS = -lmemcached -lmemcachedutil  -lexpat -lrt -lpthread -ldl -liconv -lmysqlclient -lcurl\
				../../lib/libbreakpad_client.a ../../lib/libhiredis.a ../../lib/robot_interface.a\
				$(CUSTOM_THIRD_PATH)/dmg_fp/lib/dmg_fp.a $(LIB_DIR)/base/lib/mybase.a

##3rd
THIRD_PATH = $(LIB_DIR)/public/
# 编译
COMPILE = $(COMPIlER) $(CPP_FLAG) $(INCLUDE_PATHS) -o $@

# 链接
LINK = $(COMPIlER) $(LINK_FLAGS) $(LINK_PATHS) $? -o $(OUTPUT_FILE) $(LINK_LIBRARYS)

all: clean output delete-temp-file
output:	 http_method.o dic_comm.o logic_comm.o thread_handler.o sociality_mgr_engine.o \
	sociality_mgr_init.o http_response.o thread_lock.o db_comm.o http_comm.o \
	basic_info.o http_packet.o basictypes.o config.o base64.o radom_in.o\
	mig_log.o mem_storage_impl.o mysql_storage_impl.o redis_storage_impl.o\
	redis_warrper.o storage.o constants.o basic_util.o errno_comm.o json_reader.o\
	json_value.o json_writer.o db_storage.o dic_storage.o
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

http_comm.o:
	$(COMPILE) ./http_comm.cc

logic_comm.o:
	$(COMPILE) ./logic_comm.cc

thread_handler.o:
	$(COMPILE) ./thread_handler.cc

sociality_mgr_engine.o:
	$(COMPILE) ./sociality_mgr_engine.cc

sociality_mgr_init.o:
	$(COMPILE) ./sociality_mgr_init.cc


	
#############public#######################################
http_response.o:
	$(COMPILE) ./http_response.cc

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
	
db_storage.o:
	$(COMPILE) $(THIRD_PATH)/storage/db_storage.cc

dic_storage.o:
	$(COMPILE) $(THIRD_PATH)/storage/dic_storage.cc

constants.o:
	$(COMPILE) $(THIRD_PATH)/basic/constants.cc

basic_util.o:
	$(COMPILE) $(THIRD_PATH)/basic/basic_util.cc

errno_comm.o:
	$(COMPILE) $(THIRD_PATH)/basic/errno_comm.cc

http_method.o:
	$(COMPILE) $(THIRD_PATH)/http/http_method.cc

json_reader.o:
	$(COMPILE) $(THIRD_PATH)/json/json_reader.cpp

json_value.o:
	$(COMPILE) $(THIRD_PATH)/json/json_value.cpp

json_writer.o:
	$(COMPILE) $(THIRD_PATH)/json/json_writer.cpp

