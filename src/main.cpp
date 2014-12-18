#include <boost/asio/ssl.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/signals2.hpp>
#include <boost/thread.hpp>

#include <vector>
#include <unordered_set>
#include <utility>
#include <memory>
#include <string>

using TcpSocket = boost::asio::ip::tcp::socket;
using SslSocket = boost::asio::ssl::stream<TcpSocket>;
using Acceptor  = boost::asio::ip::tcp::acceptor;
using EndPoint  = boost::asio::ip::tcp::endpoint;

using Signals2Connection = boost::signals2::connection;

class Session:
	public std::enable_shared_from_this<Session>
{
public:
	using Pointer     = std::shared_ptr<Session>;
	using EndedSignal = boost::signals2::signal<void(Pointer, boost::system::error_code)>;

	explicit Session(TcpSocket &&);
	virtual ~Session();

	auto addEndedSignal(const EndedSignal::slot_type &) const -> Signals2Connection;

	virtual auto start() -> void;
	virtual auto end(boost::system::error_code) -> void;

	auto socket() -> const TcpSocket &;
private:
	virtual auto read() -> void;
	virtual auto write() -> void;
protected:
	TcpSocket _socket;
	std::vector<char> _buffer;
	mutable EndedSignal _endedSignal;
	boost::system::error_code _errorCode;
};

Session::Session(TcpSocket &&tcpSocket)
	:_socket(std::move(tcpSocket)){}
Session::~Session(){}
auto Session::addEndedSignal(const EndedSignal::slot_type &slot) const -> Signals2Connection{
	return _endedSignal.connect(slot);
}
auto Session::start() -> void{
	read();
}
auto Session::end(boost::system::error_code ec) -> void{
	_endedSignal(shared_from_this(), ec);
}
auto Session::socket() -> const TcpSocket &{
	return _socket;
}
auto Session::read() -> void{
	auto self(shared_from_this());
	boost::asio::async_read(
		_socket, boost::asio::buffer(_buffer),
		[this, self](boost::system::error_code ec, std::size_t){
			ec? end(ec) : write();
		}
	);
}
auto Session::write() -> void{
	auto self(shared_from_this());
	boost::asio::async_write(
		_socket, boost::asio::buffer(_buffer),
		[this, self](boost::system::error_code ec, std::size_t){
			ec? end(ec) : read();
		}
	);
}

class Server{
	using SessionContainer = std::unordered_set<Session::Pointer>;
public:
	Server(boost::asio::io_service &, const EndPoint &);
	virtual ~Server();
	auto start() -> void;
protected:
	virtual auto startAcceptingSessions() -> void;
	virtual auto makeNewSession(TcpSocket &&) -> Session::Pointer;
	virtual auto handleNewSession(Session::Pointer) -> void;
	virtual auto onSessionEnd(Session::Pointer, boost::system::error_code) -> void;
protected:
	Acceptor _acceptor;
	TcpSocket _socket;
	const EndPoint &_endPoint;
	SessionContainer _sessions;
};

Server::Server(boost::asio::io_service &ioService, const EndPoint &ep):
	_acceptor(ioService, ep),
	_endPoint(ep),
	_socket(ioService)
{}
Server::~Server(){}
auto Server::start() -> void{
	startAcceptingSessions();
}
auto Server::startAcceptingSessions() -> void{
	_acceptor.async_accept(
		_socket,
		[this](boost::system::error_code ec){
			if(!ec) handleNewSession(makeNewSession(std::move(_socket)));
			startAcceptingSessions();
		}
	);
}
auto Server::makeNewSession(TcpSocket &&socket) -> Session::Pointer{
	return std::make_shared<Session>(std::move(socket));
}
auto Server::handleNewSession(Session::Pointer sp) -> void{
	_sessions.insert(sp);
	sp->addEndedSignal(boost::bind(&Server::onSessionEnd, this, _1, _2));
	sp->start();
}

auto Server::onSessionEnd(Session::Pointer sessionPtr, boost::system::error_code ec) -> void{
	_sessions.erase(sessionPtr);
}

class LoggingSession: public Session{
public:
	explicit LoggingSession(TcpSocket &&);
	virtual ~LoggingSession() override;
};

LoggingSession::LoggingSession(TcpSocket &&socket): Session(std::move(socket)){
	std::clog << __FUNCTION__ << std::endl;
}
LoggingSession::~LoggingSession(){
	std::clog << __FUNCTION__ << std::endl;
}

class LoggingServer: public Server{
public:
	explicit LoggingServer(boost::asio::io_service &, const EndPoint &);
	virtual ~LoggingServer() override;
	
	virtual auto makeNewSession(TcpSocket &&) -> Session::Pointer override;
	virtual auto handleNewSession(Session::Pointer) -> void override;
	virtual auto onSessionEnd(Session::Pointer, boost::system::error_code) -> void override;
};

LoggingServer::LoggingServer(boost::asio::io_service &ioService, const EndPoint &endPoint):
	Server(ioService, endPoint)
{
	std::clog << __FUNCTION__ << std::endl;
}
LoggingServer::~LoggingServer(){
	std::clog << __FUNCTION__ << std::endl;
}
auto LoggingServer::makeNewSession(TcpSocket &&socket) -> Session::Pointer{
	std::clog << __FUNCTION__ << std::endl;
	return std::make_shared<LoggingSession>(std::move(socket));
}
auto LoggingServer::handleNewSession(Session::Pointer sessionPtr) -> void{
	std::clog << __FUNCTION__ << std::endl;
	Server::handleNewSession(sessionPtr);
}
auto LoggingServer::onSessionEnd(Session::Pointer sessionPtr, boost::system::error_code errorCode) -> void{
	std::clog << __FUNCTION__ << std::endl;
	Server::onSessionEnd(sessionPtr, errorCode);
}

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
