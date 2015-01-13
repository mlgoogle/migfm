
###############################################################################
## 编译器
COMPILER = g++
COMPILERC = gcc

## 编译选项
## for debug -DROOM_DEBUG_MCACHE
CPP_FLAG = -g -Wall -w -fPIC -c -D_LINUX -DDEBUG -D_DEBUG -D_REENTRANT -D_GNU_SOURCE -D__DEBUG_CORE__ -DFCGI_STD #-DTEST
RCPP_FLAG = -g -Wall -w -fPIC -c -D_LINUX -DDEBUG -D_DEBUG -D_REENTRANT -D_GNU_SOURCE -DFCGI_STD #-DTEST
## for release
##CPP_FLAG = -O3 -Wall -s -w -fPIC -c -D_LINUX -DNDEBUG -D_REENTRANT -D_GNU_SOURCE -DFCGI_STD

## 链接选项
## for debug
LINK_FLAGS = -g  -fPIC
## for release
## LINK_FLAGS = -s -O3 -shared -fPIC

LIB_DIR = ./../../library
BASE_DIR = ./../pub
## 头文件
INCLUDE_PATHS = -I$(LIB_DIR)/public/ -I /usr/local/libmemcached-1.0.7/include\
		-I/usr/local/lib -I$(LIB_DIR)/third_party/hiredis -I$(BASE_DIR)/\
		-I$(LIB_DIR)/third_party/breakpad/src

## 库
LINK_PATHS =  -L/usr/local/libmemcached-1.0.7/lib/ -L/usr/lib64/mysql
LINK_LIBRARYS = -lmemcached -lmemcachedutil -lrt -lpthread  -liconv -lfcgi++ -lmysqlclient -lexpat -lcurl\
					./../lib/libbreakpad_client.a ./../lib/libhiredis.a $(LINK_CORE_PATH)

# 编译
COMPILE = $(COMPILER) $(CPP_FLAG) $(INCLUDE_PATHS) -o $@
RCOMPILE = $(COMPILER) $(RCPP_FLAG) $(INCLUDE_PATHS) -o $@

# 链接
LINK = $(COMPILER) $(LINK_FLAGS) $(LINK_PATHS) $? -o $(OUTPUT_FILE) $(LINK_LIBRARYS)
LINKALL = $(COMPILER) $(LINK_FLAGS) $(LINK_PATHS) $?  $(LINK_LIBRARYS)











POBJ =  mig_log.o core_interface.o
OUTPUT_FILE = /var/www/cgi-bin/miyo
#############################################################################################################################
all: clean proc delete-temp-file

proc: robotproc usrmgrproc spreadproc socialityproc musicproc lbsproc managerproc


#####################robot################################################
ROBOTPROC_PATH = /robot
OTJERPROC_VERSION = /v1/
robotproc:getnewmusic.fcgi addnewmusic.fcgi getmailinfo.fcgi getspreadmail.fcgi getrobots.fcgi updaterobotpic.fcgi\
	 getvailedurlmusic.fcgi updatevailedurlmusic.fcgi getvailedlyric.fcgi updatelyric.fcgi getwbtoken.fcgi

getspreadmail.fcgi: getspreadmail.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(ROBOTPROC_PATH)$(OTJERPROC_VERSION)./getspreadmail.fcgi
	
getmailinfo.fcgi: getmailinfo.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(ROBOTPROC_PATH)$(OTJERPROC_VERSION)./getmailinfo.fcgi
	
getnewmusic.fcgi: getnewmusic.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(ROBOTPROC_PATH)$(OTJERPROC_VERSION)./getnewmusic.fcgi

addnewmusic.fcgi: addnewmusic.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(ROBOTPROC_PATH)$(OTJERPROC_VERSION)./addnewmusic.fcgi
	
getrobots.fcgi: getrobots.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(ROBOTPROC_PATH)$(OTJERPROC_VERSION)./getrobots.fcgi

updaterobotpic.fcgi: updaterobotpic.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(ROBOTPROC_PATH)$(OTJERPROC_VERSION)./updaterobotpic.fcgi
	
getvailedurlmusic.fcgi: getvailedurlmusic.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(ROBOTPROC_PATH)$(OTJERPROC_VERSION)./getvailedurlmusic.fcgi
	
getvailedlyric.fcgi: getvailedlyric.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(ROBOTPROC_PATH)$(OTJERPROC_VERSION)./getvailedlyric.fcgi
	
updatevailedurlmusic.fcgi: updatevailedurlmusic.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(ROBOTPROC_PATH)$(OTJERPROC_VERSION)./updatevailedurlmusic.fcgi

updatelyric.fcgi: updatelyric.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(ROBOTPROC_PATH)$(OTJERPROC_VERSION)./updatelyric.fcgi
	
getwbtoken.fcgi: getwbtoken.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(ROBOTPROC_PATH)$(OTJERPROC_VERSION)./getwbtoken.fcgi

### 编译
getspreadmail.o:
	$(COMPILE) ./getspreadmail/getspreadmail.cc
	
getmailinfo.o:
	$(COMPILE) ./getmailinfo/getmailinfo.cc
	
getnewmusic.o:
	$(COMPILE) ./getnewmusic/getnewmusic.cc
	
addnewmusic.o:
	$(COMPILE) ./addnewmusic/addnewmusic.cc
	
getrobots.o:
	$(COMPILE) ./getrobots/getrobots.cc

updaterobotpic.o:
	$(COMPILE) ./updaterobotpic/updaterobotpic.cc

getvailedurlmusic.o:
	$(COMPILE) ./getvailedurlmusic/getvailedurlmusic.cc
	
getvailedlyric.o:
	$(COMPILE) ./getvailedlyric/getvailedlyric.cc
	
updatevailedurlmusic.o:
	$(COMPILE) ./updatevailedurlmusic/updatevailedurlmusic.cc
	
updatelyric.o:
	$(COMPILE) ./updatelyric/updatelyric.cc
	
getwbtoken.o:
	$(COMPILE) ./getwbtoken/getwbtoken.cc
	
#####################chat_mgr#############################################
CHATPROC_PATH = /chat
OTJERPROC_VERSION = /v1/
chatmgrproc:getsc.fcgi

getsc.fcgi: getsc.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(CHATPROC_PATH)$(OTJERPROC_VERSION)./getsc.fcgi
	
### 编译

getsc.o:
	$(COMPILE) ./getsc/getsc.cc
	
	
######################usrmgrproc###############################################
USERPROC_PATH = /usr
OTJERPROC_VERSION = /v1
usrmgrproc:regedit.fcgi updateusr.fcgi guest.fcgi login.fcgi bdbindpush.fcgi

regedit.fcgi: regedit.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(USERPROC_PATH)$(OTJERPROC_VERSION)./regedit.fcgi
	
updateusr.fcgi: updateusr.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(USERPROC_PATH)$(OTJERPROC_VERSION)./updateusr.fcgi
	
get.fcgi: get.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(USERPROC_PATH)$(OTJERPROC_VERSION)./get.fcgi
	
guest.fcgi: guest.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(USERPROC_PATH)$(OTJERPROC_VERSION)./guest.fcgi
	
login.fcgi: login.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(USERPROC_PATH)$(OTJERPROC_VERSION)./login.fcgi
	
bdbindpush.fcgi: bdbindpush.o $(POBJ)
	$(LINKALL) -o$(OUTPUT_FILE)$(USERPROC_PATH)$(OTJERPROC_VERSION)./bdbindpush.fcgi
	
### 编译

regedit.o:
	$(COMPILE) ./regedit/regedit.cc

updateusr.o:
	$(COMPILE) ./updateusr/updateusr.cc
	
get.o:
	$(COMPILE) ./get/get.cc
	
guest.o:
	$(COMPILE) ./guest/guest.cc
	
login.o:
	$(COMPILE) ./login/login.cc
	
bdbindpush.o:
	$(COMPILE) ./bdbindpush/bdbindpush.cc
######################spread###############################################
SPREADPROC_PATH = /spread
OTJERPROC_VERSION = /v1/
spreadproc:weixin.fcgi

weixin.fcgi: weixin.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(SPREADPROC_PATH)$(OTJERPROC_VERSION)./weixin.fcgi
	
### 编译

weixin.o:
	$(COMPILE) ./weixin/weixin.cc
	
######################sociality###############################################
SOCPROC_PATH = /soc
OTJERPROC_VERSION = /v1/
socialityproc:setuserconfigofpush.fcgi  getpushmsg.fcgi\
	getpushmsgsummary.fcgi  commentsong.fcgi get_comment.fcgi\
	getmusicuser.fcgi getshareinfo.fcgi shareresult.fcgi

setuserconfigofpush.fcgi: setuserconfigofpush.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(SOCPROC_PATH)$(OTJERPROC_VERSION)./setuserconfigofpush.fcgi

getpushmsg.fcgi: getpushmsg.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(SOCPROC_PATH)$(OTJERPROC_VERSION)./getpushmsg.fcgi	
	
getpushmsgsummary.fcgi: getpushmsgsummary.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(SOCPROC_PATH)$(OTJERPROC_VERSION)./getpushmsgsummary.fcgi	
	
getfriendlist.fcgi: getfriendlist.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(SOCPROC_PATH)$(OTJERPROC_VERSION)./getfriendlist.fcgi
	
sendfriendmsg.fcgi: sendfriendmsg.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(SOCPROC_PATH)$(OTJERPROC_VERSION)./sendfriendmsg.fcgi

adduserbacklist.fcgi: adduserbacklist.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(SOCPROC_PATH)$(OTJERPROC_VERSION)./adduserbacklist.fcgi
	
addfriend.fcgi: addfriend.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(SOCPROC_PATH)$(OTJERPROC_VERSION)./addfriend.fcgi

importsonglist.fcgi: importsonglist.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(SOCPROC_PATH)$(OTJERPROC_VERSION)./importsonglist.fcgi
	
importfriend.fcgi: importfriend.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(SOCPROC_PATH)$(OTJERPROC_VERSION)./importfriend.fcgi
	
commentsong.fcgi: commentsong.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(SOCPROC_PATH)$(OTJERPROC_VERSION)./commentsong.fcgi
	
get_comment.fcgi: get_comment.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(SOCPROC_PATH)$(OTJERPROC_VERSION)./get_comment.fcgi
	
getmusicuser.fcgi: getmusicuser.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(SOCPROC_PATH)$(OTJERPROC_VERSION)./getmusicuser.fcgi

getshareinfo.fcgi: getshareinfo.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(SOCPROC_PATH)$(OTJERPROC_VERSION)./getshareinfo.fcgi
	
shareresult.fcgi: shareresult.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(SOCPROC_PATH)$(OTJERPROC_VERSION)./shareresult.fcgi
	
	
### 编译

setuserconfigofpush.o:
	$(COMPILE) ./setuserconfigofpush/setuserconfigofpush.cc
	
getpushmsg.o:
	$(COMPILE) ./getpushmsg/getpushmsg.cc

getpushmsgsummary.o:
	$(COMPILE) ./getpushmsgsummary/getpushmsgsummary.cc

getfriendlist.o:
	$(COMPILE) ./getfriendlist/getfriendlist.cc
	
sendfriendmsg.o:
	$(COMPILE) ./sendfriendmsg/sendfriendmsg.cc
	
adduserbacklist.o:
	$(COMPILE) ./adduserbacklist/adduserbacklist.cc
	
addfriend.o:
	$(COMPILE) ./addfriend/addfriend.cc
	
importsonglist.o:
	$(COMPILE) ./importsonglist/importsonglist.cc
	
importfriend.o:
	$(COMPILE) ./importfriend/importfriend.cc
	
commentsong.o:
	$(COMPILE) ./commentsong/commentsong.cc
	
get_comment.o:
	$(COMPILE) ./get_comment/get_comment.cc
	
getmusicuser.o:
	$(COMPILE) ./getmusicuser/getmusicuser.cc
	
getshareinfo.o:
	$(COMPILE) ./getshareinfo/getshareinfo.cc

shareresult.o:
	$(COMPILE) ./shareresult/shareresult.cc
	
######################music###################################################
MUSICPROC_PATH = /music
OTJERPROC_VERSION = /v1/
musicproc: collectsong.fcgi getcltsongs.fcgi delcltsong.fcgi hatesong.fcgi\
	delthsong.fcgi recordcursong.fcgi getdefsongs.fcgi gettypesongs.fcgi\
	recordlocalsongs.fcgi updateconfigfile.fcgi getuserhis.fcgi songid.fcgi\
	getword.fcgi word_song.fcgi getlyric.fcgi

collectsong.fcgi: collectsong.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(MUSICPROC_PATH)$(OTJERPROC_VERSION)./collectsong.fcgi

getcltsongs.fcgi: getcltsongs.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(MUSICPROC_PATH)$(OTJERPROC_VERSION)./getcltsongs.fcgi

delcltsong.fcgi: delcltsong.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(MUSICPROC_PATH)$(OTJERPROC_VERSION)./delcltsong.fcgi
	
hatesong.fcgi: hatesong.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(MUSICPROC_PATH)$(OTJERPROC_VERSION)./hatsong.fcgi
	
delthsong.fcgi: delthsong.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(MUSICPROC_PATH)$(OTJERPROC_VERSION)./delthsong.fcgi
	
recordcursong.fcgi: recordcursong.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(MUSICPROC_PATH)$(OTJERPROC_VERSION)./recordcursong.fcgi
	
getdefsongs.fcgi: getdefsongs.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(MUSICPROC_PATH)$(OTJERPROC_VERSION)./getdefsongs.fcgi
	
gettypesongs.fcgi: gettypesongs.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(MUSICPROC_PATH)$(OTJERPROC_VERSION)./gettypesongs.fcgi
	
recordlocalsongs.fcgi: recordlocalsongs.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(MUSICPROC_PATH)$(OTJERPROC_VERSION)./recordlocalsongs.fcgi
	
updateconfigfile.fcgi: updateconfigfile.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(MUSICPROC_PATH)$(OTJERPROC_VERSION)./updateconfigfile.fcgi
	
getuserhis.fcgi: getuserhis.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(MUSICPROC_PATH)$(OTJERPROC_VERSION)./getuserhis.fcgi
	
songid.fcgi: songid.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(MUSICPROC_PATH)$(OTJERPROC_VERSION)./songid.fcgi
	
getword.fcgi: getword.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(MUSICPROC_PATH)$(OTJERPROC_VERSION)./getword.fcgi
	
word_song.fcgi: word_song.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(MUSICPROC_PATH)$(OTJERPROC_VERSION)./word_song.fcgi
	
getlyric.fcgi: getlyric.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(MUSICPROC_PATH)$(OTJERPROC_VERSION)./getlyric.fcgi
	
### 编译

getcltsongs.o:
	$(COMPILE) ./getcltsongs/getcltsongs.cc

collectsong.o:
	$(COMPILE) ./collectsong/collectsong.cc
	
delcltsong.o:
	$(COMPILE) ./delcltsong/delcltsong.cc
	
hatesong.o:
	$(COMPILE) ./hatesong/hatesong.cc
	
delthsong.o:
	$(COMPILE) ./delthsong/delthsong.cc
	
recordcursong.o:
	$(COMPILE) ./recordcursong/recordcursong.cc
	
getdefsongs.o:
	$(COMPILE) ./getdefsongs/getdefsongs.cc
	
gettypesongs.o:
	$(COMPILE) ./gettypesongs/gettypesongs.cc
	
recordlocalsongs.o:
	$(COMPILE) ./recordlocalsongs/recordlocalsongs.cc
	
updateconfigfile.o:
	$(COMPILE) ./updateconfigfile/updateconfigfile.cc

getuserhis.o:
	$(COMPILE) ./getuserhis/getuserhis.cc
	
songid.o:
	$(COMPILE) ./songid/songid.cc
	
word_song.o:
	$(COMPILE) ./word_song/word_song.cc
	
getword.o:
	$(COMPILE) ./getword/getword.cc
	
getlyric.o:
	$(COMPILE) ./getlyric/getlyric.cc
######################manager###################################################
MANAGERPROC_PATH = /manager
OTJERPROC_VERSION = /v1/
managerproc: pushusermsg.fcgi pushusersmsg.fcgi

pushusermsg.fcgi: pushusermsg.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(MANAGERPROC_PATH)$(OTJERPROC_VERSION)./pushusermsg.fcgi

pushusersmsg.fcgi: pushusersmsg.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(MANAGERPROC_PATH)$(OTJERPROC_VERSION)./pushusersmsg.fcgi
	
### 编译

pushusermsg.o:
	$(COMPILE) ./pushusermsg/pushusermsg.cc
	
pushusersmsg.o:
	$(COMPILE) ./pushusersmsg/pushusersmsg.cc
	
	
#######################LBS####################################################
LBSPROC_PATH = /lbs
lbsproc:setuserpos.fcgi searchnearby.fcgi nearuser.fcgi collectandnearnum.fcgi\
	nearmusic.fcgi musicfri.fcgi samemusic.fcgi

setuserpos.fcgi: setuserpos.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(LBSPROC_PATH)$(OTJERPROC_VERSION)./setuserpos.fcgi
	
searchnearby.fcgi: searchnearby.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(LBSPROC_PATH)$(OTJERPROC_VERSION)./searchnearby.fcgi

nearuser.fcgi: nearuser.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(LBSPROC_PATH)$(OTJERPROC_VERSION)./nearuser.fcgi
	
collectandnearnum.fcgi: collectandnearnum.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(LBSPROC_PATH)$(OTJERPROC_VERSION)./collectandnearnum.fcgi
	
nearmusic.fcgi: nearmusic.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(LBSPROC_PATH)$(OTJERPROC_VERSION)./nearmusic.fcgi
	
musicfri.fcgi: musicfri.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(LBSPROC_PATH)$(OTJERPROC_VERSION)./musicfri.fcgi
	
samemusic.fcgi: samemusic.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(LBSPROC_PATH)$(OTJERPROC_VERSION)./samemusic.fcgi
	
	
### 编译

setuserpos.o:
	$(COMPILE) ./setuserpos/setuserpos.cc

searchnearby.o:
	$(COMPILE) ./searchnearby/searchnearby.cc

nearuser.o:
	$(COMPILE) ./nearuser/nearuser.cc

collectandnearnum.o:
	$(COMPILE) ./collectandnearnum/collectandnearnum.cc
	
nearmusic.o:
	$(COMPILE) ./nearmusic/nearmusic.cc
	
musicfri.o:
	$(COMPILE) ./musicfri/musicfri.cc

samemusic.o:
	$(COMPILE) ./samemusic/samemusic.cc






clean:
	-rm -f *.o
	-rm -f $(OUTPUT_FILE)./*.fcgi

delete-temp-file:
	-rm -f *.o




############public###############################
mig_log.o:
	$(COMPILE) $(LIB_DIR)/public/log/mig_log.cc

core_interface.o:
	$(COMPILE) $(LIB_DIR)/public/comm/core_interface.cc
