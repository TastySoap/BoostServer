#pragma once
#include "typedefs.hpp"

#include <unordered_set>

#include "Session.hpp"

template<typename TSession, typename TDatabase>
class Server{
public:
	using Session = TSession;
	using SessionContainer = std::unordered_set<typename Session::Pointer>;
	using Database = TDatabase;
public:
	Server::Server(boost::asio::io_service &ioService, const EndPoint &ep) :
		_acceptor(ioService, ep),
		_endPoint(ep),
		_socket(ioService){}

	virtual ~Server() = default;
public:
	inline auto start() -> void
	{ startAcceptingSessions(); }
protected:
	virtual auto startAcceptingSessions() -> void{
		_acceptor.async_accept(
			_socket,
			[this](boost::system::error_code ec){
				if (!ec) handleNewSession(std::make_shared<Session>(std::move(_socket)));
				startAcceptingSessions(); 
			}
		);
	}

	virtual auto handleNewSession(typename Session::Pointer sp) -> void{
		_sessions.insert(sp);
		sp->addEndedSignal(boost::bind(&Server::onSessionEnd, this, _1, _2));
		sp->start();
	}

	virtual auto onSessionEnd(typename Session::Pointer sp, boost::system::error_code) -> void{
		_sessions.erase(sp);
	}
protected:
	Acceptor _acceptor;
	TcpSocket _socket;
	const EndPoint &_endPoint;
	SessionContainer _sessions;
};