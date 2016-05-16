#pragma once
#define _SCL_SECURE_NO_WARNINGS
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/signals2.hpp>
#include <string>
#include <sstream>
#include <iostream>

///定义绑定函数
#define MEM_FN(x)			boost::bind(&self_type::x, shared_from_this())
#define MEM_FN1(x, y)		boost::bind(&self_type::x, shared_from_this(), y)
#define MEM_FN2(x, y, z)	boost::bind(&self_type::x, shared_from_this(), y, z)

using namespace boost::asio;

enum clientSignal {
	ok_connect,
	ok_login,
	ok_register,
	ok_sendto,
	ok_exit,
	other_sig
};

/*
 * class
 * client
 * autor deng
 */
class talk_to_svr : public boost::enable_shared_from_this<talk_to_svr>, boost::noncopyable
{
public:
	using self_type = talk_to_svr;
	using error_code = boost::system::error_code;
	using self_ptr = boost::shared_ptr<talk_to_svr>;

	~talk_to_svr();
	static self_ptr create(ip::tcp::endpoint ep);
	void stop();
	bool started() { return _started; }
	clientSignal getSignal() { return _csignal; }
	std::vector<std::string> getOnlineUser() { return _online_users; }
	std::deque<std::string> &getMsgQue() { return _msg_que; }

	void do_register(const std::string &username, const std::string &password);
	void do_login(const std::string &username, const std::string &password);
	void do_ping();
	void do_ask_clients();
	void do_sendto(const std::string &user, const std::string &msg);
	void do_write(const std::string &msg);
	void do_read();
	void do_exit();

private:///方法
	talk_to_svr();
	void connect(ip::tcp::endpoint ep);
	size_t read_complete(const error_code &err, size_t bytes);
	void on_connect(const error_code &err);
	void on_read(const error_code &err, size_t bytes);
	void analyze_answer(std::string &msg);

	void on_register(std::string &msg);
	void on_login(std::string &msg);
	void on_ping(std::string &msg);
	void on_ask_client(std::string &msg);
	void on_sendto(std::string &msg); 
	void on_write(const error_code &err, size_t bytes);
	void on_exit(const std::string &msg);
	void on_sendfrom(const std::string &msg);

private:///成员
	enum { buff_size = 2048 };
	ip::tcp::socket _sock;
	bool _started;
	char *_read_buff;
	char *_write_buff;
	size_t _alread_read;
	std::string _username;
	std::string _password;
	std::vector<std::string> _online_users;
	std::deque<std::string> _msg_que;
	clientSignal _csignal;
};