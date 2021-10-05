//
// Created by giuliano on 06/08/2021.
//

#ifndef BACKEND_CLIENT_H
#define BACKEND_CLIENT_H

#include <iostream>
#include <unistd.h>
#include <boost/asio.hpp>

#include "logs/logs.h"

using boost::asio::ip::address;
using boost::asio::ip::tcp;

std::string ping_server() {
    LOGE("Ping function called", "a");
    char query[200] = "TESTANDD";
    size_t query_length = 9;

    char response[200];
    size_t response_length;

    boost::system::error_code ec; // Boost asio tends not to use c++ exceptions
    boost::asio::io_service m_io_service;
    tcp::socket socket(m_io_service);

    address ip_address = address::from_string("127.0.0.1", ec);
    if (ec) {
        throw "Could not create address";
    }

    sleep(3);

    tcp::endpoint ep(ip_address, 9001);

    LOGE("will connect to server", "a");

    socket.connect(ep);

    LOGE("connect", "a");

    std::string message(query);
    LOGG("CLIENT", message)

    socket.write_some(boost::asio::buffer(query, query_length), ec);
    if (ec)
        throw boost::system::system_error(ec);

    response_length = socket.read_some(boost::asio::buffer(response), ec);

    return response;
}

#endif //BACKEND_CLIENT_H
