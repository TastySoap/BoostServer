#pragma once

#include "Server.hpp"
#include "LoggingSession.hpp"

class LoggingServer : public Server{
public:
	explicit LoggingServer(boost::asio::io_service &, const EndPoint &);
	virtual ~LoggingServer() override;

	virtual auto makeNewSession(TcpSocket &&)->Session::Pointer override;
	virtual auto handleNewSession(Session::Pointer) -> void override;
	virtual auto onSessionEnd(Session::Pointer, boost::system::error_code) -> void override;
};