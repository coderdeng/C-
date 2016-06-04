#include "asyncService.h"
#include "orderAnalysis.h"

extern io_service service;
extern client_map clients;

ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), 8000));

void handle_accept(client_ptr client, const serror_code &err)
{
	client->start();
	client_ptr new_ = talk_to_client::create();
	acceptor.async_accept(new_->sock(), boost::bind(handle_accept, new_, _1));
}

int main()
{
#define TEST
#ifndef TEST
	client_ptr client = talk_to_client::create();
	acceptor.async_accept(client->sock(), boost::bind(handle_accept, client, _1));
	service.run();
#endif
	orderAnalysis::analysis("");
	std::cin.get();
}

/*
#include "syncService.h"

extern io_service service;
extern client_array clients;
extern boost::recursive_mutex cs;


void accept_thread()
{
	ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), 8000));
	while (true){
		client_ptr new_(new talk_to_client);
		acceptor.accept(new_->sock());
		boost::recursive_mutex::scoped_lock lk(cs);
		clients.push_back(new_);
	}
}

void handle_clients_thread()
{
	while (true){
		boost::this_thread::sleep(boost::posix_time::millisec(1));
		boost::recursive_mutex::scoped_lock lk(cs);
		for (client_array::iterator it = clients.begin(); it != clients.end(); ++it)
			(*it)->answer_to_client();
		clients.erase(std::remove_if(clients.begin(), clients.end(),
			boost::bind(&talk_to_client::timed_out, _1)), clients.end());
	}
}

int main()
{
	boost::thread_group threads;
	threads.create_thread(accept_thread);
	threads.create_thread(handle_clients_thread);
	threads.join_all();
}

#include "syncClient.h"

int main()
{
	ip::tcp::endpoint ep(ip::address::from_string("139.129.129.181"), 8000);
	std::string client_name;
	std::cout << "输入用户名" << std::endl;
	std::cin >> client_name;
	talk_to_svr client(client_name);
	try{
		client.connect(ep);
		client.loop();
	}
	catch (boost::system::error_code &err){
		std::cout << "client terminated" << std::endl;
		std::cout << err.message() << std::endl;
	}
	catch (boost::system::system_error &err){
		std::cout << err.code() << err.what() << std::endl;
	}
	system("pause");
	return 0;
}
*/