#include "wx_packet.h"

namespace wxinfo{


void WXPacket::PutAttrib(const std::string &strName, const std::string &strValue){
	
	map_value_[strName] = strValue;
}

bool WXPacket::GetAttrib(const std::string& strName,std::string& strValue){
	std::map<std::string,std::string>::iterator it = map_value_.find(strName);
	if (it!=map_value_.end()){
		strValue=(*it).second;
		return true;
	}
	return false;
}

bool WXPacket::GetPacketType(std::string &strValue){
	std::map<std::string,std::string>::iterator it = map_value_.find(MsgType);
	if (it!=map_value_.end()){
		strValue=(*it).second;
		return true;
	}
	return false;
}

}