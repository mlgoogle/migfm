#ifndef _MIG_FM_BASE_WEATHER_LOGIC_UNIT_H__
#define _MIG_FM_BASE_WEATHER_LOGIC_UNIT_H__

#include "basic/basictypes.h"
#include <string>
namespace base_weather{
class ResolveJson{
public:
	static bool ReolveJsonCaiYunWeather(const std::string& content,std::string& status,
			std::string& skycon,int32& temp);
};

}
#endif
