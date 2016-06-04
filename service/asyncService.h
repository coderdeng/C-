#pragma once
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <string>
#include <sstream>
#include <map>
#include <iostream>

#define MEN_FN(x)			boost::bind(&self_type::x, shared_from_this())
#define MEN_FN1(x, y)		boost::bind(&self_type::x, shared_from_this(), y)
#define MEN_FN2(x, y, z)	boost::bind(&self_type::x, shared_from_this(), y, z)

using namespace boost::asio;

/*
 *  请求
 */
struct Requests {
	const std::string request_register = "register ";
	const std::string request_login = "login ";
	const std::string request_ping = "ping ";
	const std::string request_ask = "ask for clients ";
	const std::string request_send_to = "send to ";
	const std::string request_send_from = "send from ";
	const std::string request_exit = "exit ";
};

/*
 * class
 * service
 * autor deng
 */
class talk_to_client : public boost::enable_shared_from_this<talk_to_client>, boost::noncopyable
{
public:
	using self_type = talk_to_client;
	using error_code = boost::system::error_code;
	using self_ptr = boost::shared_ptr<talk_to_client>;
	~talk_to_client();

	static self_ptr create();
	void start();
	void stop();
	bool started() const { return _started; }
	ip::tcp::socket &sock() { return _sock; }
	std::string get_user() const { return _username; }
	void set_chaged_clients() { _clients_change = true; }

	void do_read();
	void do_login(const std::string &msg);
	void do_register(const std::string &msg);
	void do_ping();
	void do_anw_clients();
	void do_sendto(const std::string &msg);
	void do_write(const std::string &msg);
	void do_send(const std::string &msg);
	void do_exit();

private: ///函数操作
	talk_to_client();
	size_t read_complete(const error_code &err, size_t bytes);

	void on_read(const error_code &err, size_t bytes);
	void on_ping(const std::string &msg);
	void on_anw_clients(const std::string &msg);
	void on_sendto(const std::string &msg);
	void on_write(const error_code &err, size_t bytes);
	void do_analyze(const std::string &msg);
	void on_send(const error_code &err, size_t bytes);
	
	void load_user_inf(std::map<std::string, std::string> &_user_psw);

private: ///元素
	Requests requests;
	ip::tcp::socket _sock;
	std::string _username;
	bool _started;
	bool _clients_change;
	enum { buff_size = 2048 };
	char *_read_buff;
	char *_write_buff;
	//boost::shared_ptr<manager> _manager;

};

typedef boost::shared_ptr<talk_to_client>client_ptr;
typedef std::map<std::string, client_ptr> client_map;
typedef boost::system::error_code serror_code;
