/*
 * http_comm.cc
 *
 *  Created on: 2013-8-11
 *      Author: huaiyu
 */

#include "http_comm.h"
#include "http_response.h"
#include "logic_comm.h"
#include "json/json.h"
#include <sstream>

namespace mig_sociality {

static const std::string URL_PUSH_SERVICE("121.199.32.88:9090/api/push");

bool mig_sociality::HttpComm::PushMessage(const std::string &device_token,
		const std::string &msg, int badge/*=1*/, const std::string &sound/*=""*/) {
	if (device_token.empty())
		return false;

	HttpPost post(URL_PUSH_SERVICE);
	std::stringstream post_cont;
	post_cont << "devicetoken=" << device_token;
	post_cont << "&message=" << msg;
	post_cont << "&badge=" << badge;
	if (sound.empty())
		post_cont << "&sound=" << sound;
	post_cont << std::ends;

	std::string post_str(post_cont.str());

	LOG_DEBUG2("Push msg post:%s", post_str.c_str());

	try {
		post.Post(post_str.c_str());
		std::string result;
		post.GetContent(result);

		Json::Reader rd;
		Json::Value value;
		if (!rd.parse(result, value)) {
			LOG_DEBUG("Push msg failed: 返回值解析失败");
			return false;
		}

		int ret_code = value["status"].asInt();
		if (1 != ret_code) {
			LOG_DEBUG2("Push msg failed:推送失败,%d", ret_code);
			return false;
		}
	} catch (const std::exception &ex) {
		LOG_DEBUG2("Push msg failed:%s", ex.what());
		return false;
	}catch (...) {
		LOG_DEBUG("Push msg failed");
		return false;
	}

	return true;
}

} /* namespace mig_sociality */
