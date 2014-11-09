#ifndef _RESTFUL_HTTP_API_H__
#define _RESTFUL_HTTP_API_H__
#include "protocol/http_packet.h"

namespace base_net{

class MYHttpApi{
public:
	MYHttpApi(){};
	virtual ~MYHttpApi(){};

public:
	class UserManager{
		static bool OnUserRegister(packet::HttpPacket& packet,std::string& username,
				std::string& password,std::string& nickname,std::string& source,
				std::string& session,std::string& sex,std::string& birthday,std::string& location,
				std::string& address,std::string& head,int& err);

		static bool OnUserUpdate(packet::HttpPacket& packet,std::string& uid,
				         std::string& nickname,std::string& gender,
				         std::string& birthday,int& err);

		static bool OnUserLogin(packet::HttpPacket& packet,std::string& username,
				std::string& password,std::string& clientid,int& err);

		//static bool OnBindPush(packet::HttpPacket& packet,std::string& uid,std::string& channel,
			//	std::string& userid,int& err);
	};

	class ChatManager{
		static bool OnGetBestIdle(packet::HttpPacket& packet,std::string& platformid,int& err);

		static bool OnGetHisChat(packet::HttpPacket& packet,std::string& platformid,std::string& uid,
					std::string& tid,std::string& msgid,std::string& from,std::string& count,int& err);
	};

	class MusicManager{
		static bool OnGetChannel(packet::HttpPacket& packet,std::string& num,int& err);

		static bool OnGetChannelSong(packet::HttpPacket& packet,std::string& channel,int& err);

		static bool OnGetMoodAndScene(packet::HttpPacket& packet,std::string& mode,int& err);

		static bool OnGetMoodAndSceneMusic(packet::HttpPacket& packet,std::string& mode,std::string tid,int& err);

		static bool OnGetSongInfo(packet::HttpPacket& packet,std::string& songid,int& err);

		static bool OnGetLyric(packet::HttpPacket& packet,std::string& songid,int& err);

		static bool OnGetHandselSong(packet::HttpPacket& packet,std::string& uid,std::string& fromid,
				std::string& count,std::string& islike,int& err);

		static bool OnGetUpdateconfigFile(packet::HttpPacket& packet,std::string& version,std::string& uid,int& err);

		static bool OnGetTypeSongs(packet::HttpPacket& packet,std::string& uid,std::string& tid,
				std::string& typeindex,std::string& moodid,std::string& moodindex,std::string& sceneid,
				std::string& sceneindex,std::string& num,int& err);

		static bool OnGetCollectionSong(packet::HttpPacket& packet,std::string& uid,std::string& tid,int& err);

		static bool OnSubmitCurrentListenSongs(packet::HttpPacket& packet,std::string& uid,
				std::string& mode,std::string& type_id,std::string& cur_song,std::string& last_song,
				std::string& name,std::string& singer,std::string& state,int& err);

		static bool OnSubmitDelHateSongs(packet::HttpPacket& packet,std::string& uid,std::string& songid,int& err);

		static bool OnSubmitDelCollectionSongs(packet::HttpPacket& packet,std::string& uid,
				std::string& songid,int& err);

		static bool OnSubmitAddHateSongs(packet::HttpPacket& packet,std::string& uid,std::string& songid,
				std::string& modetype,std::string& type_id,int& err);

		static bool OnSubmitAddCollectionSongs(packet::HttpPacket& packet,std::string& uid,std::string& songid,
						std::string& modetype,std::string& type_id,int& err);

		static bool OnSubmitShareResult(packet::HttpPacket& packet,std::string& uid,std::string& songid,
								std::string& share_plat,int& err);
	};

	class LbsManager{
		static bool OnSetUserPos(packet::HttpPacket& packet,std::string& uid,std::string& location,int& err);

		static bool OnNearSet(packet::HttpPacket& packet,std::string& uid,std::string& location,
				std::string& page_index,std::string& page_size,int& err);
	};

	class Sociality{
		static bool OnPresentSong(packet::HttpPacket& packet,std::string& uid,std::string& tuid,
				std::string& ext_msg,int& err);

		static bool OnGetMsgPush(packet::HttpPacket& packet,std::string& uid,std::string& page_index,
				std::string& page_size,int& err);

		static bool OnGetFriendList(packet::HttpPacket& packet,std::string& uid,std::string& page_index,
				std::string& page_size,int& err);

		static bool OnSayHello(packet::HttpPacket& packet,std::string& uid,std::string& tuid,
				std::string& ext_msg,int& err);

		static bool OnMsgCommentSong(packet::HttpPacket& packet,std::string& uid,std::string& songid,
				std::string& comment,std::string& tid,int& err);

		static bool OnGetComment(packet::HttpPacket& packet,std::string& uid,std::string& songid,
				std::string& from,std::string& count,int& err);

		static bool OnGetMusicUser(packet::HttpPacket& packet,std::string& uid,std::string& from,
				std::string& count,int& err);

		static bool OnGetShareInfo(packet::HttpPacket& packet,std::string& uid,std::string& songid,
				std::string& plat,std::string& mode,std::string& index,std::string& latitude,
				std::string& longitude,int& err);

		static bool OnGiftLuckMessage(packet::HttpPacket& packet,std::string& uid,std::string& songid,
				std::string& plat,std::string& mode,int& err);
	};
};

}
#endif
