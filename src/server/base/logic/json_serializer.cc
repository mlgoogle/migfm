/*
 * json_serializer.cc
 *
 *  Created on: 2015年1月3日
 *      Author: kerry
 */
#include "json_serializer.h"
#include "string_escape.h"
#include "basic/basictypes.h"
#include "basic/basic_util.h"
#include "basic/scoped_ptr.h"
#include <math.h>

namespace base_logic{

static const Token kInvalidToken(Token::INVALID_TOKEN,0,0);
static const int kStackLimit = 100;

inline int HexToInt(wchar_t c) {
	if ('0' <= c && c <= '9') {
		return c - '0';
	} else if ('A' <= c && c <= 'F') {
		return c - 'A' + 10;
	} else if ('a' <= c && c <= 'f') {
		return c - 'a' + 10;
	}
	return 0;
}
/*
Token ParseStringToken(const wchar_t* str_pos){
	Token token(Token::STRING,str_pos,1);
	wchar_t c = token.NextChar();
	while('\0' != c){
		if('\\'==c){
			++token.length;
			c = token.NextChar();
			switch (c){
			   case 'x':
				 if(!ReadHexDigits(token,2))
					 return kInvalidToken;
				 break;

			   case 'u':
				 if(!ReadHexDigits(token,4))
					 return kInvalidToken;
				 break;

			   case '\\':
			   case '/':
			   case 'b':
			   case 'f':
			   case 'n':
			   case 'r':
			   case 't':
			   case 'v':
			   case '"':
				break;
			   default:
				   ++token.length;
				   return token;
			}
		 }else if('"' == c){
			 return token;
		 }
		++token.length;
		c = token.NextChar();
	}
	return token;
}*/


/////////////////////////////////////////////////////
static const char kpretty_print_line_ending[] = "\n";
//json
JsonValueSerializer::JsonValueSerializer()
:pretty_print_(false)
,json_string_(NULL){

}

JsonValueSerializer::JsonValueSerializer(std::string* json)
:pretty_print_(true)
,json_string_(json){

}

JsonValueSerializer::~JsonValueSerializer(){

}

bool JsonValueSerializer::NextStringMatch(const std::wstring& str) {
  for (size_t i = 0; i < str.length(); ++i) {
    if ('\0' == *json_pos_)
      return false;
    if (*(json_pos_ + i) != str[i])
      return false;
  }
  return true;
}

Value* JsonValueSerializer::DecodeNumber(const Token& token) {
	const std::wstring num_wstring(token.begin,token.length);
	const std::string  num_string =  base::BasicUtil::StringConversions::WideToASCII(num_wstring);
	int32 num_int;
	//if(base::BasicUtil::StringUtil::StringToInt(num_string,&num_int))
		//return Value::CreateIntegerValue(num_int);

	int64 num_int64;
	if(base::BasicUtil::StringUtil::StringToInt64(num_string,&num_int64))
		return Value::CreateBigIntegerValue(num_int64);

	double num_double;

	if(base::BasicUtil::StringUtil::StringToDouble(num_string,&num_double)&&finite(num_double))
		return Value::CreateRealValue(num_double);

	return NULL;
}

Value* JsonValueSerializer::DecodeString(const Token& token) {
  std::wstring decoded_str;
  decoded_str.reserve(token.length - 2);

  for (int i = 1; i < token.length - 1; ++i) {
    wchar_t c = *(token.begin + i);
    if ('\\' == c) {
      ++i;
      c = *(token.begin + i);
      switch (c) {
        case '"':
        case '/':
        case '\\':
          decoded_str.push_back(c);
          break;
        case 'b':
          decoded_str.push_back('\b');
          break;
        case 'f':
          decoded_str.push_back('\f');
          break;
        case 'n':
          decoded_str.push_back('\n');
          break;
        case 'r':
          decoded_str.push_back('\r');
          break;
        case 't':
          decoded_str.push_back('\t');
          break;
        case 'v':
          decoded_str.push_back('\v');
          break;

        case 'x':
          decoded_str.push_back((HexToInt(*(token.begin + i + 1)) << 4) +
                                HexToInt(*(token.begin + i + 2)));
          i += 2;
          break;
        case 'u':
          decoded_str.push_back((HexToInt(*(token.begin + i + 1)) << 12 ) +
                                (HexToInt(*(token.begin + i + 2)) << 8) +
                                (HexToInt(*(token.begin + i + 3)) << 4) +
                                HexToInt(*(token.begin + i + 4)));
          i += 4;
          break;

        default:
          // We should only have valid strings at this point.  If not,
          // ParseStringToken didn't do it's job.
          return NULL;
      }
    } else {
      // Not escaped
      decoded_str.push_back(c);
    }
  }
  return Value::CreateStringValue(decoded_str);
}

bool JsonValueSerializer::EatComment() {
  if ('/' != *json_pos_)
    return false;

  wchar_t next_char = *(json_pos_ + 1);
  if ('/' == next_char) {
    // Line comment, read until \n or \r
    json_pos_ += 2;
    while ('\0' != *json_pos_) {
      switch (*json_pos_) {
        case '\n':
        case '\r':
          ++json_pos_;
          return true;
        default:
          ++json_pos_;
      }
    }
  } else if ('*' == next_char) {
    // Block comment, read until */
    json_pos_ += 2;
    while ('\0' != *json_pos_) {
      if ('*' == *json_pos_ && '/' == *(json_pos_ + 1)) {
        json_pos_ += 2;
        return true;
      }
      ++json_pos_;
    }
  } else {
    return false;
  }
  return true;
}

void JsonValueSerializer::FreeValue(base_logic::Value* value){
	//类别识别
	if(value->GetType()==base_logic::Value::TYPE_DICTIONARY){
		base_logic::DictionaryValue* dict = (base_logic::DictionaryValue*)(value);
		if(dict!=NULL){delete value;value=NULL;}
	}
	//if(value!=NULL){delete value;value=NULL;}
}

Value* JsonValueSerializer::Deserialize(int* error_code,std::string* error_str){
	base_logic::DictionaryValue* value = new base_logic::DictionaryValue;
	value->SetBigInteger(L"AGE",1321321);
	value->SetString(L"name","TTTTRRRR");
	value->SetBigInteger(L"AGE1",1321321);
	value->SetString(L"name1","TTTTRRRR");

	value->SetBigInteger(L"AGE2",1321321);
	value->SetString(L"name2","TTTTRRRR");

	value->SetBigInteger(L"AGE3",1321321);
	value->SetString(L"name3","TTTTRRRR");

	value->SetBigInteger(L"AGE4",1321321);
	value->SetString(L"name4","TTTTRRRR");

	value->SetBigInteger(L"AGE5",1321321);
	value->SetString(L"name5","TTTTRRRR");

	value->SetBigInteger(L"AGE6",1321321);
	value->SetString(L"name6","TTTTRRRR");
	for(int i =0;i<10000;i++){
		value->SetInteger(base::BasicUtil::StringConversions::ASCIIToWide(base::BasicUtil::StringUtil::Int64ToString(i)),i);
	}


	return NULL;

	/*std::wstring json_wide(base::BasicUtil::StringConversions::UTF8ToWide(*json_string_));
	start_pos_ = json_wide.c_str();

	if(!json_wide.empty() && start_pos_[0] == 0xFEFF)
		++start_pos_;

	json_pos_ = start_pos_;
	allow_trailing_comma_ = true;
	stack_depth_ = 0;

	error_code_ = NO_ERROR;

	scoped_ptr<Value> root(BuildValue(true));
	if(root.get()){
		if(ParseToken().type == Token::END_OF_INPUT){ // 空值处理
			return root.release();
		}else{
			SetErrorCode(UNEXPECTED_DATA_AFTER_ROOT,json_pos_);
		}
	}

	if(error_code == 0)
		SetErrorCode(SYNTAX_ERROR,json_pos_);

	return root.release();*/
}


Value* JsonValueSerializer::BuildValue(bool is_root){
	++stack_depth_;
	if(stack_depth_ > kStackLimit){
		//SetErrorCode(TOO_MUCH_NESTING,http_pos_);
		return NULL;
	}
	Token token = ParseToken();
	  // The root token must be an array or an object.
	  if (is_root && token.type != Token::OBJECT_BEGIN &&
	      token.type != Token::ARRAY_BEGIN) {
	    SetErrorCode(BAD_ROOT_ELEENT_TYPE, json_pos_);
	    return NULL;
	  }

	  scoped_ptr<Value> node;

	  switch (token.type) {
	    case Token::END_OF_INPUT:
	    case Token::INVALID_TOKEN:
	      return NULL;

	    case Token::NULL_TOKEN:
	      node.reset(Value::CreateNullValue());
	      break;

	    case Token::BOOL_TRUE:
	      node.reset(Value::CreateBooleanValue(true));
	      break;

	    case Token::BOOL_FALSE:
	      node.reset(Value::CreateBooleanValue(false));
	      break;

	    case Token::NUMBER:
	      node.reset(DecodeNumber(token));
	      if (!node.get())
	        return NULL;
	      break;

	    case Token::STRING:
	      node.reset(DecodeString(token));
	      if (!node.get())
	        return NULL;
	      break;

	    case Token::ARRAY_BEGIN:
	      {
	        json_pos_ += token.length;
	        token = ParseToken();

	        node.reset(new ListValue());
	        while (token.type != Token::ARRAY_END) {
	          Value* array_node = BuildValue(false);
	          if (!array_node)
	            return NULL;
	          static_cast<ListValue*>(node.get())->Append(array_node);

	          // After a list value, we expect a comma or the end of the list.
	          token = ParseToken();
	          if (token.type == Token::LIST_SEPARATOR) {
	            json_pos_ += token.length;
	            token = ParseToken();
	            // Trailing commas are invalid according to the JSON RFC, but some
	            // consumers need the parsing leniency, so handle accordingly.
	            if (token.type == Token::ARRAY_END) {
	              if (!allow_trailing_comma_) {
	                SetErrorCode(TRAILING_COMMA, json_pos_);
	                return NULL;
	              }
	              // Trailing comma OK, stop parsing the Array.
	              break;
	            }
	          } else if (token.type != Token::ARRAY_END) {
	            // Unexpected value after list value.  Bail out.
	            return NULL;
	          }
	        }
	        if (token.type != Token::ARRAY_END) {
	          return NULL;
	        }
	        break;
	      }

	    case Token::OBJECT_BEGIN:
	      {
	        json_pos_ += token.length;
	        token = ParseToken();

	        node.reset(new DictionaryValue);
	        while (token.type != Token::OBJECT_END) {
	          if (token.type != Token::STRING) {
	            SetErrorCode(UNQUOTED_DICTIONARY_KEY, json_pos_);
	            return NULL;
	          }
	          scoped_ptr<Value> dict_key_value(DecodeString(token));
	          if (!dict_key_value.get())
	            return NULL;

	          // Convert the key into a wstring.
	          std::wstring dict_key;
	          bool success = dict_key_value->GetAsString(&dict_key);


	          json_pos_ += token.length;
	          token = ParseToken();
	          if (token.type != Token::OBJECT_PAIR_SEPARATOR)
	            return NULL;

	          json_pos_ += token.length;
	          token = ParseToken();
	          Value* dict_value = BuildValue(false);
	          if (!dict_value)
	            return NULL;
	          static_cast<DictionaryValue*>(node.get())->SetWithoutPathExpansion(
	              dict_key, dict_value);

	          // After a key/value pair, we expect a comma or the end of the
	          // object.
	          token = ParseToken();
	          if (token.type == Token::LIST_SEPARATOR) {
	            json_pos_ += token.length;
	            token = ParseToken();
	            // Trailing commas are invalid according to the JSON RFC, but some
	            // consumers need the parsing leniency, so handle accordingly.
	            if (token.type == Token::OBJECT_END) {
	              if (!allow_trailing_comma_) {
	                SetErrorCode(TRAILING_COMMA, json_pos_);
	                return NULL;
	              }
	              // Trailing comma OK, stop parsing the Object.
	              break;
	            }
	          } else if (token.type != Token::OBJECT_END) {
	            // Unexpected value after last object value.  Bail out.
	            return NULL;
	          }
	        }
	        if (token.type != Token::OBJECT_END)
	          return NULL;

	        break;
	      }

	    default:
	      // We got a token that's not a value.
	      return NULL;
	  }
	  json_pos_ += token.length;

	  --stack_depth_;
	  return node.release();
}

Token JsonValueSerializer::ParseToken(){
	  static const std::wstring kNullString(L"null");
	  static const std::wstring kTrueString(L"true");
	  static const std::wstring kFalseString(L"false");

	  EatWhitesspaceAndComments();

	  Token token(Token::INVALID_TOKEN, 0, 0);
	  switch (*json_pos_) {
	    case '\0':
	      token.type = Token::END_OF_INPUT;
	      break;

	    case 'n':
	      if (NextStringMatch(kNullString))
	        token = Token(Token::NULL_TOKEN, json_pos_, 4);
	      break;

	    case 't':
	      if (NextStringMatch(kTrueString))
	        token = Token(Token::BOOL_TRUE, json_pos_, 4);
	      break;

	    case 'f':
	      if (NextStringMatch(kFalseString))
	        token = Token(Token::BOOL_FALSE, json_pos_, 5);
	      break;

	    case '[':
	      token = Token(Token::ARRAY_BEGIN, json_pos_, 1);
	      break;

	    case ']':
	      token = Token(Token::ARRAY_END, json_pos_, 1);
	      break;

	    case ',':
	      token = Token(Token::LIST_SEPARATOR, json_pos_, 1);
	      break;

	    case '{':
	      token = Token(Token::OBJECT_BEGIN, json_pos_, 1);
	      break;

	    case '}':
	      token = Token(Token::OBJECT_END, json_pos_, 1);
	      break;

	    case ':':
	      token = Token(Token::OBJECT_PAIR_SEPARATOR, json_pos_, 1);
	      break;

	    case '0':
	    case '1':
	    case '2':
	    case '3':
	    case '4':
	    case '5':
	    case '6':
	    case '7':
	    case '8':
	    case '9':
	    case '-':
	      token = ParseNumberToken();
	      break;

	    case '"':
	      token = ParseStringToken();
	      break;
	  }
	  return token;
}

bool JsonValueSerializer::ReadInt(Token& token, bool can_have_leading_zeros){
	 wchar_t first = token.NextChar();
	  int len = 0;

	  // Read in more digits
	  wchar_t c = first;
	  while ('\0' != c && '0' <= c && c <= '9') {
	    ++token.length;
	    ++len;
	    c = token.NextChar();
	  }
	  // We need at least 1 digit.
	  if (len == 0)
	    return false;

	  if (!can_have_leading_zeros && len > 1 && '0' == first)
	    return false;

	  return true;
}

// A helper method for ParseStringToken.  It reads |digits| hex digits from the
// token. If the sequence if digits is not valid (contains other characters),
// the method returns false.
bool JsonValueSerializer::ReadHexDigits(Token& token, int digits) {
  for (int i = 1; i <= digits; ++i) {
    wchar_t c = *(token.begin + token.length + i);
    if ('\0' == c)
      return false;
    if (!(('0' <= c && c <= '9') || ('a' <= c && c <= 'f') ||
          ('A' <= c && c <= 'F'))) {
      return false;
    }
  }

  token.length += digits;
  return true;
}

Token JsonValueSerializer::ParseNumberToken() {
  // We just grab the number here.  We validate the size in DecodeNumber.
  // According   to RFC4627, a valid number is: [minus] int [frac] [exp]
  Token token(Token::NUMBER, json_pos_, 0);
  wchar_t c = *json_pos_;
  if ('-' == c) {
    ++token.length;
    c = token.NextChar();
  }

  if (!ReadInt(token, false))
    return kInvalidToken;

  // Optional fraction part
  c = token.NextChar();
  if ('.' == c) {
    ++token.length;
    if (!ReadInt(token, true))
      return kInvalidToken;
    c = token.NextChar();
  }

  // Optional exponent part
  if ('e' == c || 'E' == c) {
    ++token.length;
    c = token.NextChar();
    if ('-' == c || '+' == c) {
      ++token.length;
      c = token.NextChar();
    }
    if (!ReadInt(token, true))
      return kInvalidToken;
  }

  return token;
}

Token JsonValueSerializer::ParseStringToken() {
  Token token(Token::STRING, json_pos_, 1);
  wchar_t c = token.NextChar();
  while ('\0' != c) {
    if ('\\' == c) {
      ++token.length;
      c = token.NextChar();
      // Make sure the escaped char is valid.
      switch (c) {
        case 'x':
          if (!ReadHexDigits(token, 2)) {
            SetErrorCode(INVALID_ESCAPE, json_pos_ + token.length);
            return kInvalidToken;
          }
          break;
        case 'u':
          if (!ReadHexDigits(token, 4)) {
            SetErrorCode(INVALID_ESCAPE, json_pos_ + token.length);
            return kInvalidToken;
          }
          break;
        case '\\':
        case '/':
        case 'b':
        case 'f':
        case 'n':
        case 'r':
        case 't':
        case 'v':
        case '"':
          break;
        default:
          SetErrorCode(INVALID_ESCAPE, json_pos_ + token.length);
          return kInvalidToken;
      }
    } else if ('"' == c) {
      ++token.length;
      return token;
    }
    ++token.length;
    c = token.NextChar();
  }
  return kInvalidToken;
}

void JsonValueSerializer::SetErrorCode(ParseError error,const wchar_t* error_pos){
	int line_number = 1;
	int colum_number =1;

	// Figure out the line and column the error occured at.
	for(const wchar_t* pos = start_pos_;pos!=error_pos;++pos){
		if(*pos == '\0')
			return;
		if(*pos == '\n'){
			++line_number;
			colum_number = 1;
		}else{
			++colum_number;
		}
	}

	error_line_ = line_number;
	error_col_ = colum_number;
	error_code_ = error;

}


void JsonValueSerializer::EatWhitesspaceAndComments() {
  while ('\0' != *json_pos_) {
    switch (*json_pos_) {
      case ' ':
      case '\n':
      case '\r':
      case '\t':
        ++json_pos_;
        break;
      case '/':
        // TODO(tc): This isn't in the RFC so it should be a parser flag.
        if (!EatComment())
          return;
        break;
      default:
        // Not a whitespace char, just exit.
        return;
    }
  }
}

bool JsonValueSerializer::Serialize(const Value& root){
	BuildJSONString(&root,0,false);
	return true;
}

void JsonValueSerializer::BuildJSONString(const Value* const node,int depth,bool escape){
	if(node==NULL){//node
		assert(0);
		return;
	}
	switch (node->GetType()){
		case Value::TYPE_NULL:
			json_string_->append("null");
			break;

		case Value::TYPE_BOOLEAN:
		  {
			 bool value;
			 bool result = node->GetAsBoolean(&value);
			 json_string_->append(value ? "true" : "false");
			 break;
		  }

		case Value::TYPE_INTEGER:
		  {
			  int32 value;
			  bool result = node->GetAsInteger(&value);
			  base::BasicUtil::StringUtil::StringAppendF(json_string_,"%d",value);
			  break;
		  }

		case Value::TYPE_BIG_INTEGER:
		  {
			  int64 value;
			  bool  result = node->GetAsBigInteger(&value);
#if __LP64__
			  base::BasicUtil::StringUtil::StringAppendF(json_string_,"%ld",value);
#else
			  base::BasicUtil::StringUtil::StringAppendF(json_string_,"%lld",value);
#endif
			  break;

		  }
		case Value::TYPE_REAL:
		  {
		      double value;
		      bool result = node->GetAsReal(&value);
		      std::string real = base::BasicUtil::StringUtil::DoubleToString(value);
		      if (real.find('.') == std::string::npos &&
		            real.find('e') == std::string::npos &&
		            real.find('E') == std::string::npos) {
		          real.append(".0");
		      }
		        // The JSON spec requires that non-integer values in the range (-1,1)
		        // have a zero before the decimal point - ".52" is not valid, "0.52" is.
		      if (real[0] == '.') {
		          real.insert(0, "0");
		      } else if (real.length() > 1 && real[0] == '-' && real[1] == '.') {
		          // "-.1" bad "-0.1" good
		          real.insert(1, "0");
		      }
		      json_string_->append(real);
		      break;
		    }

		case Value::TYPE_STRING:
		    {
			   std::string value;
			   bool result = node->GetAsString(&value);
			   if(escape){//暂只支持UTF8

			   }else{
				   StringEscape::JsonDoubleQuote(value,true,json_string_);
			   }
			   break;

		    }
		case Value::TYPE_LIST:
		   {
			   json_string_->append("[");
			   if (pretty_print_)
				   json_string_->append(" ");

			   const ListValue* list = static_cast<const ListValue*>(node);
			   for(size_t i = 0; i< list->GetSize(); ++i){
				   if(i != 0){
					   json_string_->append(",");
					   if (pretty_print_)
						   json_string_->append(" ");
				   }
				   Value* value = NULL;
				   bool result = list->Get(i, &value);
				   BuildJSONString(value,depth,escape);
			   }
			   if (pretty_print_)
				   json_string_->append(" ");
			   json_string_->append("]");
			   break;
		   }

		case Value::TYPE_DICTIONARY:
		   {
			  json_string_->append("{");
			  if (pretty_print_)
				  json_string_->append(kpretty_print_line_ending);

			  const DictionaryValue* dict =
					  static_cast<const DictionaryValue*>(node);
			  for(DictionaryValue::key_iterator key_itr = dict->begin_keys();
					  key_itr != dict->end_keys();++key_itr){
				  if(key_itr != dict->begin_keys()){
					  json_string_->append(",");
					  if (pretty_print_)
						  json_string_->append(kpretty_print_line_ending);
				  }

				  Value* value = NULL;
				  bool result = dict->GetWithoutPathExpansion(*key_itr,&value);
				  if(pretty_print_)
					  IndentLine(depth + 1);
				  AppendQuoteString(base::BasicUtil::StringConversions::WideToASCII(*key_itr));
				  if(pretty_print_){
					  json_string_->append(": ");
				  }else{
					  json_string_->append(":");
				  }
				  BuildJSONString(value,depth+1,escape);
			  }

			  if(pretty_print_){
				  json_string_->append(kpretty_print_line_ending);
				  IndentLine(depth);
				  json_string_->append("}");
			  }else{
				  json_string_->append("}");
			  }
			  break;

		   }

	}
}

}



