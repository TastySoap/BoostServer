#include <boost/thread.hpp>

#include "LoggingServer.hpp"

//c10k - 10k clients!
int main(){
	try{
		auto ioService = boost::asio::io_service();
		auto port = 1337;
		auto endPoint = EndPoint(boost::asio::ip::tcp::v4(), port);
		auto server = LoggingServer(ioService, endPoint);
		server.start();

		auto action = [&server, &ioService, port]{
			boost::asio::ip::tcp::resolver resolver(ioService);
			boost::asio::ip::tcp::resolver::query query("localhost", std::to_string(port));
			boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
			boost::asio::ip::tcp::resolver::iterator end;

			std::vector<std::shared_ptr<TcpSocket>> clients;
			for (auto i = 0; i < 100; ++i)
			{
				auto client = std::make_shared<TcpSocket>(
					std::ref(ioService));
				boost::system::error_code error = boost::asio::error::host_not_found;
				auto epit = endpoint_iterator;
				while (error && epit != end)
				{
					client->close();
					client->connect(*epit++, error);
				}
				clients.push_back(client);
			}
		};
		
		boost::thread_group group;
		std::vector<boost::thread> threads;
		for(size_t i = 0; i < 100; ++i){
			threads.push_back(boost::thread(action));
			group.add_thread(&threads.back());
		}

		boost::thread timeout([&ioService, &group]{
			group.join_all();
			ioService.stop();
		});

		ioService.run();
		timeout.join();
	}
	catch(std::exception &e){
		std::cerr << "Std exception: " << e.what() << std::endl;
	}
	catch(boost::system::system_error &e){
		std::cerr << "Boost exception:" << e.what() << std::endl;
	}
	catch (...){
		std::cerr << "Unknown error!" << std::endl;
	}
	return 0;
}
