#ifndef MIG_WX_GET_SONG_H__
#define MIG_WX_GET_SONG_H__
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "basic/basictypes.h"

namespace wxinfo{

enum{
	TYPE_BAIDU = 0,
};


class WXGetSongUrl{
public:
	static WXGetSongUrl* Create(int32 type);
	~WXGetSongUrl(void) {}
	virtual void Init(std::string& song_url) = 0;
	virtual bool GetSongInfo(const std::string& artist,const std::string& title,
					const std::string album,std::string& song_url) = 0;
};



class WXBaiduGetSongImpl:public wxinfo::WXGetSongUrl{
public:
	WXBaiduGetSongImpl(){};
	~WXBaiduGetSongImpl();

	void Init(std::string& song_url);

	bool GetSongInfo(const std::string& artist,const std::string& title,
					const std::string album,std::string& song_url);
private:
	bool HttpGetSongInfo(const std::string& key,std::string& song_url);
private:
	std::string     requst_song_url_;
};

}
#endif