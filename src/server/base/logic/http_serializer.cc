#include "http_serializer.h"
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

bool HttpValueSerializer::NextStringMatch(const std::wstring& str){
	for(size_t i = 0; i < str.length(); ++i){
		if('\0' == *http_pos_)
			return false;
		if(*(http_pos_ + i) != str[i])
			return false;
	}

	return true;
}

bool HttpValueSerializer::NextNumberMatch(){
	/*for(int i = 0; (*(str_pos+i))!='&';++i){
		//if(((int)(*(str_pos+i)))==0)
		//	return true;
		if('\0' == str_pos[i])
			return true;
		if('0' > str_pos[i] || str_pos[i] > '9')
			return false;
	}*/



	int pos = 0;
	int spot = 0;//一个小数只能有一个 .
	wchar_t c = http_pos_[pos];
	while('&' != c && '\0' != c){//处理浮点数 指数
		if( ('0'<=c && c<='9')){
			++pos;
			c = http_pos_[pos];
			continue;
		}

		if('.' == c){
			++pos;
			c = http_pos_[pos];
			spot++;
			if(spot>1)
				return false;
			continue;
		}

		if('e' == c||'E' == c){
			++pos;
			c = http_pos_[pos];
			continue;
		}

		if('-' == c||'+' == c){
			++pos;
			c = http_pos_[pos];
			continue;
		}

		return false;
	}

	if(pos>19)//fix me
		return false;

	return true;
}


Token HttpValueSerializer::ParseNumberToken(){
	Token token(Token::NUMBER, http_pos_,0);
	wchar_t c = *http_pos_;
	if('-' == c){ //负数
		++token.length;
		c =token.NextChar();
	}

	if(!ReadInt(token,false))
		return kInvalidToken;

	//Optional fraction part
	c = token.NextChar();
	if('.' == c){
		++token.length;
		if(!ReadInt(token,true))
			return kInvalidToken;
		c = token.NextChar();
	}

	//Optional exponent part
	if('e' == c || 'E' == c){
		++token.length;
		c = token.NextChar();
		if('-' == c || '+' == c){
			++token.length;
			c = token.NextChar();
		}
		if(!ReadInt(token,true))
			return kInvalidToken;
	}

	return token;
}


Token HttpValueSerializer::ParseStringToken(){
	Token token(Token::STRING,http_pos_,0);
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
		 }else if('&' == c||'=' == c){
			 return token;
		 }
		++token.length;
		c = token.NextChar();
	}
	return token;
}

bool HttpValueSerializer::ReadHexDigits(Token& token,int digits){
	for(int i = 1; i<= digits; ++i){
		wchar_t c = *(token.begin + token.length + i);
		if('\0' == c)
			return false;
		if(!(('0' <= c && c <= '9')||('a' <= c && c<= 'f')||
				('A' <= c && c <= 'F')))
			return false;
	}

	token.length += digits;
	return true;
}

bool HttpValueSerializer::ReadInt(Token& token,bool can_have_leading_zeros){
	wchar_t first =token.NextChar();
	int len = 0;

	wchar_t c = first;
	while('\0' != c && '0'<=c && c<='9'){
		++token.length;
		++len;
		c = token.NextChar();
	}

	if(len==0)
		return false;

	if(!can_have_leading_zeros && len >1 &&'0'==first)
		return false;

	return true;
}

Value* HttpValueSerializer::DecodeNumber(const Token& token){
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

Value* HttpValueSerializer::DecodeString(const Token& token){
	std::wstring decoded_str;
	decoded_str.reserve(token.length);
	for(int i = 0; i<token.length;++i){
		wchar_t c = *(token.begin + i);
		if('\\' == c){
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
				   	   HexToInt(* (token.begin + i + 2)));
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
		}else{
			decoded_str.push_back(c);
		}
	}

	return Value::CreateStringValue(decoded_str);
}




//http

HttpValueSerializer::HttpValueSerializer(std::string* http)
:pretty_print_(true)
,http_string_(http){

}

HttpValueSerializer::~HttpValueSerializer(){

}

bool HttpValueSerializer::Serialize(const Value& root){
	BuildHTTPString(&root,0,false);
	return true;
}

void HttpValueSerializer::BuildHTTPString(const Value* const node, int depth,bool escape){
	switch(node->GetType()){
	case Value::TYPE_NULL:
		http_string_->append("null");
		break;
	case Value::TYPE_BOOLEAN:
		{
			bool value;
			bool result = node->GetAsBoolean(&value);
			http_string_->append(value ? "true" : "false");
			break;
		}
	case Value::TYPE_INTEGER:
		{
			int32 value;
			bool result = node->GetAsInteger(&value);
			base::BasicUtil::StringUtil::StringAppendF(http_string_,"%d",value);
			break;
		}
	case Value::TYPE_BIG_INTEGER:
		{
			int64 value;
			bool result = node->GetAsBigInteger(&value);
#if __LP64__
			base::BasicUtil::StringUtil::StringAppendF(http_string_,"%ld",value);
#else
			base::BasicUtil::StringUtil::StringAppendF(http_string_,"%lld",value);
#endif
			break;
		}
	case Value::TYPE_REAL:
		{
			double value;
			bool result = node->GetAsReal(&value);
			std::string real = base::BasicUtil::StringUtil::DoubleToString(value);
			if (real[0] == '.') {
		          real.insert(0, "0");
			} else if (real.length() > 1 && real[0] == '-' && real[1] == '.') {
		          // "-.1" bad "-0.1" good
		          real.insert(1, "0");
			}
			http_string_->append(real);
			break;
		}

	case Value::TYPE_STRING:
		{
			std::string value;
			bool result = node->GetAsString(&value);
			if(escape){

			}else{
				StringEscape::HttpDoubleQuote(value,true,http_string_);
			}
		}
		break;
	case Value::TYPE_DICTIONARY:
		{
			const DictionaryValue* dict =
					static_cast<const DictionaryValue*>(node);
			for(DictionaryValue::key_iterator key_itr = dict->begin_keys();
					key_itr != dict->end_keys();++key_itr){
				if(key_itr!=dict->begin_keys())
					http_string_->append("&");

				Value* value = NULL;
				bool result = dict->GetWithoutPathExpansion(*key_itr,&value);
				AppendQuoteString(base::BasicUtil::StringConversions::WideToASCII(*key_itr));
				http_string_->append("=");
				BuildHTTPString(value,depth+1,escape);
			}

		}
		break;
	}
}

Value* HttpValueSerializer::Deserialize(int* error_code,std::string* error_str){
	Value*  value = NULL;
	//The input must be in UTF-8
	/*if(!base::BasicUtil::StringUtil::IsStringUTF8(*http_string_)){
		error_code_ = UNSUPPORTED_ENCODING;
		return NULL;
	}*/
	//末尾加入介绍表示付
	(*http_string_)[http_string_->length() - 1] = '\0';
	std::wstring http_wide(base::BasicUtil::StringConversions::UTF8ToWide(*http_string_));
	start_pos_ = http_wide.c_str();

	//When the input string starts with a UTF-8 Byte-Order-Mark (0xEF, 0xBB, 0xBF),
	//the UTF8ToWide() function converts it to a Unicode BOM (U+FEFF).
	//To avoid the BuildValue() function from mis-treating a Unicode BOM as an invalid character and returning NULL,
	//skip a converted Unicode BOM if it exists.

	if(!http_wide.empty() && start_pos_[0] == 0xFEFF)
		++start_pos_;

	http_pos_ = start_pos_;
	allow_trailing_comma_ = true;
	stack_depth_ = 0;

	error_code_ = NO_ERROR;

	scoped_ptr<Value> root(BuildValue(true));
	if(root.get()){
		if(ParseToken().type == Token::END_OF_INPUT||ParseToken().type == Token::LIST_SEPARATOR){ // 空值处理
			return root.release();
		}else{
			SetErrorCode(UNEXPECTED_DATA_AFTER_ROOT,http_pos_);
		}
	}

	if(error_code == 0)
		SetErrorCode(SYNTAX_ERROR,http_pos_);

	return root.release();
}

void HttpValueSerializer::SetErrorCode(ParseError error,const wchar_t* error_pos){
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
Value* HttpValueSerializer::BuildValue(bool is_root){
	++stack_depth_;
	Token token(Token::INVALID_TOKEN,0,0);
	if(stack_depth_ > kStackLimit){
		SetErrorCode(TOO_MUCH_NESTING,http_pos_);
		return NULL;
	}

	//
	if(is_root)
		token = Token(Token::OBJECT_BEGIN,http_pos_,0);
	else
		token = ParseToken();

	scoped_ptr<Value> node;

	switch(token.type){
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

	  case Token::STRING:
		  node.reset(DecodeString(token));
		  if(!node.get())
			  return NULL;
		  break;

	  case Token::NUMBER:
		  node.reset((DecodeNumber(token)));
		  if(!node.get())
			  return NULL;
		  break;

	  case Token::OBJECT_BEGIN:
		  http_pos_ += token.length;
		  token = ParseToken();
		  node.reset(new DictionaryValue);
		  while(token.type != Token::OBJECT_END){
			  if(token.type != Token::STRING){
				  SetErrorCode(UNQUOTED_DICTIONARY_KEY,http_pos_);
				  return NULL;
			  }
			  scoped_ptr<Value> dict_key_value(DecodeString(token));
			  if(!dict_key_value.get())
				  return NULL;

			  //Convert the key into a wstring.
			  std::wstring dict_key;
			  bool success = dict_key_value->GetAsString(&dict_key);
			  http_pos_ += token.length;

			 token = ParseToken();
			 if(token.type != Token::OBJECT_PAIR_SEPARATOR)
				 return NULL;

			 http_pos_ += token.length;
			 token = ParseToken();

			 Value* dict_value = BuildValue(false);
			 if(!dict_value)////针对value为空处理
				 dict_value = Value::CreateStringValue("");
			 static_cast<DictionaryValue*>(node.get())->SetWithoutPathExpansion(dict_key,dict_value);
			 //if(!dict_value)
				// return NULL;
			 //static_cast<DictionaryValue*>(node.get())->SetWithoutPathExpansion(dict_key,dict_value);

			 //after a key/value pair,  &
			 token = ParseToken();
			 if(token.type == Token::LIST_SEPARATOR){
				 http_pos_ += token.length;
				 token = ParseToken();
			 }else if(token.type==Token::OBJECT_END){
				 break;
			 }else if(token.type != Token::OBJECT_END){
				 return NULL;
			 }
		  }//while
		  if(token.type!=Token::OBJECT_END)
			  return NULL;
		  break;
	  default:
		  return NULL;
	}// switch
	http_pos_ += token.length;
	--stack_depth_;
	return node.release();
}

Token HttpValueSerializer::ParseToken(){
	static const std::wstring kNullString(L"null");
	static const std::wstring kTrueString(L"true");
	static const std::wstring kFalseString(L"false");

	EatWhitesspaceAndComments();

	Token token(Token::INVALID_TOKEN,0,0);

	switch (*http_pos_){
		case 'n' :
		  if(NextStringMatch(kNullString))
			token = Token(Token::NULL_TOKEN,http_pos_,4);
		  else
			 token = ParseStringToken();
		  break;

		case 't':
		  if(NextStringMatch(kTrueString))
			  token = Token(Token::BOOL_TRUE,http_pos_,4);
		  else
			  token = ParseStringToken();
		  break;

		case 'f':
		  if(NextStringMatch(kFalseString))
			  token = Token(Token::BOOL_FALSE,http_pos_,4);
		  else
			  token = ParseStringToken();
		  break;

		case '&':
		  token = Token(Token::LIST_SEPARATOR,http_pos_,1);
		  break;

		case '=':
		  token = Token(Token::OBJECT_PAIR_SEPARATOR,http_pos_,1);
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
		case '-'://负数
			if(NextNumberMatch())
				token = ParseNumberToken();
			else
				token = ParseStringToken();
		 break;
		case '\0':
		   token = Token(Token::OBJECT_END,http_pos_,0);
		   break;
		//case '|':内容连接符
		default://字符串
		   token = ParseStringToken();
		 break;
	}
	return token;
}

void HttpValueSerializer::EatWhitesspaceAndComments(){
	while('\n' != *http_pos_){ //
		switch (*http_pos_){
			case ' ':
			//case ' \n':
			case '\r':
			case '\t':
				++http_pos_;
				break;
			default:
				return;
		}
	}
}


}
