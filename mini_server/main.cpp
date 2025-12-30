#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp;

class SessionAsync : public std::enable_shared_from_this<SessionAsync> {
private:
    tcp::socket socket_;

    enum { max_length = 1024 };

    char buff[max_length] = {0};

    void do_read() {
        auto self = shared_from_this();
        socket_.async_read_some(
            boost::asio::buffer(
                buff, max_length),
            [this, self](const boost::system::error_code &ec, std::size_t length) {
                if (!ec) {
                    std::cout << "Received message: " << std::string(buff, length) << std::endl;
                    do_write(length);
                }
            });
    }

    void do_write(std::size_t length) {
        auto self = shared_from_this();
        boost::asio::async_write(
            socket_,
            boost::asio::buffer(buff, length),
            [this,self](const boost::system::error_code &ec, std::size_t length) {
                if (!ec) {
                    do_read();
                }
            });
    }

public:
    SessionAsync(tcp::socket socket) : socket_(std::move(socket)) {
    }

    void start() {
        do_read();
    }
};

class ServerAsync {
private:
    tcp::acceptor acceptor_;

    void do_accept() {
        acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                std::cout << "Connection accepted: " << socket.remote_endpoint().address().to_string() << std::endl;
                std::make_shared<SessionAsync>(std::move(socket))->start();
            } else {
                std::cerr << "Accept error: " << ec.message() << std::endl;
            }

            do_accept();
        });
    }

public:
    ServerAsync(boost::asio::io_context &io_context, uint16_t port) : acceptor_(
        io_context, tcp::endpoint(tcp::v4(), port)) {
        do_accept();
    }
};

class ServerSync {
private:
    tcp::acceptor acceptor_;
    tcp::socket socket_;

    enum { max_length = 1024 };

    char buff[max_length] = {0};

public:
    ServerSync(boost::asio::io_context &io_context, uint32_t port) : acceptor_(io_context,
                                                                               tcp::endpoint(tcp::v4(), port)),
                                                                     socket_(io_context) {
        std::cout << "Waiting for connection..." << std::endl;
    }

    void accept() {
        try {
            acceptor_.accept(socket_);
            std::cout << "Connection accepted: " << socket_.remote_endpoint().address().to_string() << std::endl;
        } catch (const boost::system::error_code &ec) {
            std::cerr << "Accept error: " << ec.message() << std::endl;
        }
    }

    void receive() {
        try {
            std::size_t length = socket_.read_some(boost::asio::buffer(buff, max_length));
            std::cout << "Received message: " << std::string(buff, length) << std::endl;
        } catch (const boost::system::error_code &ec) {
            std::cerr << "Read error: " << ec.message() << std::endl;
        }
    }

    void write() {
        std::string msg;
        std::cout << "Enter message: ";
        getline(std::cin, msg);

        try {
            socket_.write_some(boost::asio::buffer(msg));
        } catch (boost::system::error_code &ec) {
            std::cerr << "Read error: " << ec.message() << std::endl;
        }
    }
};

[[noreturn]] int main() {
    boost::asio::io_context io_context;
#if 0
    ServerSync serversync(io_context, 80);

    serversync.accept();

    while (true) {
        serversync.receive();
        serversync.write();
    }
#endif

    ServerAsync serverasync(io_context, 80);
    io_context.run();
}
