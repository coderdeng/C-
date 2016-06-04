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
	//����
	void connect(ip::tcp::endpoint);
	//ѭ��
	void loop();
	//��������
	void write_request();
	//��ȡ��Ӧ
	void read_answer();
	//������Ϣ
	void process_msg();
	//��¼����-�ص�
	void on_login();
	//ping-�ص�
	void on_ping(const std::string &);
	//client-�ص�
	void on_client(const std::string &);
	//ִ�з���
	void do_ask_client();
	//ִ�з���
	void write(const std::string &);
	//��ȡ��ɺ���
	size_t read_complete(const boost::system::error_code &, size_t);
	//��ȡ�û���
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