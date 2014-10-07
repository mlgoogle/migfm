
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

## 头文件
INCLUDE_PATHS = -I./../public/ -I /usr/local/libmemcached-1.0.7/include\
		-I/usr/local/lib -I./../../../third_party/hiredis\
	        -I../../../third_party/breakpad/src -I/usr/include/mysql 

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
OUTPUT_FILE = /var/www/cgi-bin/
#############################################################################################################################
all: clean proc delete-temp-file

proc: robotproc chatmgrproc usrmgrproc spreadproc socialityproc musicproc lbsproc managerproc


#####################robot################################################
robotproc:getnewmusic.fcgi addnewmusic.fcgi getmailinfo.fcgi getspreadmail.fcgi getrobots.fcgi updaterobotpic.fcgi\
	 getvailedurlmusic.fcgi updatevailedurlmusic.fcgi getvailedlyric.fcgi updatelyric.fcgi getwbtoken.fcgi

getspreadmail.fcgi: getspreadmail.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./getspreadmail.fcgi
	
getmailinfo.fcgi: getmailinfo.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./getmailinfo.fcgi
	
getnewmusic.fcgi: getnewmusic.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./getnewmusic.fcgi

addnewmusic.fcgi: addnewmusic.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./addnewmusic.fcgi
	
getrobots.fcgi: getrobots.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./getrobots.fcgi

updaterobotpic.fcgi: updaterobotpic.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./updaterobotpic.fcgi
	
getvailedurlmusic.fcgi: getvailedurlmusic.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./getvailedurlmusic.fcgi
	
getvailedlyric.fcgi: getvailedlyric.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./getvailedlyric.fcgi
	
updatevailedurlmusic.fcgi: updatevailedurlmusic.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./updatevailedurlmusic.fcgi

updatelyric.fcgi: updatelyric.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./updatelyric.fcgi
	
getwbtoken.fcgi: getwbtoken.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./getwbtoken.fcgi

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
chatmgrproc:getsc.fcgi hischat.fcgi

getsc.fcgi: getsc.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./getsc.fcgi
	
hischat.fcgi: hischat.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./hischat.fcgi
	
getsc1.fcgi: getsc1.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./getsc1.fcgi
	
### 编译

getsc.o:
	$(COMPILE) ./getsc/getsc.cc
	
hischat.o:
	$(COMPILE) ./hischat/hischat.cc
	
getsc1.o:
	$(COMPILE) ./getsc1/getsc1.cc
	
	
######################usrmgrproc###############################################
usrmgrproc:regedit.fcgi updateusr.fcgi guest.fcgi login.fcgi

regedit.fcgi: regedit.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./regedit.fcgi
	
updateusr.fcgi: updateusr.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./updateusr.fcgi
	
get.fcgi: get.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./get.fcgi
	
guest.fcgi: guest.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./guest.fcgi
	
login.fcgi: login.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./login.fcgi
	
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
	
######################spread###############################################
spreadproc:weixin.fcgi

weixin.fcgi: weixin.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./weixin.fcgi
	
### 编译

weixin.o:
	$(COMPILE) ./weixin/weixin.cc
	
######################sociality###############################################
socialityproc:setuserconfigofpush.fcgi presentsong.fcgi getpushmsg.fcgi\
	getpushmsgsummary.fcgi sayhello.fcgi commentsong.fcgi get_comment.fcgi\
	getmusicuser.fcgi getshareinfo.fcgi

setuserconfigofpush.fcgi: setuserconfigofpush.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./setuserconfigofpush.fcgi

presentsong.fcgi: presentsong.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./presentsong.fcgi

getpushmsg.fcgi: getpushmsg.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./getpushmsg.fcgi	
	
getpushmsgsummary.fcgi: getpushmsgsummary.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./getpushmsgsummary.fcgi	
	
getfriendlist.fcgi: getfriendlist.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./getfriendlist.fcgi
	
sendfriendmsg.fcgi: sendfriendmsg.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./sendfriendmsg.fcgi

sayhello.fcgi: sayhello.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./sayhello.fcgi

adduserbacklist.fcgi: adduserbacklist.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./adduserbacklist.fcgi
	
addfriend.fcgi: addfriend.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./addfriend.fcgi

importsonglist.fcgi: importsonglist.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./importsonglist.fcgi
	
importfriend.fcgi: importfriend.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./importfriend.fcgi
	
commentsong.fcgi: commentsong.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./commentsong.fcgi
	
get_comment.fcgi: get_comment.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./get_comment.fcgi
	
getmusicuser.fcgi: getmusicuser.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./getmusicuser.fcgi

getshareinfo.fcgi: getshareinfo.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./getshareinfo.fcgi
	
	
### 编译

setuserconfigofpush.o:
	$(COMPILE) ./setuserconfigofpush/setuserconfigofpush.cc

presentsong.o:
	$(COMPILE) ./presentsong/presentsong.cc
	
getpushmsg.o:
	$(COMPILE) ./getpushmsg/getpushmsg.cc

getpushmsgsummary.o:
	$(COMPILE) ./getpushmsgsummary/getpushmsgsummary.cc

getfriendlist.o:
	$(COMPILE) ./getfriendlist/getfriendlist.cc
	
sendfriendmsg.o:
	$(COMPILE) ./sendfriendmsg/sendfriendmsg.cc
	
sayhello.o:
	$(COMPILE) ./sayhello/sayhello.cc
	
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
	
######################music###################################################
musicproc: collectsong.fcgi getcltsongs.fcgi delcltsong.fcgi hatsong.fcgi\
	delthsong.fcgi recordcursong.fcgi getdefsongs.fcgi gettypesongs.fcgi\
	recordlocalsongs.fcgi updateconfigfile.fcgi getuserhis.fcgi songid.fcgi\
	getword.fcgi word_song.fcgi getlyric.fcgi

collectsong.fcgi: collectsong.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./collectsong.fcgi

getcltsongs.fcgi: getcltsongs.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./getcltsongs.fcgi

delcltsong.fcgi: delcltsong.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./delcltsong.fcgi
	
hatsong.fcgi: sethtsong.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./hatsong.fcgi
	
delthsong.fcgi: delthsong.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./delthsong.fcgi
	
recordcursong.fcgi: recordcursong.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./recordcursong.fcgi
	
getdefsongs.fcgi: getdefsongs.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./getdefsongs.fcgi
	
gettypesongs.fcgi: gettypesongs.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./gettypesongs.fcgi
	
recordlocalsongs.fcgi: recordlocalsongs.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./recordlocalsongs.fcgi
	
updateconfigfile.fcgi: updateconfigfile.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./updateconfigfile.fcgi
	
getuserhis.fcgi: getuserhis.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./getuserhis.fcgi
	
songid.fcgi: songid.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./songid.fcgi
	
getword.fcgi: getword.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./getword.fcgi
	
word_song.fcgi: word_song.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./word_song.fcgi
	
getlyric.fcgi: getlyric.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./getlyric.fcgi
	
### 编译

getcltsongs.o:
	$(COMPILE) ./getcltsongs/getcltsongs.cc

collectsong.o:
	$(COMPILE) ./collectsong/collectsong.cc
	
delcltsong.o:
	$(COMPILE) ./delcltsong/delcltsong.cc
	
sethtsong.o:
	$(COMPILE) ./sethtsong/sethtsong.cc
	
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
managerproc: pushusermsg.fcgi pushusersmsg.fcgi

pushusermsg.fcgi: pushusermsg.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./pushusermsg.fcgi

pushusersmsg.fcgi: pushusersmsg.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./pushusersmsg.fcgi
	
### 编译

pushusermsg.o:
	$(COMPILE) ./pushusermsg/pushusermsg.cc
	
pushusersmsg.o:
	$(COMPILE) ./pushusersmsg/pushusersmsg.cc
	
	
#######################LBS####################################################
lbsproc:setuserpos.fcgi searchnearby.fcgi nearuser.fcgi collectandnearnum.fcgi\
	nearmusic.fcgi musicfri.fcgi samemusic.fcgi

setuserpos.fcgi: setuserpos.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./setuserpos.fcgi
	
searchnearby.fcgi: searchnearby.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./searchnearby.fcgi

nearuser.fcgi: nearuser.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./nearuser.fcgi
	
collectandnearnum.fcgi: collectandnearnum.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./collectandnearnum.fcgi
	
nearmusic.fcgi: nearmusic.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./nearmusic.fcgi
	
musicfri.fcgi: musicfri.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./musicfri.fcgi
	
samemusic.fcgi: samemusic.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)./samemusic.fcgi
	
	
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
	$(COMPILE) ./../public/log/mig_log.cc

core_interface.o:
	$(COMPILE) ./../public/comm/core_interface.cc
