#ifndef _MASTER_PLUGIN_MANAGER_SPREAD_BASIC_INFOS_H__
#define _MASTER_PLUGIN_MANAGER_SPREAD_BASIC_INFOS_H__

#include <string>
namespace base{

class WeiXin{
public:
/*	class WeiXinMenuInfo{
		explicit WeiXinMenuInfo(const int64 id,const std::string& name,const std::string& key,
				const std::string& url);
		explicit WeiXinMenuInfo();
	};
*/

	class GraphicTextInfo{
	public:
		explicit GraphicTextInfo();
		explicit GraphicTextInfo(const std::string& title,const std::string& desction,
				const std::string& picurl,const std::string& url);

		GraphicTextInfo(const GraphicTextInfo& graphic_info);
		GraphicTextInfo& operator = (const GraphicTextInfo& graphic_info);

		const std::string& title() const {return data_->title_;}
		const std::string& desction() const {return data_->desction_;}
		const std::string& picurl() const {return data_->picurl_;}
		const std::string& url() const {return data_->url_;}

	private:
		class Data{
		public:
			Data():refcount_(1){};
			Data(const std::string& title,const std::string& desction,
					const std::string& picurl,const std::string& url):refcount_(1)
			,title_(title)
			,desction_(desction)
			,picurl_(picurl)
			,url_(url){};

			void AddRef(){refcount_ ++;}
			void Release(){if (!--refcount_)delete this;}
		public:
			const std::string title_;
			const std::string desction_;
			const std::string picurl_;
			const std::string url_;
		private:
			int                  refcount_;
		};

		Data*                    data_;
	};
};


}
#endif
