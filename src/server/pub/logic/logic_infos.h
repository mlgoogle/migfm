/*

 * logic_infos.h
 *
 *  Created on: 2014年12月2日
 *      Author: kerry
 */
#ifndef _BUDDHA_LOGIC_INFOS_H__
#define _BUDDHA_LOGIC_INFOS_H__
#include "basic/basictypes.h"
#include <string>
#include <map>

namespace base_logic{

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
	const std::string name() const {return data_->name_;}
	const std::string description() const {return data_->description_;}

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
		const int64         id_;
		const std::string   name_;
		const std::string   description_;
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




