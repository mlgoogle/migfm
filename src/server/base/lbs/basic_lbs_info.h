#ifndef _MIG_FM_BASIC_LBS_INFO_H__
#define _MIG_FM_BASIC_LBS_INFO_H__
#include <string>
namespace base_lbs{

class BaiDuAccessKey{
public:
	explicit BaiDuAccessKey();
	explicit BaiDuAccessKey(const std::string& access_key);
	BaiDuAccessKey(const BaiDuAccessKey& access_key_info);
	BaiDuAccessKey& operator = (const BaiDuAccessKey& access_key_info);
	void add_count(){__sync_fetch_and_add(&data_->count_,1);} //因为多线程调用故使用原子计数的方式
	const int count() const {return data_->count_;}
	const std::string& access_token() const {return data_->access_key_;}
private:
	class Data{
	public:
		Data():refcount_(0)
			,count_(0){}
		Data(const std::string& access_key)
			:refcount_(0)
			,count_(0)
			,access_key_(access_key){}
		void AddRef(){refcount_ ++;}
		void Release(){if (!--refcount_)delete this;}
	public:
		const std::string access_key_;
		int count_;
	private:
		int refcount_;
	};
	Data*                    data_;
};


class LBSInfos{
public:
	explicit LBSInfos();
	explicit LBSInfos(const std::string& host,const double& latitude,
			const double& longitude,const std::string& city,const std::string& district,
			const std::string& province,std::string& street,time_t current_time);

	LBSInfos(const LBSInfos& lbs_basic_info);
	LBSInfos& operator = (const LBSInfos& lbs_basic_info);

	void JsonSerializer(std::string* str);
	void JsonDeserialize(std::string* str);

	const std::string& host() const {return data_->host_;}
	const double latitude() const {return data_->latitude_;}
	const double longitude() const {return data_->longitude_;}
	const std::string& city() const {return data_->city_;}
	const std::string& district() const {return data_->district_;}
	const std::string& province() const {return data_->province_;}
	const std::string& street() const {return data_->street_;}
	const time_t current_time() const {return data_->current_time;}

	void set_host(const std::string& host) {data_->host_ = host;}
	void set_latitude(const double latitude) {data_->latitude_ = latitude;}
	void set_longitude(const double longitude) {data_->longitude_ = longitude;}
	void set_city(const std::string& city) {data_->city_ = city;}
	void set_district(const std::string& district) {data_->district_ = district;}
	void set_province(const std::string& province) {data_->province_ = province;}
	void set_street(const std::string&  street) {data_->street_ = street;}
	void set_current_time(const time_t current_time){data_->current_time = current_time;}


private:
	class Data{
	public:
		Data():refcount_(0)
		,latitude_(0.0)
		,longitude_(0.0)
		,current_time(0){}

		Data(const std::string& host,const double latitude,
				const double longitude,const std::string& city,const std::string& district,
				const std::string& province,std::string& street,time_t current_time)
		:refcount_(0)
		,latitude_(latitude)
		,longitude_(longitude)
		,city_(city)
		,district_(district)
		,province_(province)
		,street_(street)
		,current_time(current_time){}
		void AddRef(){refcount_ ++;}
		void Release(){if (!--refcount_)delete this;}
	public:
		std::string host_;
		double latitude_;
		double longitude_;
		std::string city_;
		std::string district_;
		std::string province_;
		std::string street_;
		time_t  current_time;
	private:
		int refcount_;
	};
	Data*       data_;

};

}
#endif
