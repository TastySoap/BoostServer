#pragma once

#include "Session.hpp"

class LoggingSession : public Session{
public:
	explicit LoggingSession(TcpSocket &&);
	virtual ~LoggingSession() override;
};