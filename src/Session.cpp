#include "Session.hpp"

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
			ec ? end(ec) : write();
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