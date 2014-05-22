#ifndef __EQUATION_CLASS__
#define __EQUATION_CLASS__

#include <vector>
#include <ostream>
#include <iostream>

class Equation : public std::vector<unsigned short int>
{
public:
  Equation & operator+= (Equation &e);  
  void printPython();
  void print();
};

Equation
operator+(Equation &e1, Equation &e2);

std::vector <unsigned short int> 
addSortedVectors(Equation &v1, Equation &v2);



#endif
