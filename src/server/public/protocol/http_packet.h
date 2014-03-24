#ifndef MIG_FM_PUBLIC_PROTOCOL_HTTP_PACKET_H__
#define MIG_FM_PUBLIC_PROTOCOL_HTTP_PACKET_H__

#include "basic/basictypes.h"
#include "log/mig_log.h"
#include <map>
#include <string>
namespace packet{

class HttpPacket{
public:
	HttpPacket(void);
	HttpPacket(const HttpPacket& packet);
	HttpPacket& operator=(const HttpPacket& packet);
	HttpPacket(const char* data,const int32 len);

	void PutAttrib(const std::string& strName,const std::string& strValue);

	bool GetAttrib(const std::string& strName,std::string& strValue);

	bool GetPacketType(std::string& strValue);

	void HttpPacketDump();
private:
	void SetData(const char* data,const int32 len);
public:
	class Data{
      public:
		  Data():
		   refcount_(1){}
		  ~Data(){map_value_.clear();}
		  void AddRef(){
			   __sync_fetch_and_add(&refcount_,1);
		  }

		  void Release(){
			  if ((__sync_fetch_and_sub(&refcount_,1)-1)==0)
				  delete this;
		  }
		  std::map<std::string,std::string>  map_value_;
	private:
		 int32                              refcount_;
    };
	Data*                                    data_;
private:
	std::string                              type_;

};

}
#endif
