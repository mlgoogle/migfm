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
	bool OnMsgSetUserConfigOfPush(packet::HttpPacket& packet, Json::Value &result,
			int &status, int &err_code);
	bool OnMsgPresentSong(packet::HttpPacket& packet, Json::Value &result,
			int &status, int &err_code);
	bool OnMsgGetPushMsg(packet::HttpPacket& packet, Json::Value &result,
			int &status, int &err_code);
	bool OnMsgGetPushMsgSummary(packet::HttpPacket& packet, Json::Value &result,
			int &status, int &err_code);
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

private:
	bool CheckAndTransHMTime(const std::string &str, unsigned &time);

	bool MakePresentSongContent(const std::string& send_uid,
			const std::string& to_uid,int64 song_id,
			int64 msg_id, const std::string &msg,
			std::string &detail, std::string &summary);

	bool GetPushMsgDetail(const std::string &msg, Json::Value &content);
	bool GetPresentSongDetail(Json::Value &content);

	bool GetMusicInfos(const std::string& songid, Json::Value &info);
};

}
#endif
