#pragma once
#include "typedefs.hpp"

#include <unordered_set>

#include "Session.hpp"

class Server{
	using SessionContainer = std::unordered_set<Session::Pointer>;
public:
	Server(boost::asio::io_service &, const EndPoint &);
	virtual ~Server();
	auto start() -> void;
protected:
	virtual auto startAcceptingSessions() -> void;
	virtual auto makeNewSession(TcpSocket &&)->Session::Pointer;
	virtual auto handleNewSession(Session::Pointer) -> void;
	virtual auto onSessionEnd(Session::Pointer, boost::system::error_code) -> void;
protected:
	Acceptor _acceptor;
	TcpSocket _socket;
	const EndPoint &_endPoint;
	SessionContainer _sessions;
};