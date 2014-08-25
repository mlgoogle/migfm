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

}
#endif
