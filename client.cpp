#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string.h>
#include <boost/asio.hpp>

using namespace boost::asio;

io_service service;
int port;
char addr_server[16];
char inBuffer[1024], outBuffer[1024];

ip::tcp::socket sock(service);

void connect_handler(const boost::system::error_code & ec);
void on_read(const boost::system::error_code &err, std::size_t bytes);
void on_write(const boost::system::error_code &err, std::size_t bytes);

int main(int argc, char* argv[]) {
  if(argc < 3) {
    std::cerr << "Error run with server's <ip> and <port>: client <ip> <port>\n";
    return -1;
  } else {
    strcpy(addr_server, argv[1]);
    port = std::atoi(argv[2]);
  }
  ip::tcp::endpoint ep(ip::address::from_string(addr_server), 2001);
  sock.async_connect(ep, connect_handler);
  service.run();

  return 0;
}

void connect_handler(const boost::system::error_code & err) {
  if(err) {
    std::cerr << "Failed connect to server " << addr_server << ":" << port << "\n";
    return;
  }
  sock.async_write_some(buffer(inBuffer, strlen(inBuffer)), on_write);
}

void on_read(const boost::system::error_code &err, std::size_t bytes) {\
  std::cout << outBuffer << "\n";
  memset(outBuffer,'\0',sizeof(outBuffer));
  std::cin >> inBuffer;
  sock.async_write_some(buffer(inBuffer, strlen(inBuffer)), on_write);
}

void on_write(const boost::system::error_code &err, std::size_t bytes) {
  sock.async_read_some(buffer(outBuffer), on_read);
}