# Makefile SuperRouting
# export MACOSX_DEPLOYMENT_TARGET=10.6

EXE = sVRP

CPXINC = -I lib/CPLEX-12.6.3/cplex/include -I lib/CPLEX-12.6.3/concert/include -DIL_STD -DNDEBUG
CPXmacFLAG = $(CPXINC) -stdlib=libstdc++ -mmacosx-version-min=10.6
CPXgccFLAG = $(CPXINC)

CPXLIB = -lilocplex -lconcert -lcplex -lm -lpthread
CPXmacLIB = -Llib/CPLEX-12.6.3/concert/lib/x86-64_osx/static_pic -Llib/CPLEX-12.6.3/cplex/lib/x86-64_osx/static_pic $(CPXLIB)
CPXgccLIB = -Llib/CPLEX-12.6.3/concert/lib/x86-64_linux/static_pic -Llib/CPLEX-12.6.3/cplex/lib/x86-64_linux/static_pic $(CPXLIB)

OS := $(shell uname)

ifeq ($(OS),Darwin)
	CC = clang++ -std=c++14 -Ofast
	CCFLAGS = -Wall -Wextra $(CPXmacFLAG)
	LIBS = lib/LKH-2.0.7/liblkh-osx.a lib/VRPH-1.0.0/libvrph-osx.a $(CPXmacLIB)
	LDD = otool -L
endif

ifeq ($(OS),Linux)
	#CC = g++ -std=c++14 -Ofast -march=sandybridge -mtune=intel
	#CC = g++ -std=c++14 -g -march=sandybridge -mtune=intel
	CC = g++ -std=c++14 -g -no-pie -march=sandybridge -mtune=intel
	CCFLAGS = -Wall -Wextra $(CPXgccFLAG)
	#CCFLAGS = -Wall -Wextra 
	LIBS = lib/LKH-2.0.7/liblkh-gcc.a lib/VRPH-1.0.0/libvrph-gcc.a $(CPXgccLIB)
	#LIBS = lib/LKH-2.0.7/liblkh-gcc.a lib/VRPH-1.0.0/libvrph-gcc.a 
	LDD = ldd
endif

SRC = $(wildcard *.cpp) 

OBJ = $(SRC:.cpp=.o)

%.o: %.cpp
	$(CC) -c $(CCFLAGS) $< -o $@

$(EXE): $(OBJ)
	$(CC) -o $@ $^ $(CCFLAGS) $(LIBS)
	$(LDD) $@

clean:
	-rm $(OBJ)
	-rm -r DerivedData

purge: clean
	-rm $(EXE)
