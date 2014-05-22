#ifndef __BIT_ARRAY__
#define __BIT_ARRAY__

//#include "unroll.hpp"

#include <cstdlib>
#include <cstring>

extern "C" {
#include <stdint.h>
}


#define PHP_MT_RAND true
#define MT_RAND false



#define N             (624)                /* length of state vector */
#define M             (397)                /* a period parameter */

typedef struct {
  uint32_t state[N];
  uint32_t left;
  uint32_t *next;  
} MTState;


class MTRand
{
private:
  MTState mtInfo;
  bool PHPMtRand;

  void mtInitialize(uint32_t);
  void mtReload();  
public:
  MTRand(bool php = false) { PHPMtRand = php; }
  void mt_srand(uint32_t);  
  void setState(uint32_t *s);
  uint32_t mt_rand();
  uint32_t php_mt_rand();
  uint32_t *getState() { return  mtInfo.state; }
};

#endif
