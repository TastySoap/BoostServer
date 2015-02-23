#include <boost/thread.hpp>

#include "Server.hpp"
#include "LoggingSession.hpp"
#include "LoggingServer.hpp"

#include <vector>

//c10k - 10k clients!
int main(){
	using Server = LoggingServer<Server<std::vector<LoggingSession<>::Pointer>, void>>;

	auto ioService = boost::asio::io_service();
	auto port = 1337;
	auto endPoint = EndPoint(boost::asio::ip::tcp::v4(), port);

	auto server = Server(ioService, endPoint);
	server.start();

	while (true){}
	return 0;
}
