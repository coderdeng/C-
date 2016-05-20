#include "asyncService.h"
#include <fstream>

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
	std::ofstream debug("debugStart.log");
	//clients.size();
	debug << "start begin" << std::endl; 
	//clients.insert(std::make_pair(_username, shared_from_this()));
	///考虑是否需要超时控制
	debug << "start end" << std::endl;
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
void talk_to_client::do_login(const std::string &msg)
{
	if (!_started)return;
	std::string password;
	std::ifstream ifs("userInf.u");
	std::map<std::string, std::string> user_psw;
	load_user_inf(user_psw);
	std::istringstream iss(msg);
	iss >> _username >> _username >> password;
	///检查是否存在
	if (user_psw.find(_username) == user_psw.end()){
		do_write(requests.request_login + "false no this user\n");
		return;
	}
	///检查密码是否真确
	else if (user_psw.find(_username)->second != password){
		do_write(requests.request_login + "false psw error");
		return;
	}
	else{
		clients.insert(std::make_pair(_username, shared_from_this()));
		for (std::map<std::string, client_ptr>::iterator it = clients.begin();
			it != clients.end(); ++it){
			it->second->set_chaged_clients();
		}
		do_write(requests.request_login + "succeed\n");
	}
}
//处理注册请求
void talk_to_client::do_register(const std::string &msg)
{
	if (!_started)return;
	//if 确保没有相同用户
	std::map<std::string, std::string> user_psw;
	std::istringstream iss(msg);
	std::string username, password;
	iss >> username >> username >> password;
	load_user_inf(user_psw);
	if(user_psw.find(username) != user_psw.end()){
		do_write(requests.request_register + "false\n");
		return;
	}
	std::ofstream ofs("userInf.u", std::ostream::app);
	ofs << username << " " << password << std::endl;
	ofs.close();
	do_write(requests.request_register + "succeed\n");
}
//处理ping请求
void talk_to_client::do_ping()
{
	if (!_started)return;
	if (_clients_change) {
		_clients_change = false;
		do_write(requests.request_ping + "clients changed\n");
	}
	else
		do_write(requests.request_ping + "clients not changed\n");
}
//处理ask for clients 请求
void talk_to_client::do_anw_clients()
{
	if (!_started)return;
	std::string answer(requests.request_ask);
	for (std::map<std::string, client_ptr>::iterator it = clients.begin();
		it != clients.end(); ++it) {
		answer += it->first + " ";
	}
	if (clients.empty()){
		do_write(requests.request_ask + "谭伟丰 陈明浩 陈伦宝 \n");
	}
	answer += "\n";

	do_write(answer);
}
//处理发送信息请求
void talk_to_client::do_sendto(const std::string &msg)
{
	std::istringstream iss(msg);
	std::string to_user;
	iss >> to_user >> to_user >> to_user;
	///如果不包含
	if (clients.find(to_user) == clients.end()) {
		do_write(requests.request_send_to + "false\n");
		return;
	}
	size_t pos = requests.request_send_to.size() + to_user.size() + 1;
	std::string send_msg(msg, pos);
	if (to_user == _username){
		do_write("you can send message to youself\n");
	}
	else {
		clients.at(to_user)->do_write(requests.request_send_from + _username + " " + send_msg + "\n");
		do_write(requests.request_send_to + "succeed\n");
	}
}
//执行退出
void talk_to_client::do_exit()
{
	clients.erase(_username);
	_clients_change = true;
	do_write(requests.request_exit + "succeed\n");
}
//执行写入buffer
void talk_to_client::do_write(const std::string &msg)
{
	std::ofstream talk_to("talk_to_clients.log", std::ostream::app);
	if (!started())return;
	talk_to << msg << std::endl;
	_sock.async_write_some(buffer(msg.c_str(), msg.size()), MEN_FN2(on_write, _1, _2));
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
	std::ofstream debug("debugRead.log", std::ostream::app);
	debug << "client say:" + msg << std::endl;
	do_analyze(msg);
}
//分析message函数， on_read之后调用
void talk_to_client::do_analyze(const std::string &msg)
{
	if (msg.find(requests.request_register) == 0)do_register(msg);
	else if (msg.find(requests.request_login) == 0)do_login(msg);
	else if (msg.find(requests.request_ping) == 0)do_ping();
	else if (msg.find(requests.request_ask) == 0)do_anw_clients();
	else if (msg.find(requests.request_send_to) == 0)do_sendto(msg);
	else if (msg.find(requests.request_exit) == 0)do_exit();
	else
		return;
}
//载入用户信息
void talk_to_client::load_user_inf(std::map<std::string, std::string> &_user_psw)
{
	std::ifstream ifs("userInf.u");
	std::string username, password;
	while(ifs >> username >> password){
		_user_psw.insert(std::make_pair(username, password));
	}
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
