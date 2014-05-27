import subprocess
from time import sleep	
from random import randrange
from derand import MTDerandClient, MTRand

MIN_MAX = 10000
MAX_MIN = 100000000
RANDMAX = 0x7fffffff

PHP_MT_RAND=True
MT_RAND = False
DERAND_SERVER='./derand'

def main():
	"""
	"""
	print '==- Testing derandomization client python API -=='
	devnull = open('/dev/null', 'w')
	for php in range(2):

		args = [DERAND_SERVER, "--daemon"]
		if php == True:
			args.append('--php')
		subprocess.call(args, stdout=devnull)
		sleep(1) # Give time to the server to startup

		if php == True:
			print '[+] Testing MTDerandClient on PHP Generator:',
		else:
			print '[+] Testing MTDerandClient on MT Generator:',

		seed = randrange(1 << 31)
		derand = MTDerandClient(php)
		gen = MTRand(php)
		randInts = []
		gen.mt_srand(seed)
		for i in range(10):
			randInts.append(gen.php_mt_rand())

		gen.mt_srand(seed)

		# Will Connect to the derandomization server, default arguments
		# are localhost:8080.
		derand.connectDerandServer()
		low_end = randrange(MIN_MAX)
		high_end = randrange(MIN_MAX+1,MAX_MIN)
		while True:
			num = gen.php_mt_rand(low_end,high_end)

			# add the leak obtained to the system. We need to provide also
			# the range in which that number was mapped.
			if derand.addLeak(num,low_end,high_end) == True:
				break

		# When add leak returns True we can recover the generators internal state
		# and also set it to the derandomization system's internal generator, to 
		# get the same output as the generator.
		derand.recoverState()

		msg = 'OK'
		for i in range(10):
			if derand.gen.php_mt_rand() != randInts[i]:
				msg = 'Failed' 
				break
		print msg

		# Will respawn for next iteration if needed.
		derand.shutdownServer()
		sleep(1) # Give time to the server to shutdown.
	devnull.close()

if __name__ == '__main__':
	main()