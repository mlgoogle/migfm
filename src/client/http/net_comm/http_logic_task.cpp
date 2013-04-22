#include "http_logic_task.h"

#include "base\string_number_conversions.h"
#include "xmpp\xmlelement.h"

#include "http_engine_impl.h"
#include "message.h"
#include "message_factory.h"
#include "wrapper_http.h"
#include "mig_http_module_impl.h"
#include "message_dispatcher.h"

namespace net_comm {

HttpLogicTask::HttpLogicTask(HttpEngineImpl* pctx)
	: pctx_(pctx)
	, state_(LOGICSTATE_INIT)
	, pelStanza_(NULL)
	, isStart_(false)
{
}

HttpLogicTask::~HttpLogicTask(void)
{
}

void HttpLogicTask::IncomingStanza( const base::XmlElement * element, bool isStart )
{
	pelStanza_ = element;
	isStart_ = isStart;
	Advance();
	pelStanza_ = NULL;
	isStart_ = false;
}

void HttpLogicTask::Advance()
{
	for ( ; ; ) {
		switch (state_) {
		case LOGICSTATE_GET_USER_INFO:
			if (!isStart_) {
				OnResultGetUserInfo(pelStanza_);
				return;
			}
			break;
		}
	}
}

void HttpLogicTask::GetUserInfo( const std::string &user_name )
{
	// 	string url, result;
	// 	int32 code = 0;
	// 	url = "http://42.121.112.248/cgi-bin/show.fcgi?";
	// 	url += "username=";
	// 	url += GetUser().username();
	// 	url += "&token=";
	// 	url += token_;
	// 	output_handler_->WriteOutput(url, result, code);
	// 
	// 	stanzaParser_.Reset();
	// 	stanzaParser_.Parse(result.c_str(),result.length(),true);

	HttpEngineImpl::EnterExit ee(pctx_);

	std::stringstream &output = pctx_->output();
	std::stringstream &output_post = pctx_->output_post();

	state_ = LOGICSTATE_GET_USER_INFO;

	output << "http://42.121.112.248/cgi-bin/show.fcgi?"
		<< "username=" << user_name;

	pctx_->SetHttpType(HttpEngineImpl::HTTP_GET_TYPE);
}

void HttpLogicTask::OnResultGetUserInfo(const base::XmlElement *xml)
{
	using namespace base;
	using namespace message;

	static const std::string NAMESPACE = "vcard-temp";
	static const QName QN_UINFO_ID(NAMESPACE, "id");
	static const QName QN_UINFO_SEX(NAMESPACE, "sex");
	static const QName QN_UINFO_ADDRESS(NAMESPACE, "address");
	static const QName QN_UINFO_HEAD(NAMESPACE, "head");
	static const QName QN_UINFO_BIRTH(NAMESPACE, "birthday");
	static const QName QN_UINFO_NICK(NAMESPACE, "nickname");

	MessageLogin_Result *login_res = new MessageLogin_Result();

	const XmlElement *elem = NULL;
	int int_val = 0;

	if (elem = xml->FirstNamed(QN_UINFO_ID))
		StringToInt64(elem->BodyText(), &login_res->user_info.user_id);
	
	if (elem = xml->FirstNamed(QN_UINFO_SEX)) {
		StringToInt(elem->BodyText(), &int_val);
		login_res->user_info.sex = (int8)int_val;
	}

	if (elem = xml->FirstNamed(QN_UINFO_ADDRESS)) {
		const XmlElement *elem1 = NULL;
		if (elem1 = elem->FirstNamed(QName(NAMESPACE, "extadd")))
			login_res->user_info.address.ext_address = elem1->BodyText();
		if (elem1 = elem->FirstNamed(QName(NAMESPACE, "street")))
			login_res->user_info.address.street = elem1->BodyText();
		if (elem1 = elem->FirstNamed(QName(NAMESPACE, "locality")))
			login_res->user_info.address.locality = elem1->BodyText();
		if (elem1 = elem->FirstNamed(QName(NAMESPACE, "regin")))
			login_res->user_info.address.regin = elem1->BodyText();
		if (elem1 = elem->FirstNamed(QName(NAMESPACE, "pcode")))
			login_res->user_info.address.post_code = elem1->BodyText();
		if (elem1 = elem->FirstNamed(QName(NAMESPACE, "ctry")))
			login_res->user_info.address.country = elem1->BodyText();
	}

	if (elem = xml->FirstNamed(QN_UINFO_HEAD))
		login_res->user_info.head_image = elem->BodyText();
	if (elem = xml->FirstNamed(QN_UINFO_BIRTH))
		login_res->user_info.birthday = elem->BodyText();
	if (elem = xml->FirstNamed(QN_UINFO_NICK))
		login_res->user_info.nick_name = elem->BodyText();

	WrapperHttp::GetInstance()->http_module()->GetMessageDispatcher()->PostMessage(login_res);
}

} // namespace net_comm