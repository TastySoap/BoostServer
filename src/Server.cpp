#include "Server.hpp"

Server::Server(boost::asio::io_service &ioService, const EndPoint &ep):
	_acceptor(ioService, ep),
	_endPoint(ep),
	_socket(ioService)
{}

Server::~Server(){}

auto Server::start() -> void{
	startAcceptingSessions();
}

auto Server::startAcceptingSessions() -> void{
	_acceptor.async_accept(
		_socket,
		[this](boost::system::error_code ec){
			if (!ec) handleNewSession(makeNewSession(std::move(_socket)));
			startAcceptingSessions();
		}
	);
}

auto Server::makeNewSession(TcpSocket &&socket) -> Session::Pointer{
	return std::make_shared<Session>(std::move(socket));
}

auto Server::handleNewSession(Session::Pointer sp) -> void{
	_sessions.insert(sp);
	sp->addEndedSignal(boost::bind(&Server::onSessionEnd, this, _1, _2));
	sp->start();
}

auto Server::onSessionEnd(Session::Pointer sessionPtr, boost::system::error_code ec) -> void{
	_sessions.erase(sessionPtr);
}