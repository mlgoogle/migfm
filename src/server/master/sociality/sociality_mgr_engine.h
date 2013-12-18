#ifndef _MASTER_PLUGIN_MUSIC_MGR_MUSIC_MGR_ENGINE_H__
#define _MASTER_PLUGIN_MUSIC_MGR_MUSIC_MGR_ENGINE_H__
#include "plugins.h"
#include "basic/http_packet.h"
#include "basic/basic_info.h"
#include "json/json.h"

namespace mig_sociality {

class SocialityMgrEngine{
public:
	SocialityMgrEngine();
	virtual ~SocialityMgrEngine();

	static SocialityMgrEngine *GetInstance();
	static void FreeInstance();

private:
	static SocialityMgrEngine   *instance_;

public:
	bool OnConnect(struct server* srv,int socket);

    bool OnReadMessage(struct server *srv, int socket,
		                 const void *msg, int len);

	bool OnClose(struct server *srv, int socket);

	bool OnBroadcastConnect(struct server *srv, 
		                    int socket, void *data, 
							int len);

	bool OnBroadcastMessage (struct server *srv, 
		                     int socket, void *msg, 
							 int len);

	bool OnBroadcastClose (struct server *srv, int socket);


	bool OnIniTimer (const struct server* srv);

	bool OnTimeout (struct server *srv, char* id, int opcode, int time);

private:
	bool OnMsgSetUserConfigOfPush(packet::HttpPacket& packet, 
								  Json::Value &result,int &status, 
								  int &err_code);

	bool OnMsgPresentSong(packet::HttpPacket& packet, Json::Value &result,
						  int &status, int &err_code);

	bool OnMsgGetPushMsg(packet::HttpPacket& packet, Json::Value &result,
		 	             int &status, int &err_code);

	bool OnMsgGetPushMsgSummary(packet::HttpPacket& packet, 
		                        Json::Value &result,int &status, 
								int &err_code);

	bool OnMsgGetFriendList(packet::HttpPacket& packet, Json::Value &result,
			int &status, int &err_code);

	bool OnMsgSendFriendMsg(packet::HttpPacket& packet, Json::Value &result,
			int &status, int &err_code);

	bool OnMsgSayHello(packet::HttpPacket& packet, Json::Value &result,
			int &status, int &err_code);

	bool OnMsgAddUserBacklist(packet::HttpPacket& packet, Json::Value &result,
			int &status, int &err_code);

	bool OnMsgAddFriend(packet::HttpPacket& packet, Json::Value &result,
			int &status, int &err_code);

	bool OnMsgImportSongList(packet::HttpPacket& packet, Json::Value &result,
			int &status, int &err_code);

	bool OnMsgImportFriend(packet::HttpPacket& packet, Json::Value &result,
			int &status, int &err_code);

	bool OnMsgCommentSong(packet::HttpPacket& packet, Json::Value &result,
			int &status, int &err_code);

	bool OnMsgGetComment(packet::HttpPacket& packet, Json::Value &result,
			int &status, int &err_code);

	bool OnMsgGetMusicFriend(packet::HttpPacket& packet, Json::Value &result,
		int &status, int &err_code);

private:
	bool CheckAndTransHMTime(const std::string &str, unsigned &time);

	bool MakePresentSongContent(const std::string& send_uid,
		const std::string& to_uid,const std::string& song_id,
			int64 msg_id, const std::string &msg,
			std::string &detail, std::string &summary);

	bool MakePresentSongContent(const std::string& send_uid,
		const std::string& to_uid,const std::string& song_id,
		int64 msg_id, const std::string &msg,
		std::string &detail);

	bool MakeHalloContent(const std::string& send_uid,const std::string& to_uid,
						int64 msg_id,const std::string& msg,std::string& detail,
						std::string &summary);

	bool GetPushMsgDetail(const std::string& uid,
		const std::string &msg, Json::Value &content);
	
	bool GetPresentSongDetail(const std::string& uid,Json::Value &content);

	bool GetMusicInfos(const std::string& uid,const std::string& songid, Json::Value &info);

	bool SetMusicHostCltCmt(const std::string& songid,const int32 flag,
		                    std::string& json);//1 热度 2 收藏数 3评论数 

	bool GetMusicHotCltCmt(const std::string &songid, std::string &hot_num, 
		std::string &cmt_num, std::string &clt_num);

	bool GetMusicInfo(Json::Value &value,const std::string& uid,
		std::map<std::string,std::string> &temp_usersong, 
		std::map<std::string,std::string> &collect_map, 
		std::map<std::string,base::MusicInfo> &user_song);

	bool GetUserInfo(Json::Value &value,struct MusicFriendInfo& userinfo);

	bool PushPresentMsg(std::string& msg,std::string& summary,
		                std::string& uid,std::string& to_uid,
						int& err_code,int& status);

	bool GetUserCurrentMusic(Json::Value &value,const std::string& tar_uid);
};

}
#endif
