#ifndef _MIG_FM_BASE_LBS_LOGIC_UNIT_H__
#define _MIG_FM_BASE_LBS_LOGIC_UNIT_H__
#include <string>
namespace base_lbs{
class ResolveJson{
public:
	static bool ReolveJsonBaiduAddress(const std::string& content,std::string& city,
			std::string& district,std::string& province,std::string& street);

	static bool ReolveJsonBaiduIPToAddress(const std::string& content,std::string& city,
			std::string& district,std::string& province,std::string& street,double& latitude,
			double& longitude);

	static bool ReolveJsonBaiduAddressForGeocoder(const std::string& content,double& latitude,
			double& longitude);
};

class CalcGEO{//用于自行计算和地理相关的
public:
	static double  CalcGEODistance(double latitude1, double longitude1,
			double latitude2, double longitude2);
};

}
#endif
