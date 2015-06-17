/*
 * data_cache_manager.cc
 *
 *  Created on: 2015年5月19日
 *      Author: pro
 */
#include "data_cache_manager.h"
#include "storage_operation.h"
#include "logic/logic_unit.h"
#include "logic/logic_comm.h"
#include "basic/template.h"
#include <list>

#define DEFAULT_CONFIG_PATH     "./plugins/usersvc/usersvc_config.xml"

base_logic::DataEngine *GetDateEngine(void){
	return new base_logic::DateEngineImpl();
}


namespace base_logic{

bool GetInfoMapTraits::BatchGetUsers(std::vector<int64>& uid_list,USER_INFO_MAP& usermap,struct threadrw_t* lock,
		Container& container){
	//memcache 批量获取
	base_logic::StorageOperation::Instance()->BatchGetUserInfo(uid_list,container);
	return true;

}

bool GetInfoListTraits::BatchGetUsers(std::vector<int64>& uid_list,USER_INFO_MAP& usermap,struct threadrw_t* lock,
		Container& container){
	return true;
}

//模板
template<typename GetInfoContainerTraits>
bool BatchGetUserInfos(std::vector<int64>& uid_vector,USER_INFO_MAP& usermap,struct threadrw_t* lock,
		typename GetInfoContainerTraits::Container& container){
	typedef GetInfoContainerTraits traits;
	return traits::BatchGetUsers(uid_vector,usermap,lock,container);
}

/////////////////////////////////////////////////////////////////////////////

bool DateEngineImpl::SetUserInfo(const int64 uid,base_logic::UserInfo& info){
	return DataCacheEngine::GetWholeManager()->SetUserInfo(uid,info);
}

bool DateEngineImpl::GetUserInfo(const int64 uid,base_logic::UserInfo& info){
	return DataCacheEngine::GetWholeManager()->GetUserInfo(uid,info);
}

bool DateEngineImpl::BatchGetUserInfo(int64* batch_uid,int32* uid_count,base_logic::UserInfo* users_info,
				int32* userinfo_count){
	return DataCacheEngine::GetWholeManager()->BatchGetUserInfo(batch_uid,uid_count,users_info,userinfo_count);
}

bool DateEngineImpl::DelUserInfo(const int64 uid){
	return DataCacheEngine::GetWholeManager()->DelUserInfo(uid);
}


DataWholeManager* DataCacheEngine::whole_mgr_ = NULL;
DataCacheEngine* DataCacheEngine::data_cache_engine_ = NULL;


DataWholeManager::DataWholeManager(){
	data_cache_ = new DataCache();
	Init();
}

DataWholeManager::~DataWholeManager(){

}


void DataWholeManager::Init(std::list<base::ConnAddr>& addrlist){

}

void DataWholeManager::Init(){
	InitThreadrw(&lock_);
	//
	bool r = false;
	std::string path = DEFAULT_CONFIG_PATH;
	config::FileConfig* config = config::FileConfig::GetFileConfig();
	if(config==NULL){
		return ;
	}
	r = config->LoadConfig(path);

	base_logic::StorageOperation::Instance()->Init(config);
	//读取音乐信息
	base_logic::StorageOperation::Instance()->GetAvailableMusicInfo(data_cache_->musicinfo_map_);
	CreateDimensions(10000,"ms",MOOD_NAME);
	CreateDimensions(20000,"mm",SCENE_NAME);
	CreateDimensions(30000,"chl",CHANNEL_NAME);
	CreateRadomin();//创建随机数
	InitDimensionsMusic();//初始化各个音乐维度

}


bool DataWholeManager::SetUserInfo(const int64 uid,base_logic::UserInfo& info){
	base_logic::WLockGd lk(lock_);
	//base_logic::UserMemComm::SetUserInfo(uid,info);
	return base::MapAdd<USER_INFO_MAP,int64,base_logic::UserInfo>(data_cache_->userinfo_map_,uid,info);
}

bool DataWholeManager::GetUserInfo(const int64 uid,base_logic::UserInfo& info){

	bool r = false;
	{
		base_logic::RLockGd lk(lock_);
		r = base::MapGet<USER_INFO_MAP,USER_INFO_MAP::iterator,int64,base_logic::UserInfo>
		(data_cache_->userinfo_map_,uid,info);
	}
	if(r)
		return r;

	base_logic::StorageOperation::Instance()->GetUserInfo(uid,info);

	{
		base_logic::WLockGd lk(lock_);
		base::MapAdd<USER_INFO_MAP,int64,base_logic::UserInfo>(data_cache_->userinfo_map_,uid,info);
	}
	return true;
}

bool DataWholeManager::BatchGetUserInfo(int64* batch_uid,int32* uid_count,base_logic::UserInfo* users_info,
				int32* userinfo_count){
	/*std::vector<int64> uid_vector;
	int64 uid1 = 10149;
	int64 uid2 = 10150;
	int64 uid3 = 10151;
	int64 uid4 = 10154;
	int64 uid5 = 10310;
	int64 uid6 = 10308;
	int64 uid7 = 10302;
	uid_vector.push_back(uid1);
	uid_vector.push_back(uid2);
	uid_vector.push_back(uid3);
	uid_vector.push_back(uid4);
	uid_vector.push_back(uid5);
	uid_vector.push_back(uid6);
	uid_vector.push_back(uid7);
	std::map<int64,base_logic::UserInfo> map_user_info;
	bool r =BatchGetUserInfos<GetInfoMapTraits>(uid_vector,data_cache_->userinfo_map_,lock_,map_user_info);*/
	std::vector<int64> uid_vector;
	int32 i = 0;
	while(i<(*uid_count)){
		uid_vector.push_back(batch_uid[i]);
		i++;
	}
	std::map<int64,base_logic::UserInfo> map_user_info;
	bool r =BatchGetUserInfos<GetInfoMapTraits>(uid_vector,data_cache_->userinfo_map_,lock_,map_user_info);
	(*userinfo_count) = map_user_info.size();
	std::map<int64,base_logic::UserInfo>::iterator it = map_user_info.begin();
	for(i=0;it!=map_user_info.end();++it,i++){
		if(it->second.Isvalid())
			users_info[i] = it->second;
	}
	return r;
}

bool DataWholeManager::DelUserInfo(const int64 uid){
	base_logic::WLockGd lk(lock_);
	return base::MapDel<USER_INFO_MAP,USER_INFO_MAP::iterator,int64>(data_cache_->userinfo_map_,uid);
}


//////以下是音乐相关//////////////////////////////////////////////////////

void DataWholeManager::CreateRadomin(){
	CreateRadomin(data_cache_->channel_dimension_);
	CreateRadomin(data_cache_->mood_dimension_);
	CreateRadomin(data_cache_->scene_dimension_);
}

void DataWholeManager::CreateRadomin(DimensionCache& dimension_cache){

	//遍历MAP
	MULTI_DIMENSION_MAP multl_dimension_map = dimension_cache.dimension_map_;
	for(MULTI_DIMENSION_MAP::iterator it = multl_dimension_map.begin();
			it!=multl_dimension_map.end();it++){
		DIMENSION_MAP dimenson_map = it->second;
		base::MigRadomInV2* radomV2 = new base::MigRadomInV2((dimenson_map.size()));
		dimension_cache.dimension_radomin_[it->first] = radomV2;
	}
}
void DataWholeManager::CreateDimensions(const int64 id,
		const std::string& type,const std::string& name){
	base_logic::Dimensions dimensions(id,name,type);
	base_logic::StorageOperation::Instance()->GetDimensions(type,dimensions);
	data_cache_->dimensions_[type] = dimensions;
}


void DataWholeManager::InitDimensionsMusic(){
	std::list<base_logic::Dimension> list;
	base_logic::StorageOperation::Instance()->GetAllDimension(list);
	//读取每个维度的歌曲
	while(list.size()>0){
		base_logic::Dimension dimension = list.front();
		list.pop_front();
		InitDimensionMusic(dimension);
	}
}


void DataWholeManager::InitDimensionMusic(base_logic::Dimension& dimension){
	DIMENSION_MAP dimension_map;
	DIMENSION_VEC dimension_vec;
	bool r = base_logic::StorageOperation::Instance()->GetDimensionMusic(dimension.class_name(),dimension.id(),
			dimension_map,dimension_vec);
	if(!r)
		return;
	//根据维度添加缓存
	if(dimension.class_name()=="chl"){
		data_cache_->channel_dimension_.dimension_map_[dimension.id()] = dimension_map;
		data_cache_->channel_dimension_.dimension_vec_[dimension.id()] = dimension_vec;
	}
	else if(dimension.class_name()=="mm"){
		data_cache_->mood_dimension_.dimension_map_[dimension.id()] = dimension_map;
		data_cache_->mood_dimension_.dimension_vec_[dimension.id()] = dimension_vec;
	}
	else if(dimension.class_name()=="ms"){
		data_cache_->scene_dimension_.dimension_map_[dimension.id()] = dimension_map;
		data_cache_->scene_dimension_.dimension_vec_[dimension.id()] = dimension_vec;
	}
}


void DataWholeManager::GetMusicInfo(MUSICINFO_MAP& list){
	base_logic::RLockGd lk(lock_);
	bool r = false;
	MUSICINFO_MAP::iterator it = list.begin();
	for(;it!=list.end();it++){
		base_logic::MusicInfo tmusicinfo = it->second;
		base_logic::MusicInfo rmusicinfo;
		//读取信息
		r = base::MapGet<MUSICINFO_MAP,MUSICINFO_MAP::iterator,int64,base_logic::MusicInfo>
		(data_cache_->musicinfo_map_,tmusicinfo.id(),rmusicinfo);
		if(r)
			tmusicinfo.BaseCopy(rmusicinfo);
	}
}

void DataWholeManager::GetCollectList(const int64 uid,MUSICINFO_MAP& music_list){
	GetMusicListT(uid,data_cache_->collect_map_,music_list,base_logic::StorageOperation::GetCollectListS);
	GetMusicInfo(music_list);
}

void DataWholeManager::SetCollectSong(const int64 uid,base_logic::MusicInfo& music){
	//SetMusicPreference(uid,music,data_cache_->collect_map_,musicsvc_logic::MusicDicComm::SetCollect);
}

void DataWholeManager::DelCollectSong(const int64 uid,const int64 songid){
	//musicsvc_logic::MusicDicComm::DelCollect(uid,songid);
	bool r = false;
	MUSICINFO_MAP music_list;
	//删除缓存
	{
		base_logic::WLockGd lk(lock_);
			//写入个人红心歌单
		r = base::MapGet<MUSICINFONLIST_MAP,MUSICINFONLIST_MAP::iterator,
						int64,MUSICINFO_MAP >(data_cache_->collect_map_,uid,music_list);
		if(!r)
			return;
		r = base::MapDel<MUSICINFO_MAP,MUSICINFO_MAP::iterator,int64>(music_list,songid);
		if(!r)
			return;
		r = base::MapAdd<MUSICINFONLIST_MAP,int64,MUSICINFO_MAP>
		(data_cache_->collect_map_,uid,music_list);
	}
}

void DataWholeManager::SetHatList(const int64 uid,base_logic::MusicInfo& music){
	//SetMusicPreference(uid,music,data_cache_->hate_map_,musicsvc_logic::MusicDicComm::SetHate);
}

void DataWholeManager::CheckIsCollectSong(const int64 uid,std::list<base_logic::UserAndMusic>& infolist){
	MUSICINFO_MAP music_list;
	GetCollectList(uid,music_list);

	//遍历
	for(std::list<base_logic::UserAndMusic>:: iterator it = infolist.begin();
			it!=infolist.end();++it){
		base_logic::UserAndMusic info = (*it);
		base_logic::MusicInfo musicinfo;
		if(base::MapGet<MUSICINFO_MAP,MUSICINFO_MAP::iterator,
				int64,base_logic::MusicInfo >(music_list,info.musicinfo_.id(),musicinfo))
			info.musicinfo_.set_like(1);
	}
}

void DataWholeManager::GetDimensionList(const std::string& name,const int64 id,MUSICINFO_MAP& music_list,const int64 num){
	//
	bool r = false;

	MULTI_DIMENSION_VEC  multi_dimension_vec;
	RADOMIN_MAP   dimension_radomin;
	std::list<int32>  radom_list;
	{
		base_logic::RLockGd lk(lock_);
		if(name=="chl"){
			multi_dimension_vec = data_cache_->channel_dimension_.dimension_vec_;
			dimension_radomin = data_cache_->channel_dimension_.dimension_radomin_;
		}
		else if(name=="ms"){
			multi_dimension_vec = data_cache_->scene_dimension_.dimension_vec_;
			dimension_radomin = data_cache_->scene_dimension_.dimension_radomin_;
		}
		else if(name=="mm"){
			multi_dimension_vec = data_cache_->mood_dimension_.dimension_vec_;
			dimension_radomin = data_cache_->mood_dimension_.dimension_radomin_;
		}
	}
	//获取随机数
	GetRadomin(dimension_radomin,id,num,radom_list);

	{

		base_logic::RLockGd lk(lock_);
		DIMENSION_VEC dimension_vec;
		r = base::MapGet<MULTI_DIMENSION_VEC,MULTI_DIMENSION_VEC::iterator,int64,DIMENSION_VEC>
		(multi_dimension_vec,id,dimension_vec);

		int i = 0;
		while(radom_list.size()>0){
			int32 radom = radom_list.front();
			radom_list.pop_front();
			base_logic::MusicInfo info;
			if(radom<=dimension_vec.size()){
				info = dimension_vec[radom];
				music_list[info.id()] = info;
			}
		}

	}
	GetMusicInfo(music_list);
}

void DataWholeManager::GetDimensionInfo(netcomm_send::DimensionInfo* net_dimension){
	GetDimensionInfos("ms",net_dimension);
	GetDimensionInfos("mm",net_dimension);
	GetDimensionInfos("chl",net_dimension);
}

void DataWholeManager::GetDimensionInfos(const std::string& type,
		netcomm_send::DimensionInfo* net_dimension){
	base_logic::Dimensions dimensions = data_cache_->dimensions_[type];
	std::map<int64,base_logic::Dimension> dimension;
	dimensions.swap(dimension);
	GetDimensionInfoUnit(type,dimension,net_dimension);
}

void DataWholeManager::GetDimensionInfoUnit(const std::string& type,
		std::map<int64,base_logic::Dimension>& map,
		netcomm_send::DimensionInfo* net_dimension){

	std::map<int64,base_logic::Dimension>::iterator it =map.begin();
	for(;it!=map.end();it++){
		base_logic::Dimension info = it->second;
		if(type=="chl")
			net_dimension->set_channel(info.Release());
		else if(type=="ms")
			net_dimension->set_scens(info.Release());
		else if(type=="mm")
			net_dimension->set_mood(info.Release());
	}
}

void DataWholeManager::GetMusicListT(const int64 uid,MUSICINFONLIST_MAP& container,
		MUSICINFO_MAP& music_list,void (*redis_get)(const int64,std::list<std::string>&)){
	bool r = false;
	std::list<std::string> list;
	{
		base_logic::RLockGd lk(lock_);
		//加锁
		r = base::MapGet<MUSICINFONLIST_MAP,MUSICINFONLIST_MAP::iterator,
				int64,MUSICINFO_MAP >
		(container,uid,music_list);
	}

	if(!r||music_list.size()==0)//读取
		redis_get(uid,list);
	if(list.size()<=0)//
			return ;
		//写入存储
	while(list.size()>0){//
		std::string str = list.front();
		list.pop_front();
		base_logic::MusicInfo info;
		info.JsonSeralize(str);
		music_list[info.id()] = info;
	}
	{
		base_logic::WLockGd lk(lock_);
		//加锁
		r = base::MapAdd<MUSICINFONLIST_MAP,int64,MUSICINFO_MAP >
		(container,uid,music_list);
	}
}

void DataWholeManager::GetRadomin(RADOMIN_MAP& dimension_radomin,const int32 id,
		int32 num,std::list<int32>& list){
	//
	base::MigRadomInV2* radomin;
	int i = 0;
	int32* rands = new int32[num];
	bool r = base::MapGet<RADOMIN_MAP,RADOMIN_MAP::iterator,
				int32,base::MigRadomInV2* >(dimension_radomin,id,radomin);
	if(!r)
		return;
	radomin->GetPrize(rands,num);
	while(i<=num){
		list.push_back(rands[i]);
		i++;
	}

	if (rands){
		delete[] rands;
		rands = NULL;
	}
}

void DataWholeManager::SetMusicPreference(const int64 uid,base_logic::MusicInfo& music,MUSICINFONLIST_MAP& map,
		void (*redis_set)(const int64,const int64, const std::string&)){
	bool r = false;
	std::string str_json;
	MUSICINFO_MAP music_list;
	//写入redis
	music.JsonDeserialize(str_json,1);
	redis_set(uid,music.id(),str_json);
	{
		base_logic::WLockGd lk(lock_);
		//读取完整信息存入缓存
		r = base::MapGet<MUSICINFO_MAP,MUSICINFO_MAP::iterator,
				int64,base_logic::MusicInfo >(data_cache_->musicinfo_map_,music.id(),music);

		//写入个人红心歌单
		r = base::MapGet<MUSICINFONLIST_MAP,MUSICINFONLIST_MAP::iterator,
					int64,MUSICINFO_MAP >(map,uid,music_list);
		if(!r)
			return;
		r =base::MapAdd<MUSICINFO_MAP,int64,base_logic::MusicInfo>
		(music_list,music.id(),music);
		if(!r)
			return;
		r = base::MapAdd<MUSICINFONLIST_MAP,int64,MUSICINFO_MAP>
		(map,uid,music_list);
	}
}



}


