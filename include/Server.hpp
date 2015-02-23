#pragma once
#include "typedefs.hpp"

#include "Session.hpp"

template<
	typename TSessionContainer, 
	typename TDatabase, 
	typename TAcceptor = Acceptor
>
class Server{
	using this_type = Server<TSessionContainer, TDatabase, TAcceptor>;
public:
	using SessionContainer = TSessionContainer;
	using SessionPtr = typename SessionContainer::value_type;
	using Session = typename SessionPtr::element_type;
	using Database = TDatabase;
	using Acceptor = TAcceptor;
	using TcpSocket = typename Session::TcpSocket;
public:
	Server::Server(boost::asio::io_service &ioService, const EndPoint &ep):
		_acceptor(ioService, ep),
		_endPoint(ep),
		_socket(ioService){}

	virtual ~Server() = default;
public:
	inline auto start() -> void
	{ startAcceptingSessions(); }
protected:
	auto startAcceptingSessions() -> void{
		_acceptor.async_accept(
			_socket,
			[this](boost::system::error_code ec){
				if (!ec) handleNewSession(std::make_shared<Session>(std::move(_socket)));
				startAcceptingSessions(); 
			}
		);
	}

	auto handleNewSession(SessionPtr sp) -> void{
		_sessions.push_back(sp);
		sp->start();
	}

	auto onSessionEnd(SessionPtr sp, boost::system::error_code) -> void{
		using namespace std;
		_sessions.erase(find(begin(_sessions), end(_sessions), sp));
	}
protected:
	Acceptor _acceptor;
	TcpSocket _socket;
	const EndPoint &_endPoint;
	SessionContainer _sessions;
};