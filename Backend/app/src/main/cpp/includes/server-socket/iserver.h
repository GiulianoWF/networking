//
// Created by giuliano on 06/08/2021.
//

#ifndef BACKEND_ISERVER_H
#define BACKEND_ISERVER_H
/*
 * All services implemented through this TCP server should inherit from this interface.
 * Each time the tcp server receive a message, it calls it's service method "process" on a new thread.
 */
class Tcp_service_interface
{
public:
    virtual auto process(char* input, const size_t inputLenght, char* response, size_t& responseLength) -> void = 0;
};
#endif //BACKEND_ISERVER_H
