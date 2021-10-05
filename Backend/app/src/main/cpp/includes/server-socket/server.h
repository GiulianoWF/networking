//
// Created by giuliano on 06/08/2021.
//

#ifndef BACKEND_SERVER_H
#define BACKEND_SERVER_H

#include "iserver.h"

#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>

#include "logs/logs.h"

using boost::asio::ip::tcp;

const int max_length = 1024;

class Tcp_server
{
    boost::asio::io_service m_io_service; //Contain platform specific code to handle i/o context (socket readines, file descriptors...)
    std::shared_ptr<Tcp_service_interface> m_service;
    int * m_shutdown;

    auto handle_session(std::shared_ptr<tcp::socket> sock) noexcept ->void;

public:
    Tcp_server(std::shared_ptr<Tcp_service_interface> service, int port, int * shutdown);
};

#endif