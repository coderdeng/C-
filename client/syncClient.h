#ifndef __SYNCCLIENT_H__
#define __SYNCCLIENT_H__

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <string>

using namespace boost::asio;

class talk_to_svr
{
public:
	talk_to_svr(std::string&);
	//连接
	void connect(ip::tcp::endpoint);
	//循环
	void loop();
	//发送请求
	void write_request();
	//读取回应
	void read_answer();
	//解析信息
	void process_msg();
	//登录请求-回调
	void on_login();
	//ping-回调
	void on_ping(const std::string &);
	//client-回调
	void on_client(const std::string &);
	//执行发送
	void do_ask_client();
	//执行发送
	void write(const std::string &);
	//读取完成函数
	size_t read_complete(const boost::system::error_code &, size_t);
	//获取用户名
	std::string username(){ return username_; }


private:
	ip::tcp::socket sock_;
	bool started_;
	std::string username_;
	enum { max_msg = 1024 };
	int already_read_;
	char buff_[max_msg];
};


#endif