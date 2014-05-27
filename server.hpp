#ifndef __SIMPLE_SERVER__
#define __SIMPLE_SERVER__

extern "C" {
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h> 
#include <unistd.h>
}

#include <cstring>
#include <string>


class SimpleServer {
  int lsock, csock;
  struct sockaddr_in addr, caddr;  

  bool idle;
  unsigned short int port;
  unsigned char *ipaddr;
public:
  SimpleServer(){}
  int setup(unsigned short int port = 8080, std::string ipaddr="0.0.0.0");
  int waitForClient();
  ssize_t crecv(void  *buffer, size_t len);
  ssize_t csend(const void *buffer, size_t len);
  void shutdown();
  bool isIdle();
  void closeClient() { close(csock); }
};

#endif
