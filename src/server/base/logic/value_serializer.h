#ifndef VALUE_SERIALIZER_H_
#define VALUE_SERIALIZER_H_
#include "logic/base_values.h"
#include <string>

namespace base_logic{

// Error codes during parsing
enum ParseError{
	NO_ERROR = 0,
	BAD_ROOT_ELEENT_TYPE,
	INVALID_ESCAPE,
	SYNTAX_ERROR,
	TRAILING_COMMA,
	TOO_MUCH_NESTING,
	UNEXPECTED_DATA_AFTER_ROOT,
	UNSUPPORTED_ENCODING,
	UNQUOTED_DICTIONARY_KEY
};

//A struct to hold a rest token
class Token{
public:
	enum Type{
		STRING,
		NUMBER,
		BOOL_TRUE,
		BOOL_FALSE,
		NULL_TOKEN,
		LIST_SEPARATOR,        // &
		OBJECT_PAIR_SEPARATOR, // =
		OBJECT_BEGIN, //
		OBJECT_END,
		ARRAY_BEGIN,  //json [
		ARRAY_END,  //json ]
		END_OF_INPUT,
		INVALID_TOKEN
	};

	Token(Type t,const wchar_t* b,int len)
		:type(t),begin(b),length(len){}
	Type type;

	const wchar_t*   begin;

	int length;

	wchar_t NextChar(){
		return *(begin + length);
	}
};

/*
//xml
class XmlValueSerializer:public ValueSerializer{
public:
	XmlValueSerializer();
	virtual ~XmlValueSerializer();
	XmlValueSerializer(std::string* xml);
public:
	virtual bool Serialize(const Value& root);

	virtual Value* Deserialize(int* error_code, std::string* error_str);

	virtual void FreeValue(base_logic::Value* value){}
private:
	std::string*     xml_string_;

	bool             pretty_print_;
};*/


}
#endif
