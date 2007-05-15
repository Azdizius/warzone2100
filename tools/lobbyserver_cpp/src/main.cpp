/*
    Warzone 2100 Lobbyserver, serves as a meeting place to set up games
    Copyright (C) 2007  Giel van Schijndel

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    $Revision$
    $Id$
    $HeadURL$
*/

#include <iostream>
#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "networking/tcp_server.hpp"
#include "lobby.hpp"
#include <boost/thread/recursive_mutex.hpp>

const unsigned short lobbyPort = 9998;

static GameLobby lobby;
static boost::thread_group threads;
boost::recursive_mutex cout_mutex;
boost::recursive_mutex cerr_mutex;

static void handleRequestInThread(boost::shared_ptr<boost::asio::ip::tcp::socket> socket)
{
	threads.create_thread(boost::bind(&GameLobby::handleRequest, &lobby, socket));
}

int main(int argc, char* argv[])
{
	//ThreadingTCPServer lobbyServer;

	//return lobbyServer(lobbyPort);

	unsigned int returnValue = 1;
	try
	{
		boost::shared_ptr<boost::asio::io_service> io_service(new boost::asio::io_service);
		TCPServer tcp_server(io_service, handleRequestInThread);

		tcp_server.listen(lobbyPort);
		io_service->run();
		returnValue = 0;
	}
	catch (boost::asio::error& e)
	{
		boost::recursive_mutex::scoped_lock lock(cerr_mutex);
		std::cerr << "main: Boost::asio exception: " << e << std::endl;
	}
	catch (std::exception& e)
	{
		boost::recursive_mutex::scoped_lock lock(cerr_mutex);
		std::cerr << "main: Exception: " << e.what() << std::endl;
	}

	threads.join_all();

	return returnValue;
}
