#include <boost/asio/ssl.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/signals2.hpp>

#include <vector>
#include <unordered_set>

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

	Session(TcpSocket);
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

Session::Session(TcpSocket tcpSocket)
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
			ec ? end(ec) : read();
		}
	);
}

class Server{
	using SessionContainer = std::unordered_set<Session::Pointer>;
public:
	Server(boost::asio::io_service &, const EndPoint &);
	virtual ~Server();
	auto start() -> void;
private:
	virtual auto startAcceptingSessions() -> void;
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
			if(!ec) handleNewSession(std::make_shared<Session>(std::move(_socket)));
			startAcceptingSessions();
		}
	);
}

#pragma warning(push)
#pragma warning(disable:4996)
auto Server::handleNewSession(Session::Pointer sp) -> void{
	_sessions.insert(sp);
	sp->addEndedSignal(boost::bind(&Server::onSessionEnd, this, _1, _2));
	sp->start();
}
#pragma warning(pop)
auto Server::onSessionEnd(Session::Pointer sessionPtr, boost::system::error_code ec) -> void{
	_sessions.erase(sessionPtr);
}



int main(){

}