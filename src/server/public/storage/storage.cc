#include "storage/storage.h"
#include "storage/mem_storage_impl.h"
#include "storage/redis_storage_impl.h"
#include "storage/mysql_storage_impl.h"
#include "storage/mssql_storage_impl.h"

namespace base_storage{
	
DictionaryStorageEngine* DictionaryStorageEngine::Create(int32 type){
    
    DictionaryStorageEngine* engine = NULL;
  
    switch(type){
    	
        case IMPL_MEM:
            engine = new MemStorageEngineImpl();
            break;
		case IMPL_RADIES:
			engine = new RedisStorageEngineImpl();
			break;
        default:
        	break;
    }

    return engine;
}


DBStorageEngine* DBStorageEngine::Create(int32 type){
	DBStorageEngine* engine = NULL;
	
	switch(type){
	
	    case IMPL_MYSQL:
	    	engine = new MysqlStorageEngineImpl();
	    	break;
	    case IMPL_MSSQL:
	    	engine = new MssqlStorageEngineImpl();
	    default:
	    	break;
	}
	return engine;
}

}
