#include "LoggingSession.hpp"

#include <iostream>

LoggingSession::LoggingSession(TcpSocket &&socket): Session(std::move(socket)){
	std::clog << __FUNCTION__ << std::endl;
}

LoggingSession::~LoggingSession(){
	std::clog << __FUNCTION__ << std::endl;
}

auto LoggingSession::read() -> void{
	std::clog << __FUNCTION__ << std::endl;
	Session::read();
}