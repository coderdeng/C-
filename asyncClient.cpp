#include "asyncClient.h"
#include <qdebug.h>
#include <qstring.h>

io_service service;

talk_to_svr::talk_to_svr() :
	_sock(service),
	_started(false),
	_read_buff(new char[buff_size]),
	_write_buff(new char[buff_size]),
	_alread_read(0),
	_username(""),
	_password(""),
	_csignal(other_sig)
{
	std::cout << "create" << std::endl;
}

talk_to_svr::~talk_to_svr()
{
	std::cout << "~create" << std::endl;
	delete _read_buff;
	delete _write_buff;
	_sock.close();
}
///创建新的客户端连接
talk_to_svr::self_ptr talk_to_svr::create(ip::tcp::endpoint ep)
{
	std::cout << "new" << std::endl;
	self_ptr _new(new talk_to_svr());
	_new->connect(ep);
	return _new;
}
///停止服务端
void talk_to_svr::stop()
{
	std::cout << "tcop" << std::endl;
	if (!_started)return;
	_started = false;
	_sock.close();
}
///退出服务器
void talk_to_svr::do_exit()
{
	do_write("exit\n");
}
///注册，发送注册请求给服务器
void talk_to_svr::do_register(const std::string &username, const std::string &password)
{
	do_write("register " + username + " " + password + "\n");
}
///登录，发送登陆请求给服务器
void talk_to_svr::do_login(const std::string &username, const std::string &password)
{
	_username = username;
	_password = password;
	std::string writeMsg("login " + username + " " + password + "\n");
	service.post(MEM_FN1(do_write, writeMsg));
}
///ping，发送ping请求给服务器
void talk_to_svr::do_ping()
{
	service.post(MEM_FN1(do_write, "ping\n"));
}
///ask for clients 亲求客户端信息
void talk_to_svr::do_ask_clients()
{
	service.post(MEM_FN1(do_write, "ask for clients\n"));
}
///sendto 发送信息到目的用户端
void talk_to_svr::do_sendto(const std::string &username, const std::string &msg)
{
	//username 和 msg 赋值
	std::string writeMsg("send to " + username + " " + msg + "\n");
	service.post(MEM_FN1(do_write, writeMsg));
}
///写数据到缓存区
void talk_to_svr::do_write(const std::string &msg)
{
	if (!started()) return;
	qDebug(std::string("write " + msg).c_str());
	_sock.async_write_some(buffer(msg.c_str(), msg.size()), MEM_FN2(on_write, _1, _2));
}
///异步读取
void talk_to_svr::do_read()
{
	qDebug("reading....");
	async_read(_sock, buffer(_read_buff, buff_size), MEM_FN2(read_complete, _1, _2), MEM_FN2(on_read, _1, _2));
}
///private func
///connect
void talk_to_svr::connect(ip::tcp::endpoint ep)
{
	_started = true;
	_sock.async_connect(ep, MEM_FN1(on_connect, _1));
}
///读取控制函数每次读取bytes位，‘\n’结束
size_t talk_to_svr::read_complete(const error_code &err, size_t bytes)
{
	if (err)return 0;
	bool found = std::find(_read_buff, _read_buff + bytes, '\n') < _read_buff + bytes;
	return (found ? 0 : 1);
}
///链接完成函数
void talk_to_svr::on_connect(const error_code &err)
{
	if (!err) {
		_csignal = ok_connect;
		do_read();
	}
	else
		return;
}
///读取完成函数
void talk_to_svr::on_read(const error_code &err, size_t bytes)
{
	if (!started())return;
	if (err) {
		std::cout << err.message() << std::endl;
		return;
	}
	//deal message
	qDebug("%d", bytes);
	std::string msg(_read_buff, bytes);
	qDebug(std::string("svr answer: " + msg).c_str());
	analyze_answer(msg);
	do_read();
}
///分析服务器回应
void talk_to_svr::analyze_answer(std::string &msg)
{
	if (msg.find("register") == 0)on_register(msg);
	else if (msg.find("login") == 0)on_login(msg);
	else if (msg.find("ping") == 0)on_ping(msg);
	else if (msg.find("ask for clients") == 0)on_ask_client(msg);
	else if (msg.find("send to") == 0)on_sendto(msg);
	else if (msg.find("send from") == 0)on_sendfrom(msg);
	else {
		//funcSelect();
		return; /// 处理错误信息 error answer
	}
}
///登陆回应处理 on_ 这部分代码改为成功之后将状态改写，提供接口给外部获得
void talk_to_svr::on_login(std::string &msg)
{
	std::string answer;
	std::istringstream iss(msg);
	iss >> answer >> answer;
	if (answer.find("succeed") == 0) {
		_csignal = ok_login;
		do_ping();
	}
	else /// 重新登录
		return;
}
///注册回应处理
void talk_to_svr::on_register(std::string &msg)
{
	std::string answer;
	std::istringstream iss(msg);
	iss >> answer >> answer;
	if (answer.find("succeed") == 0) {
		_csignal = ok_register;
	}
	else ///重新注册
		return;
}
///ping回应处理
void talk_to_svr::on_ping(std::string &msg)
{
	std::string answer;
	std::istringstream iss(msg);
	iss >> answer >> answer >> answer;
	qDebug(answer.c_str());
	if (answer.find("changed") == 0) {
		do_ask_clients();
	}
	else ///do nothing
		return;
}
///获取在线用户回应处理
void talk_to_svr::on_ask_client(std::string &msg)
{
	_online_users.clear();
	std::string answer;
	std::istringstream iss(msg);
	iss >> answer >> answer >>answer;
	while (iss >> answer) {
		_online_users.push_back(answer);
	}
	_csignal = ok_login;
}
///发送回应处理
void talk_to_svr::on_sendto(std::string &msg)
{
	std::string answer;
	std::istringstream iss(msg);
	iss >> answer >> answer;
	if (answer.find("succeed") == 0) {
		_csignal = ok_sendto; /// 消息发送成功
	}
	else ///消息发送失败
		return;
}
///接收信息
void talk_to_svr::on_sendfrom(const std::string &msg)
{
	std::string message;
	std::istringstream iss(msg);
	iss >> message >> message >> message;
	message += " say:";
	std::string tmp;
	while (iss >> tmp) {
		message += tmp + " ";
	}
	_msg_que.push_back(message);
}
///退出
void talk_to_svr::on_exit(const std::string &msg)
{
	std::string answer;
	std::istringstream iss(msg);
	iss >> answer >> answer;
	if (answer.find("succeed") == 0) {
		stop();
		service.stop();
		_csignal = ok_exit;
	}
}
///写入回应处理
void talk_to_svr::on_write(const error_code &err, size_t bytes)
{
	///写完后异步读取内容
	qDebug("write ok");
	//do_read();
}