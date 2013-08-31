#include "music_recording.h"
#include "logic_comm.h"
#include "file/file_path.h"
#include "file/file_util.h"
#include "file/string_serialization.h"
#include <sstream>

namespace music_record{

RecordingEngine::RecordingEngine(std::string& dir,std::string& path)
:path_(path){
	file::FilePath temp_dir(dir);
	dir_ = temp_dir;
}

bool RecordingEngine::WriteFileXml(const char* data,const int32 len){
	time_t current_time = time(NULL);
	std::stringstream os1;
	struct tm* local_time = localtime(&current_time);
	os1<<(local_time->tm_year+1900)<<(local_time->tm_mon+1)<<(local_time->tm_mday);
	std::string current_dir = dir_.value()+os1.str();
	LOG_DEBUG2("path[%s]",current_dir.c_str());
	file::FilePath current_dir_path(current_dir);
	if (!file::DirectoryExists(current_dir_path)){
		file::CreateDirectory(current_dir_path);
		//重新创建文件
		//获取前一天的时间
		time_t last_time = current_time - 60*60*24;
		struct tm* last_local_time = localtime(&last_time);
		std::stringstream os2;
		os2<<(last_local_time->tm_year+1900)
			<<(last_local_time->tm_mon+1)
			<<(last_local_time->tm_mday);
		std::string last_current_dir = dir_.value()+os2.str();
		last_current_dir.append("/");
		last_current_dir.append(path_.c_str());
		std::string tempstr = "</location>";
		file::FilePath temp_last_file_path(last_current_dir);
		file::WriteFile(temp_last_file_path,tempstr.c_str(),tempstr.length());
		/////


		std::string temp_path = current_dir_path.value()+"/"+path_;
		file::FilePath temp_file_path(temp_path);
		LOG_DEBUG2("file[%s]",temp_path.c_str());
		file_ = temp_file_path;
		tempstr = "<location>";
		file::WriteFile(file_,tempstr.c_str(),tempstr.length());
	}

	int32 bytes = file::WriteFile(file_,data,len);
	if (bytes==len)
		return true;
	else
		return false;
}

bool RecordingEngine::WriteFile(const char *data, const int32 len){

	time_t current_time = time(NULL);
	std::stringstream os1;
	struct tm* local_time = localtime(&current_time);
	os1<<(local_time->tm_year+1900)<<(local_time->tm_mon+1)<<(local_time->tm_mday);
	std::string current_dir = dir_.value()+os1.str();
	LOG_DEBUG2("path[%s]",current_dir.c_str());
	file::FilePath current_dir_path(current_dir);
    if (!file::DirectoryExists(current_dir_path)){
		file::CreateDirectory(current_dir_path);
	    //重新创建文件
		std::string temp_path = current_dir_path.value()+"/"+path_;
		file::FilePath temp_file_path(temp_path);
		LOG_DEBUG2("file[%s]",temp_path.c_str());
		file_ = temp_file_path;
	}

	int32 bytes = file::WriteFile(file_,data,len);
	if (bytes==len)
		return true;
	else
		return false;
}

///////////////////////////////////////////user local music////////////////////////////

UserLocalMusicRecodingEngine::UserLocalMusicRecodingEngine(std::string &dir, std::string &path)
:RecordingEngine(dir,path){

}


bool UserLocalMusicRecodingEngine::RecordingLocalMusic(const std::string& uid,
													   const std::string& plat,
								std::list<base::RecordingLocalMusic> &record_list){
	std::stringstream os;
	bool r = false;
	os<<"<info type=\"set\" from=\""<<uid.c_str()
		<<" source=\""<<plat.c_str()
		<<"\">";
	while (record_list.size()>0){
		base::RecordingLocalMusic rlm = record_list.front();
		record_list.pop_front();
		os<<"<music name=\""<<rlm.name().c_str()<<"\" "
			<<"singer=\""<<rlm.singer().c_str()<<"\""
			<<"/>";
	}
	os<<"</info>";
	WriteFileXml(os.str().c_str(),os.str().length());
	return true;
}
///////////////////////////////////////////moodrecording/////////////////////////////
MoodRecordingEngine::MoodRecordingEngine(std::string &dir, std::string &path)
:RecordingEngine(dir,path){

}

bool MoodRecordingEngine::RecordingMood(const int64 uid,const int32 type_mood){
	bool r = false;
	int32 stream_len = sizeof(int32)+sizeof(int64)+
		               sizeof(int32)+sizeof(time_t);
	time_t current_time = time(NULL);
	char* stream = new char[stream_len];
	r = serial::MemSerialition(stream,stream_len,
		                       PACK_INT64,uid,
							   PACK_INT32,type_mood,
							   PACK_INT64,current_time,
							   NULL);
	if (r){
		WriteFile(stream,stream_len);
	}

	if (stream){
		delete [] stream;
		stream = NULL;
	}
	return true;
}


}