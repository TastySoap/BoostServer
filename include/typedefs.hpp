#pragma once

#include <boost/asio/ssl.hpp>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>

using TcpSocket = boost::asio::ip::tcp::socket;
using SslSocket = boost::asio::ssl::stream<TcpSocket>;
using Acceptor = boost::asio::ip::tcp::acceptor;
using EndPoint = boost::asio::ip::tcp::endpoint;

using Signals2Connection = boost::signals2::connection;