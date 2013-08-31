#ifndef _MIG_FM_PUBLIC_BASIC_FILE_PATH_H__
#define _MIG_FM_PUBLIC_BASIC_FILE_PATH_H__

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>

namespace file{
class FilePath{
public:
  typedef std::string StringType;
	typedef StringType::value_type CharType;
	static const CharType kSeparators[];
	static const CharType kCurrentDirectory[];
	static const CharType kParentDirectory[];

	FilePath();
	FilePath(const FilePath& that);
	explicit FilePath(const StringType& path);
    ~FilePath();
	FilePath& operator=(const FilePath& that);

	//bool operator==(const FilePath& that) const;
	//bool operator!=(const FilePath& that) const;

	/*bool operator<(const FilePath& that) const{
	    return path_<that.path_;
	}*/

	const StringType& value() const {return path_;}

	bool empty() const {return path_.empty();}

	void clear() {return path_.clear();}

	bool IsSeparator(CharType character);

	FilePath DirName() const;
private:
	void StripTrailingSparatorsInternal();
private:
	StringType  path_;
};
}
#endif
