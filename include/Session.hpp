#pragma once
#include "typedefs.hpp"

class Session:
	public std::enable_shared_from_this<Session>
{
public:
	using Pointer = std::shared_ptr<Session>;
	using EndedSignal = boost::signals2::signal<void(Pointer, boost::system::error_code)>;

	explicit Session(TcpSocket &&);
	virtual ~Session();

	auto addEndedSignal(const EndedSignal::slot_type &) const->Signals2Connection;

	virtual auto start() -> void;
	virtual auto end(boost::system::error_code) -> void;

	auto socket() -> const TcpSocket &;
protected:
	virtual auto read() -> void;
	virtual auto write() -> void;
protected:
	TcpSocket _socket;
	std::vector<char> _buffer;
	mutable EndedSignal _endedSignal;
	boost::system::error_code _errorCode;
};