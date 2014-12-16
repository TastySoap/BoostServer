#include <boost/asio/ssl.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include <vector>
#include <queue>

typedef boost::asio::ip::tcp::socket TcpSocket;
typedef boost::asio::ssl::stream<TcpSocket> SslSocket;
typedef boost::asio::ip::tcp::acceptor Acceptor;
typedef boost::asio::ip::tcp::endpoint EndPoint;

class Session:
	public std::enable_shared_from_this<Session>
{
public:
	Session(TcpSocket);
	virtual ~Session();

	virtual auto start() -> void;
	virtual auto end() -> void;

	auto socket() -> const TcpSocket &;
protected:
	TcpSocket _socket;
	std::vector<char> _buffer;
private:
	virtual auto read() -> void;
	virtual auto write() -> void;
};

Session::Session(TcpSocket tcpSocket)
	:_socket(std::move(tcpSocket)){}
Session::~Session(){}
auto Session::start() -> void{
	read();
}
auto Session::end() -> void{}
auto Session::socket() -> const TcpSocket &{
	return _socket;
}
auto Session::read() -> void{
	auto self(shared_from_this());
	boost::asio::async_read(
		_socket, boost::asio::buffer(_buffer),
		[this, self](boost::system::error_code ec, std::size_t){
			if(!ec) write();
		}
	);
}
auto Session::write() -> void{
	auto self(shared_from_this());
	boost::asio::async_write(
		_socket, boost::asio::buffer(_buffer),
		[this, self](boost::system::error_code ec, std::size_t){
			if (!ec) read();
		}
	);
}

class Server{
	typedef std::shared_ptr<Session> SessionPointer;
	typedef std::queue<SessionPointer> SessionContainer;
public:
	Server(boost::asio::io_service &, const EndPoint &);
	virtual ~Server();
	auto start() -> void;
private:
	virtual auto startAcceptingSessions() -> void;
	virtual auto handleNewSession(SessionPointer) -> void;
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
			if (!ec)
				handleNewSession(std::make_shared<Session>(std::move(_socket)));
			startAcceptingSessions();
		}
	);
}
auto Server::handleNewSession(SessionPointer sp) -> void{
	_sessions.push(sp);
	_sessions.back()->start();
}


int main(){

}