#pragma once

#include "Server.hpp"
#include "LoggingSession.hpp"

template<typename SessionContainer, typename Database>
class LoggingServer : public Server<SessionContainer, Database>{
public:
	using SessionPtr = typename SessionContainer::value_type;

	explicit LoggingServer(boost::asio::io_service &ioService, const EndPoint &endPoint): 
		Server<SessionContainer, Database>(ioService, endPoint)
	{ std::clog << __FUNCTION__ << std::endl; }

	~LoggingServer()
	{ std::clog << __FUNCTION__ << std::endl; }

	auto handleNewSession(SessionPtr sessionPtr) -> void override{
		std::clog << __FUNCTION__ << std::endl;
		Server::handleNewSession(sessionPtr);
	}

	auto onSessionEnd(SessionPtr sessionPtr, boost::system::error_code errorCode) -> void override{
		std::clog << __FUNCTION__ << std::endl;
		Server::onSessionEnd(sessionPtr, errorCode);
	}
};