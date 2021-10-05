//
// Created by giuliano on 06/08/2021.
//

#ifndef BACKEND_ECHO_SERVICE_H
#define BACKEND_ECHO_SERVICE_H
#include "iserver.h"

class Echo_reverse_service : public Tcp_service_interface
{
public:
    virtual auto process(char* input, const size_t inputLength, char* response, size_t& responseLength) -> void
    {
        for (size_t i=0; i < inputLength; ++i)
        {
            response[i] = input[(inputLength-1)-i];
        }
        responseLength = inputLength;
        response[responseLength] = 0;
    };
};
#endif //BACKEND_ECHO_SERVICE_H
