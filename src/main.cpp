#include <boost/thread.hpp>

#include "LoggingServer.hpp"

//c10k - 10k clients!
int main(){
	auto ioService = boost::asio::io_service();
	auto port = 1337;
	auto endPoint = EndPoint(boost::asio::ip::tcp::v4(), port);
	auto server = LoggingServer<LoggingSession, void>(ioService, endPoint);
	server.start();

	while (true){}
	return 0;
}
