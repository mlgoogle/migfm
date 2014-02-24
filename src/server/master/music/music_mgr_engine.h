#ifndef _MASTER_PLUGIN_MUSIC_MGR_MUSIC_MGR_ENGINE_H__
#define _MASTER_PLUGIN_MUSIC_MGR_MUSIC_MGR_ENGINE_H__
#include "plugins.h"
#include "get_song.h"
#include "basic/http_packet.h"
#include "basic/basic_info.h"
#include "basic/radom_in.h"
#include "music_recording.h"
#include "thread_lock.h"

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

	bool UpdateConfigFile(const int socket,const packet::HttpPacket& packet);

	bool GetSongList(const int socket,const packet::HttpPacket& packet,
		             const int type);

	bool GetSongListV2(const int socket,const packet::HttpPacket& packet,
		               const int type);

	 //0 default 1 collect

	bool GetTypeSongs(const int socket,const packet::HttpPacket& packet);

    bool SetMoodRecording(const int socket,const packet::HttpPacket& packet);

	bool GetUserMusicCltAndHis(const int socket,const packet::HttpPacket& packet);

private:
	bool GetMusicInfos(const int socket,const std::string& songid);

	bool GetOneMusicInfo(const std::string& song_id,base::MusicInfo& mi);

	bool GetMoodScensChannelSongs(const std::string& uid,
		                          const std::string mode,
								  const int32 num,
								  const std::string wordid,
								  std::stringstream& result);

	void ChangeMusicInfos(std::map<std::string,base::MusicInfo>& music_infos,
		                                std::list<std::string>& songinfolist);

	//优化版本1，获取音乐数据时，一次性从数据库和redis里面获取
	bool GetMoodScensChannelSongsV2(const std::string& uid,
		const std::string mode,const int32 num,
		const std::string wordid,
		std::map<std::string,base::MusicCltHateInfo>& song_map,
		std::stringstream& result);

	//优化版本2，在版本1的基础上加入一次性获取黑名单歌曲，并通过自建随机数
	//自建产生随机歌曲，大大降低重复性，并一次读取redis 减少连接次数
	bool GetMoodScensChannelSongsV3(const std::string& uid,
		const std::string mode,const int32 num,
		const std::string wordid,
		std::map<std::string,base::MusicCltHateInfo>& clt_song_map,
		std::map<std::string,base::MusicCltHateInfo>& hate_song_map,
		std::stringstream& result);


	bool GetMusicHotCltCmt(const std::string& songid,std::string& hot_num,
		                   std::string& cmt_num,std::string& clt_num);

	bool SetMusicHostCltCmt(const std::string& songid,const int32 flag,
		                    const int32 value = 1);//1 热度 2 收藏数 3评论数 

	bool RestMusicListRandom();

	bool CreateTypeRamdon(std::string& type,std::list<int>& list);

	bool GetTypeRamdon(const std::string& type,const std::string& wordid,
		               int num,std::list<int>& list);

private:
	music_logic::GetSongUrl*                         get_song_engine_;
	music_record::MoodRecordingEngine*               mood_record_engine_;
	music_record::UserLocalMusicRecodingEngine*      user_local_music_engine_;
private:
	std::map<int,base::MigRadomInV2*>                       channel_random_map_;
	std::map<int,base::MigRadomInV2*>                       mood_random_map_;
	std::map<int,base::MigRadomInV2*>                       scene_random_map_;
	threadrw_t*                                       channel_random_lock_;
	threadrw_t*                                       mood_random_lock_;
	threadrw_t*                                       scene_random_lock_;

};

}
#endif