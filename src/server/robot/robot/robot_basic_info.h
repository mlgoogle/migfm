#ifndef _ROBOT_ROBOT_BASIC_INFO_H__
#define _ROBOT_ROBOT_BASIC_INFO_H__

#include <map>
#include <string>

#include "basic/basictypes.h"

namespace robot_base{




class PlatformInfo{
public:
	explicit PlatformInfo();
	explicit PlatformInfo(const int64 platform_id,
		         const std::string& platform_name);

	PlatformInfo(const PlatformInfo& platform_info);
	PlatformInfo& operator = (const PlatformInfo& platform_info);

private:
	class Data{
	public:
		Data():refcount_(1)
			,platform_id_(0){}
		Data(const int64 platform_id,const std::string& platform_name)
			:refcount_(1)
			,platform_id_(platform_id)
			,platform_name_(platform_name){}

		void AddRef(){refcount_ ++;}
		void Release(){if (!--refcount_)delete this;}
	public:
		int64                platform_id_;
		const std::string    platform_name_;
	private:
		int                  refcount_;
	};

	Data*                    data_;
};


class RobotBasicInfo{
public:
	explicit RobotBasicInfo(const int64 uid,const int32 sex,const double latitude,
            const double longitude,const int64 songid,const std::string nickname,
            const std::string& head_url);

	explicit RobotBasicInfo();


	RobotBasicInfo(const RobotBasicInfo& robot_info);
	RobotBasicInfo& operator = (const RobotBasicInfo& robot_info);

	const int64 uid() const {return data_->uid_;}
	const int32 sex() const {return data_->sex_;}
	const int64 songid() const {return data_->songid_;}
	const double latitude() const {return data_->latitude_;}
	const double longitude() const {return data_->longitude_;}
	const std::string& nickname() const {return data_->nickname_;}
	const std::string& head_url() const {return data_->head_url_;}


	void set_sex(const int32 sex) {data_->sex_ = sex;}
	void set_songid(const int64 songid) {data_->songid_ = songid;}
	void set_latitude(const double latitude) {data_->latitude_ = latitude;}
	void set_longitude(const double longitude) {data_->longitude_ = longitude;}
	void set_nickname(const std::string& nickname) {data_->nickname_ = nickname;}
	void set_head_url(const std::string& head_url) {data_->head_url_ = head_url;}
private:
		class Data{
		public:
			Data():refcount_(1)
				,uid_(0)
				,sex_(0)
				,latitude_(0)
				,longitude_(0)
				,songid_(0){}


			Data(const int64 uid,const int32 sex,const double latitude,const double longitude,
	                const int64 songid,const std::string nickname,const std::string& head_url)
			:refcount_(1)
			,uid_(uid)
			,sex_(sex)
			,latitude_(latitude)
			,longitude_(longitude)
			,songid_(songid)
			,nickname_(nickname)
			,head_url_(head_url){}

			void AddRef(){refcount_ ++;}
			void Release(){if (!--refcount_)delete this;}

		public:
			const int64 uid_;
			int32  sex_;
			double latitude_;
			double longitude_;
			int64 songid_;
			std::string nickname_;
			std::string head_url_;
		private:
			int refcount_;
		};

		Data*        data_;
	};
}

#endif
