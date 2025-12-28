#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <utility>

using boost::asio::ip::tcp;

class SimpleClient {
    std::string _server_ip;
    uint16_t _server_port;

    boost::system::error_code _ec;
    boost::asio::io_context _io_context;
    tcp::socket _socket;

public:
    SimpleClient(std::string server_ip, uint16_t server_port) : _server_ip(std::move(server_ip)),
                                                                _server_port(server_port),
                                                                _socket(_io_context) {
    }

    void connect() {
        try {
            _socket.connect(tcp::endpoint(boost::asio::ip::make_address(_server_ip), _server_port));
            std::cout << "Connected to server: " << _server_ip << std::endl;
        } catch (std::exception &e) {
            std::cerr << "Exception: " << e.what() << std::endl;
        }
    }

    void receive_message() {
        char buff[1024] = {0};

        try {
            _socket.read_some(boost::asio::buffer(buff), _ec);
            std::cout << "Received message: " << buff << std::endl;
        } catch (std::exception &e) {
            std::cerr << "Exception: " << e.what() << std::endl;
        }
    }

    void send_message() {
        std::string message;
        std::cout << "Enter message: ";
        getline(std::cin, message);

        try {
            boost::asio::write(_socket, boost::asio::buffer(message), _ec);
        } catch (std::exception &e) {
            std::cerr << "Exception: " << e.what() << std::endl;
        }
    }
};

[[noreturn]] int main() {
    std::string ip_address;
    std::cout << "Enter IP address: ";
    std::cin >> ip_address;
    std::cin.ignore(); // Ignore leftover newline from std::cin

    SimpleClient client(ip_address, 80);

    client.connect();

    while (true) {
        client.send_message();
        client.receive_message();
    }
}
