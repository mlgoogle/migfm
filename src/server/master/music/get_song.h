#ifndef _MASTER_PLUGIN_MUSIC_MGR_GET_SONG_H__
#define _MASTER_PLUGIN_MUSIC_MGR_GET_SONG_H__
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "basic/basictypes.h"

namespace music_logic{

enum{
	TYPE_SOGOU = 0,
};


class GetSongUrl{
public:
	static GetSongUrl* Create(int32 type);
	~GetSongUrl(void) {}
	virtual void Init(std::string& song_url) = 0;
	virtual bool GetSongInfo(const std::string& artist,const std::string& title,
					const std::string album,std::string& song_url,int flag = 1) = 0; //1 ÐèÒªbase64±àÂë
};



class SoGouGetSongImpl:public music_logic::GetSongUrl{
public:
	SoGouGetSongImpl(){};
	~SoGouGetSongImpl();

	void Init(std::string& song_url);

	bool GetSongInfo(const std::string& artist,const std::string& title,
					 const std::string album,std::string& song_url,int flag = 1);
private:
	bool HttpGetSongInfo(const std::string& key,std::string& song_url);
private:
	std::string     requst_song_url_;
};

}
#endif