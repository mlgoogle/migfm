#include "lbs_connector.h"
#include "lbs_baidu_connector.h"
namespace base_lbs{

LbsConnector* LbsConnectorEngine::lbs_connector_engine_ = NULL;

LbsConnector* LbsConnector::Create(int32 type){

	LbsConnector* engine = NULL;
    switch(type){

        case IMPL_BAIDU:
        	engine = new LbsBaiduConnectorImpl();
            break;
        default:
        	break;
    }
    return engine;
}

}
