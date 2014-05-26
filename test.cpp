#include <cstdlib>

#include "unroll.hpp"
#include "gauss.hpp"
#include "derand.hpp"
#include "test.hpp"
#include "mtrand.hpp"



/* TCPConnection Code */

bool
TCPConnection::tcpConnect(std::string ipaddr, unsigned short int port)
{
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  memcpy((char *)&addr.sin_addr.s_addr, (char *)ipaddr.c_str(), ipaddr.length());


  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
    return false;

  int flag = connect(sock, (struct sockaddr *)&addr, sizeof(addr));
  if (flag < 0) 
    return false;

  return true;
}

int
TCPConnection::csend(std::string data)
{
  int flag, done = 0, left = data.length();
  const char *dataPtr = data.c_str();

  while (left > 0)
  {
    flag = send(sock, dataPtr + done, left, 0);
    if (flag < 0)
      return done;
    done += flag;
    left -= flag;
  }

  return done;
}


int 
TCPConnection::crecv(std::string &data, unsigned int len)
{
  int flag, done = 0, left = len;
  char buffer[1024];

  while (left > 0)
  {

    flag = recv(sock, buffer, 1024, 0);
    if (flag <= 0)
      return done;
    data += buffer;
    left -= flag;
    done += flag;
  }

  return done;
}


void
TCPConnection::closeConnection()
{
  close(sock);
}






void debug() {}	

/*
 * This structure provides bit level access to the state array.
 */
struct BitArray
{
  unsigned int state[624];
  
  bool operator[](unsigned int index)
  {
    return (state[index/32] & (1 << (31 - (index % 32))));
  }
};



uint32_t *
convertToMTState(std::vector <bool> sol)
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
convertToPHPState(std::vector <bool> sol)
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



bool testUnroller()
{
	std::cout <<"==-Testing Linear Equations Produced for Mersenne Twister-==" 
		<< std::endl;


	for (unsigned short int php = 0; php < 2; php++)
	{

		if (!php) 
			std::cout << "[+] Testing the equations for the MT generator: ";
		else
			std::cout << "[+] Testing the PHP MT generator: ";

		MTRand gen(php);
		MTUnroller unroll(php);
		BitArray bs;
		unsigned short int adder = (php)? 30 : 31;
		unsigned short int parity = 0;


		srand(time(NULL));
  		gen.mt_srand(rand());
		for (int i = 0; i < 624; i ++)
			bs.state[i] = gen.getState()[i];

		for (int i = 0; i < 2000; i ++) {
    		uint32_t num = gen.php_mt_rand();
    		for (int j = 0; j < 10; j ++){
    			parity = 0;
      			unsigned short int  bit = (num & (1 << 30-j))>>30-j;
      			Equation eq = unroll.getNextEquation();
      	
      			for (int k = 0; k < eq.size(); k ++)
        			parity ^= bs[(eq[k] > 0)? eq[k] + adder : 0];
      		

     	 		if (parity != bit)
     	 		{ 
      				std::cout << "Failed" << std::endl;
      				return false;
      			}
      		}
      		unroll.skipEquations(22);
    	}

		std::cout << "OK" << std::endl;
	}
		return true;
}


bool testSolver()
{
	std::cout <<" ==-Testing Gaussian Solver for state recovery-== " 
		<< std::endl;


	for (unsigned short int php = 0; php < 2; php++)
	{

		if (!php) 
			std::cout << "[+] Testing state recovery of MT generator: ";
		else
			std::cout << "[+] Testing state recovery of the PHP MT generator: ";

		MTRand gen(php);
		GaussianEliminator solver((php)?19938 : 19937);
		MTUnroller unroll(php);
		BitArray bs;
		unsigned short int adder = (php)? 30 : 31;
		unsigned short int parity = 0;
		uint32_t buffer[10];

		srand(time(NULL));
  		gen.mt_srand(rand());
		for (int i = 0; i < 624; i ++)
			bs.state[i] = gen.getState()[i];

		for (int i = 0; i < 25000; i ++) 
		{
    		uint32_t num = gen.php_mt_rand();
    		if (i < 10)
    			buffer[i] = num;
    		for (int j = 0; j < 10; j ++)
    		{
    			parity = 0;
      			unsigned short int  bit = (num & (1 << 30-j))>>30-j;
      			Equation eq = unroll.getNextEquation();
      	
      			solver.addEquation(eq, bit);
      			if (solver.isSolvable()) 
      			{
      				uint32_t *state;
      				if (!php)
      					state = convertToMTState(solver.getSolution());
      				else
      					state = convertToPHPState(solver.getSolution());		
      				gen.setState(state);

      				for (int k = 0; k < 10; k ++)
      					if (buffer[k] != gen.php_mt_rand())
      					{
      						std::cout << "Failed" << std::endl;
      						goto LoopEnd;
						}    					
      				std::cout << "OK" << std::endl;
      				goto LoopEnd;
      			}

      		}
      		unroll.skipEquations(22);
    	}

 LoopEnd:
 		debug();
	}
		return true;
		
}

/*
bool testDerandServer()
{
	int i;

	std::cout << "==- Testing Derandomization Server -==" << std::endl;

	for (unsigned short int php = 0; php <= 1; php ++)
	{
		MTDerand derand(php);
		uint32_t buffer[10];
		MTRand gen;
		std::string cmd,resp;
		TCPConnection conn;


		if (!fork())
		{
			derand.start();
			exit(1);
		}

		if (php)
			std::cout << "Testing Derandomization for the PHP Generator: ";
		else
			std::cout <<"Testing Derandomization for the MT Generator: ";


		srand(time(NULL));
  		gen.mt_srand(rand());
		for (i = 0; i < 10; i ++)
			buffer[i] = gen.php_mt_rand();

		i = 0;
		conn.tcpConnect(std::string("127.0.0.1"), 8080);
		while (true)
		{
			std::cout << i << std::endl;
			i ++;
			cmd += MTDerand::cmdLeak;
			cmd += (char) (gen.php_mt_rand() >> 30);


			conn.csend(cmd);
			conn.crecv(resp, 1);
			if (resp[0] == MTDerand::rspSolvable)
				break;


			cmd = MTDerand::cmdSkip;
			cmd += (char) 31;
			conn.csend(cmd);
			conn.crecv(resp, 1);
		}

		return true;
		//solve system
		//recover state
		//produce new leaks

		for (i = 0; i < 10; i ++) 
			if (gen.php_mt_rand() != buffer[i])
			{
				std::cout << "Failed. " << std::endl;
				break;
			}
		if (i == 10)
			std::cout << "OK. " << std::endl;

	}
	return true;
}
*/



int main()
{
	if (!testUnroller()  || !testSolver())
		return 1;
	return 0;
}




