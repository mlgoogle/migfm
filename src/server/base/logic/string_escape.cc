/*
 * string_escape.cc
 *
 *  Created on: 2015年5月10日
 *      Author: pro
 */

#include "string_escape.h"
#include "basic/basic_util.h"
namespace base_logic{


//escape
// Try to escape |c| as a "SingleEscapeCharacter" (\n, etc).  If successful,
// returns true and appends the escape sequence to |dst|.  This isn't required
// by the spec, but it's more readable by humans than the \uXXXX alternatives.
template<typename CHAR>
static bool SingleEscapeChar(const CHAR c, std::string* dst) {
  // WARNING: if you add a new case here, you need to update the reader as well.
  // Note: \v is in the reader, but not here since the JSON spec doesn't
  // allow it.
  switch (c) {
    case '\b':
      dst->append("\\b");
      break;
    case '\f':
      dst->append("\\f");
      break;
    case '\n':
      dst->append("\\n");
      break;
    case '\r':
      dst->append("\\r");
      break;
    case '\t':
      dst->append("\\t");
      break;
    case '\\':
      dst->append("\\\\");
      break;
    case '"':
      dst->append("\\\"");
      break;
    default:
      return false;
  }
  return true;
}

template <class STR>
void HttpDoubleQuoteT(const STR& str,
                      bool put_in_quotes,
                      std::string* dst) {
  /*if (put_in_quotes)
    dst->push_back('"');
*/
  for (typename STR::const_iterator it = str.begin(); it != str.end(); ++it) {
    typename base::BasicUtil::ToUnsigned<typename STR::value_type>::Unsigned c = *it;
    if (!SingleEscapeChar(c, dst)) {
      if (c < 32 || c > 126 || c == '<' || c == '>') {
        // 1. Escaping <, > to prevent script execution.
        // 2. Technically, we could also pass through c > 126 as UTF8, but this
        //    is also optional.  It would also be a pain to implement here.
        unsigned char as_uint = static_cast<unsigned char>(c);
        //用于存储进行转，以BIN的方式存储
        //base::BasicUtil::StringUtil::StringAppendF(dst, "\\u%04X", as_uint);
        dst->push_back(as_uint);
      } else {
        unsigned char ascii = static_cast<unsigned char>(*it);
        dst->push_back(ascii);
      }
    }
  }

  /*if (put_in_quotes)
    dst->push_back('"');*/
}

template <class STR>
void JsonDoubleQuoteT(const STR& str,
                      bool put_in_quotes,
                      std::string* dst) {
  if (put_in_quotes)
    dst->push_back('"');

  for (typename STR::const_iterator it = str.begin(); it != str.end(); ++it) {
    typename base::BasicUtil::ToUnsigned<typename STR::value_type>::Unsigned c = *it;
    if (!SingleEscapeChar(c, dst)) {
      if (c < 32 || c > 126 || c == '<' || c == '>') {
        // 1. Escaping <, > to prevent script execution.
        // 2. Technically, we could also pass through c > 126 as UTF8, but this
        //    is also optional.  It would also be a pain to implement here.
        unsigned char as_uint = static_cast<unsigned char>(c);
        //用于存储进行转，以BIN的方式存储
        //base::BasicUtil::StringUtil::StringAppendF(dst, "\\u%04X", as_uint);
        dst->push_back(as_uint);
      } else {
        unsigned char ascii = static_cast<unsigned char>(*it);
        dst->push_back(ascii);
      }
    }
  }

  if (put_in_quotes)
    dst->push_back('"');
}

template <class STR>
void XMLDoubleQuoteT(const STR& str,
					bool put_in_quotes,
					std::string* dst){
	//if(put_in_quotes)
		//dst->push_back('"');
	for(typename STR::const_iterator it = str.begin(); it!= str.end();++it){
		typename base::BasicUtil::ToUnsigned<typename STR::value_type>::Unsigned c = *it;
		if(!SingleEscapeChar(c,dst)){
			if(c < 32 || c > 126 || c=='<' || c == '>'){
				unsigned char as_uint = static_cast<unsigned char>(c);
				//base::BasicUtil::StringUtil::StringAppendF(dst, "\\u%04X", as_uint);
				dst->push_back(as_uint);
			}else{
				unsigned char ascii = static_cast<unsigned char>(*it);
				dst->push_back(ascii);
			}
		}
	}
	//if (put_in_quotes)
	  //  dst->push_back('"');
}

void StringEscape::JsonDoubleQuote(const std::string& str,bool put_in_quotes,
			std::string* dst){
	JsonDoubleQuoteT(str,put_in_quotes,dst);
}

std::string StringEscape::GetDoubleQuoteJson(const std::string& str){
	std::string dst;
	JsonDoubleQuote(str,true,&dst);
	return dst;
}

void StringEscape::HttpDoubleQuote(const std::string& str,bool put_in_quotes,
			std::string* dst){
	HttpDoubleQuoteT(str,put_in_quotes,dst);
}

std::string StringEscape::GetDoubleQuoteHttp(const std::string& str){
	std::string dst;
	HttpDoubleQuote(str,true,&dst);
	return dst;
}

void StringEscape::XMLDoubleQuote(const std::string& str,bool put_in_quotes,
			std::string* dst){
	XMLDoubleQuoteT(str,put_in_quotes,dst);
}

std::string StringEscape::GetDoubleQuoteXML(const std::string& str){
	std::string dst;
	return dst;
}




}



