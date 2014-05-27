CC=g++
CFLAGS= -O3 -fno-stack-protector -funroll-loops -finline-functions
LDFLAGS= -O4
TARGETS= equation.o unroll.o gauss.o derand.o server.o mtrand.o
BIN=derand
TEST_BIN=test_components
TEST_TARGETS=equation.o unroll.o gauss.o mtrand.o test_components.o

all: $(TARGETS)
	$(CC) $(LDFLAGS) -o $(BIN) $(TARGETS) 

derand.o: derand.cpp derand.hpp
	$(CC) $(CFLAGS) -c derand.cpp

mtrand.o: mtrand.cpp mtrand.hpp
	$(CC) $(CFLAGS) -c mtrand.cpp

equation.o: equation.cpp equation.hpp
	$(CC) $(CFLAGS) -c equation.cpp

gauss.o: gauss.cpp gauss.hpp
	$(CC) $(CFLAGS) -c gauss.cpp

unroll.o: unroll.cpp unroll.hpp
	$(CC) $(CFLAGS) -c unroll.cpp

server.o: server.cpp server.hpp
	$(CC) $(CFLAGS) -c server.cpp

test_components.o: test_components.cpp
	$(CC) $(CFLAGS) -c test_components.cpp

test_components: $(TEST_TARGETS)
	$(CC) $(LDFLAGS) -o $(TEST_BIN) $(TEST_TARGETS)

test: test_components 
	./test_components 
	python test_derand.py



clean:
	rm -rf $(TARGETS) $(BITTARGETS) $(BITTEST) $(BIN) $(TEST_BIN)
