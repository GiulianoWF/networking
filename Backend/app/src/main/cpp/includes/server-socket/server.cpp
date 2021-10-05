//
// Created by giuliano on 06/08/2021.
//
#include "server.h"

Tcp_server::Tcp_server(std::shared_ptr<Tcp_service_interface> service, int port, int * shutdown)
        :m_service (service), m_shutdown(shutdown)
{
    LOGG("SERVER", std::string("Accepting"))
    tcp::acceptor acceptor(m_io_service, tcp::endpoint(tcp::v4(), port)); //When a message arrives in the socket, the acceptor object check if the message follow TCP protocol, then it generate a session.

    while(*shutdown)
    {
        std::shared_ptr<tcp::socket> sock(new tcp::socket(m_io_service)); //Handle socket
        acceptor.accept(*sock); //Wait for a correct socket message

        LOGE("Accept on thread", "a");
        std::thread thread([this, sock](){this->handle_session(sock);}); //Spawn a thread to handle the tcp session
        thread.detach();
    }
}

auto Tcp_server::handle_session(std::shared_ptr<tcp::socket> sock) noexcept ->void
{
    std::cout << "Session created" << std::endl;

    char data[max_length] = "DEFAULT";
    size_t length = 8;

    char response[max_length];
    size_t responseLength;

    boost::system::error_code error;

    //Catch exception inside the session thread
    try
    {
        // Handle messages until the session is closed
        while (true)
        {

            LOGE("Server reading message", "a");

            // Read a message
            length = sock->read_some(boost::asio::buffer(data), error);

            // Handle errors and disconections============
            if (error == boost::asio::error::eof)
                break;
            else if (error)
                throw boost::system::system_error(error);
            //============================================

            std::string message(data);
            LOGG("SERVER read", message)
            LOGG("SERVER length", std::to_string(length))

            // Call the service
            m_service->process(data, length, response, responseLength);

//            message=response;
//            for (int i =0; i <= responseLength; ++i)
//            {
//                LOGG("SERVER message", std::to_string((int)message[i]));
//            }
            LOGG("SERVER write", std::string(response))

            // Send the response back
            boost::asio::write(*sock, boost::asio::buffer(response, responseLength));

//            const char messagee[] = "HTTP/1.0 200 OK\r\nContent-Length: 45\r\n\r\n<html><body><i>Hello, world</i></body></html>";
//            boost::asio::write(*sock, boost::asio::buffer(messagee), error);

            break;
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception in thread: " << e.what() << "\n";
    }
}