/*
 * string_escape.h
 *
 *  Created on: 2015年5月10日
 *      Author: pro
 */

#ifndef STRING_ESCAPE_H_
#define STRING_ESCAPE_H_
#include <string>
namespace base_logic{

class StringEscape{
public:

	static void JsonDoubleQuote(const std::string& str,bool put_in_quotes,
			std::string* dst);
	static std::string GetDoubleQuoteJson(const std::string& str);

	static void HttpDoubleQuote(const std::string& str,bool put_in_quotes,
			std::string* dst);

	static std::string GetDoubleQuoteHttp(const std::string& str);

	static void XMLDoubleQuote(const std::string& str,bool put_in_quotes,
			std::string* dst);

	static std::string GetDoubleQuoteXML(const std::string& str);
};


}



#endif /* STRING_ESCAPE_H_ */
