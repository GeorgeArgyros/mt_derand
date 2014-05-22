#include "equation.hpp"

std::vector <unsigned short int>
addSortedVectors(Equation &v1, Equation &v2)
{
  std::vector <unsigned short int> result;
  std::vector <unsigned short int>::iterator it1 = v1.begin();
  std::vector <unsigned short int>::iterator it2 = v2.begin();
  std::vector <unsigned short int>::iterator end1 = v1.end();
  std::vector <unsigned short int>::iterator end2 = v2.end();

  result.reserve(v1.size() + v2.size());
  while (it1 < end1 || it2 < end2) {
    if (it1 < end1 && it2 < end2) {
      
      if (*it1 < *it2) {
	result.push_back(*it1);
	it1 ++;
      } else if (*it2 < *it1) {
	result.push_back(*it2);
	it2 ++;
      } else {
	it1 ++;
	it2 ++;
      }
	
    } else if (it1 < end1) {
      result.push_back(*it1);
      it1 ++;
    } else if (it2 < end2) {
      result.push_back(*it2);
      it2 ++;
    }        
  }
  return result;
}

  
Equation &
Equation::operator+=(Equation &e)
{

  /*
   * This can be possibly optimized a bit, by doing the operations on the
   * vector itself instead of copying the result afterwards
   */
  std::vector <unsigned short int> result;
  std::vector <unsigned short int>::iterator it1 = this->begin();
  std::vector <unsigned short int>::iterator it2 = e.begin();
  std::vector <unsigned short int>::iterator end1 = this->end();
  std::vector <unsigned short int>::iterator end2 = e.end();

  result.reserve(this->size() + e.size());
  while (it1 < end1 || it2 < end2) {
    if (it1 < end1 && it2 < end2) {
      
      if (*it1 < *it2) {
	result.push_back(*it1);
	it1 ++;
      } else if (*it2 < *it1) {
	result.push_back(*it2);
	it2 ++;
      } else {
	it1 ++;
	it2 ++;
      }
	
    } else if (it1 < end1) {
      result.push_back(*it1);
      it1 ++;
    } else if (it2 < end2) {
      result.push_back(*it2);
      it2 ++;
    }        
  }



  assign(result.begin(), result.end());
  return *this;
}

Equation operator+(Equation &e1, Equation &e2)
{
  Equation result;
  Equation::iterator it1 = e1.begin();
  Equation::iterator it2 = e2.begin();
  Equation::iterator end1 = e1.end();
  Equation::iterator end2 = e2.end();

  // maybe wasting some space but improves performance
  result.reserve(e1.size() + e2.size());
  while (it1 < end1 || it2 < end2) {
    if (it1 < end1 && it2 < end2) {
      
      if (*it1 < *it2) {
	result.push_back(*it1);
	it1 ++;
      } else if (*it2 < *it1) {
	result.push_back(*it2);
	it2 ++;
      } else {
	it1 ++;
	it2 ++;
      }
	
    } else if (it1 < end1) {
      result.push_back(*it1);
      it1 ++;
    } else if (it2 < end2) {
      result.push_back(*it2);
      it2 ++;
    }        
  }
  return result;
}

void
Equation::print()
{
  std::cout <<" ";
  for (Equation::iterator it = this -> begin();
       it != this -> end() - 1; it ++)
    std::cout << *it << " + ";
  std::cout << at(this -> size() - 1) << std::endl;
  return;
}

void
Equation::printPython()
{
  std::cout << "[";
  for (Equation::iterator it = this -> begin();
       it != this -> end() - 1; it ++)
    std::cout << *it << ",";
  std::cout << at(this->size() - 1) << "]" << std::endl;
  return;
}
