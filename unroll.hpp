#ifndef __MT_UNROLLER__
#define __MT_UNROLLER__

#include <vector>
#include <cstdlib>

#include "equation.hpp"

class MTUnroller {
private:
  const unsigned int a;
  const unsigned short int N;
  const unsigned short int M;
  const unsigned short int stateSize;
  unsigned int currentPos;
  bool update;
  bool mtPHP;
  Equation *varArray;
  std::vector <unsigned short int> tempering[32];

  void fillTempering(); 
  void generateXi();

public:
  void latexPrintTempering();
  MTUnroller(bool php=false);
  ~MTUnroller() { delete []varArray; }
  Equation getNextEquation();
  void skipEquations(unsigned short int num = 32);
  void clear();
  void dumpVarArray();
};

#endif
