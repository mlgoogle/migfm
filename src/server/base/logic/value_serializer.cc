#include "value_serializer.h"
#include "basic/basictypes.h"
#include "basic/basic_util.h"


namespace base_logic{

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

bool JsonValueSerializer::Serialize(const Value& root){
	BuildJSONString(&root,0,false);
	return true;
}

void JsonValueSerializer::BuildJSONString(const Value* const node,int depth,bool escape){
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

Value* JsonValueSerializer::Deserialize(int* error_code, std::string* error_str){
	Value*  value = NULL;
	return value;
}

//escape
// Try to escape |c| as a "SingleEscapeCharacter" (\n, etc).  If successful,
// returns true and appends the escape sequence to |dst|.  This isn't required
// by the spec, but it's more readable by humans than the \uXXXX alternatives.
template<typename CHAR>
static bool JsonSingleEscapeChar(const CHAR c, std::string* dst) {
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
void JsonDoubleQuoteT(const STR& str,
                      bool put_in_quotes,
                      std::string* dst) {
  if (put_in_quotes)
    dst->push_back('"');

  for (typename STR::const_iterator it = str.begin(); it != str.end(); ++it) {
    typename base::BasicUtil::ToUnsigned<typename STR::value_type>::Unsigned c = *it;
    if (!JsonSingleEscapeChar(c, dst)) {
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

void StringEscape::JsonDoubleQuote(const std::string& str,bool put_in_quotes,
			std::string* dst){
	JsonDoubleQuoteT(str,put_in_quotes,dst);
}

std::string StringEscape::GetDoubleQuoteJson(const std::string& str){
	std::string dst;
	JsonDoubleQuote(str,true,&dst);
	return dst;
}



//xml
XmlValueSerializer::XmlValueSerializer()
:pretty_print_(false)
,xml_string_(NULL){

}

XmlValueSerializer::XmlValueSerializer(std::string* xml)
:pretty_print_(true)
,xml_string_(xml){

}

XmlValueSerializer::~XmlValueSerializer(){

}

bool XmlValueSerializer::Serialize(const Value& root){
	return true;
}

Value* XmlValueSerializer::Deserialize(int* error_code, std::string* error_str){
	Value*  value = NULL;
	return value;
}


}
