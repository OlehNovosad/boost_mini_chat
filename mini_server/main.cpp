#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <iostream>

using boost::asio::ip::tcp;

class SimpleServer {
    boost::asio::io_context _io_context;
    boost::system::error_code _ec;

    tcp::socket _socket;
    tcp::acceptor _acceptor;

public:
    SimpleServer()
        : _io_context(),
          _socket(_io_context),
          _acceptor(_io_context, tcp::endpoint(tcp::v4(), 80)) {
        std::cout << "Waiting for connection..." << std::endl;
    }

    ~SimpleServer() {
        delete &_acceptor;
        delete &_socket;
    }

    void accept_client_sync() {
        _acceptor.accept(_socket);
        std::cout << "Client: " << _socket.remote_endpoint().address() << " connected." << std::endl;
    }

    void receive_msg_sync() {
        char buff[1024] = {0};

        try {
            _socket.read_some(boost::asio::buffer(buff), _ec);
            if (_ec) { throw boost::system::system_error(_ec); }

            std::cout << "Message from client: " << buff << std::endl;
        } catch (std::exception &e) {
            std::cerr << "Exception: " << e.what() << std::endl;
        }
    }

    void write_msg_sync() {
        std::string message;
        std::cout << "Message to client: ";
        getline(std::cin, message);

        try {
            boost::asio::write(_socket, boost::asio::buffer(message), _ec);
        } catch (std::exception &e) {
            std::cerr << "Exception: " << e.what() << std::endl;
        }
    }
};


[[noreturn]] int main() {
    SimpleServer server;

#ifdef SYNC_SERVER
    server.accept_client_sync();

    while (true) {
        server.receive_msg_sync();
        server.write_msg_sync();
    }
#endif
}
