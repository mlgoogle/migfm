/*
 * location_server.h
 *
 *  Created on: 2015年3月6日
 *      Author: pro
 */

#ifndef LOCATION_SERVER_H_
#define LOCATION_SERVER_H_

#include <list>
#include "basic/basic_info.h"

namespace base_lbs{

//单件模型
class LocationServer{
public:
	LocationServer(){};
	virtual ~LocationServer(){};
public:
	static void Init(std::list<base::ConnAddr>& db_addrlist,
			std::list<base::ConnAddr>& mem_addrlist);

	static void Dest();

	//
	static bool IPtoAddress(const std::string& host,double& latitude,double& longitude,
			std::string& city,std::string& district,std::string& province,std::string& street);

	static bool GeocoderForAddress(const double latitude,const double longitude,
				std::string& city,std::string& district,std::string& province,std::string& street);
};
}





#endif /* LOCATION_SERVER_H_ */
