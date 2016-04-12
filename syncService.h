#ifndef __SYNCSERVICE_H__
#define __SYNCSERVICE_H__

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost::asio;

class talk_to_client : public boost::enable_shared_from_this<talk_to_client>
{
public:
	//构造
	talk_to_client();
	//获取用户名
	std::string username()const { return username_; }
	//回应客户
	void answer_to_client();
	//设置用户改变
	void set_clients_changed() { clients_changed_ = true; };
	//获取socket
	ip::tcp::socket& sock() { return sock_; }
	//超时
	bool timed_out() const;
	//关闭sock
	void stop();
	//读取ping请求
	void read_request();
	//解析信息
	void process_request();
	//登录回应
	void on_login(const std::string &);
	//ping回应
	void on_ping();
	//clients回应
	void on_clients();
	//发送信息
	void write(const std::string &);

private:
	ip::tcp::socket sock_;
	std::string username_;
	int already_read_;
	bool stared_;
	enum { max_msg = 1024 };
	char buff_[max_msg];
	bool clients_changed_;
	boost::posix_time::ptime last_ping;
};

typedef boost::shared_ptr<talk_to_client> client_ptr;
typedef std::vector<client_ptr> client_array;

#endif