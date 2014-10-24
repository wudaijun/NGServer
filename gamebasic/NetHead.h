#ifndef __NGSERVER_NETHEAD_H_INCLUDE__
#define __NGSERVER_NETHEAD_H_INCLUDE__

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>
#include "Locker.h"


typedef boost::asio::io_service IOService;
typedef boost::asio::ip::tcp::acceptor Accepter;
typedef boost::asio::ip::tcp::socket Socket;
typedef boost::asio::ip::tcp::endpoint EndPoint;

using namespace std;
using namespace boost;

#endif