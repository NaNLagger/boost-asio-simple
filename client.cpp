#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string.h>
#include <boost/asio.hpp>

using namespace boost::asio;

io_service service;
char inBuffer[1024], outBuffer[1024];

ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);
ip::tcp::socket sock(service);

void connect_handler(const boost::system::error_code & ec);
void on_read(const boost::system::error_code &err, std::size_t bytes);
void on_write(const boost::system::error_code &err, std::size_t bytes);

int main(int argc, char* argv[]) {
  sock.async_connect(ep, connect_handler);
  service.run();

  return 0;
}

void connect_handler(const boost::system::error_code & ec) {
  std::cin >> inBuffer;
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