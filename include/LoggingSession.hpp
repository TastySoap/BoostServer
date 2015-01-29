#pragma once

#include "Session.hpp"

class LoggingSession : public Session, public std::enable_shared_from_this<LoggingSession>{
public:
	explicit LoggingSession(TcpSocket &&);
	virtual ~LoggingSession() override;

	auto read() -> void override;
};