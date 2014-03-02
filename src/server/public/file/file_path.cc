#include "file_path.h"
#include "basic/basictypes.h"

namespace file{

const FilePath::CharType FilePath::kSeparators[] = "/";
const FilePath::CharType FilePath::kCurrentDirectory[] = ".";
const FilePath::CharType FilePath::kParentDirectory[] = "..";

FilePath::StringType::size_type FindDriveLetter(FilePath::StringType& path){
    return FilePath::StringType::npos;
}

FilePath::FilePath(){
}

FilePath::FilePath(const FilePath& that)
:path_(that.path_){

}

FilePath::FilePath(const StringType& path)
:path_(path){

}

FilePath::~FilePath(){

}

FilePath& FilePath::operator=(const FilePath& that){
    path_ = that.path_;
	return *this;
}
/*
bool FilePath::operator==(const FilePath& that) const{
    return path_ == that.path_;
}*/
/*
bool FilePath::operator!=(const FilePath& that) const {
    return path_ != that.path_;
}*/

bool FilePath::IsSeparator(CharType character){
    for (size_t i=0;i<arraysize(kSeparators)-1;++i){
        if(character == kSeparators[i]){
            return true;
        }	
    }
    return false;
}
void FilePath::StripTrailingSparatorsInternal(){
	StringType::size_type start = FindDriveLetter(path_)+2;
	StringType::size_type last_stripped = StringType::npos;
	for(StringType::size_type pos = path_.length();
			pos>start&&IsSeparator(path_[pos-1]);
			--pos){
	       if(pos!=start+1||last_stripped==start+2||!IsSeparator(path_[start-1])){
			   path_.resize(pos-1);
			   last_stripped = pos;
		   }
	}
}

FilePath  FilePath::DirName() const{
    FilePath new_path(path_);
	new_path.StripTrailingSparatorsInternal();
	StringType::size_type letter = FindDriveLetter(new_path.path_);

	StringType::size_type last_separator = 
		new_path.path_.find_last_of(kSeparators,StringType::npos,
			                         arraysize(kSeparators)-1);
	if(last_separator==StringType::npos){
	    new_path.path_.resize(letter+1);
	}else if(last_separator == letter+1)
		new_path.path_.resize(letter+2);
	else if(last_separator==letter+2
			/*IsSeparator(new_path.path_[letter + 1])*/){
		new_path.path_.resize(letter+3);
	}else if(last_separator!=0){
		new_path.path_.resize(last_separator);
	}

	new_path.StripTrailingSparatorsInternal();
	if(!new_path.path_.length())
		new_path.path_= kCurrentDirectory;
	return new_path;
}

}
