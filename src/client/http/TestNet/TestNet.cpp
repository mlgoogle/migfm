// TestNet.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <string>
#include "net_comm/interface.h"

int _tmain(int argc, _TCHAR* argv[])
{
	std::string name = "flaght@gmail.com";
	std::string password = "19860903";
	std::string host ="app.miglab.com";
	std::string entry = "miglab";
	MigInit(host,entry);
	MigUsrLogin(name,password,host);
	return 0;
}

