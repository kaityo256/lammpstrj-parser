LAMMPS = lmp_serial
CXX = g++
CXXFLAGS = -std=c++14 -I../../include

all: a.out

a.out: condensation.cpp
	$(CXX) $(CXXFLAGS) condensation.cpp -o a.out

run: a.out condensation.lammpstrj 
	./a.out

condensation.lammpstrj:
	$(LAMMPS) < condensation.input

.PHONY: clean

clean:
	rm -f a.out log.lammps *.lammpstrj
