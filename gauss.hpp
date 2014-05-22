#ifndef __GAUSSIAN__
#define __GAUSSIAN__

#include "equation.hpp"

#include <vector>

struct Row {
  bool set;
  Equation eq;

  Row(){ set = false; } 
};

class GaussianEliminator
{
private:
  unsigned int rowsUnset;  
  unsigned int vars;
  Row *rows;
  bool *constTerms; 
public:
  GaussianEliminator(unsigned int v);
  ~GaussianEliminator() { delete[] rows, delete[] constTerms; }
  bool addEquation(Equation eq, bool term);
  std::vector <bool> getSolution();
  void clear();
  void printStats();
  bool isSolvable() { return (rowsUnset == 0); } 
};

#endif
