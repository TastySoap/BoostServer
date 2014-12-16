#include <boost/asio/ssl.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <vector>

typedef boost::asio::ip::tcp::socket TcpSocket;
typedef boost::asio::ssl::stream<TcpSocket> SslSocket;

class Session:
	public std::enable_shared_from_this<Session>
{
public:
	Session(TcpSocket);
	virtual ~Session();

	virtual auto start() -> void;
	virtual auto end() -> void;
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
public:
	Server(boost::asio::io_service &, const boost::asio::ip::tcp::endpoint &);
	virtual ~Server();
	auto start() -> void;
private:
	virtual auto startAcceptingConnections() -> void;
	virtual auto handleNewConnection() -> void;
};



int main(){

}