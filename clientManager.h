#pragma once
#include "asyncClient.h"

/*
 * ��������
 * �����ͻ������� ��
 */
class clientManager {
public:
	clientManager();
	~clientManager();
	boost::shared_ptr<talk_to_svr> getClient();
	std::vector<std::string> getOnlineUser();
	std::string checkGetMsg();

	bool do_register(const std::string &username, const std::string &password);
	bool do_login(const std::string &username, const std::string &password);
	bool do_sendto(const std::string &toUsername, const std::string &sendMsg);
	bool do_keep();
	bool do_exit();
private:
	void run_service();
	boost::shared_ptr<talk_to_svr> talk_svr;
};