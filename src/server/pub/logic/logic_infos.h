/*

 * logic_infos.h
 *
 *  Created on: 2014年12月2日
 *      Author: kerry
 */
#ifndef _BUDDHA_LOGIC_INFOS_H__
#define _BUDDHA_LOGIC_INFOS_H__
#include "logic/base_values.h"
#include "basic/scoped_ptr.h"
#include "basic/basictypes.h"
#include <string>
#include <map>

namespace base_logic{
//音乐信息 //音乐模块 社交模块 使用
class MusicInfo{
public:
	explicit MusicInfo();
	MusicInfo(const MusicInfo& music_info);
	MusicInfo& operator = (const MusicInfo& music_info);

	MusicInfo& BaseCopy(const MusicInfo& music_info);

	base_logic::DictionaryValue* Release(bool all =true);


	const int64 id() const {return data_->id_;}
	const int64 clt() const {return data_->clt_;}
	const int64 cmt() const {return data_->cmt_;}
	const int64 hot() const {return data_->hot_;}
	const int32 like() const {return data_->like_;}
	const int32 tclass() const {return data_->class_;}
	const std::string& class_name() const {return data_->class_name_;}
	const std::string& ablum() const {return data_->ablum_;}
	const std::string& artist() const {return data_->artist_;}
	const std::string& title() const {return data_->title_;}
	const std::string& hq_url() const {return data_->hq_url_;}
	const std::string& pic() const {return data_->pic_;}
	const std::string& pubtime() const {return data_->pubtime_;}
	const std::string& url() const {return data_->url_;}
	const std::string& lyric() const {return data_->lyric_;}


	void set_id(const int64 id){data_->id_ = id;}
	void set_clt(const int64 clt){data_->clt_ = clt;}
	void set_cmt(const int64 cmt){data_->cmt_ = cmt;}
	void set_hot(const int64 hot){data_->hot_ = hot;}
	void set_like(const int32 like){data_->like_ = like;}
	void set_class(const int32 tclass){data_->class_ = tclass;}
	void set_class_name(const std::string& class_name){data_->class_name_ = class_name;}
	void set_ablum(const std::string& ablum);//{data_->ablum_ = ablum;}
	void set_artist(const std::string& artist);//{data_->artist_ = artist;}
	void set_title(const std::string& title);//{data_->title_ = title;}
	void set_hq_url(const std::string& hq_url){data_->hq_url_ = hq_url;}
	void set_pic(const std::string& pic){data_->pic_ = pic;}
	void set_pubtime(const std::string& pubtime){data_->pubtime_ = pubtime;}
	void set_url(const std::string& url){data_->url_ = url;}
	void set_lyric(const std::string& lyric){data_->lyric_ = lyric;}


	//解析JSON
	void JsonSeralize(std::string& str);
	//组装json
	void JsonDeserialize(std::string& str,int32 dimenon = 0);

private:
	class Data{
	public:
		 Data()
			:refcount_(1)
			,id_(0)
			,clt_(-1)
			,cmt_(-1)
			,hot_(-1)
			,like_(-1)
			,class_(-1){}
		void AddRef(){refcount_ ++;}
		void Release(){if (!--refcount_)delete this;}

	public:
		int64                  id_;
		int64                  clt_;
		int64                  cmt_;
		int64                  hot_;
		int32                  like_;
		int32                  class_;
		std::string            class_name_;
		std::string            ablum_;
		std::string            artist_;
		std::string            title_;
		std::string            hq_url_;
		std::string            pic_;
		std::string            pubtime_;
		std::string            url_;
		std::string            lyric_;

	private:
		int refcount_;

	};
	Data*       data_;
};



class LBSInfos{
public:
	explicit LBSInfos();
	explicit LBSInfos(const std::string& host,const double& latitude,
			const double& longitude,const std::string& city,const std::string& district,
			const std::string& province,std::string& street);

	LBSInfos(const LBSInfos& lbs_basic_info);
	LBSInfos& operator = (const LBSInfos& lbs_basic_info);

	const std::string& host() const {return data_->host_;}
	const double latitude() const {return data_->latitude_;}
	const double longitude() const {return data_->longitude_;}
	const std::string& city() const {return data_->city_;}
	const std::string& district() const {return data_->district_;}
	const std::string& province() const {return data_->province_;}
	const std::string& street() const {return data_->street_;}


private:
	class Data{
	public:
		Data():refcount_(0)
		,latitude_(0.0)
		,longitude_(0.0){}

		Data(const std::string& host,const double latitude,
				const double longitude,const std::string& city,const std::string& district,
				const std::string& province,std::string& street)
		:refcount_(0)
		,latitude_(latitude)
		,longitude_(longitude)
		,city_(city)
		,district_(district)
		,province_(province)
		,street_(street){}
		void AddRef(){refcount_ ++;}
		void Release(){if (!--refcount_)delete this;}
	public:
		const std::string host_;
		const double latitude_;
		const double longitude_;
		const std::string city_;
		const std::string district_;
		const std::string province_;
		const std::string street_;
	private:
		int refcount_;
	};
	Data*       data_;


};

//维度属性
class Dimension{
public:
	explicit Dimension();
	explicit Dimension(const int64 id,const std::string& name,const std::string& description);

	Dimension(const Dimension& dimension_info);
	Dimension& operator = (const Dimension& dimension_info);

	const int64 id() const {return data_->id_;}
	const std::string class_name() const {return data_->class_name_;}
	const std::string name() const {return data_->name_;}
	const std::string description() const {return data_->description_;}

	void set_id(const int64 id){data_->id_ = id;}
	void set_name(const std::string& name){data_->name_ = name;}
	void set_class_name(const std::string& class_name){data_->class_name_ = class_name;}
	void set_description(const std::string& description){data_->description_ = description;}
	class Data{
	public:
		Data()
		:refcount_(1)
		,id_(0){}

		Data(const int64 id,const std::string& name,const std::string& description)
		 :refcount_(1)
		 ,id_(id)
		 ,name_(name)
		 ,description_(description){}

	public:
		void AddRef(){refcount_ ++;}
		void Release(){if (!--refcount_)delete this;}
	public:
		int64         id_;
		std::string   class_name_;
		std::string   name_;
		std::string   description_;
	private:
		int refcount_;
	};
	Data*     data_;
};

class Dimensions{
public:
	explicit Dimensions();
	explicit Dimensions(const int64 id,const std::string& name,const std::string& alias);
	Dimensions(const Dimensions& dimensions_info);
	Dimensions& operator = (const Dimensions& dimensions_info);

	void set_dimension(const int64 id,base_logic::Dimension& dimension){
		data_->dimensions_map_[id] = dimension;
	}

	const int64 id() const {return data_->id_;}
	const std::string& name() const {return data_->name_;}
	const std::string& alias() const {return data_->alias_;}

	void dimension_name(const int64 id,std::string& name);

public:
	class Data{
	public:
		Data()
		:refcount_(1)
		,id_(0){}

		Data(const int64 id,const std::string& name,const std::string& alias)
		 :refcount_(1)
		 ,id_(id)
		 ,name_(name)
		 ,alias_(alias){}
	public:
		void AddRef(){refcount_ ++;}
		void Release(){if (!--refcount_)delete this;}
	public:
		const int64  id_;
		const std::string name_;
		const std::string alias_;
	    std::map<int64,base_logic::Dimension>   dimensions_map_;
	private:
		int refcount_;
	};
	Data*     data_;
};
}

#endif




