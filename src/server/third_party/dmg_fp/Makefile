
###############################################################################
## 输出文件名
OUTPUT_FILE = ./lib/dmg_fp.a

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
INCLUDE_PATHS =  -I/usr/local/include


## 库
LINK_PATHS = -L./ 
LINK_LIBRARYS =  -lrt -lpthread -ldl 


# 编译
COMPILE = $(COMPIlER) $(CPP_FLAG) $(INCLUDE_PATHS) -o $@

# 链接
LINK = $(AR)  $(OUTPUT_FILE) $? 

all: clean output delete-temp-file
output:	dtoa.o g_fmt.o
	$(LINK)

clean:
	-rm -f *.o
	-rm -f $(OUTPUT_FILE)

delete-temp-file:
	-rm -f *.o

### 编译

dtoa.o:
	$(COMPILE) ./dtoa.cc

g_fmt.o:
	$(COMPILE) ./g_fmt.cc


