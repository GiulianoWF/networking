#include <iostream>
#include <string>
// #include <experimental/net> // NetworkingTS: c++23 maybe?
#include <asio/ip/tcp.hpp>

// namespace net = std::experimental::net;
using asio::ip::tcp;

int main(int argc, char* argv[])
{
    std::string url {"think-async.com"};
    // net::ip::tcp::iostream s(url, "http");
    tcp::iostream s(url, "http");

    assert(s.good());  

    s << "GET / HTTP/1.0\r\n";
    s << "Host: " << url << "\r\n";
    s << "Accept: */*\r\n";
    s << "Connection: close\r\n";
    s << "\r\n";

    std::string header;
    while(std::getline(s,header) && header!= "\r")
    {
        std::cout << "<HEADER>:    " <<header << "\n";
    }

    std::cout << s.rdbuf();
}