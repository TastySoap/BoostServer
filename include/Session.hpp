#pragma once
#include "typedefs.hpp"
#include "Packet.hpp"

/* WARNING! SERVER BASED ON PACKETS AND THEIRS BUFFER SEQUENCES IS NOT TESTED, 
   AND IT'S HIGLY RECCOMENDED TO KEEP THE COMMIT HISTORY!               
*/

template<class TPacket, class TBuffer> auto
inline translatePacketToBuffers(TPacket &packet) -> std::vector<TBuffer>{
	return{
		boost::asio::buffer(packet.header),
		boost::asio::buffer(packet.body)
	};
}

template<
	typename TTcpSocket = TcpSocket,
	typename TPacket = Packet<>
>
class Session final:
	public std::enable_shared_from_this<Session<TTcpSocket>>
{
public:
	using TcpSocket = TTcpSocket;
	using Pointer = std::shared_ptr<Session<TcpSocket>>;
	
	using Packet = TPacket;

	Session<TTcpSocket, TPacket>(TcpSocket &&tcpSocket):
		_socket(std::move(tcpSocket))
	{}

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
		using buffer_type = boost::asio::mutable_buffer;
		boost::asio::async_read(
			_socket, translatePacketToBuffers<Packet, buffer_type>(_packet),
			[this, self](boost::system::error_code ec, std::size_t){
				ec? end(ec) : write();
			}
		);
	}

	auto Session::write() -> void{
		auto self(shared_from_this());
		using buffer_type = boost::asio::mutable_buffer;
		boost::asio::async_write(
			_socket, translatePacketToBuffers<Packet, buffer_type>(_packet),
			[this, self](boost::system::error_code ec, std::size_t){
				ec? end(ec) : read();
			}
		);
	}
protected:
	TcpSocket _socket;
	Packet _packet;
	boost::system::error_code _errorCode;
};