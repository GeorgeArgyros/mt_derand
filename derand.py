#!/usr/bin/python


from socket import *
from struct import pack, unpack
import sys

bitMask = 0xffffffff


PHP_MT_RAND = True
MT_RAND = False

def bin(n, count=31):
    """
    Thanks stack overflow!
    """
    return "".join([str((n >> y) & 1) for y in range(count-1, -1, -1)])



class MTRand:

    N = 624
    M = 397

    def __init__(self, php = False):
        self.php = php
        self.twist = self.phpTwist if php else self.mtTwist
        self.seeded = False
        self.state = []
        self.next = self.N


    def phpTwist(self, m, u, v):
        """
        The invalid twist operation of the PHP generator
        """
        return (m ^ (((u) & 0x80000000)|((v) & 0x7fffffff))>>1) ^ \
            ((-((u) & 0x00000001)) & 0x9908b0df)


    def mtTwist(self, m, u, v):
        """
        The original mt twist operation
        """
        return (m ^ (((u) & 0x80000000)|((v) & 0x7fffffff))>>1) ^ \
            ((-((v) & 0x00000001)) & 0x9908b0df)


    def mt_srand(self, seed):
        """
        The default seeding procedure from the original MT code
        """
        self.seeded = True
        self.next = self.N
        self.state = [seed & bitMask]
        for i in range(1, self.N):
            s = (1812433253 * (self.state[i-1] ^ (self.state[i-1] >> 30))+ i)
            self.state.append(s & bitMask)
           
 
    def setState(self, state):
        """
        Replace existing state with another one and considers the 
        generator initialized
        """
        self.next = self.N
        self.state = state
        self.seeded = True


    def reload(self):
        """
        Generate the next N words of the internal state
        """        
        N = self.N
        M = self.M
        for i in range(N - M):
            self.state[i] = self.twist(self.state[M + i], self.state[i], 
                                       self.state[i+1])
        for i in range(i+1, N-1):
            self.state[i] = self.twist(self.state[i+(M-N)], self.state[i], 
                                       self.state[i+1])
        self.state[N-1] = self.twist(self.state[M-1], self.state[N-1],
                                     self.state[0])
        self.next = 0
        return


    def mt_rand(self, rmin = None, rmax = None):
        """
        Generate a 32 bit integer
        """
        if not self.seeded:
            self.mt_srand(0xdeadbeef)
            
        if self.next == self.N:
            self.reload()

        num = self.state[ self.next ]
        self.next += 1
        
        num = (num ^ (num >> 11))
        num = (num ^ ((num << 7) & 0x9d2c5680))
        num = (num ^ ((num << 15) & 0xefc60000))
        num = (num ^ (num >> 18))

        if not rmin and not rmax:
            return num
        return (rmin + (num*(rmax - rmin + 1)) / (1<<32))


    def php_mt_rand(self, rmin = None, rmax= None):
        """
        as returned by PHP
        """
        num = self.mt_rand() >> 1
        #print bin(num)
        if not rmin and not rmax:
            return num
        return rmin + (num*(rmax - rmin + 1) / (1 <<31))





class MTDerandClient:
            
    cmdLeak = 'L'
    cmdSkip = 'N'
    cmdSolve = 'S'
    cmdClear = 'C'
    cmdShutdown = 'T'
    rspOK = 'O'
    rspError = 'E'
    rspSolvable = 'S'
    rspMore = 'M'
    cmdSize = 2
    respSize = 1
    stateSize = 624
    intSize = 4


    def __init__(self, php=False):
    	"""
    	Initialize some internal variables. 
    	"""
        self.numLeaks = 0
        self.gen = MTRand(php)



    def __submitCmd(self, cmd, arg = None):
        """
        Send a command to the MT Derandomization server.
        """
        if cmd == self.cmdLeak or cmd == self.cmdSkip:
             pcmd = pack('cc', cmd, chr(arg))
        elif cmd == self.cmdSolve or cmd == self.cmdClear or cmd == self.cmdShutdown:
            pcmd =  pack('cx', cmd)
        else:
            return None        

        self.sock.send(pcmd)
        resp = self.sock.recv(self.respSize)
        
        if cmd != self.cmdSolve or resp == self.rspError:
            return resp
        
        state = self.sock.recv(self.stateSize * self.intSize)
        fmt = '%dI' % self.stateSize
        return (resp, unpack(fmt, state))       


    def __skipLeaks(self, num):
        """
        Skip a number of leaks. Because the command header is 2 bytes we
        can skip a maximum number of 255 bits with a single request.
        """
        self.numLeaks += num
        num = num * 32
        for i in range(num / 255):
            #print 'inside'
            resp = self.__submitCmd(self.cmdSkip, 255)
            if resp == self.rspError:
                return False
        last = num % 255
        #print last
        resp = self.__submitCmd(self.cmdSkip, last)
        return resp


    def __bitsFromLeak(self, leak, min = 0, max = 0x7fffffff, randMax=0x7fffffff):
        """
        Collect all the bits that we can possible recollect from the leak given that
        is was mapped to the min/max range.
        """
        bits = []
        l = ((leak-min)*(randMax+1))/(max-min+1)
        u = (((leak+1-min)*(randMax+1))/(max-min+1)) - 1
        lower = bin(l)
        upper = bin(u)
        i = 0
        while i < 31:
            if lower[i] == upper[i]:
                bits.append(int(lower[i]))
            else:
                break
            i += 1
        return bits


    def __skipLeaks(self, num):
        """
        Skip a number of leaks. Because the command header is 2 bytes we
        can skip a maximum number of 255 bits with a single request.
        """
        self.numLeaks += num
        num = num * 32
        for i in range(num / 255):
            #print 'inside'
            resp = self.__submitCmd(self.cmdSkip, 255)
            if resp == self.rspError:
                return False
        last = num % 255
        #print last
        resp = self.__submitCmd(self.cmdSkip, last)
        return resp


    def connectDerandServer(self, host='localhost', port=8080):
        """
        Connect to the derandomization server in order to         
        """
        self.sock = socket(AF_INET, SOCK_STREAM)
        self.sock.connect((host, port))


    def addLeak(self, leak, rmin = None, rmax = None, randMax=0x7fffffff):
        """
        Add a leak to the system.
        """
        self.numLeaks += 1
        solvable = False
        if rmin == None or rmax == None:
            bits = self.__bitsFromLeak(leak)
        else:
            bits = self.__bitsFromLeak(leak, rmin, rmax, randMax)
        for bit in bits:
            resp = self.__submitCmd(self.cmdLeak, bit)
            if resp == self.rspSolvable:
                solvable = True
        self.__submitCmd(self.cmdSkip, 32 - len(bits))        
        return solvable


    def recoverState(self):
        """
        Recover the state from the server and set to it the 
        state of the internal MT generator. This function will fail
        if we don't have enough leaks to recover the internal state.
        """
        (resp, state) = self.__submitCmd(self.cmdSolve)
        if resp == self.rspError:
            return False
        l =  list(state)
        self.gen.setState(list(state))
        return True    


    def advanceToCurrentState(self):
        """
        Advance the internal generator to the current position 
        skiping all the outputs we have received but send to 
        the derandomization system.
        """
        for i in range(self.numLeaks):
            self.gen.mt_rand()
        return 

    def shutdownServer(self):
    	"""
    	Shutdown the server by submitting a termination command
    	"""
    	self.__submitCmd(self.cmdShutdown)
    	return




if __name__ == '__main__':
    print 'MTDerand client and MTRand generator.'