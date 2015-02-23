#pragma once
#include "typedefs.hpp"

template<typename TTcpSocket = TcpSocket>
class Session final:
	public std::enable_shared_from_this<Session<TTcpSocket>>
{
public:
	using TcpSocket = TTcpSocket;
	using Pointer = std::shared_ptr<Session<TcpSocket>>;
	using EndedSignal = boost::signals2::signal<void(Pointer, boost::system::error_code)>;

	Session<TTcpSocket>(TcpSocket &&tcpSocket):
		_socket(std::move(tcpSocket))
	{}

	auto addEndedSignal(const typename EndedSignal::slot_type &slot) const -> Signals2Connection{
		return _endedSignal.connect(slot);
	}

	auto start() -> void{
		read();
	}

	auto end(boost::system::error_code ec) -> void{
		/* TODO */
	}

	auto socket() -> const TcpSocket &{
		return _socket;
	}

	auto read() -> void{
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
protected:
	TcpSocket _socket;
	std::vector<char> _buffer;
	mutable EndedSignal _endedSignal;
	boost::system::error_code _errorCode;
};