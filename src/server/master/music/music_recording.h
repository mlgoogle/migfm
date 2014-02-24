#ifndef _MASTER_PLUGIN_MUSIC_RECOEDING_H__
#define _MASTER_PLUGIN_MUSIC_RECOEDING_H__
#include "file/file_path.h"
#include "file/file_util.h"
#include "basic/basictypes.h"
#include "basic/basic_info.h"
#include <list>
namespace music_record{

class RecordingEngine{
public:
	RecordingEngine(std::string& dir,std::string& path);
	virtual ~RecordingEngine(){}
	bool WriteFile(const char* data,const int32 len);
	bool WriteFileXml(const char* data,const int32 len);
private:
	file::FilePath       dir_;
	file::FilePath       file_;
	std::string          path_;
};

class MoodRecordingEngine:public RecordingEngine{
public:
	MoodRecordingEngine(std::string& dir,std::string& path);
	virtual ~MoodRecordingEngine(){}
	bool RecordingMood(const int64 uid,const int32 type_mood);
};

class UserLocalMusicRecodingEngine:public RecordingEngine{
public:
	UserLocalMusicRecodingEngine(std::string& dir,std::string& path);
	virtual ~UserLocalMusicRecodingEngine(){}
	bool RecordingLocalMusic(const std::string& uid,
		                     const std::string& plat,
		                     std::list<base::RecordingLocalMusic> &record_list);
};

}

#endif