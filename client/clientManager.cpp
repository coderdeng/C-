#include "clientManager.h"
#include <qdebug.h>
#define _SCL_SECURE_NOWARNINGS

extern io_service service;
///����
clientManager::clientManager()
{
	ip::tcp::endpoint ep(ip::address::from_string("139.129.129.181"), 8000);
	talk_svr = talk_to_svr::create(ep);
	boost::thread(boost::bind(&clientManager::run_service, this));
}
///����
clientManager::~clientManager()
{

}
///��ȡ��Ϣ�����ж���ͷ����Ϣ
std::string clientManager::checkGetMsg()
{
	std::string msg;
	if (!(talk_svr->getMsgQue().empty())) {
		msg = talk_svr->getMsgQue().front();
		talk_svr->getMsgQue().pop_front();
	}
	//qDebug(std::string("in CheckGetMsg :" + msg).c_str());
	return msg;
}
///��ȡ�ͻ���
boost::shared_ptr<talk_to_svr> clientManager::getClient()
{
	return talk_svr;
}
///��ȡ�����û�
std::vector<std::string> clientManager::getOnlineUser()
{
	talk_svr->do_ping();
	return talk_svr->getOnlineUser();
}
///ִ��ע�Ṧ��
bool clientManager::do_register(const std::string &username, const std::string &password)
{
	talk_svr->do_register(username, password);
	boost::this_thread::sleep(boost::posix_time::seconds(2));
	if (talk_svr->getSignal() == ok_register)
		return true;
	else
		return false;
}
///ִ�е�¼����
bool clientManager::do_login(const std::string &username, const std::string &password)
{
	talk_svr->do_login(username, password);
	boost::this_thread::sleep(boost::posix_time::seconds(2));
	if (talk_svr->getSignal() == ok_login)
		return true;
	else
		return false;
}
///ִ�з��͹���
bool clientManager::do_sendto(const std::string &toUsername, const std::string &sendMsg)
{
	talk_svr->do_sendto(toUsername, sendMsg);
	boost::this_thread::sleep(boost::posix_time::seconds(2));
	if (talk_svr->getSignal() == ok_login)
		return true;
	else
		return false;
}
///���ֻ
bool clientManager::do_keep()
{
	talk_svr->do_ping();
	boost::this_thread::sleep(boost::posix_time::seconds(2));
	if (talk_svr->getSignal() == ok_login)
		return true;
	else
		return false;
}
///�˳�
bool clientManager::do_exit()
{
	talk_svr->do_exit();
	boost::this_thread::sleep(boost::posix_time::seconds(2));
	if (talk_svr->getSignal() == ok_exit)
		return true;
	else
		return false;
}
///����service
void clientManager::run_service()
{
	service.reset();
	service.run();
	qDebug("over");
}