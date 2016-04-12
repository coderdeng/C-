#include "syncService.h"

client_array clients;
boost::recursive_mutex cs; //���̰߳�ȫ��ʽ���ʿͻ�������

io_service service;

talk_to_client::talk_to_client() : sock_(service), stared_(true)
{

}
//��Ӧ�ͻ�
void talk_to_client::answer_to_client()
{
	try{
		read_request();
		process_request();
	}
	catch (boost::system::error_code &err){
		stop();
	}
	if (timed_out())
		stop();
}
//��ʱ���
bool talk_to_client::timed_out() const
{
	boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();//��ȡ��ǰ΢�뼶ʱ��
	long long ms = (now - last_ping).total_milliseconds();
	return ms > 5000;
}
//�ر�socket
void talk_to_client::stop()
{
	boost::system::error_code err;
	sock_.close(err);
}
//��ȡ����
void talk_to_client::read_request()
{
	if (sock_.available())
		already_read_ += sock_.read_some(buffer(buff_ + already_read_, max_msg - already_read_));//��ȡ���Ѷ��ĺ���
}
//��������
void talk_to_client::process_request()
{
	bool found_enter = std::find(buff_, buff_ + already_read_, '\n') < buff_ + already_read_;
	if (!found_enter)//��Ϣ������
		return;
	last_ping = boost::posix_time::microsec_clock::local_time();
	size_t pos = std::find(buff_, buff_ + already_read_, '\n') - buff_;//��һ���س���λ��
	std::string msg(buff_, pos);
	std::copy(buff_ + already_read_, buff_ + max_msg, buff_);//��Ϣǰ��
	already_read_ -= pos + 1; //ǰ����Ϣ�Ƴ����壬����������Ϣ����
	if (msg.find("login ") == 0)on_login(msg);
	else if (msg.find("ping") == 0)on_ping();
	else if (msg.find("ask_clients") == 0)on_clients();
	else std::cerr << "invalid msg" << msg << std::endl;
}
//��¼��Ӧ
void talk_to_client::on_login(const std::string &msg)
{
	std::istringstream in(msg);
	in >> username_ >> username_;
	write("login ok\n");
	set_clients_changed();
}
//��Ӧping
void talk_to_client::on_ping()
{
	write(clients_changed_ ? "ping client_list_changed\n" : "ping ok\n");
}
//��Ӧclient
void talk_to_client::on_clients()
{
	std::string msg;
	{
		boost::recursive_mutex::scoped_lock lk(cs);
		for (client_array::const_iterator it = clients.begin(); it != clients.end(); ++it)
			msg += (*it)->username() + " ";
	}
	write(msg);
}
//������Ϣ
void talk_to_client::write(const std::string &msg)
{
	sock_.write_some(buffer(msg));
}
