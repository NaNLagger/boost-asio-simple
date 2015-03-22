#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

using namespace boost::asio;
using namespace std;

class Session {
  public:
    Session(io_service& io_service) : client_socket(io_service) {
    
    }

    ip::tcp::socket& socket() {
      return client_socket;
    }

    void start() {
      cout << "New client connected\n";
      client_socket.async_read_some(buffer(data, max_length),
          boost::bind(&Session::handle_read, this,
            placeholders::error,
            placeholders::bytes_transferred));
    } 

  private:
    void handle_read(const boost::system::error_code& error, size_t bytes_transferred) {
      if (!error) {
        cout << data << "\n";
        for (int i = 0; i < strlen(data)/2; i++) {
        	char symbol;
        	symbol = data[i];
        	data[i] = data[strlen(data)-1-i];
        	data[strlen(data)-1-i] = symbol;
        }
        async_write(client_socket,
            buffer(data, bytes_transferred),
            boost::bind(&Session::handle_write, this,
              placeholders::error));
      }
      else {
        delete this;
      }
    }

    void handle_write(const boost::system::error_code& error) {
      if (!error) {
      	memset(data,'\0',sizeof(data));
        client_socket.async_read_some(buffer(data, max_length),
            boost::bind(&Session::handle_read, this,
              placeholders::error,
              placeholders::bytes_transferred));
      }
      else {
        delete this;
      }
    }

    ip::tcp::socket client_socket;
    enum { max_length = 1024 };
    char data[max_length];
};

class Server {
  public:
    Server(io_service& io_service, short port) : service(io_service), acc(io_service, ip::tcp::endpoint(ip::tcp::v4(), port)) {
      cout << "Server start\n";
      start_accept();
    }

  private:
    void start_accept() {
      Session* new_session = new Session(service);
      cout << "Waiting new client connected\n";
      acc.async_accept(new_session->socket(),
          boost::bind(&Server::handle_accept, this, new_session,
            placeholders::error));
    }

    void handle_accept(Session* new_session, const boost::system::error_code& error) {
      cout << "Connecting...\n";
      if (!error) {
        new_session->start();
      }
      else {
        cout << "AAAAAAAAA suka...\n";
        delete new_session;
      }

      start_accept();
    }

    io_service& service;
    ip::tcp::acceptor acc;
};

int main(int argc, char* argv[]) {
	io_service service;
	Server s(service, 2001);
    service.run();
	return 0;
}