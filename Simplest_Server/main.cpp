#include <iostream>
#include <boost/iostreams/stream.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/asio.hpp>

int main()
{
    try
    {
        boost::asio::io_context ioContext;

        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), 80);
        boost::asio::ip::tcp::acceptor acceptor(ioContext, endpoint);

        while (true)
        {
            boost::asio::ip::tcp::iostream stream;
            boost::system::error_code errorCode;
            
            acceptor.accept(stream.socket(), errorCode);

            if (true != errorCode)
            {
                stream << "Server Response!";
            }
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}