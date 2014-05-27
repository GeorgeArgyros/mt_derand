mt_derand
=========

Mersenne Twister is one of the most popular pseudorandom number generators. 
Although the generator is by design not cryptographically secure many 
applications tend to use it for cryptographic operations. Specifically, 
many web applications tend to use Mersenne Twister as it is the default 
PRNG in the PHP core system. This led in many attacks in PHP applications 
in the past, see for example the paper: 
"I Forgot Your Password: Randomness Attacks in PHP applications"

In a nutshell Mersenne Twister generator is a linear generator. This means that
the generator can be expressed as a linear equation of its internal state or 
equally we can express its state transformation with a matrix multiplication 
on a matrix A and the state S. The matrix A is non-singular and therefore the
inverse matrix A^{-1} can give us back the internal state from the output of
the generator. However, most of the times, particularly in web applications the
output of the generator is truncated and the attacker is able to obtain only
a few bits of each output word. This breaks the trivial technique of obtaining
the internal state described above. 

The MTDerand code implements the state recovery attack described in the paper.
In order to bypass this problem, we express each bit of the output as
a linear quation over the internal state bits. Then, enough outputs will give
us the internal state of the generator. Since the equations we will obtain 
might be different over different applications and even different seeds of the
generator, an online version of gaussian elimination is implemented were 
equations are added as obtained by the applications and when enough information
is gathered we solve the system uniquely and recover the internal state. 


Generators supported
====================

The Mersenne Twister is used under many different variations to produce different
PRNGs. For example, PHP and python both use the Mersenne Twister but the output is
different due to variations of the generators. In particular, the PHP generator 
resembles the original MT19937 generator but has minor differences due to a bug
in the code. The MTDerand code supports both the original MT19937 generator and
the PHP variation of the generator.


API
===

In order to use the API to the derandomization system there are two main systems:

Derand: This is the server side of the system, implementing the aforementioned 
attack. It accepts output bits from the generator and when enough bits are 
obtained returns the internal state of the generator at the time the first 
bit was received. Since Gaussian elimination can be an expensive procedure, 
the derand binary runs as a server application which bounds to a specified port
(defaults at port 8080) and accepts the generators bits over the network. This
way, the server can be mounted on a fast system accepting outputs from a client
over the internet.


Derand.py: The derand.py file contains the python API which is to be used as a 
client for the derandomization server. It exports the MTDerandClient and MTRand
classes. The MTDerandClient contains all the necessary methods to invert the 
Mersenne Twister generator and then use the reconstructed state of the generator
while the MTRand class contains both the MT19937 generator and the PHP generator.
For details in how to use the client consult the code at test_derand.py file.



Build
=====

To build the system run 

$ make
$ make test

The second command will run a number of tests to ensure that the derandomization
system works properly. 

Afterwards, the derandomization server should be executed.

$ ./derand --help

This will print a help menu with available options. By default the server will bind
at port 8080 and use the MT19937 generator. To use the PHP generator the --php option
should be used. 

$ ./derand --php 

Finally, the exploits coded in python can use the derand.py module by importing the
necessary classes in order to use the MTDerand server. For example the code

from derand import MTDerandClient,MTRand

allow to use both important classes of the client which provide an easy interface to
the derandomization server. Afterwards, one can simply feed the interface outputs of
the generator and when enough outputs are collected the MTDerandClient internal 
generator will receive a copy of the target generator's internal state which can
then be used to produce the same outputs as the target generator.



Licence
=======

This software was written based on research funded by the ERC project CODAMODA
and is released under the New BSD License.


Final Comments
==============

The release of this code was scheduled for 2012, however due to various reason the
relase was greatly delayed. Apologies to all people urged to use this code :).

In addition, this code is still in testing phase. There will be  additions
to improve both performance and documentation of the code in the following months.



