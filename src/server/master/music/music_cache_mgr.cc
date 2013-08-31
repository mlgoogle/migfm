#include "music_cache_mgr.h"
#include "http_response.h"
#include "db_comm.h"
#include "dic_comm.h"
#include "logic_comm.h"
#include "json/json.h"
#include <vector>
#include <sstream>

namespace music_logic{

MusicCacheManager* CacheManagerOp::music_cache_manager_ = NULL;
template <typename MapType, typename MapTypeIT, typename ValType>
static bool MapGet (MapType &map, int idx, ValType &val)
{
	MapTypeIT it = map.find (idx);
	if (it == map.end ())
		return false;
	val = it->second;
	return true;
}

template <typename MapType, typename ValType>
static bool MapAdd (MapType &map, int idx, const ValType &val)
{
	map[idx] = val;
	return true;
}

template <typename MapType, typename MapTypeIT>
static bool MapDel (MapType &map, int idx)
{
	MapTypeIT it = map.find (idx);
	if (it == map.end ())
		return false;
	map.erase (it);
	return true;
}

template <typename MapType>
static bool MapCount (MapType &map, int &count)
{
	count = (int) map.size ();
	return true;
}

template <typename MapType>
static bool MapClr (MapType &map)
{
	map.clear ();
	return true;
}

template <typename ListType, typename ListTypeIT, typename ValType>
static bool ListGet (ListType &list, int idx, ValType &val)
{
	ListTypeIT it = list.begin ();
	while (it != list.end ()) {
		if (idx == (*it))
			return true;
		++it;
	}
	return false;
}

template <typename ListType, typename ValType>
static bool ListAdd (ListType &list, const ValType &val)
{
	list.push_back (val);
	return true;
}

template <typename ListType, typename ListTypeIT>
static bool ListDel (ListType &list, int idx)
{
	ListTypeIT it = list.begin ();
	while (it != list.end ()) {
		if (idx == (*it).sidx) {
			it = list.erase (it);
			return true;
		} else
			it++;
	}
	return false;
}

template <typename ListType, typename ListTypeIT,typename ListKey>
bool ListDelType (ListType &tlist, ListKey idx)
{
	ListTypeIT it = tlist.begin ();
	while (it != tlist.end ()) {
		if (idx == (*it)) {
			it = tlist.erase (it);
			return true;
		} else
			it++;
	}
	return false;
}

template <typename ListType>
static bool ListCount (ListType &list, int &count)
{
	count = (int) list.size ();
	return true;
}

template <typename ListType>
static bool ListClr (ListType &list)
{
	list.clear ();
	return true;
}

template <typename VectorType,typename TypeElem, typename ValType>
bool VectorGet(VectorType &tvector, TypeElem& key,ValType &val){
	if(key!=-1){
		if(key>tvector.size())
			return false;
		val = tvector[key];
		if(val!=0)
			return true;
	}

	return false;
}

template <typename VectorType,typename Type>
void VectorInit(VectorType &tvector,int num){
	for(int i=0;i<num;i++){
		tvector.push_back(1);
	}
}

template <typename VectorType,typename Type>
void VectorAdd(VectorType& tvector,Type& key,Type& value){
	tvector[key] = value;	
}

template <typename VectorType,typename Type>
void VectorDel(VectorType& tvector,Type& key){
	tvector[key] = 1;	
}

template <typename VectorType,typename Type>
void VectorDel2(VectorType& tvector,Type& key,int num){
	int j =0;
	while (j<num){
		if (tvector[j]==key){
			tvector[j] = 1;
		}
		j++;
	}
}

template <typename VectorType,typename Type>
bool VectorCount(VectorType &tvector,Type &count){
	count = tvector.size();
	return true;	
}

template <typename VectorType>
bool VectorDestroy(VectorType &vec){
	VectorType vtTemp;
	vtTemp.swap(vec);
	return true;
}


MusicCacheManager::MusicCacheManager()
:channel_num_(0){
	douban_url_= "http://douban.fm/j/mine/playlist?type=n&sid=&pt=0.0&from=mainsite&channel=";

	InitThreadrw(&cache_mgr_lock_);
	InitMusicChannel();
	get_song_engine_ = music_logic::GetSongUrl::Create(music_logic::TYPE_SOGOU);
	if (get_song_engine_==NULL){
		MIG_ERROR(USER_LEVEL,"get_song create error");
		return ;
	}
	std::string get_song_url = "http://121.199.32.88/getmusicurl.ashx";
	get_song_engine_->Init(get_song_url);
}

MusicCacheManager::~MusicCacheManager(){
	std::map<int,ChannelCache*>::iterator it = channel_cache_map_.begin ();
	for (; it != channel_cache_map_.end(); it++)
		delete it->second;
	DeinitThreadrw (cache_mgr_lock_);
}


ChannelCache* MusicCacheManager::GetChannelCache(int32 channel){
	std::map<int,ChannelCache*>::iterator it = channel_cache_map_.find(channel);
	if (it==channel_cache_map_.end())
		return NULL;
	return it->second;
}


bool MusicCacheManager::PutJsonMusicChannel(int channel,
											std::string& content){
	std::list<base::MusicInfo> music_list;
	bool r = false;

	int32 music_infos_size;
	Json::Reader reader;
	Json::Value  root;
	Json::Value song;

	r = reader.parse(content.c_str(),root);
	if (!r){
		LOG_ERROR("parser json error");
		return false;
	}
	music_infos_size = root["song"].size();
	if (music_infos_size<=0){
	   LOG_ERROR2("song valiled size[%d]",music_infos_size_);
	   return false;
	}
	song = root["song"];
	for (int i = 0;i<music_infos_size;i++){
		std::string artist;
		std::string id;
		std::string ssid;
		std::string aid;
		std::string durl;
		std::string hq_url;
		std::string titile;
		std::string pub_time;
		std::string album;
		std::string pic_url;
		int music_time = 0;
		if (song[i].isMember("artist")){
			artist = song[i]["artist"].asString();
		}

		if (song[i].isMember("url")){
			durl = song[i]["url"].asString();
		}
		
		if(song[i].isMember("title")){
			titile = song[i]["title"].asString();
		}

		if (song[i].isMember("public_time")){
			pub_time = song[i]["public_time"].asString();
		}

		if (song[i].isMember("albumtitle")){
			album = song[i]["albumtitle"].asString();
		}
		
		if (song[i].isMember("length")){
			music_time = song[i]["length"].asInt();
		}

		if (song[i].isMember("sid")){
			id = song[i]["sid"].asString();
		}
		
		if (song[i].isMember("ssid")){
			ssid = song[i]["ssid"].asString();
		}
		
		if (song[i].isMember("aid")){
			aid = song[i]["aid"].asString();
		}
		
		if (song[i].isMember("picture")){
			pic_url = song[i]["picture"].asString();
		}
		
		if (song[i].isMember("hq_url")){
			hq_url = song[i]["picture"].asString();
		}else{
			hq_url = durl;
		}

		base::MusicInfo mi(id,ssid,aid,album,titile,hq_url,pub_time,artist,pic_url,durl,music_time);
		music_list.push_back(mi);
	}

	AddMusicChannelInfos(channel,music_list);

}

bool MusicCacheManager::AddMusicChannelInfos(int channel,
											 std::list<base::MusicInfo>& list){
	usr_logic::WLockGd lw(cache_mgr_lock_);
	ChannelCache* cc = GetChannelCache(channel);
	if(cc==NULL)
		return false;
	cc->channel_music_infos_ = list;
	cc->current_time = time(NULL)+(60*60*2);
	return true;
}

int MusicCacheManager::GetMusicCHannelNum(int channel){
	usr_logic::RLockGd lr(cache_mgr_lock_);
	ChannelCache* cc = GetChannelCache(channel);
	int num = -1;
	if (cc==NULL)
		return num;
	num = cc->channel_music_infos_.size();
	return num;
}


time_t MusicCacheManager::GetMusicTime(int channel){
	usr_logic::RLockGd lr(cache_mgr_lock_);
	ChannelCache* cc = GetChannelCache(channel);
	time_t music_time = -1;
	if (cc==NULL)
		return music_time;
	music_time = cc->current_time;
	return music_time;
}

void MusicCacheManager::IsTimeMusiChannelInfos(const std::string& channel){
	time_t music_time = GetMusicTime(atol(channel.c_str()));
	std::string content;
	bool r = false;
	if (music_time==-1)
		return;
	time_t current_time = time(NULL);
	if (music_time<current_time){
		r = RequestDoubanMusicInfos(channel,content);
		if (r){
			PutJsonMusicChannel(atol(channel.c_str()),content);
		}
	}
}

void MusicCacheManager::IsLessMuciChannelInfos(const std::string& channel, int num){
	int current_num = GetMusicCHannelNum(atol(channel.c_str()));
	std::string content;
	bool r = false;
	if (num==-1)
		return;
	if (current_num<num){
		r = RequestDoubanMusicInfos(channel,content);
		if (r){
		   LOG_DEBUG2("current[%d] num[%d] [%s]",current_num,num,content.c_str());
		   PutJsonMusicChannel(atol(channel.c_str()),content);
		}
	}
}


bool MusicCacheManager::GetMusicCahnelTypeInfos(int channel,const std::string& uid,const int nun,
												std::stringstream& os){
	usr_logic::RLockGd lr(cache_mgr_lock_);
	int32 i = 0;
	int32 max_num = 0;
	bool r = false;
	ChannelCache* cc = GetChannelCache(channel);
	if (cc==NULL)
		return false;
	max_num = nun;
	while(i<max_num){
		std::list<base::MusicInfo>::iterator it =cc->channel_music_infos_.begin();

		if (it!=cc->channel_music_infos_.end()){
			std::string songid;
			base::MusicInfo mi = (*it);
			int32 is_like = 0;

			//id转化
			storage::DBComm::GetSongidFromDoubanId(mi.id(),songid);
			mi.set_id(songid);
			//是否拉黑
			r = storage::RedisComm::IsHateSong(uid,mi.id());
			if (r){
				cc->channel_music_infos_.pop_front();
				continue;
			}
			//是否收藏
			r = storage::RedisComm::IsCollectSong(uid,mi.id());
			if (r)
				is_like = 1;
			else 
				is_like = 0;

			if (r)//拉黑
				continue;
			os<<"{\"id\":\""<<mi.id().c_str()
				<<"\",\"title\":\""<<mi.title().c_str()
				<<"\",\"artist\":\""<<mi.artist().c_str()
				<<"\",\"pub_time\":\""<<mi.pub_time().c_str()
				<<"\",\"album\":\""<<mi.album_title().c_str()
				<<"\",\"hq_url\":\""<<mi.hq_url().c_str()
				<<"\",\"url\":\""<<mi.url().c_str()
				<<"\",\"pic\":\""<<mi.pic_url().c_str()<<"\",\"time\":\""
				<<mi.music_time()<<"\",\""
				<<is_like<<"\":\"0\"}";
			if (i!=0){
				os<<",";
			}
			cc->channel_music_infos_.pop_front();
		}
		i++;
	}
}

bool MusicCacheManager::GetMusicChannelInfos(int channel, 
											 std::string &json_content,
											 const int flag,const int cur_num){
    usr_logic::RLockGd lr(cache_mgr_lock_);
	std::stringstream os;
	int32 i = 0;
	int32 max_num = 0;
	bool r = false;
	ChannelCache* cc = GetChannelCache(channel);
	if (cc==NULL)
		return false;
	os<<"\"channel\":[";
	if (flag==0)
		max_num = 1;
	else
		max_num = cur_num;
	while(i<max_num){
		std::list<base::MusicInfo>::iterator it =cc->channel_music_infos_.begin();

		if (it!=cc->channel_music_infos_.end()){
			std::string content_url;
			base::MusicInfo mi = (*it);
			if (flag==0){
			    r = get_song_engine_->GetSongInfo(mi.artist(),mi.title(),
				   mi.album_title(),content_url,0);
			//豆瓣不支持html5 故从爬虫获取
			}else{
				content_url = mi.url();
			}
			os<<"{\"id\":\""<<mi.id().c_str()
				<<"\",\"title\":\""<<mi.title().c_str()
				<<"\",\"artist\":\""<<mi.artist().c_str()
				<<"\",\"pub_time\":\""<<mi.pub_time().c_str()
				<<"\",\"album\":\""<<mi.album_title().c_str()
				<<"\",\"hq_url\":\""<<mi.hq_url().c_str()
				<<"\",\"url\":\""<<content_url.c_str()
				<<"\",\"pic\":\""<<mi.pic_url().c_str()<<"\",\"time\":\""
				<<mi.music_time()<<"\",\"like\":\"0\"}";
			if (i==0){
				os<<",";
			}
			cc->channel_music_infos_.pop_front();
		}
		i++;
	}
	os<<"]";
	json_content = os.str();
	return true;
}


bool MusicCacheManager::InitMusicChannel(){
	bool r = false;
	r = storage::DBComm::GetChannelInfo(channel_mode_,channel_num_);
	if (!r){
	    LOG_ERROR("get channel error");
		return r;
	}
	LOG_DEBUG("get douban music data....");
	for (int i = 0;i<channel_num_;i++){
		base::ChannelInfo ci = channel_mode_[i];
		int32 channel = atol(ci.douban_index().c_str());
		music_logic::ChannelCache* cc = GetChannelCache(atol(ci.douban_index().c_str()));
		if (cc == NULL){
			cc = new music_logic::ChannelCache;
			channel_cache_map_[channel] = cc;
		}
		std::string content;
		//r = RequestDoubanMusicInfos(ci.douban_index(),content);
		PutJsonMusicChannel(channel,content);
		cc->current_time = time(NULL);
	}
	LOG_DEBUG("get douban music data success....");
}

bool MusicCacheManager::RequestDoubanMusicInfos(const std::string& channel,
												std::string& content){
	bool r =false;
	std::string douban_url;
	douban_url = douban_url_;

	douban_url+=channel;

	music_logic::HttpResponse http(douban_url);
	r = http.Get();
	if (!r){
		LOG_ERROR2("http request error");
		return false;
	}
	r = http.GetContent(content);
	return r;

}

bool MusicCacheManager::GetMusicChannel(std::string& num,std::string& content){
	usr_logic::RLockGd lr(cache_mgr_lock_);
	int32 pos = (atol(num.c_str()))%(channel_mode_.size());
	std::stringstream os;
	
	LOG_DEBUG2("id[%s] name[%s] pic[%s]",channel_mode_[pos].douban_index().c_str(),
		      channel_mode_[pos].channel_name().c_str(),
			  channel_mode_[pos].channel_pic().c_str());
	 os<<"\"channle\":[{\"id\":\""<<channel_mode_[pos].douban_index().c_str()
		 <<"\",\"name\":\""<<channel_mode_[pos].channel_name().c_str()
		 <<"\",\"pic\":\""<<channel_mode_[pos].channel_pic().c_str()
		 <<"\"}";
	 for (int i = pos+1;i<(pos+9);i++){
		 os<<",";
		 if (i<channel_num_)
			 os<<"{\"id\":\""<<channel_mode_[i].douban_index().c_str()
			 <<"\",\"name\":\""<<channel_mode_[i].channel_name().c_str()
			 <<"\",\"pic\":\""<<channel_mode_[i].channel_pic().c_str()
			 <<"\"}";
		 else{
			 int current = i%channel_num_;
			 LOG_DEBUG2("current[%d] i[%d]",current,i);
			 os<<"{\"id\":\""<<channel_mode_[current].douban_index().c_str()
				 <<"\",\"name\":\""<<channel_mode_[current].channel_name().c_str()
				 <<"\",\"pic\":\""<<channel_mode_[current].channel_pic().c_str()
				 <<"\"}";
		 }
	 }
	 os<<"]";
	 content = os.str();
	 return true;
}


}