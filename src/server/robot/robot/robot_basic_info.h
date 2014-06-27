#ifndef _ROBOT_ROBOT_BASIC_INFO_H__
#define _ROBOT_ROBOT_BASIC_INFO_H__

#include <map>
#include <string>

#include "basic/basictypes.h"
#include <map>

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
	const int socket() const {return data_->socket_;}
	const int32 task_count() const {return data_->task_count_;}
	const int32 song_task_count() const {return data_->song_task_count_;}
	const int32 say_hello_count() const {return data_->say_hello_count_;}
	const int32 listen_task_count() const {return data_->listen_task_count_;}



	void set_sex(const int32 sex) {data_->sex_ = sex;}
	void set_songid(const int64 songid) {data_->songid_ = songid;}
	void set_latitude(const double latitude) {data_->latitude_ = latitude;}
	void set_longitude(const double longitude) {data_->longitude_ = longitude;}
	void set_nickname(const std::string& nickname) {data_->nickname_ = nickname;}
	void set_head_url(const std::string& head_url) {data_->head_url_ = head_url;}
	void set_socket(const int socket){data_->socket_ = socket;}
	void set_follow_uid(const int64 uid){data_->follow_map_[uid] = uid;}

	void add_task_count(){data_->task_count_++;}
	void add_song_task_count(){data_->song_task_count_++;add_task_count();}
	void add_hello_task_count(){data_->say_hello_count_++;add_task_count();}
	void add_listen_task_count(){data_->listen_task_count_++;add_task_count();}
private:
		class Data{
		public:
			Data():refcount_(1)
				,uid_(0)
				,sex_(0)
				,latitude_(0)
				,longitude_(0)
				,songid_(0)
				,socket_(0){}


			Data(const int64 uid,const int32 sex,const double latitude,const double longitude,
	                const int64 songid,const std::string nickname,const std::string& head_url)
			:refcount_(1)
			,task_count_(0)
			,song_task_count_(0)
			,say_hello_count_(0)
			,listen_task_count_(0)
			,uid_(uid)
			,sex_(sex)
			,latitude_(latitude)
			,longitude_(longitude)
			,songid_(songid)
			,nickname_(nickname)
			,head_url_(head_url)
			,socket_(0){}

			void AddRef(){refcount_ ++;}
			void Release(){if (!--refcount_)delete this;}

		public:
			const int64 uid_;
			int32  sex_;
			double latitude_;
			double longitude_;
			int64 songid_;
			int socket_;
			int task_count_;
			int song_task_count_;
			int say_hello_count_;
			int listen_task_count_;
			std::string nickname_;
			std::string head_url_;
			std::map<int64,int64> follow_map_;
		private:
			int refcount_;
		};

		Data*        data_;
	};


class SchedulerInfo{
public:
	SchedulerInfo(const int64 platform_id,const int socket,const std::string& ip,
			const std::string& machine_id);
	SchedulerInfo();
	SchedulerInfo(const SchedulerInfo& scheduler_info);
	SchedulerInfo& operator = (const SchedulerInfo& scheduler_info);
public:
	const std::string& machine_id() const {return data_->machine_id_;}
	const std::string& ip() const {return data_->ip_;}
	const int socket() const {return data_->socket_;}
	const int client_count() const {return data_->client_count_;}

	void add_client_count(){data_->client_count_++;}
	void relase_client_count(){data_->client_count_--;}

	void set_machine_id(const std::string& machine_id){data_->machine_id_ = machine_id;}
	void set_ip(const std::string& ip){data_->ip_ = ip;}
	void set_socket(const int socket){data_->socket_ = socket;}

public:
	class Data{
	public:
		Data(const int64 platform_id,const int socket,const std::string& ip,
				const std::string& machine_id)
		:refcount_(1)
		,platform_id_(platform_id)
		,socket_(socket)
		,ip_(ip)
		,client_count_(0)
		,machine_id_(machine_id){}

		Data():refcount_(1),platform_id_(0),client_count_(0){}

		void AddRef(){refcount_ ++;}
		void Release(){if (!--refcount_)delete this;}

	public:
		int64 platform_id_;
		int socket_;
		int client_count_;
		std::string ip_;
		std::string machine_id_;
	private:
		int refcount_;

	};
	Data*   data_;
};


}

#endif
