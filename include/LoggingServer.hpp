#pragma once

#include "Server.hpp"
#include "LoggingSession.hpp"

template<typename Session, typename Database>
class LoggingServer: public Server<Session, Database>{
public:
	explicit LoggingServer(boost::asio::io_service &ioService, const EndPoint &endPoint): 
		Server<Session, Database>(ioService, endPoint)
	{ std::clog << __FUNCTION__ << std::endl; }

	~LoggingServer()
	{ std::clog << __FUNCTION__ << std::endl; }

	auto handleNewSession(typename Session::Pointer sessionPtr) -> void override{
		std::clog << __FUNCTION__ << std::endl;
		Server::handleNewSession(sessionPtr);
	}

	auto onSessionEnd(typename Session::Pointer sessionPtr, boost::system::error_code errorCode) -> void override{
		std::clog << __FUNCTION__ << std::endl;
		Server::onSessionEnd(sessionPtr, errorCode);
	}
};