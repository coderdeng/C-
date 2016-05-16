#include "asyncService.h"


io_service service;
client_map clients;

///public func
//创建新的客户端虚拟socket连接
talk_to_client::self_ptr talk_to_client::create()
{
	self_ptr _new(new talk_to_client());
	return _new;
}
//开始运行socket
void talk_to_client::start()
{
	_started = true;
	clients->insert(std::make_pair(_username, shared_from_this()));
	///考虑是否需要超时控制
	do_read();
}
//停止socket
void talk_to_client::stop()
{
	_started = false;
	_sock.close();
}
//读取请求信息
void talk_to_client::do_read()
{
	async_read(_sock, buffer(_read_buff, buff_size), 
		MEN_FN2(read_complete, _1, _2), MEN_FN2(on_read, _1, _2));
}
//处理登录请求
void talk_to_client::do_login()
{
	if (!_started)return;
	//if 密码正确
	do_write("login succeed\n");
}
//处理注册请求
void talk_to_client::do_register()
{
	if (!_started)return;
	//if 确保没有相同用户 没实现
	do_write("register succeed\n");
}
//处理ping请求
void talk_to_client::do_ping()
{
	if (!_started)return;
	if (_clients_change) {
		do_write("ping clients changed\n");
	}
	else
		do_write("ping clients not changed\n");
}
//处理ask for clients 请求
void talk_to_client::do_anw_clients()
{
	if (!_started)return;
	std::string answer("ask for clients");
	for (std::map<std::string, client_ptr>::iterator it = clients->begin();
		it != clients->end(); ++it) {
		answer += " " + it->first;
	}
	answer += "\n";

	do_write(answer);
}
//处理发送信息请求
void talk_to_client::do_sendto(const std::string &msg)
{
	std::istringstream iss(msg);
	std::string to_user;
	iss >> to_user >> to_user;
	///如果不包含 目标客户
	if (clients->find(to_user) == clients->end()) {
		do_write("sned to false\n");
		return;
	}
	//测试iss是否不包括前面内容
	clients->at(to_user)->do_write("send from " + _username + " " + iss.str());
	do_write("send to succeed\n");
}
//执行写入buffer
void talk_to_client::do_write(const std::string &msg)
{
	if (!started())return;
	_sock.async_write_some(buffer(msg, msg.size()), MEN_FN2(on_write, _1, _2));
}
//析构函数
talk_to_client::~talk_to_client()
{
	delete _write_buff;
	delete _read_buff;
}
///private func
//构造函数
talk_to_client::talk_to_client() :
	_sock(service),
	_username(),
	_clients_change(false),
	_read_buff(new char[buff_size]),
	_write_buff(new char[buff_size])
{

}
//读取控制函数
size_t talk_to_client::read_complete(const error_code &err, size_t bytes)
{
	if (err)return 0;
	bool found = std::find(_read_buff, _read_buff + bytes, '\n') < _read_buff + bytes;
	return found ? 0 : 1;
}
//读取完成函数
void talk_to_client::on_read(const error_code &err, size_t bytes)
{
	if (err)return;
	std::string msg(_read_buff, bytes);
	do_analyze(msg);
}
//分析message函数， on_read之后调用
void talk_to_client::do_analyze(const std::string &msg)
{
	if (msg.find("register") == 0)do_register();
	else if (msg.find("login") == 0)do_login();
	else if (msg.find("ping") == 0)do_ping();
	else if (msg.find("ask for clients") == 0)do_anw_clients();
	else if (msg.find("send to") == 0)do_sendto(msg);
	else
		return;
}
//登录完成函数
void talk_to_client::on_login(const std::string &msg)
{

}
//注册完成函数
void talk_to_client::on_register(const std::string &msg)
{

}
//ping完成函数
void talk_to_client::on_ping(const std::string &msg)
{

}
//ask 完成函数
void talk_to_client::on_anw_clients(const std::string &msg)
{

}
//发送完成函数
void talk_to_client::on_sendto(const std::string &msg)
{

}
//写入buffer完成函数
void talk_to_client::on_write(const error_code &err, size_t bytes)
{
	do_read();
}
