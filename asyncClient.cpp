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
///�����µĿͻ�������
talk_to_svr::self_ptr talk_to_svr::create(ip::tcp::endpoint ep)
{
	std::cout << "new" << std::endl;
	self_ptr _new(new talk_to_svr());
	_new->connect(ep);
	return _new;
}
///ֹͣ�����
void talk_to_svr::stop()
{
	std::cout << "tcop" << std::endl;
	if (!_started)return;
	_started = false;
	_sock.close();
}
///�˳�������
void talk_to_svr::do_exit()
{
	do_write("exit\n");
}
///ע�ᣬ����ע�������������
void talk_to_svr::do_register(const std::string &username, const std::string &password)
{
	do_write("register " + username + " " + password + "\n");
}
///��¼�����͵�½�����������
void talk_to_svr::do_login(const std::string &username, const std::string &password)
{
	_username = username;
	_password = password;
	std::string writeMsg("login " + username + " " + password + "\n");
	service.post(MEM_FN1(do_write, writeMsg));
}
///ping������ping�����������
void talk_to_svr::do_ping()
{
	service.post(MEM_FN1(do_write, "ping\n"));
}
///ask for clients ����ͻ�����Ϣ
void talk_to_svr::do_ask_clients()
{
	service.post(MEM_FN1(do_write, "ask for clients\n"));
}
///sendto ������Ϣ��Ŀ���û���
void talk_to_svr::do_sendto(const std::string &username, const std::string &msg)
{
	//username �� msg ��ֵ
	std::string writeMsg("send to " + username + " " + msg + "\n");
	service.post(MEM_FN1(do_write, writeMsg));
}
///д���ݵ�������
void talk_to_svr::do_write(const std::string &msg)
{
	if (!started()) return;
	qDebug(std::string("write " + msg).c_str());
	_sock.async_write_some(buffer(msg.c_str(), msg.size()), MEM_FN2(on_write, _1, _2));
}
///�첽��ȡ
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
///��ȡ���ƺ���ÿ�ζ�ȡbytesλ����\n������
size_t talk_to_svr::read_complete(const error_code &err, size_t bytes)
{
	if (err)return 0;
	bool found = std::find(_read_buff, _read_buff + bytes, '\n') < _read_buff + bytes;
	return (found ? 0 : 1);
}
///������ɺ���
void talk_to_svr::on_connect(const error_code &err)
{
	if (!err) {
		_csignal = ok_connect;
		do_read();
	}
	else
		return;
}
///��ȡ��ɺ���
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
///������������Ӧ
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
		return; /// ���������Ϣ error answer
	}
}
///��½��Ӧ���� on_ �ⲿ�ִ����Ϊ�ɹ�֮��״̬��д���ṩ�ӿڸ��ⲿ���
void talk_to_svr::on_login(std::string &msg)
{
	std::string answer;
	std::istringstream iss(msg);
	iss >> answer >> answer;
	if (answer.find("succeed") == 0) {
		_csignal = ok_login;
		do_ping();
	}
	else /// ���µ�¼
		return;
}
///ע���Ӧ����
void talk_to_svr::on_register(std::string &msg)
{
	std::string answer;
	std::istringstream iss(msg);
	iss >> answer >> answer;
	if (answer.find("succeed") == 0) {
		_csignal = ok_register;
	}
	else ///����ע��
		return;
}
///ping��Ӧ����
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
///��ȡ�����û���Ӧ����
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
///���ͻ�Ӧ����
void talk_to_svr::on_sendto(std::string &msg)
{
	std::string answer;
	std::istringstream iss(msg);
	iss >> answer >> answer;
	if (answer.find("succeed") == 0) {
		_csignal = ok_sendto; /// ��Ϣ���ͳɹ�
	}
	else ///��Ϣ����ʧ��
		return;
}
///������Ϣ
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
///�˳�
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
///д���Ӧ����
void talk_to_svr::on_write(const error_code &err, size_t bytes)
{
	///д����첽��ȡ����
	qDebug("write ok");
	//do_read();
}