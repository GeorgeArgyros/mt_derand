#include "mtrand.hpp"
#include <iostream>

// Global parameters
#define SEED_SIZE 4

// Mersenne Twister parameters -- stripped from the PHP source
#define hiBit(u)      ((u) & 0x80000000U)  /* mask all but highest   bit of u */
#define loBit(u)      ((u) & 0x00000001U)  /* mask all but lowest    bit of u */
#define loBits(u)     ((u) & 0x7FFFFFFFU)  /* mask     the highest   bit of u */
#define mixBits(u, v) (hiBit(u)|loBits(v)) /* move hi bit of u to hi bit of v */


static inline uint32_t
php_twist(uint32_t m, uint32_t u, uint32_t v)
{
  return (m ^ (mixBits(u,v)>>1) ^ ((uint32_t)(-(uint32_t)(loBit(u))) & 0x9908b0dfU));
}

static inline uint32_t
mt_twist(uint32_t m, uint32_t u, uint32_t v)
{
  return (m ^ (mixBits(u,v)>>1) ^ ((uint32_t)(-(uint32_t)(loBit(v))) & 0x9908b0dfU));
}


#define RAND_RANGE(__n, __min, __max, __tmax)				\
  (__n) = (__min) + (long) ((double) ( (double) (__max) - (__min) + 1.0) * ((__n) / ((__tmax) + 1.0)))

#define PHP_MT_RAND_MAX ((long) (0x7FFFFFFF)) /* (1<<31) - 1 */ 

// Some structures and Macros to make our life easier...



void 
MTRand::mtInitialize(uint32_t seed)
{
	/* Initialize generator state with seed
	   See Knuth TAOCP Vol 2, 3rd Ed, p.106 for multiplier.
	   In previous versions, most significant bits (MSBs) of the seed affect
	   only MSBs of the state array.  Modified 9 Jan 2002 by Makoto Matsumoto. */

	register uint32_t *s = mtInfo.state;
	register uint32_t *r = mtInfo.state;
	register int i = 1;
	
	*s++ = seed & 0xffffffffU;
	for( ; i < N; ++i ) {
	  *s++ = ( 1812433253U * ( *r ^ (*r >> 30) ) + i ) & 0xffffffffU;
	  r++;		
	}
}


void 
MTRand::mtReload()
{
	/* Generate N new values in state
	   Made clearer and faster by Matthew Bellew (matthew.bellew@home.com) */

	register uint32_t *p = mtInfo.state;
	register int i;
	register uint32_t (*twist)(uint32_t, uint32_t, uint32_t) = 
	  (PHPMtRand)? php_twist : mt_twist;
	

	for (i = N - M; i--; ++p)
	  *p = twist(p[M], p[0], p[1]);
	for (i = M; --i; ++p)
	  *p = twist(p[M-N], p[0], p[1]);
	*p = twist(p[M-N], p[0], mtInfo.state[0]);
	mtInfo.left = N;
	mtInfo.next = mtInfo.state;
}


void 
MTRand::mt_srand(uint32_t seed) {
  mtInitialize(seed);
  mtInfo.left = 0;

  return;
}


uint32_t 
MTRand::mt_rand()
{
  /* Pull a 32-bit integer from the generator state
     Every other access function simply transforms the numbers extracted here */
  
  register uint32_t s1;
  
  if (mtInfo.left == 0)
    mtReload();

  -- (mtInfo.left);  
  s1 = *mtInfo.next ++; 

  s1 ^= (s1 >> 11);
  s1 ^= (s1 <<  7) & 0x9d2c5680U;
  s1 ^= (s1 << 15) & 0xefc60000U;
  s1 ^= (s1 >> 18) ;
  return s1;
}


uint32_t
MTRand::php_mt_rand()
{
  return mt_rand() >> 1;
}

void 
MTRand::setState(uint32_t *s)
{
  memcpy(mtInfo.state, s, N*sizeof(uint32_t)); 
  mtInfo.left = 0;
  mtInfo.next = mtInfo.state;
}

#if 0

int main(int argc, char *argv[])
{
  MTRand gen(PHP_MT_RAND);
  uint32_t s[N];
  
  gen.mt_srand(atoi(argv[1]));
  for (int i = 0; i < 15; i ++){
    unsigned long num = gen.php_mt_rand();
    //RAND_RANGE(num, 0, 36, 0x7fffffff);
    std::cout << num << std::endl;
  }
  return 0;
  for (int i = 0; i < 1000000; i ++){
    gen.mt_srand(i);
    gen.mt_rand();
  }
  return 0;
  memcpy(s, gen.getState(), N*sizeof(uint32_t)); 

  std::cout << gen.mt_rand() << std::endl;
  std::cout << gen.mt_rand() << std::endl;
  for (int i = 0; i < 100; i ++) {
    gen.mt_rand();
  }

  gen.setState(s);  
  std::cout << gen.mt_rand() << std::endl;
  std::cout << gen.mt_rand() << std::endl;
  return 0;
}

#endif
