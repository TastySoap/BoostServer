#include "LoggingServer.hpp"

#include <iostream>

LoggingServer::LoggingServer(boost::asio::io_service &ioService, const EndPoint &endPoint):
	Server(ioService, endPoint)
{
	std::clog << __FUNCTION__ << std::endl;
}

LoggingServer::~LoggingServer(){
	std::clog << __FUNCTION__ << std::endl;
}

auto LoggingServer::makeNewSession(TcpSocket &&socket) -> Session::Pointer{
	std::clog << __FUNCTION__ << std::endl;
	return std::make_shared<LoggingSession>(std::move(socket));
}

auto LoggingServer::handleNewSession(Session::Pointer sessionPtr) -> void{
	std::clog << __FUNCTION__ << std::endl;
	Server::handleNewSession(sessionPtr);
}

auto LoggingServer::onSessionEnd(Session::Pointer sessionPtr, boost::system::error_code errorCode) -> void{
	std::clog << __FUNCTION__ << std::endl;
	Server::onSessionEnd(sessionPtr, errorCode);
}