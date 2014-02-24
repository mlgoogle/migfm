#ifndef MIG_FM_USER_ENGINE_H__
#define MIG_FM_USER_ENGINE_H__
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include "basic/constants.h"
#include "log/mig_log.h"
#include "basic/scoped_ptr.h"
#include "storage/db_serialization.h"
#include "storage/dic_serialization.h"
#include "xmpp/xml_serialization.h"
namespace record{
	
class RecordEngine{
public:
    RecordEngine();
    ~RecordEngine();
    bool InitEngine(std::string& path);

    bool Recording(std::string& content);
public: 
    static RecordEngine* GetEngine();
    static void  FreeEngine();
private:
    static record::RecordEngine* engine_;
};
}
#endif
