#ifndef __TEST_DERANDOMIZATION__
#define __TEST_DERANDOMIZATION__

extern "C" {
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h> 
#include <unistd.h>
}

#include <cstring>
#include <string>


class TCPConnection {
private:
  int sock;
  struct sockaddr_in addr;
public:
  TCPConnection() {} ;
  bool tcpConnect(std::string ipaddr, unsigned short int port);
  int csend(std::string data);
  int crecv(std::string &data, unsigned int len);
  void closeConnection();
};





#endif