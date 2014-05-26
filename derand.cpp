#include "derand.hpp"

#include <math.h>
#include <cstdlib>

extern "C"{
# include <getopt.h>
# include <unistd.h>
}


#define N 624


void
MTDerand::cmdSolveSystem()
{
  char rsp = rspError;
  unsigned int stateSize = N * sizeof(uint32_t);

  if (!solver.isSolvable()){
    server.csend(&rsp, respSize);
    return;
  }
  rsp = rspOK;
  std::vector <bool> sol = solver.getSolution();
  uint32_t *state = (derandPHP)? convertToPHPState(sol) : convertToMTState(sol);

  server.csend(&rsp, respSize);
  server.csend(state, stateSize);
  return;
}


uint32_t *
MTDerand::convertToMTState(std::vector <bool> &sol)
{
  uint32_t *state = new uint32_t[N];
  
  state[0] = (sol[0] & 1) << 31;
  for (unsigned int i = 1; i < N; i ++) {
    state[i] = 0;
    for (int j = 31; j >= 0; j --) 
      if (sol[(i-1)*32 + 31 - j + 1]) 
	state[i] |= (1 << j);        
  }

  return state;
}

uint32_t *
MTDerand::convertToPHPState(std::vector <bool> &sol)
{
  uint32_t *state = new uint32_t[N];

  state[0] = sol[0] << 31;
  state[0] |= sol[1];

  for (unsigned int i = 1; i < N; i ++) {
    state[i] = 0;
    for (int j = 31; j >= 0; j --) 
      if (sol[(i-1)*32 + 31 - j + 2]) 
	state[i] |= (1 << j);        
  }

  return state;
}

bool
MTDerand::start()
{
  char cmd[cmdSize];
  char rsp = rspOK;
  bool is;
  unsigned short int s;
  if (server.setup(localPort) < 0)
    return false;
  
  while (1) {   
 
#ifdef __DEBUG__
    std::cout << " [d][i] Waiting for client\n";
#endif
    if (server.isIdle())
      server.waitForClient();

#ifdef __DEBUG__
    std::cout <<"[d][i] Waiting for data\n";
#endif
    
    if (server.crecv(cmd, cmdSize) <= 0)
      continue;
    
    switch (cmd[0]) {
    case cmdLeak:
      is = solver.addEquation(unroller.getNextEquation(), (bool) cmd[1]);
#ifdef __DEBUG__
      std::cout << "[d][l] Added bit " << (bool)cmd[1] << std::endl;
#endif
        solver.printStats();
      if (solver.isSolvable())
        rsp = rspSolvable;
      else
      rsp = rspMore;
      server.csend(&rsp, respSize);
      break;
    case cmdSkip:
      s  = (unsigned char)cmd[1];
      unroller.skipEquations((unsigned short int) s);
#ifdef __DEBUG__
      std::cout << "[d][s] Skipped " << s << " equations." << std::endl;
#endif
      server.csend(&rsp, respSize);
      break;
    case cmdSolve:    
      cmdSolveSystem();
      break;
    case cmdClear:
      solver.clear();
      unroller.clear();
      rsp = rspOK;
      server.csend(&rsp, 1);
      break;
    case cmdShutdown:
      rsp = rspOK;
      server.csend(&rsp, 1);
      server.shutdown();
      return true;
    default:
      rsp = rspError;
      server.csend(&rsp, 1);
      break;      
    }       
 
  }  

  server.shutdown();
  return true;
}


#define PHP  1
#define PORT  2
#define BACKGROUND  3

static struct option long_options[] = {
  {"php",   no_argument, 0,  PHP },
  {"port", required_argument, 0, PORT},
  {"daemon", no_argument, 0, BACKGROUND},
  {0, 0, 0,  0}
};


int main(int argc, char *argv[])
{
  bool daemonize = false, php = false;
  unsigned short int port = 8080;
  int c,options_index = 0;
  
  while ((c = getopt_long(argc, argv, "dp:", long_options, &options_index)) 
	 != -1) {
    switch (c) {
    case PORT:
      port = atoi(optarg);
      break;
    case BACKGROUND:
      std::cout << "[+] Server will work in the background" << std::endl;
      daemonize = true;
      break;
    case PHP:
      std::cout << "[+] Generator set to the PHP generator" << std::endl;
      php = true;
      break;
    default:
      return EXIT_FAILURE;
    }
  }

  if (daemonize)
    daemon(0, 0);

  MTDerand dr(php);
  dr.setPort(port);  
  dr.start();
  return 0;
}
