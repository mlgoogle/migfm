#ifndef _NET_COMM_SCHEMA_OUT_PUT_H__
#define _NET_COMM_SCHEMA_OUT_PUT_H__

#include "base/basictypes.h"
#include "base/scoped_ptr.h"
#include "net_comm/export.h"
#include "net_comm/schema.pb.h"
#include "net_comm/content.pb.h"

// namespace schema{
// 	namespace type{
// 		class Content;
// 	}
// }
namespace net_comm{

class SchemaOutPut{

public:
	SchemaOutPut(schema::type::Content* content,
		schema::type::SchemaStatus* status)
		:content_(content)
		,status_(status){}
	
	SchemaOutPut(schema::type::Content* content)
		:content_(content)
		,status_(NULL){}

	SchemaOutPut(void)
		:content_(NULL)
		,status_(NULL){}

	virtual ~SchemaOutPut(void){};



	schema::type::Content* content(void){return content_.get();}
	void set_content(schema::type::Content* content){
		content_.reset(content);
	}

	schema::type::SchemaStatus* status(void){return status_.get();}
	void set_staus(schema::type::SchemaStatus* status){
		status_.reset(status);
	}

private:
	scoped_ptr<schema::type::Content>	    content_;
	scoped_ptr<schema::type::SchemaStatus>  status_;

	DISALLOW_COPY_AND_ASSIGN(SchemaOutPut);
};

}
#endif