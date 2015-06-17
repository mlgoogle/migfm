#ifndef HTTP_SERIALIZER_H_
#define HTTP_SERIALIZER_H_
#include "string_escape.h"
#include "logic/base_values.h"
#include "logic/value_serializer.h"
#include <string>

namespace base_logic{


//httpRest
class HttpValueSerializer:public ValueSerializer{

public:
	HttpValueSerializer();
	virtual ~HttpValueSerializer();
	HttpValueSerializer(std::string* http);
public:
	virtual bool Serialize(const Value& root);

	virtual Value* Deserialize(int* error_code,std::string* error_str);

	virtual void FreeValue(base_logic::Value* value){}

private:
	void BuildHTTPString(const Value* const node, int depth,bool escape);

	inline void IndentLine(int depth){http_string_->append(std::string(depth * 3, ' '));}

	void AppendQuoteString(const std::string& str){StringEscape::HttpDoubleQuote(str,true,http_string_);}
private:
	Token ParseToken();

	void EatWhitesspaceAndComments();
	//Recursively build Value.  Returns NULL if we don't have a valid string.
	//If |is_root| is true, we verify that the root element is either an object or an array.
	Value* BuildValue(bool is_root);

	void SetErrorCode(ParseError error,const wchar_t* error_pos);

private:
	bool NextStringMatch(const std::wstring& str);

	bool NextNumberMatch();

	bool ReadInt(Token& token,bool can_have_leding_zeros);

	bool ReadHexDigits(Token& token,int digits);

	Token ParseNumberToken();

	Token ParseStringToken();

	// Try and convert the substring that token holds into an int or a double. If
	// we can (ie., no overflow), return the value, else return NULL.

	Value* DecodeNumber(const Token& token);

	Value* DecodeString(const Token& token);
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
