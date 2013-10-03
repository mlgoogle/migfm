#ifndef MIG_WX_ENGINE_H__
#define MIG_WX_ENGINE_H__
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include "basic/constants.h"
#include "log/mig_log.h"
#include "basic/scoped_ptr.h"
#include "storage/db_serialization.h"
#include "storage/dic_serialization.h"
#include "xmpp/xml_serialization.h"
#include "xmpp/xmppstanzaparser.h"
#include "json/json.h"
#include "wx_packet.h"
#include "wx_get_song.h"

namespace wxinfo{


class WXInfoEngine{
public:
    WXInfoEngine();
    ~WXInfoEngine();
    bool InitEngine(std::string& path);
    bool GetMusicInfosFromStroage();

	void PostContent(const std::string& content);
	inline std::string& GetProcessContent(){return content_;}
    inline void ClearProcessContent(){content_ = "";}
private:
	bool InitChannelAndMode(void);
	bool StorageOneWords(std::string &reponse_msg);
	void StorageOneWord(std::string &attr, std::string &value);
	class StanzaParseHandler:public base::XmppStanzaParseHandler{
	public:
		StanzaParseHandler(WXInfoEngine* outer):outer_(outer){}
		virtual void StartStream(const base::XmlElement * pelStream)
		{outer_->IncomingStart(pelStream);}
		virtual void Stanza(const base::XmlElement * pelStream)
		{outer_->IncomingStanza(pelStream);}
		virtual void EndStream()
		{outer_->IncomingEnd(false);}
		virtual void XmlError()
		{outer_->IncomingEnd(true);}
	private:
		WXInfoEngine* const outer_;
	};
friend class StanzaParseHandler;
private:
	StanzaParseHandler stanzaParseHandler_;
	base::XmppStanzaParser stanzaParser_;
private:
	void IncomingStanza(const base::XmlElement* pelStanza);
	void IncomingStart(const base::XmlElement* pelStanza);
	void IncomingEnd(bool isError);
	void ProcessMsg(WXPacket& msg);
	void ProcessMsgText(WXPacket& msg);
	void ProcessEventText(WXPacket& msg);
	void PackageTextMsg(std::string& to_user,std::string& from_user,
		                std::string& content);

	void HttpNewArticle(std::string& to_user,std::string& from_user);
	void PackageMusicMsg(const std::string& to_user,
						const std::string& from_user,
						const std::string& title,
						const std::string& description,
						const std::string& url, const std::string& hq_url);

	bool GetUserMode(const std::string& from_user,std::string& mode);

	bool SetUserMode(std::string& from_user,std::string to_user,
		             std::string& mode);

	bool SegmentWordMsg(std::string& to_user,std::string& from_user,
						std::string& content_s);


	bool GetOneMusicInfo(const std::string& song_id,
		                 base::MusicInfo& mi,
						 std::string& content);

	bool GetMusicInfo(base::MusicInfo& mi,std::string& content,std::string& key);

	bool GetOneMusicInfo(base::MusicInfo& mi,std::string& flag,std::string& key,
						std::string& content);

	bool GetMoodAndScenesMusicInfos(base::MusicInfo& mi,std::string& content,
									std::string& word_flag);

	bool GetMusicInfos(base::MusicInfo& mi,std::string& content);

	bool GetMusicWordInfo(std::list<std::string>& word_list,
		                  std::string& word_name);

	bool PutDesignationMusicInfo(base::MusicInfo& mi,std::list<std::string> mt_list,
		                         std::string& artist,std::string& content);

	bool PutRadomSong(base::MusicInfo& mi,std::list<std::string> mt_list,
		              std::string& content,std::string& flag);

	void PullAnyMusicMsg(std::string& to_user,std::string& from_user);

	void ChangeChannel(std::string& to_user,std::string& from_user);

	void SettingChannel(std::string& to_user,std::string& from_user,
		                int new_channel);

	void RecommendationMusic(std::string& to_user,std::string& from_user,
		                     std::string& content);

	void PullRobotTextMsg(std::string& to_user,std::string& from_user,
							std::string& content_s);

	bool PullMoodAndSenceMusicInfo(std::string& to_user,
		                           std::string& from_user,
		                           std::string& content_s,
								   std::string& word_flag);


	void StorageWordsDump();

	bool GetMemMusicInfo(std::string& from_user,std::string& durl,
		                 std::string& title,std::string& decs,
						 time_t& current,int& current_channel);

	bool GetMemChanncel(std::string& from_user,int& current_channel);

	bool ParseJson(int32 num,std::string& content,std::string& from_user,
		           std::string& durl,
				   std::string& title,
		           std::string& decs);

	bool SetMemMusicInfo(std::string& from_user,int32 num,int32 channel,
		                 std::string& json,time_t json_time);

	bool HttpGetDoubanMusicInfo(std::string& content,int32 channel);

	void HttpMigPicText(const std::string& title,const std::string& decs,
		                const std::string& pic_url,const std::string&url,
						std::string& msg,const int flag = 1);

	void MigFMShow(std::string& msg,const int flag);

	void NewArticle(const int flag,std::string& msg);
    
    void HttpShowWebFM(std::string& to_user,std::string& from_user);
    
    void GetMenuChannelMusic(std::string& from_user,
                             std::string& to_user,
                             const std::string& envent_key);
    
    bool GetMemMusicInfos(std::string& from_user,
                          const std::string& key,
                          const int current_channel,
                          std::string& durl,
                          std::string& hq_url,
                          std::string& title,
                          std::string& decs,
                          time_t& json_time);
    
    bool SetMemMusicInfos(const std::string& key,
                          const int32 num, std::string& json,
                          time_t json_time);
    
    bool GetMoodSceneWordSongMenu(std::string& from_user,std::string& to_user,
                              std::string& content);
    
    bool GetHistroyNews(std::string& from_user,std::string& to_user);
    
    void GetWxToken();
    
    void CreateMenu();

// 	void UserReadMe(std::string& msg);
// 
// 	void NetUserMusic(std::string& msg);

public: 
    static WXInfoEngine* GetEngine();
    static void  FreeEngine();
private:
    static wxinfo::WXInfoEngine*                  engine_;
	std::string                                   content_;
	std::string                                   url_;
	std::string                                   douban_url_;
	std::string                                   segment_url_;
	std::string                                   get_song_url_;
	Json::Value                                   song_;
	int32                                         count_;
	int32                                         music_infos_size_;
	wxinfo::WXGetSongUrl*                         wx_get_song_;
	std::map<std::string,std::list<std::string> > word_map_;
private:
	std::vector<std::string>                      channel_;
	std::map<std::string,std::string>             mode_map_;
	std::vector<base::ChannelInfo>                channel_mode_;
	int                                           channel_num_;
	std::string                                   channel_desc_;
    std::string                                   wx_token_;
    int                                           expires_in_token_time_;

};
}
#endif
