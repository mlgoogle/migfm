#ifndef _MIG_FM_PUBLIC_BASIC_BASIC_INFO_H__
#define _MIG_FM_PUBLIC_BASIC_BASIC_INFO_H__
#include <stdio.h>
#include "constants.h"
#include "basictypes.h"
namespace base{

class MusicUsrInfo{
public:
    explicit MusicUsrInfo();
    explicit MusicUsrInfo(const std::string& name,const std::string& phone,
                          const std::string& content);

    MusicUsrInfo(const MusicUsrInfo& mi);
    MusicUsrInfo& operator=(const MusicUsrInfo& mi);

    ~MusicUsrInfo(){
        if(data_!=NULL){
            data_->Release();
        }
    }
    const std::string& name() const {return !data_?STR_EMPTY:data_->name_;}
    const std::string& phone() const {return !data_?STR_EMPTY:data_->phone_;}
    const std::string& content() const {return !data_?STR_EMPTY:data_->content_;}
private:
    class Data{
        public:
            Data():refcount_(1){}
            Data(const std::string& name,const std::string& phone,const std::string& content)
                :name_(name)
                ,phone_(phone)
                ,content_(content)
                ,refcount_(1){}
            void AddRef(){refcount_++;}
            void Release(){if(!--refcount_) delete this;}
            const std::string name_;
            const std::string phone_;
            const std::string content_;
        private:
            int refcount_;
    };

    Data*           data_;
};

class MusicInfo{
public:
	explicit MusicInfo();
	explicit MusicInfo(const std::string id,const std::string& sid,
					   const std::string& ssid,const std::string& album_title,
					   const std::string& title,const std::string& hq_url,
					   const std::string& pub_time,const std::string& artist,
					   const std::string& pic_url,const std::string& url = " ",
					   const int32 time = 0);

	MusicInfo(const MusicInfo& mi);
	MusicInfo& operator=(const MusicInfo& mi);

	~MusicInfo(){
		if(data_!=NULL){
			data_->Release();
		}
	}

	void set_id(const std::string& id) {data_->id_ = id;}
	void set_sid(const std::string& sid) {data_->sid_ = sid;}
	void set_ssid(const std::string& ssid) {data_->ssid_ = ssid;}
	void set_album_title(const std::string& album_title) {data_->album_title_ = album_title;}
	void set_title(const std::string& title) {data_->titile_ = title;}
	void set_hq_url(const std::string& hq_url){data_->hq_url_ = hq_url;}
	void set_url(const std::string& url) {data_->url_ = url;}
	void set_pub_time(const std::string& pub_time) {data_->pub_time_ = pub_time;}
	void set_artist(const std::string& artist) {data_->artist_ = artist;}
	void set_pic_url(const std::string& pic_url) {data_->pic_url_ = pic_url;}
	void set_music_time(const int32 music_time) {data_->music_time_ = music_time;}

	const std::string& id() const {return !data_?STR_EMPTY:data_->id_;}
	const std::string& sid() const {return !data_?STR_EMPTY:data_->sid_;}
	const std::string& ssid() const {return !data_?STR_EMPTY:data_->ssid_;}
	const std::string& album_title() const {return !data_?STR_EMPTY:data_->album_title_;}
	const std::string& title() const {return !data_?STR_EMPTY:data_->titile_;}
	const std::string& url() const {return !data_?STR_EMPTY:data_->url_;}
	const std::string& pub_time() const {return !data_?STR_EMPTY:data_->pub_time_;}
	const std::string& artist() const {return !data_?STR_EMPTY:data_->artist_;}
	const std::string& pic_url() const {return !data_?STR_EMPTY:data_->pic_url_;}
	const std::string& hq_url() const {return !data_?STR_EMPTY:data_->hq_url_;}
	const int32 music_time() const {return !data_?0:data_->music_time_;}

	bool SerializedJson(std::string& json);
 	bool UnserializedJson(std::string& str);
// 
// 	bool SerializedXml(std::string& xml);
// 	bool UnserializedXml(std::string& str);
private:
	class Data{
	public:
		Data():refcount_(1){}
		Data(const std::string id,const std::string& sid,
			const std::string& ssid,const std::string& album_title,
			const std::string& title,const std::string& hq_url,const std::string& pub_time,
			const std::string& artist,const std::string& pic_url,const std::string& url,
			int32 time)
			:id_(id)
			,sid_(sid)
			,ssid_(ssid)
			,album_title_(album_title)
			,titile_(title)
			,hq_url_(hq_url)
			,pub_time_(pub_time)
			,artist_(artist)
			,pic_url_(pic_url)
			,url_(url)
			,music_time_(time)
			,refcount_(1){}
		void AddRef(){refcount_++;}
		void Release(){if(!--refcount_) delete this;}
		std::string id_;
		std::string sid_;
		std::string ssid_;
		std::string album_title_;
		std::string titile_;
		std::string url_;
		std::string hq_url_;
		std::string pub_time_;
		std::string artist_;
		std::string pic_url_;
		int32       music_time_;
	private:
		int refcount_;
	};

	Data*           data_;
};


class WordAttrInfo{
public:
	explicit WordAttrInfo();
	explicit WordAttrInfo(const std::string& word_id,const std::string& name);

	~WordAttrInfo(){
		if(data_!=NULL){
			data_->Release();
		}
	}

	WordAttrInfo(const WordAttrInfo& mi);
	WordAttrInfo& operator=(const WordAttrInfo& mi);

	const std::string& id(){return data_->word_id_;}
	const std::string& name() {return data_->name_;}

private:
	class Data{
	public:
		Data():refcount_(1){}
		Data(const std::string& word_id,const std::string& name)
			:word_id_(word_id)
			,name_(name)
			,refcount_(1){}
		void AddRef(){refcount_++;}
		void Release() {if(!--refcount_) delete this;}
		const std::string word_id_;
		const std::string name_;
	private:
		int refcount_;
	};
	Data*    data_;
};

class ChannelInfo{
public:
	explicit ChannelInfo();
	explicit ChannelInfo(const std::string& index,
		const std::string& douban_index, 
		const std::string& channel_name,
		const std::string& channel_dec,
		const std::string pic = "http://fm.miglab.com");

	~ChannelInfo(){
		if(data_!=NULL){
			data_->Release();
		}
	} 

	ChannelInfo(const ChannelInfo& mi);
	ChannelInfo& operator=(const ChannelInfo& mi);

	const std::string& index() const {return data_->index_;}
	const std::string& douban_index() const {return data_->douban_index_;}
	const std::string& channel_name() const {return data_->channel_name_;}
	const std::string& channel_pic() const {return data_->channel_pic_;}
	const std::string& channel_dec() const {return data_->channel_dec_;}

private:
	class Data{
	public:
		Data():refcount_(1){}
		Data(const std::string& index,const std::string& douban_index,
			const std::string& channel_name,const std::string pic,
			const std::string& dec)
			:index_(index)
			,douban_index_(douban_index)
			,channel_name_(channel_name)
			,channel_pic_(pic)
			,channel_dec_(dec)
			,refcount_(1){}
		void AddRef(){refcount_++;}
		void Release() {if(!--refcount_) delete this;}
		const std::string index_;
		const std::string douban_index_;
		const std::string channel_name_;
		const std::string channel_pic_;
		const std::string channel_dec_;
	private:
		int refcount_;
	};

    Data*    data_;
};

class ConnAddr{
public:
    explicit ConnAddr();
    explicit ConnAddr(const std::string& host,const int port,
    				const std::string& usr="",const std::string& pwd="",const std::string& source="");

    ConnAddr(const ConnAddr& ca);
	ConnAddr& operator=(const ConnAddr& ca);
	
    ~ConnAddr(){
        if(data_!=NULL){
            data_->Release();
        }
    } 
    
    const std::string& host() const {return !data_?STR_EMPTY:data_->host_;}
    const int port() const {return !data_?0:data_->port_;}
    const std::string& usr() {return !data_?STR_EMPTY:data_->usr_;}
    const std::string& pwd() {return !data_?STR_EMPTY:data_->pwd_;}   
    const std::string& source() {return !data_?STR_EMPTY:data_->source_;} 	

private:
   class Data{
    public:
	Data():refcount_(1),port_(0){}
	Data(const std::string& host,const int port,const std::string& usr,const std::string& pwd,const std::string& source)
	   :host_(host)
	   ,port_(port)
	   ,usr_(usr)
	   ,pwd_(pwd)
	   ,source_(source)
       ,refcount_(1){}
    void AddRef(){refcount_++;}
    void Release() {if(!--refcount_) delete this;}
    const std::string host_;
    const int port_;
    const std::string usr_;
    const std::string pwd_;
    const std::string source_;
    private:
	int refcount_;
    };
    
    Data*    data_;
};


class CompareInfo{
public:
	explicit CompareInfo();
	explicit CompareInfo(const std::string& info_id,const std::string& info_index);
	CompareInfo(const CompareInfo& ci);
	CompareInfo& operator=(const CompareInfo& ci);
	~CompareInfo(){
		if(data_!=NULL){
			data_->Release();
		}
	}

	void set_info_num(const int32 info_num){data_->info_num_ = info_num;}
	void set_info_id(const std::string info_id){data_->info_id_ = info_id;}
	void set_info_index(const std::string info_index){data_->info_index_ = info_index;}

	const std::string& info_id() const {return data_->info_id_;}
	const std::string& info_index() const {return data_->info_index_;}
	const int32 info_num() const {return data_->info_num_;}
private:
	class Data{
	public:
		Data():refcount_(1){}
		Data(const std::string& info_id,const std::string& info_index)
			:info_id_(info_id)
			,info_index_(info_index)
			,refcount_(1){}
		void AddRef(){refcount_++;}
		void Release(){if(!--refcount_) delete this;}
		std::string info_id_;
		std::string info_index_;
		int info_num_;
	private:
		int refcount_;
	};

	Data*      data_;
};

class RecordingLocalMusic{
public:
	explicit RecordingLocalMusic();
	explicit RecordingLocalMusic(const std::string& name,const std::string& singer);
	RecordingLocalMusic(const RecordingLocalMusic& rlm);
	RecordingLocalMusic& operator=(const RecordingLocalMusic& rlm);
	~RecordingLocalMusic(){
		if(data_!=NULL){
			data_->Release();
		}
	}

	const std::string name() const {return data_->name_;}
	const std::string singer() const {return data_->singer_;}

private:
	class Data{
	public:
		Data():refcount_(1){}
		Data(const std::string& name,const std::string& singer)
			:name_(name)
			,singer_(singer)
			,refcount_(1){}
		void AddRef(){refcount_++;}
		void Release(){if(!--refcount_) delete this;}
		std::string name_;
		std::string singer_;
		int info_num_;
	private:
		int refcount_;
	};

	Data*      data_;
};

}

#endif
