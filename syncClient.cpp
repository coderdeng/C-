#include "syncClient.h"
#include <memory>

io_service service;

//构造
talk_to_svr::talk_to_svr(std::string &username) : sock_(service), started_(true), username_(username)
{
}
//连接
void talk_to_svr::connect(ip::tcp::endpoint ep)
{
	sock_.connect(ep);
}
//循环
void talk_to_svr::loop()
{
	write("login " + username_ + '\n');
	read_answer();
	while (started_){
		write_request();
		read_answer();
		boost::this_thread::sleep(boost::posix_time::millisec(10000));
		if (!sock_.available()){
			sock_.close();
			ip::tcp::endpoint ep(ip::address::from_string("139.129.129.181"), 8000);
			sock_.connect(ep);
			write("login " + username_ + "\n");
			read_answer();
		}
	}
}
//发送请求-ping
void talk_to_svr::write_request()
{
	write("ping\n");
}
//读取回应
void talk_to_svr::read_answer()
{
	//memset(buff_, 0, sizeof(buff_));
	//try{
		already_read_ = read(sock_, buffer(buff_), boost::bind(&talk_to_svr::read_complete, this, _1, _2));
	//}
	//catch (boost::system::system_error &err){
		//std::cout << buff_  << already_read_ << std::endl;
		//system("pause");
	//}
	process_msg();
}
//解析信息
void talk_to_svr::process_msg()
{
	std::string msg(buff_, already_read_);
	if (msg.find("login ") == 0)on_login();
	else if (msg.find("ping") == 0)on_ping(msg);
	else if (msg.find("clients") == 0)on_client(msg);
	else std::cerr << "invalid msg " << msg << std::endl;
}
//处理登录返回
void talk_to_svr::on_login()
{
	return;
}
//处理ping返回
void talk_to_svr::on_ping(const std::string &msg)
{
	std::istringstream in(msg);
	std::string answer;
	in >> answer >> answer; //读取第二个有用信息
	if (answer == "client_list_changed")
		do_ask_client();
}
//处理client返回
void talk_to_svr::on_client(const std::string &msg)
{
	std::string clients = msg.substr(8);//从8到结尾
	std::cout << username_ << ", new client list:" << clients << std::endl;
}
//自输入请求
void talk_to_svr::do_ask_client()
{
	write("ask_clients\n");
	read_answer();
}
//发送信息
void talk_to_svr::write(const std::string &msg)
{
	sock_.write_some(buffer(msg));
}
//读取完成回调
size_t talk_to_svr::read_complete(const boost::system::error_code &err, size_t bytes)
{
	if (err)return 0;
	bool found = std::find(buff_, buff_ + bytes, '\n') < buff_ + bytes;
	return found ? 0 : 1;
}