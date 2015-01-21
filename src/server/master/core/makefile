
###############################################################################
## 输出文件名
OUTPUT_FILE = ./../../server_dev/bin/hfcrsrvV2/plugins/core/core.so
## 编译器
COMPILER = g++
COMPILERC = gcc

## 编译选项
## for debug -DROOM_DEBUG_MCACHE
CPP_FLAG = -g -Wall -w -fPIC -c -rdynamic -D_LINUX -DDEBUG -D_DEBUG -D_REENTRANT -D_GNU_SOURCE -DPROCESS_WORK
## for release
##CPP_FLAG = -O3 -Wall -s -w -fPIC -c -rdynamic -D_LINUX -DNDEBUG -D_REENTRANT -D_GNU_SOURCE -DFCGI_STD -DPROCESS_WORK

## 链接选项
## for debug
LINK_FLAGS = -g  -fPIC -shared -lrt
## for release
## LINK_FLAGS = -s -O3 -shared -fPIC

## 头文件
INCLUDE_PATHS = -I./../public -I../../../third_party/breakpad/src

## 库
LINK_PATHS =   -L/usr/local/lib

LINK_LIBRARYS = -lpthread -lexpat -ldl -levent

# 编译
COMPILE = $(COMPILER) $(CPP_FLAG) $(INCLUDE_PATHS) -o $@
COMPILEC = $(COMPILERC) $(CPP_FLAG) $(INCLUDE_PATHS) -o $@

# 链接
LINK = $(COMPILER) $(LINK_FLAGS) $(LINK_PATHS) $? -o $(OUTPUT_FILE) $(LINK_LIBRARYS)

all: clean output delete-temp-file

output: base_thread.o base_threadlock.o base_threadpool.o buffer.o\
	clock_task.o core.o network.o parser_xml.o plugins.o thread.o\
	mig_log.o native_library.o 
	$(LINK)

clean:
	-rm -f *.o
	-rm -f $(OUTPUT_FILE)

delete-temp-file:
	-rm -f *.o

### 编译
core.o:
	$(COMPILE) ./core.c

buffer.o:
	$(COMPILE) ./buffer.c

parser_xml.o:
	$(COMPILE) ./parser_xml.c

plugins.o:
	$(COMPILE) ./plugins.c

base_thread.o:
	$(COMPILEC) ./base_thread.c

base_threadlock.o:
	$(COMPILEC) ./base_threadlock.c

base_threadpool.o:
	$(COMPILEC) ./base_threadpool.c

thread.o:
	$(COMPILE) ./thread.c

network.o:
	$(COMPILE) ./network.c

native_library.o:
	$(COMPILE) ./native_library.c

clock_task.o:
	$(COMPILE) ./clock_task.c

mig_log.o:
	$(COMPILE) ./../public/log/mig_log.cc 

