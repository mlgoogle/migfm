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
		END_OF_INPUT = OBJECT_END,
		INVALID_TOEKN,
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


bool NextStringMatch(const wchar_t* str_pos,
		const std::wstring& str);

bool NextNumberMatch(const wchar_t* str_pos);

bool ReadInt(Token& token,bool can_have_leding_zeros);

bool ReadHexDigits(Token& token,int digits);

Token ParseNumberToken(const wchar_t* str_pos);

Token ParseStringToken(const wchar_t* str_pos);

// Try and convert the substring that token holds into an int or a double. If
// we can (ie., no overflow), return the value, else return NULL.

Value* DecodeNumber(const Token& token);

Value* DecodeString(const Token& token);

class StringEscape{
public:
	static void JsonDoubleQuote(const std::string& str,bool put_in_quotes,
			std::string* dst);

	static std::string GetDoubleQuoteJson(const std::string& str);
};

//json
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

//xml
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

//httpRest
class HttpValueSerializer:public ValueSerializer{

public:
	HttpValueSerializer();
	virtual ~HttpValueSerializer();
	HttpValueSerializer(std::string* http);
public:
	virtual bool Serialize(const Value& root);

	virtual Value* Deserialize(int* error_code,std::string* error_str);

private:
	Token ParseToken();

	void EatWhitesspaceAndComments();
	//Recursively build Value.  Returns NULL if we don't have a valid string.
	//If |is_root| is true, we verify that the root element is either an object or an array.
	Value* BuildValue(bool is_root);

	void SetErrorCode(ParseError error,const wchar_t* error_pos);


private:
	std::string*     http_string_;

	bool             pretty_print_;

	const wchar_t*   start_pos_;

	const wchar_t*   http_pos_;

	// Used to keep track of how many nested lists/dicts there are.
	int stack_depth_;

	// A parser flag that allows trailing commas in objects and arrays.
	bool allow_trailing_comma_;

	ParseError error_code_;

	int error_line_;
	int error_col_;
};


}
#endif
