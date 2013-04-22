#include "hid.h"
#include <ctype.h>

#include <string>

#include "base/logging.h"

namespace net_comm{

Hid::Hid():data_(NULL){
}

Hid::Hid(bool special,const std::string& special_string,const std::string& hostname){
	data_ = special ? (new Data(special_string,STR_EMPTY,hostname)):NULL;
}

Hid::Hid(const std::string &hid_string,const std::string& hostname){
	if (hid_string==STR_EMPTY){
		data_ = NULL;
		return ;
	}

	size_t slash = hid_string.find('/');
	std::string resourece_name = (slash == std::string::npos?STR_EMPTY:
						hid_string.substr(slash+1));
	
	//fix me

	//check mail format
	size_t at = hid_string.find('@');
	std::string node_name;
	size_t domain_begin;
	if (at<slash&&at!=std::string::npos){
		node_name = hid_string.substr(0,at);
		domain_begin = at+1;
	}else{
		domain_begin = 0;
	}

	size_t domain_length = (slash == std::string::npos?
							hid_string.length() - domain_begin
							:slash - domain_begin);
	std::string domain_name;
	domain_name = hid_string.substr(domain_begin,domain_length);

	if (domain_name.empty()){
		data_ = NULL;
		return;
	}

	bool valid_node;
	std::string validated_node = preNode(node_name,
		node_name.begin(),node_name.end(),&valid_node);
	bool valid_domain;
	std::string validated_domain = prepDomain(domain_name,
		domain_name.begin(),domain_name.end(),&valid_domain);
	bool valid_resource;
	std::string validated_resourece = preResource(resourece_name,
		resourece_name.begin(),resourece_name.end(),&valid_resource);
	if (!valid_node || !valid_domain || !valid_resource){
		data_ = NULL;
		return;
	}
	std::string username = node_name + "@" + domain_name;
	data_ = new Data(username,resourece_name,hostname);
}

Hid::Hid(const std::string& usrname, const std::string& resource_name,const std::string& hostname){
	size_t slash = usrname.find('@');
	if (slash == std::string::npos) {
		return;
	}
	std::string node_name = usrname.substr(0, slash);
	std::string domain_name = (slash == std::string::npos?STR_EMPTY:
		usrname.substr(slash+1));

	if (domain_name.empty()){
		data_ = NULL;
		return;
	}

	bool valid_node;
	std::string validated_node = preNode(node_name,
		node_name.begin(),node_name.end(),&valid_node);
	bool valid_domain;
	std::string validated_domain = prepDomain(domain_name,
		domain_name.begin(),domain_name.end(),&valid_domain);
	bool valid_resource;
	std::string validated_resourece = preResource(resource_name,
		resource_name.begin(),resource_name.end(),&valid_resource);
	if (!valid_node || !valid_domain || !valid_resource){
		data_ = NULL;
		return;
	}
	std::string tmp_username = node_name + "@" + domain_name;
	data_ = new Data(tmp_username,resource_name,hostname);
}


std::string
Hid::preNode(const std::string str, std::string::const_iterator start,std::string::const_iterator end, bool *valid){
	*valid = false;
	std::string result;

	for (std::string::const_iterator i = start;i<end;i++){
		bool char_valid = true;
		unsigned char ch = *i;
		if (ch<0x7F){// ascii
			result +=prepNodeAscii(ch,&char_valid);
		}
		else{
			result +=tolower(ch);
		}

		if (!char_valid){
			return STR_EMPTY;
		}
	}

	if (result.length()>1023){
		return STR_EMPTY;
	}
	*valid = true;
	return result;
}
char
Hid::prepNodeAscii(char ch, bool *valid) {
	*valid = true;
	switch (ch) {
	case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
	case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
	case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
	case 'V': case 'W': case 'X': case 'Y': case 'Z':
		return (char)(ch + ('a' - 'A'));

	case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05:
	case 0x06: case 0x07: case 0x08: case 0x09: case 0x0A: case 0x0B:
	case 0x0C: case 0x0D: case 0x0E: case 0x0F: case 0x10: case 0x11:
	case 0x12: case 0x13: case 0x14: case 0x15: case 0x16: case 0x17:
	case ' ': case '&': case '/': case ':': case '<': case '>': case '@':
	case '\"': case '\'':
	case 0x7F:
		*valid = false;
		return 0;

	default:
		return ch;
	}
}

std::string Hid::preResource(const std::string str, std::string::const_iterator start, std::string::const_iterator end, bool *valid){
	*valid = false;
	std::string str_out;
	for (std::string::const_iterator i = start;
		 i<end;i++){
		bool char_vaild = true;
		char ch = *i;
		if (ch<0x7F){
			str_out+= prepResourceAscii(ch,&char_vaild);
		}
		else{
			str_out+=tolower(ch);
		}
		if (!valid)
			return STR_EMPTY;
	}

	if (str_out.length()>1023)
		return STR_EMPTY;

	*valid = true;
	return str_out;
}

char
Hid::prepResourceAscii(char ch,bool *valid){
	*valid = false;
	switch (ch){
		case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05:
		case 0x06: case 0x07: case 0x08: case 0x09: case 0x0A: case 0x0B:
		case 0x0C: case 0x0D: case 0x0E: case 0x0F: case 0x10: case 0x11:
		case 0x12: case 0x13: case 0x14: case 0x15: case 0x16: case 0x17:
		case 0x7F:
			*valid = false;
			return 0;

		default:
			return ch;
	}
}

char Hid::prepDomainLabelAscii(char ch, bool *valid){
	*valid = true;
	switch (ch) {
		case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
		case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
		case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
		case 'V': case 'W': case 'X': case 'Y': case 'Z':
			return (char)(ch + ('a' - 'A'));

		case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05:
		case 0x06: case 0x07: case 0x08: case 0x09: case 0x0A: case 0x0B:
		case 0x0C: case 0x0D: case 0x0E: case 0x0F: case 0x10: case 0x11:
		case 0x12: case 0x13: case 0x14: case 0x15: case 0x16: case 0x17:
		case 0x18: case 0x19: case 0x1A: case 0x1B: case 0x1C: case 0x1D:
		case 0x1E: case 0x1F: case 0x20: case 0x21: case 0x22: case 0x23:
		case 0x24: case 0x25: case 0x26: case 0x27: case 0x28: case 0x29:
		case 0x2A: case 0x2B: case 0x2C: case 0x2E: case 0x2F: case 0x3A:
		case 0x3B: case 0x3C: case 0x3D: case 0x3E: case 0x3F: case 0x40:
		case 0x5B: case 0x5C: case 0x5D: case 0x5E: case 0x5F: case 0x60:
		case 0x7B: case 0x7C: case 0x7D: case 0x7E: case 0x7F:
			*valid = false;
			return 0;

		default:
			return ch;
	}
}

void Hid::prepDomainLabel(const std::string str,
						  std::string::const_iterator start,std::string::const_iterator end,
						  std::string *buf,bool *vaild){
	*vaild = false;
	int startLen = buf->length();
	for(std::string::const_iterator i = start;i<end;i++){
		bool char_vaild = true;
		char ch = *i;
		if (ch <= 0x7F) {
			(*buf) += prepDomainLabelAscii(ch,&char_vaild);
		}else{
			*buf+= ch;
		}
		if (!true)
			return;
	}

	int count =  buf->length() - startLen;
	if (count==0){
		return;
	}
	else if(count>63){
		return;
	}

	if ((*buf)[startLen] == '-') {
		return;
	}
	if ((*buf)[buf->length() - 1] == '-') {
		return;
	}
	*vaild = true;
}

void Hid::prepDomain(const std::string str, std::string::const_iterator start, std::string::const_iterator end,
					 std::string *buf, bool *valid){
	*valid = false;
	std::string::const_iterator last =start;
	for (std::string::const_iterator i = start;
		i<end;i++){
		bool label_vaild = true;
		char ch = *i;
		switch (ch){
			case 0x002E:
				prepDomainLabel(str,start,end,buf,valid);
				*buf = '.';
				last = i+1;
				break;
		}

		if (!label_vaild)
			return ;
	}

	prepDomainLabel(str,last,end,buf,valid);
}

std::string Hid::prepDomain(const std::string str, std::string::const_iterator start, 
							std::string::const_iterator end, bool *valid){
	*valid = false;
	std::string result;

	prepDomain(str, start, end, &result, valid);
	if (!*valid) {
		return STR_EMPTY;
	}

	if (result.length() > 1023) {
		return STR_EMPTY;
	}
	*valid = true;
	return result;
}

bool Hid::IsValid() const{
	return data_!=NULL && !data_->username_.empty();
}

}