LAMMPS = lmp_serial
CXX = g++
CXXFLAGS = -I../../include

all: a.out

a.out: systeminfo.cpp
	$(CXX) $(CXXFLAGS) systeminfo.cpp -o a.out

run: a.out systeminfo.lammpstrj 
	./a.out

systeminfo.lammpstrj:
	$(LAMMPS) < systeminfo.input

.PHONY: clean

clean:
	rm -f a.out log.lammps *.lammpstrj
