#pragma once

#include "Session.hpp"

#include <iostream>

template<
	/* Must offer constructor(TcpSocket &&) */
	/* Must contain typename TcpSocket */
	typename TSession = Session<>
>
class LoggingSession final: 
	public std::enable_shared_from_this<LoggingSession<TSession>>
{
public:
	using Session = TSession;
	using TcpSocket = typename Session::TcpSocket;
	using Pointer = std::shared_ptr<LoggingSession<Session>>;

	LoggingSession(TcpSocket &&socket):
		_session(std::move(socket))
	{
		std::clog << __FUNCTION__ << std::endl;
	}

	auto start() -> void{
		std::clog << __FUNCTION__ << std::endl;
		read();
	}

	auto read() -> void{
		std::clog << __FUNCTION__ << std::endl;
		_session.read();
	};

	auto end() -> void{
		std::clog << __FUNCTION__ << std::endl;
		_session.end();
	}
private:
	Session _session;
};