#include "push_connector.h"
#include "push_baidu_connector.h"
namespace base_push{

PushConnector* PushConnectorEngine::push_connector_engine_ = NULL;

PushConnector* PushConnector::Create(int32 type){

	PushConnector* engine = NULL;
    switch(type){

        case IMPL_BAIDU:
        	engine = new PushBaiduConnectorImpl();
            break;
        default:
        	break;
    }
    return engine;
}

}
