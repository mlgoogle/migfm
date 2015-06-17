/*
 * json_serializer.h
 *
 *  Created on: 2015年1月3日
 *      Author: kerry
 */

#ifndef JSON_SERIALIZER_H_
#define JSON_SERIALIZER_H_
#include "string_escape.h"
#include "value_serializer.h"
namespace base_logic{

//json
class JsonValueSerializer:public ValueSerializer{
public:
	JsonValueSerializer();
	JsonValueSerializer(std::string* json);
	virtual ~JsonValueSerializer();
public:
	virtual bool Serialize(const Value& root);

	virtual Value* Deserialize(int* error_code, std::string* error_str);

	virtual void FreeValue(base_logic::Value* value);
private:
	void BuildJSONString(const Value* const node,int depth,bool escape);

	inline void IndentLine(int depth){json_string_->append(std::string(depth * 3, ' '));}

	void AppendQuoteString(const std::string& str){StringEscape::JsonDoubleQuote(str,true,json_string_);}

private:

	Token ParseToken();
	//Recursively build Value.  Returns NULL if we don't have a valid string.
	//If |is_root| is true, we verify that the root element is either an object or an array.
	Value* BuildValue(bool is_root);

	bool EatComment();
	void EatWhitesspaceAndComments();

	void SetErrorCode(ParseError error,const wchar_t* error_pos);

private:


	bool ReadHexDigits(Token& token, int digits);
	 // Parses a sequence of characters into a Token::NUMBER. If the sequence of
	  // characters is not a valid number, returns a Token::INVALID_TOKEN. Note
	  // that DecodeNumber is used to actually convert from a string to an
	  // int/double.
	Token ParseNumberToken();


	Token ParseStringToken();

	// A helper method for ParseNumberToken.  It reads an int from the end of
	// token.  The method returns false if there is no valid integer at the end of
	// the token.
	bool ReadInt(Token& token, bool can_have_leading_zeros);

	 // Checks if |json_pos_| matches str.
	bool NextStringMatch(const std::wstring& str);

	Value* DecodeNumber(const Token& token);
	Value* DecodeString(const Token& token);

private:
	std::string*     json_string_;

	bool             pretty_print_;

	const wchar_t*   start_pos_;

	const wchar_t*   json_pos_;
	// Used to keep track of how many nested lists/dicts there are.
	int stack_depth_;

	// A parser flag that allows trailing commas in objects and arrays.
	bool allow_trailing_comma_;

	ParseError error_code_;

	int error_line_;
	int error_col_;
};
}



#endif /* JSON_SERIALIZER_H_ */
