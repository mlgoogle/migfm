#include "storage.h"
#include "mem_storage_impl.h"
#include "mysql_storage_impl.h"
#include "redis_storage_impl.h"

StorageEngine* StorageEngine::Create(int type){
    
    StorageEngine* engine = NULL;
    switch(type){
    	
        case IMPL_MEM:
            engine = new MemStorageEngineImpl();
            break;
		case IMPL_MYSQL:
			engine = new MysqlStorageEngineImpl();
			break;
		case IMPL_RADIES:
			engine = new RedisStorageEngineImpl();
			break;
        default:
        	break;
    }

    return engine;
}
