#include "unroll.hpp"

#include <cstdlib>

MTUnroller::MTUnroller(bool php)
  : N(624), M(397), a(0x9908b0df), stateSize(19968), currentPos(0), update(true)
{
  fillTempering();
  varArray = new Equation[stateSize];
  
  //State is actually 19937 bits but word aligning the size allows better indexing
  // when generating the equations, thus the -31 in the initial values.  
  mtPHP = php;
  if (php) {
    varArray[0].push_back(0);
    varArray[31].push_back(1);
    for (int i = 32; i < stateSize; i ++)
      varArray[i].push_back(i -  30);
  } else {
    varArray[0].push_back(0);
    for (int i = 32; i < stateSize; i ++)
      varArray[i].push_back(i -  31);
      }
  return;
}

void
MTUnroller::latexPrintTempering()
{
  int j;
  for (int i = 0; i < 32; i ++) {
    std::cout << "$$ z_{" << i << "} = "; 
    for (j = 0; j < tempering[i].size()-1; j ++)
      std::cout <<"x_{" << tempering[i][j] << "} \\oplus ";
    std::cout <<"x_{" << tempering[i][j] << "} $$" << std::endl;    
  }
  return;
}

void
MTUnroller::fillTempering()
{
  unsigned short int sizes[] = {4,3,5,2,7,5,5,7,3,5,3,2,4,6,4,2,2,4,4,7,
				7,4,9,5,9,7,5,7,3,4,4,6};

  unsigned short int t[][10] = {{0, 4, 7, 15},
				{1, 5, 16},
				{2, 6, 13, 17, 24},
				{3, 10},
				{0, 4, 8, 11, 15, 19, 26},
				{1, 5, 9, 12, 20},
				{6, 10, 17, 21, 28},
				{3, 7, 11, 14, 18, 22, 29},
				{8, 12, 23},
				{9, 13, 20, 24, 31},
				{6, 10, 17},
				{0, 11},
				{1, 8, 12, 19},
				{2, 9, 13, 17, 20, 28},
				{3, 14, 18, 29},
				{4, 15},
				{5, 16},
				{6, 13, 17, 24},
				{0, 4, 15, 18},
				{1, 5, 8, 15, 16, 19, 26},
				{2, 6, 9, 13, 17, 20, 24},
				{3, 17, 21, 28},
				{0, 4, 8, 15, 18, 19, 22, 26, 29},
				{1, 5, 9, 20, 23},
				{6, 10, 13, 17, 20, 21, 24, 28, 31},
				{3, 7, 11, 18, 22, 25, 29},
				{8, 12, 15, 23, 26},
				{9, 13, 16, 20, 24, 27, 31},
				{6, 10, 28},
				{0, 11, 18, 29},
				{1, 8, 12, 30},
				{2, 9, 13, 17, 28, 31}};
  

  for (int i = 0; i < 32; i ++ )
    tempering[i].assign(t[i], t[i] + sizes[i]);
  
  return;

}

void
MTUnroller::generateXi()
{
  unsigned short int word = currentPos / 32;
  std::vector <Equation> neweq;
  unsigned short int adder = (mtPHP == true)? 0 : 1;

  for (int i = 0; i < 32; i ++ ) 
    switch (i) {
    case 0:
      neweq.push_back(varArray[((word + M) % N)*32] + 
		      varArray[((word + adder) % N)*32 + 31]);
      break;
    case 1:
      neweq.push_back(varArray[((word + M) % N)*32 + 1] + varArray[word  * 32]);
      break;
    default:      
      neweq.push_back(varArray[((word + M) % N)*32 + i] + varArray[((word + 1) % N) * 32 + i - 1]);
      if (a & (1 << (31 - i)))
	neweq[i] +=  varArray[((word + adder) % N)*32 + 31];
      break;
    }
  
  /* If we update directly the original array, we will be using the new 
     equations instead of the old ones that we need in some bit positions */
  for (int i = 0; i < 32; i ++ ){
    varArray[word*32 + i] = neweq[i];    
  }
  return;
}


Equation
MTUnroller::getNextEquation()
{
  unsigned short int bit = currentPos % 32;
  unsigned short int word = currentPos / 32;
  Equation eq; 
  

  // generate the xis for the current word...
  if (update) {    
    generateXi();
    update = false;
  }
 
  for (std::vector <unsigned short int>::iterator it = tempering[bit].begin();
       it != tempering[bit].end(); it ++) {    
    eq += varArray[word*32 + *it];        
  }

  if (bit == 31)
    update = true;
  
  currentPos = (currentPos + 1) % stateSize;
  return eq;
}


/*
 * Dump implementation
 */
void
MTUnroller::skipEquations( unsigned short int num )
{
  for (int i = 0; i < num; i ++) {
    unsigned short int bit = currentPos % 32;
    
    if (update){
      generateXi();
      update = false;
    }
    
    if (bit == 31)
      update = true;

    currentPos = (currentPos + 1) % stateSize;
  }
  return;
} 

void
MTUnroller::clear()
{
  
  for (int i = 0; i < stateSize; i ++) {
    varArray[i].clear();
    varArray[i].push_back(i);
  }
  currentPos = 0;
  update = true;
  return;
}

void
MTUnroller::dumpVarArray()
{
  for (int i = 0; i < stateSize; i ++)
    varArray[i].print();
  return;
}

#if 0
int main(int argc, char *argv[])
{
  MTUnroller roller;
  int i = 0,skip = atoi(argv[1]);
  
  while (i < 19937){
    for (int j = 0; j < skip; j++) {
      roller.getNextEquation().printPython();
      i ++;
    }
    roller.skipEquations(32-skip);  
  }
}
#endif
