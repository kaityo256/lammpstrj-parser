LAMMPS = lmp_serial
CXX = g++
CXXFLAGS = -std=c++11 -I../../include

all: a.out

a.out: temperature.cpp
	$(CXX) $(CXXFLAGS) temperature.cpp -o a.out

run: a.out temperature.lammpstrj 
	./a.out

temperature.lammpstrj:
	$(LAMMPS) < temperature.input

.PHONY: clean

clean:
	rm -f a.out log.lammps *.lammpstrj
