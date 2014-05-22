#include "gauss.hpp"

GaussianEliminator::GaussianEliminator(unsigned int v) 
  : rowsUnset(v) 
{
  vars = v;
  rows = new Row[vars];
  constTerms = new bool[vars];
}


bool
GaussianEliminator::addEquation(Equation eq, bool term)
{
  Row nrow; 

  while(eq.size()) {
    unsigned short int fp = eq[0];
    if (rows[fp].set){
      eq += rows[fp].eq;
      term ^= constTerms[fp];      
    } else {
      rows[fp].set = true;
      rows[fp].eq = eq;
      constTerms[fp] = term;
      rowsUnset --;
      break;
    }        
  }

  if (isSolvable())
    return true;

  return false;
}


std::vector <bool> 
GaussianEliminator::getSolution()
{
  std::vector <bool> sol(vars, false) ;  

  if (!isSolvable())
    throw;
			 

  for (int k = vars-1; k >= 0; k --){
    unsigned int size = rows[k].eq.size();
    bool parity = false;    
    
    for (int j = size - 1; j > 0; j --)
      parity ^= sol[rows[k].eq[j]];          
		 
    sol[k] = parity ^ constTerms[k];
  }
  
  return sol;
}

void
GaussianEliminator::clear()
{ 
  for (unsigned int i = 0; i < vars; i ++)
    rows[i].set = false; 
  rowsUnset = vars;
  return;
}

void
GaussianEliminator::printStats()
{
  std::cout << "Rows Unset: " << rowsUnset << std::endl;
  return;
}


#if 0
int main()
{
  GaussianEliminator terminator(4);
  unsigned short int r1[] = {0, 1, 2};
  unsigned short int r2[] = {1, 3};
  unsigned short int r3[] = {0, 1};
  unsigned short int r4[] = {3};

  Equation eq;
  std::copy ( r1, r1 + 3, std::back_inserter ( eq ) );
  terminator.addEquation(eq, true);
  eq.clear();
  std::copy ( r2, r2 + 2, std::back_inserter ( eq ) );
  terminator.addEquation(eq, true);
  eq.clear();
  std::copy ( r3, r3 + 2, std::back_inserter ( eq ) );
  terminator.addEquation(eq, false);
  eq.clear();
  std::copy ( r4, r4 + 1, std::back_inserter ( eq ) );
  terminator.addEquation(eq, true);
    
  std::vector <bool> sol = terminator.getSolution();
  for (std::vector <bool>::iterator it = sol.begin();
       it < sol.end(); it ++)
    std::cout << *it << std::endl;

  return 0;
}
#endif
