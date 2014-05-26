#ifndef __MT_DERAND__
#define __MT_DERAND__

#include "unroll.hpp"
#include "gauss.hpp"
#include "server.hpp"
//#include "mtrand.hpp"


class MTDerand 
{
private:  

  static const unsigned short int mtStateSize = 19937;
  static const unsigned short int phpMtStateSize = 19938;


  MTUnroller unroller;
  GaussianEliminator solver;
  SimpleServer server;
  bool derandPHP;
  unsigned short int localPort;

  void cmdAddLeaks(unsigned short int total);
  void cmdSolveSystem();  
  uint32_t *convertToMTState(std::vector <bool>&);
  uint32_t *convertToPHPState(std::vector <bool>&);
public:
  static const unsigned char cmdLeak = 'L';
  static const unsigned char cmdSkip = 'N';
  static const unsigned char cmdSolve = 'S';
  static const unsigned char cmdClear = 'C';
  static const unsigned char cmdShutdown = 'T';
  static const unsigned char rspOK = 'O';
  static const unsigned char rspError = 'E';
  static const unsigned char rspSolvable = 'S';
  static const unsigned char rspMore = 'M';
  static const unsigned short int cmdSize = 2;
  static const unsigned short int respSize = 1;



  void setPort(unsigned short int p) { localPort = p; }
  MTDerand(bool php=false) 
    : solver((php)?phpMtStateSize : mtStateSize), unroller(php) 
  {
    derandPHP=php;
    localPort = 8080;
  }
  bool start();
};

#endif
