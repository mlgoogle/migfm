#ifndef _NET_COMM_HID_H__
#define _NET_COMM_HID_H__

#include <string>
#include "net_comm/http_constants.h"
namespace net_comm{

class Hid{
public:
	explicit Hid();
	explicit Hid(const std::string& hid_string,const std::string& hostname);
	explicit Hid(const std::string& usrname,
				 const std::string& resource_name,
				 const std::string& hostname);
	explicit Hid(bool special,const std::string& special_string,const std::string& hostname);
	Hid(const Hid& hid):data_(hid.data_){
		if (data_ != NULL){
			data_->AddRef();
		}
	}

	Hid& operator=(const Hid& hid){
		if(hid.data_!=NULL){
			hid.data_->AddRef();
		}
		if (data_!=NULL){
			data_->Release();
		}

		data_ = hid.data_;
		return *this;
	}

	~Hid(){
		if(data_!=NULL){
			data_->Release();
		}
	}
	const std::string& username() const {return !data_?STR_EMPTY:data_->username_;}
	const std::string& resource() const {return !data_?STR_EMPTY:data_->resource_;}
	const std::string& hostname() const {return !data_?STR_EMPTY:data_->hostname_;} 

    bool IsValid() const;
private:

	static std::string preNode(const std::string str,
		std::string::const_iterator start,std::string::const_iterator end,
		bool *valid);
	static char prepNodeAscii(char ch, bool *valid);

	static std::string preResource(const std::string str,
		std::string::const_iterator start,std::string::const_iterator end,
		bool *valid);
	static char prepResourceAscii(char ch,bool *valid);

	static char prepDomainLabelAscii(char ch, bool *valid);
	static void prepDomainLabel(const std::string str,
		std::string::const_iterator start,std::string::const_iterator end,
		std::string *buf,bool *vaild);
	static void prepDomain(const std::string str,
		std::string::const_iterator start,std::string::const_iterator end,
		std::string *buf,bool *valid);
	std::string prepDomain(const std::string str,
		std::string::const_iterator start,std::string::const_iterator end,
		bool *valid);

  class Data{
  public:
	  Data():refcount_(1){}
	  Data(const std::string& username,const std::string& resource,const std::string& hostname)
		  :username_(username)
		  ,resource_(resource)
		  ,hostname_(hostname)
		  ,refcount_(1){}
	  void AddRef(){refcount_++;}
	  void Release() {if(!--refcount_) delete this;}
	  const std::string username_;
	  const std::string resource_;
	  const std::string hostname_;
  private:
	  int refcount_;
  };
  Data*  data_;
};
}
#endif