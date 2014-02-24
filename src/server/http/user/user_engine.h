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
namespace userinfo{
	
class UserInfoEngine{
public:
    UserInfoEngine();
    ~UserInfoEngine();
    bool InitEngine(std::string& path);
    bool GetUserInfo(const char* query,std::string& result);
public: 
    static UserInfoEngine* GetEngine();
    static void  FreeEngine();
private:
    static userinfo::UserInfoEngine* engine_;

private:
    scoped_ptr<base::XmlSerialization>  xml_serialization_;
};
}
#endif
