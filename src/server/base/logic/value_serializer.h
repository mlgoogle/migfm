#ifndef VALUE_SERIALIZER_H_
#define VALUE_SERIALIZER_H_
#include "logic/base_values.h"
#include <string>

namespace base_logic{

class StringEscape{
public:
	static void JsonDoubleQuote(const std::string& str,bool put_in_quotes,
			std::string* dst);

	static std::string GetDoubleQuoteJson(const std::string& str);
};

class JsonValueSerializer:public ValueSerializer{
public:
	JsonValueSerializer();
	JsonValueSerializer(std::string* json);
	virtual ~JsonValueSerializer();
public:
	virtual bool Serialize(const Value& root);

	virtual Value* Deserialize(int* error_code, std::string* error_str);
private:
	void BuildJSONString(const Value* const node,int depth,bool escape);

	inline void IndentLine(int depth){json_string_->append(std::string(depth * 3, ' '));}

	void AppendQuoteString(const std::string& str){StringEscape::JsonDoubleQuote(str,true,json_string_);}
private:
	std::string*     json_string_;

	bool             pretty_print_;
};

class XmlValueSerializer:public ValueSerializer{
public:
	XmlValueSerializer();
	virtual ~XmlValueSerializer();
	XmlValueSerializer(std::string* xml);
public:
	virtual bool Serialize(const Value& root);

	virtual Value* Deserialize(int* error_code, std::string* error_str);
private:
	std::string*     xml_string_;

	bool             pretty_print_;

};


}
#endif
