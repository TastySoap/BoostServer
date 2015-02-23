#pragma once

#include "Server.hpp"
#include "LoggingSession.hpp"

template<typename TServer>
class LoggingServer{
public:
	using Server = TServer;
	using SessionPtr = typename Server::SessionContainer::value_type;

	LoggingServer(boost::asio::io_service &ioService, const EndPoint &endPoint) :
		_server(ioService, endPoint)
	{ 
		std::clog << __FUNCTION__ << std::endl; 
	}

	~LoggingServer()
	{ std::clog << __FUNCTION__ << std::endl; }

	auto start() -> void{
		_server.start();
	}

	auto handleNewSession(SessionPtr sessionPtr) -> void{
		std::clog << __FUNCTION__ << std::endl;
		_server.handleNewSession(sessionPtr);
	}

	auto onSessionEnd(SessionPtr sessionPtr, boost::system::error_code errorCode) -> void{
		std::clog << __FUNCTION__ << std::endl;
		server.onSessionEnd(sessionPtr, errorCode);
	}
private:
	Server _server;
};