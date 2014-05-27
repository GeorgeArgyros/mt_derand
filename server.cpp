#include "server.hpp"


int
SimpleServer::setup( unsigned short int port, std::string ipaddr)
{

  if ((lsock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    return -1;

  memset(&addr, '\0', sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(ipaddr.c_str());

  int yes = 1;
  if ( setsockopt(lsock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1 )
    return -1;


  if (bind(lsock, (struct sockaddr *) &addr, sizeof(addr)) < 0)
    return -1;
  
  return 0;
}

int
SimpleServer::waitForClient()
{
  socklen_t len;

  if (listen(lsock, 10) < 0)
    return -1;
  
  if ((csock = accept(lsock, (struct sockaddr *)&caddr, &len)) < 0)
    return -1;
  
  idle = false;
  return 0;
}

ssize_t
SimpleServer::crecv(void *buffer, size_t len)
{
  ssize_t ret;
  ret = recv(csock, buffer, len, 0);
  if (ret == 0){
    close(csock);
    idle = true;
  }
  return ret;    
}

ssize_t
SimpleServer::csend(const void *buffer, size_t len)
{
  return send(csock, buffer, len, 0);
}

void
SimpleServer::shutdown()
{
  close(csock);
  close(lsock);
}

bool
SimpleServer::isIdle()
{
  return idle;
}


/*
int main()
{
  SimpleServer ss;
  char buf[100];

  ss.setup();
  while (1) {
    ss.waitForClient();
    ss.csend("lalala", sizeof("lalala"));
    ss.crecv((void *)buf, sizeof(buf));
    if (buf[1] == 'a')
      break;
  }
  return 0;
}
*/
