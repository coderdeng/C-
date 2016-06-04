#include "clientManager.h"
#include <qdebug.h>
#define _SCL_SECURE_NOWARNINGS

extern io_service service;
///构造
clientManager::clientManager()
{
	ip::tcp::endpoint ep(ip::address::from_string("139.129.129.181"), 8000);
	talk_svr = talk_to_svr::create(ep);
	boost::thread(boost::bind(&clientManager::run_service, this));
}
///析构
clientManager::~clientManager()
{

}
///获取消息队列中队列头部信息
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
///获取客户端
boost::shared_ptr<talk_to_svr> clientManager::getClient()
{
	return talk_svr;
}
///获取在线用户
std::vector<std::string> clientManager::getOnlineUser()
{
	talk_svr->do_ping();
	return talk_svr->getOnlineUser();
}
///执行注册功能
bool clientManager::do_register(const std::string &username, const std::string &password)
{
	talk_svr->do_register(username, password);
	boost::this_thread::sleep(boost::posix_time::seconds(2));
	if (talk_svr->getSignal() == ok_register)
		return true;
	else
		return false;
}
///执行登录功能
bool clientManager::do_login(const std::string &username, const std::string &password)
{
	talk_svr->do_login(username, password);
	boost::this_thread::sleep(boost::posix_time::seconds(2));
	if (talk_svr->getSignal() == ok_login)
		return true;
	else
		return false;
}
///执行发送功能
bool clientManager::do_sendto(const std::string &toUsername, const std::string &sendMsg)
{
	talk_svr->do_sendto(toUsername, sendMsg);
	boost::this_thread::sleep(boost::posix_time::seconds(2));
	if (talk_svr->getSignal() == ok_login)
		return true;
	else
		return false;
}
///保持活动
bool clientManager::do_keep()
{
	talk_svr->do_ping();
	boost::this_thread::sleep(boost::posix_time::seconds(2));
	if (talk_svr->getSignal() == ok_login)
		return true;
	else
		return false;
}
///退出
bool clientManager::do_exit()
{
	talk_svr->do_exit();
	boost::this_thread::sleep(boost::posix_time::seconds(2));
	if (talk_svr->getSignal() == ok_exit)
		return true;
	else
		return false;
}
///运行service
void clientManager::run_service()
{
	service.reset();
	service.run();
	qDebug("over");
}