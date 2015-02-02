/*
 * logic_infos.cc
 *
 *  Created on: 2014年12月2日
 *      Author: kerry
 */
#include "logic_infos.h"
#include "logic_unit.h"
#include "basic/base64.h"

namespace base_logic{

//音乐
MusicInfo::MusicInfo(){
	data_ = new Data();
}

MusicInfo::MusicInfo(const MusicInfo& music_info)
:data_(music_info.data_){
	if(data_!=NULL){
		data_->AddRef();
	}
}

MusicInfo& MusicInfo::operator =(const MusicInfo& music_info){
	if(music_info.data_!=NULL){
		music_info.data_->AddRef();
	}

	if(data_!=NULL){
		data_->Release();
	}
	data_ = music_info.data_;
	return (*this);
}

MusicInfo& MusicInfo::BaseCopy(const MusicInfo& music_info){
	//type(class_name)和typeid(class)具有特殊性，故不拷贝
	this->set_id(music_info.id());
	this->set_clt(music_info.clt());
	this->set_hot(music_info.hot());
	this->set_like(music_info.like());
	data_->ablum_ = music_info.ablum();
	data_->artist_ = music_info.artist();
	data_->title_ = music_info.title();
	this->set_hq_url(music_info.hq_url());
	this->set_pic(music_info.pic());
	this->set_url(music_info.url());
	this->set_lyric(music_info.lyric());
	return (*this);
}
void MusicInfo::set_ablum(const std::string& ablum){
#if defined(__BASE64_MUSIC__)
	std::string b64ablum;
	Base64Decode(ablum,&b64ablum);
	data_->ablum_ = b64ablum;
#else
	data_->ablum_ = ablum;
#endif
}

void MusicInfo::set_artist(const std::string& artist){
#if defined(__BASE64_MUSIC__)
	std::string b64artist;
	Base64Decode(artist,&b64artist);
	data_->artist_ = b64artist;
#else
	data_->artist_ = artist;
#endif
}

void MusicInfo::set_title(const std::string& title){
#if defined(__BASE64_MUSIC__)
	std::string b64title;
	Base64Decode(title,&b64title);
	data_->title_ = b64title;
#else
	data_->title_ = title;
#endif
}


void MusicInfo::JsonSeralize(std::string& str){
	bool r  = false;
	int error_code;
	std::string error_str;
	scoped_ptr<base_logic::ValueSerializer> serializer(base_logic::ValueSerializer::Create(base_logic::IMPL_JSON,&str));
	base_logic::DictionaryValue* dic =  (base_logic::DictionaryValue*)(serializer->Deserialize(&error_code,&error_str));
	std::string str_class;
	std::string str_class_name;
	std::string str_songid;
	r = dic->GetString(L"songid",&str_songid);
	r = dic->GetString(L"type",&str_class_name);
	r = dic->GetString(L"typeid",&str_class);
	this->set_id(atoll(str_songid.c_str()));
	this->set_class_name(str_class_name);
	this->set_class(atoll(str_class.c_str()));
}

base_logic::DictionaryValue* MusicInfo::Release(){
	scoped_ptr<base_logic::DictionaryValue> dict(new base_logic::DictionaryValue());
	if(data_->id_!=0)
		dict->SetBigInteger(L"id",data_->id_);
	if(data_->clt_!=-1)
		dict->SetBigInteger(L"clt",data_->clt_);
	if(data_->cmt_!=-1)
		dict->SetBigInteger(L"cmt",data_->cmt_);
	if(data_->hot_!=-1)
		dict->SetBigInteger(L"hot",data_->hot_);
	if(data_->like_!=-1)
		dict->SetInteger(L"like",data_->like_);
	if(data_->class_!=-1)
		dict->SetInteger(L"tid",data_->class_);
	if(!data_->class_name_.empty())
		dict->SetString(L"type",data_->class_name_);
	if(!data_->ablum_.empty())
		dict->SetString(L"ablum",data_->ablum_);
	if(!data_->artist_.empty())
		dict->SetString(L"artist",data_->artist_);
	if(!data_->title_.empty()){
		dict->SetString(L"title",data_->title_);
#if defined(__OLD_VERSION__)
		dict->SetString(L"titile",data_->title_);
#endif
	}
	if(!data_->hq_url_.empty())
		dict->SetString(L"hq_url",data_->hq_url_);
	if(!data_->pic_.empty())
		dict->SetString(L"pic",data_->pic_);
	if(!data_->pubtime_.empty())
		dict->SetString(L"pubtime",data_->pubtime_);
	if(!data_->url_.empty())
		dict->SetString(L"url",data_->url_);

	//返回
	return dict.release();
}



LBSInfos::LBSInfos(){
	data_ = new Data();
}

LBSInfos::LBSInfos(const std::string& host,const double& latitude,
			const double& longitude,const std::string& city,const std::string& district,
			const std::string& province,std::string& street){
	data_ = new Data(host,latitude,longitude,city,district,province,street);
}

LBSInfos::LBSInfos(const LBSInfos& lbs_basic_info)
:data_(lbs_basic_info.data_){
	if(data_!=NULL){
		data_->AddRef();
	}
}

LBSInfos& LBSInfos::operator =(const LBSInfos& lbs_basic_info){
	if(lbs_basic_info.data_!=NULL){
		lbs_basic_info.data_->AddRef();
	}

	if(data_!=NULL){
		data_->Release();
	}
	data_ = lbs_basic_info.data_;
	return (*this);
}


Dimension::Dimension(){
	data_ = new Data();
}

Dimension::Dimension(const int64 id,const std::string& name,const std::string& description){
	data_ = new Data(id,name,description);
}

Dimension::Dimension(const Dimension& dimension_info)
:data_(dimension_info.data_){
	if(data_!=NULL){
		data_->AddRef();
	}
}

Dimension& Dimension::operator =(const Dimension& dimension_info){
	if(dimension_info.data_!=NULL){
		dimension_info.data_->AddRef();
	}

	if(data_!=NULL){
		data_->Release();
	}
	data_ = dimension_info.data_;
	return (*this);
}

Dimensions::Dimensions(){
	data_ = new Data();
}

Dimensions::Dimensions(const int64 id,const std::string& name,const std::string& alias){
	data_ = new Data(id,name,alias);
}

Dimensions::Dimensions(const Dimensions& dimensions_info)
:data_(dimensions_info.data_){
	if(data_!=NULL){
		data_->AddRef();
	}
}

Dimensions& Dimensions::operator =(const Dimensions& dimensions_info){
	if(dimensions_info.data_!=NULL){
		dimensions_info.data_->AddRef();
	}

	if(data_!=NULL){
		data_->Release();
	}
	data_ = dimensions_info.data_;
	return (*this);
}

void Dimensions::dimension_name(const int64 id,std::string& name){
	std::string empty;
	std::map<int64,base_logic::Dimension>::iterator it = data_->dimensions_map_.find(id);
	if(it!=data_->dimensions_map_.end())
		name =  it->second.name();
	else
		name = UNKONW_DIMENSIONS;
}

}



