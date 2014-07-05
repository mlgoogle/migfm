
###############################################################################
## 输出文件名
OUTPUT_FILE = ./../../bin/hfcrsrv/plugins/user_manager/user_manager.so

## 编译器
COMPIlER = g++
COMPILERC = gcc
## 编译选项
## for debug -DROOM_DEBUG_MCACHE
CPP_FLAG = -g -Wall -w -fPIC -c -D_LINUX -DDEBUG -D_DEBUG -D_DB_POOL_  -D_DIC_POOL_ -DPROCESS_WORK -D_REENTRANT -D_GNU_SOURCE -DROOM_LOG_ERROR -DROOM_LOG_MSG
## for release
##CPP_FLAG = -O3 -Wall -s -w -fPIC -c -D_LINUX -DNDEBUG -D_REENTRANT -D_DB_POOL_ -D_DIC_POOL_ -DPROCESS_WORK -D_GNU_SOURCE -DROOM_LOG_ERROR -DROOM_LOG_MSG

## 链接选项
## for debug
LINK_FLAGS = -g -shared -fPIC
## for release
## LINK_FLAGS = -s -O3 -shared -fPIC

## 头文件
INCLUDE_PATHS = -I./ -I/usr/local/libmemcached-1.0.7/include/ -I../core -I../../public/ -I../../robot/  -I/usr/local/include\
				 -I/usr/include/mysql -I./../../../third_party/hiredis 

## 库
LINK_PATHS = -L./ -L/usr/local/libmemcached-1.0.7/lib/  -L/usr/lib64/mysql
LINK_LIBRARYS = -lmemcached -lmemcachedutil  -lexpat -lrt -lpthread -ldl -liconv -lmysqlclient\
				../../lib/libbreakpad_client.a ../../lib/libhiredis.a ../../lib/robot_interface.a

##3rd
THIRD_PATH = ../../public
# 编译
COMPILE = $(COMPIlER) $(CPP_FLAG) $(INCLUDE_PATHS) -o $@

# 链接
LINK = $(COMPIlER) $(LINK_FLAGS) $(LINK_PATHS) $? -o $(OUTPUT_FILE) $(LINK_LIBRARYS)

all: clean output delete-temp-file
output:	db_comm.o dic_comm.o logic_comm.o thread_handler.o thread_lock.o usr_mgr_engine.o \
	usr_mgr_init.o basic_info.o http_packet.o basictypes.o config.o \
	mig_log.o mem_storage_impl.o mysql_storage_impl.o redis_storage_impl.o\
	redis_warrper.o storage.o constants.o basic_util.o errno_comm.o md5sum.o dic_storage.o radom_in.o
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

thread_lock.o:
	$(COMPILE) ./thread_lock.cc

usr_mgr_engine.o:
	$(COMPILE) ./usr_mgr_engine.cc

usr_mgr_init.o:
	$(COMPILE) ./usr_mgr_init.cc

basic_info.o:
	$(COMPILE) ./../../public/basic/basic_info.cc

http_packet.o:
	$(COMPILE) ./../../public/protocol/http_packet.cc

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

dic_storage.o:
	$(COMPILE) ./../../public/storage/dic_storage.cc

redis_warrper.o:
	$(COMPILERC) -std=c99 -pedantic -c $(INCLUDE_PATHS) -O3 -fPIC  -Wall -W -Wstrict-prototypes -Wwrite-strings -g -ggdb ./../../public/storage/redis_warrper.c

storage.o:
	$(COMPILE) ./../../public/storage/storage.cc

constants.o:
	$(COMPILE) ./../../public/basic/constants.cc

basic_util.o:
	$(COMPILE) ./../../public/basic/basic_util.cc
	
radom_in.o:
	$(COMPILE) ./../../public/basic/radom_in.cc

errno_comm.o:
	$(COMPILE) ./../../public/basic/errno_comm.cc

md5sum.o:
	$(COMPILE) ./../../public/basic/md5sum.cc
