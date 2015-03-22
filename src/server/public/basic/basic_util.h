#ifndef MIG_FM_PUBLIC_BASIC_BASIC_UTIL_H__
#define MIG_FM_PUBLIC_BASIC_BASIC_UTIL_H__
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <iconv.h>
#include <vector>
#include <assert.h>
#include "zlib.h"
#include "basic/basictypes.h"
#include "basic/md5sum.h"
#include "basic/basic_info.h"
#include "dmg_fp/dmg_fp.h"

// (This is undocumented but matches what the system C headers do.)
#define PRINTF_FORMAT(format_param, dots_param) \
    __attribute__((format(printf, format_param, dots_param)))

#define GG_VA_COPY(a,b) (va_copy(a,b))


namespace base{


class BasicUtil{


// 字符串处理


template<typename T>
struct ToUnsigned{
	typedef T Unsigned;
};

public:

	class StringUtil{
	public:
		class StringToIntTraits{
		public:
			typedef std::string string_type;
			typedef int32 value_type;
			static const int kBase = 10;
			static inline value_type convert_func(const string_type::value_type* str,
						string_type::value_type** endptr){
				return strtol(str,endptr,kBase);
			}
			static inline bool valid_func(const string_type& str){
				return !str.empty() && !isspace(str[0]);
			}
		};

		class StringToInt64Traits{
		public:
			typedef std::string string_type;
			typedef int64 value_type;
			static const int kBase = 10;
			static inline value_type convert_func(const string_type::value_type* str,
						string_type::value_type** endptr){
				return strtoll(str,endptr,kBase);
			}
			static inline bool valid_func(const string_type& str){
				return !str.empty() && !isspace(str[0]);
			}
		};


		class StringToDoubleTraits{
		public:
			typedef std::string string_type;
			typedef double value_type;
			static inline value_type convert_func(const string_type::value_type* str,
						string_type::value_type** endptr){
				return dmg_fp::strtod(str,endptr);
			}
			static inline bool valid_func(const string_type& str){
				return !str.empty() && !isspace(str[0]);
			}
		};
	public:
		template<typename IntegerType>
		static bool StringToInteger(const std::string& input,IntegerType* output);

		static bool StringToInt(const std::string& input,int32* output);

		static bool StringToInt64(const std::string& input,int64* output);

		static bool StringToDouble(const std::string& input,double* output);


		static size_t Copy(char* buf,size_t n,size_t pos,const char* ptr,size_t length);

		static std::string Int64ToString(int64 value);

		static std::string DoubleToString(double value);

		static std::wstring DobleToWString(double value);

		static bool IsStringUTF8(const std::string& str);

		static bool IsStringASCII(const std::wstring& str);

		static bool IsStringASCII(const std::string& str);

		static void StringAppendF(std::string* dst,const char* format,...);

		static void StringAppendF(std::wstring* dst,const wchar_t* format,...);

		static void StringAppendV(std::string* dst,const char* format,va_list ap);

		static void StringAppendV(std::wstring* dst,const wchar_t* format, va_list ap);


		static inline int vsnprintfT(char* buffer,size_t buf_size,const char* format,
				va_list argptr){
			/*int length = vsnprintf_s(buffer,buf_size,buf_size - 1,format,argptr);
			if(length < 0)
				return _vscprintf(format,argptr);
			return length;*/
			return vsnprintf(buffer,buf_size,format,argptr);
		}

		static inline int vsnprintfT(wchar_t* buff,size_t buf_size,const wchar_t* format,
				va_list argptr){
			/*int length = vsnwprintf_s(buffer,buf_size,buf_size - 1,format,argptr);
			if(length < 0)
				return _vscwprintf(format,argptr);
			return length;
			*/
			return vswprintf(buff,buf_size,format,argptr);
		}

	private:
		template<class STR>
		static bool DoIsStringASCII(const STR& str);

		template<class StringType>
		static void StringAppendVT(StringType* dst,
					const typename StringType::value_type* format,va_list ap);

		//template<typename StringToNumberTraits>
		//bool StringToNumber(const typename StringToNumberTraits::string_type& input,
		  //                  typename StringToNumberTraits::value_type* output);
	};

	class StringConversions{
	public:
		static std::string WideToUTF8(const std::wstring& wide);

		static std::wstring UTF8ToWide(const std::string& utf8);

		static std::string WideToASCII(const std::wstring& wide);

		static std::wstring ASCIIToWide(const std::string& ascii);

		static bool IsValidCharacter(uint32 code_point);

	private:
		static bool WideToUTF8(const wchar_t* src,size_t src_len,
				std::string* output);

		static bool UTF8ToWide(const char* src,size_t src_len,std::wstring* output);

		template<typename SRC_CHAR,typename DEST_STRING>
		static bool ConverUnicode(const SRC_CHAR* src,size_t src_len,DEST_STRING* output);


	};

	class StringConversionsUtils{
	public:
		template<typename CHAR>
		static void PrepareForUTF8Output(const CHAR* src,size_t src_len,std::string* output);

		template<typename STRING>
		static void PrepareForUTF16Or32Output(const char* src,size_t src_len,STRING* output);

		static bool ReadUnicodeCharacter(const wchar_t* src,int32 src_len,
					int32* char_index,uint32* code_point_out);

		static bool ReadUnicodeCharacter(const char* src,int32 src_len,
							int32* char_index,uint32* code_point_out);

		static size_t WriteUnicodeCharacter(uint32 code_point,std::string* output);

		static size_t WriteUnicodeCharacter(uint32 code_point,std::wstring* output);

	private:
		static inline bool IsValidCodepoint(uint32 code_point){
			  // Excludes the surrogate code points ([0xD800, 0xDFFF]) and
			  // codepoints larger than 0x10FFFF (the highest codepoint allowed).
			  // Non-characters and unassigned codepoints are allowed.
			  return code_point < 0xD800u ||
			         (code_point >= 0xE000u && code_point <= 0x10FFFFu);
		}

	};
public:
    BasicUtil(){}
    virtual ~BasicUtil(){}
public:
    static void GetHttpParamElement(const char* query,const char* name,
                                   std::string& value);
    
    static void ParserIdpPost(const std::string& request_string,std::string& sso_info,
                              std::string& username,std::string& password);
    
    static void ParserSpPost(const std::string& request_string,std::string& samlart,
                              std::string& relay_state);
 
    static void PaserRecordPost(const std::string& request_string,std::string& enter,
    	            std::string& url_code,std::string& content);
    	                        	
    static bool ParserHttpRequest(const std::string& request,
		                  std::map<std::string,std::string >& http_map);
		                          	
    static bool CheckToken(const std::string& request);    
    
    static const std::string& FormatCurrentTime();

    static std::string  GetRawString(std::string str);

    static std::string GetSrcString(std::string str);

    static bool UrlDecode(std::string& content,std::string& out_str);

    static bool UrlEncode(const std::string& content,std::string& out_str);

    static char toHex(const char &x);

    static int32 php_htoi(char *s);

    static void ResplaceString(const char* str,int len,const char c,std::string& dest_string);

	static bool GB2312ToUTF8 (const char *input, size_t inlen, char **output /* free */, size_t *outlen);

	static bool UTF8ToGB2312 (const char *input, size_t inlen, char **output /* free */, size_t *outlen);

	static bool UTF8ToGBK(const char *input, size_t inlen, char **output /* free */, size_t *outlen);

	static double CalcGEODistance(double latitude1, double longitude1, double latitude2, double longitude2);

	static int SplitStringChr( const char *str, const char *char_set,
		std::vector<std::string> &out );

	static bool GetUserToken(const std::string& uid,std::string& token);
	
	static bool CheckUserToken(const std::string& uid,const std::string& token);

	static bool GetUserInfo(const std::string& uid,UserInfo& usrinfo);

	static bool ConverNum(const int num,std::string& conver_num);

	static int HttpgzdeCompress(Byte *zdata, uLong nzdata, Byte *data, uLong *ndata);
};

}


#endif


