#ifndef _HTTP_NET_COMM_INTER_FACE_H__
#define _HTTP_NET_COMM_INTER_FACE_H__

#define NET_Export _declspec(dllexport) __stdcall

extern "C" int NET_Export MigGetHttpTest();
extern "C" int NET_Export MigInit(std::string& host,std::string& entry);
extern "C" int NET_Export MigUsrLogin(std::string& username,std::string& passwd,std::string& host);
#endif