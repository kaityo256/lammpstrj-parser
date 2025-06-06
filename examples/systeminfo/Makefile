LAMMPS = lmp_serial
CXX = g++
CXXFLAGS = -I../../include

all: a.out

a.out: systeminfo.cpp
	$(CXX) $(CXXFLAGS) systeminfo.cpp -o a.out

run: a.out simple.lammpstrj 
	./a.out

simple.lammpstrj:
	$(LAMMPS) < simple.input

.PHONY: clean

clean:
	rm -f a.out log.lammps simple.lammpstrj
