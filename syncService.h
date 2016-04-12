#ifndef __SYNCSERVICE_H__
#define __SYNCSERVICE_H__

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost::asio;

class talk_to_client : public boost::enable_shared_from_this<talk_to_client>
{
public:
	//����
	talk_to_client();
	//��ȡ�û���
	std::string username()const { return username_; }
	//��Ӧ�ͻ�
	void answer_to_client();
	//�����û��ı�
	void set_clients_changed() { clients_changed_ = true; };
	//��ȡsocket
	ip::tcp::socket& sock() { return sock_; }
	//��ʱ
	bool timed_out() const;
	//�ر�sock
	void stop();
	//��ȡping����
	void read_request();
	//������Ϣ
	void process_request();
	//��¼��Ӧ
	void on_login(const std::string &);
	//ping��Ӧ
	void on_ping();
	//clients��Ӧ
	void on_clients();
	//������Ϣ
	void write(const std::string &);

private:
	ip::tcp::socket sock_;
	std::string username_;
	int already_read_;
	bool stared_;
	enum { max_msg = 1024 };
	char buff_[max_msg];
	bool clients_changed_;
	boost::posix_time::ptime last_ping;
};

typedef boost::shared_ptr<talk_to_client> client_ptr;
typedef std::vector<client_ptr> client_array;

#endif