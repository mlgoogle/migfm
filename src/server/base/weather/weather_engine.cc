#include "weather_engine.h"
#include "caiyun_weather_engine.h"
namespace base_weather{

WeatherConnector* WeatherConnectorEngine::weather_connector_engine_ = NULL;

WeatherConnector* WeatherConnector::Create(int32 type){

	WeatherConnector* engine = NULL;
    switch(type){

        case IMPL_CAIYUN:
        	engine = new CaiyunConnectorImpl();
            break;
        default:
        	break;
    }
    return engine;
}

}
