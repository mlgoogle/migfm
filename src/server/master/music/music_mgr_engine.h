#ifndef _MASTER_PLUGIN_MUSIC_MGR_MUSIC_MGR_ENGINE_H__
#define _MASTER_PLUGIN_MUSIC_MGR_MUSIC_MGR_ENGINE_H__
#include "plugins.h"
#include "get_song.h"
#include "basic/http_packet.h"
#include "basic/basic_info.h"
#include "music_recording.h"

namespace music_logic{

class MusicMgrEngine{
public:
	MusicMgrEngine();
	virtual ~MusicMgrEngine();

	static MusicMgrEngine *GetInstance();
	static void FreeInstance();

private:
	static MusicMgrEngine   *instance_;

public:
	bool Init();
	bool OnMusicMgrConnect(struct server* srv,int socket);

    bool OnMusicMgrMessage(struct server *srv, int socket, 
		                 const void *msg, int len);

	bool OnMusicMgrClose(struct server *srv, int socket);

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
	bool GetMusicChannel(const int socket,const packet::HttpPacket& packet);

	bool GetDoubanMusicChannelSong(const int socket,const packet::HttpPacket& packet);

	bool GetMusicChannelSong(const int socket,const packet::HttpPacket& packet);

	bool GetDescriptionWord(const int socket,const packet::HttpPacket& packet);

	bool GetMoodSceneWordSong(const int socket,const packet::HttpPacket& packet);

	bool GetWXMusicInfo(const int socket,const packet::HttpPacket& packet);

	bool GetMoodMap(const int socket,const packet::HttpPacket& packet);

	bool GetMoodParent(const int socket,const packet::HttpPacket& packet);

	bool PostCollectAndHateSong(const int socket,const packet::HttpPacket& packet,
		                        const int flag); //1 Collect 0 Hate

	bool DelCollectAndHateSong(const int socket,const packet::HttpPacket& packet,
		                       const int flag);//1 Collect 0 Hate

	bool PostUserLocalMusicinfos(const int socket,const packet::HttpPacket& packet);

	bool GetSongList(const int socket,const packet::HttpPacket& packet,const int type);

	 //0 default 1 collect

	bool GetTypeSongs(const int socket,const packet::HttpPacket& packet);

    bool SetMoodRecording(const int socket,const packet::HttpPacket& packet);
private:
	bool GetMusicInfos(const int socket,const std::string& songid);

	bool GetOneMusicInfo(const std::string& song_id,base::MusicInfo& mi);

	bool GetMoodScensSongs(const std::string& uid,const std::string mode,const int32 num,
		const std::string wordid,std::stringstream& result);
private:
	music_logic::GetSongUrl*                         get_song_engine_;
	music_record::MoodRecordingEngine*               mood_record_engine_;
	music_record::UserLocalMusicRecodingEngine*      user_local_music_engine_;

};

}
#endif