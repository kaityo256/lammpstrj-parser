LAMMPS=lmp_serial

all: a.out

a.out: systeminfo.cpp
	g++ systeminfo.cpp

run: a.out simple.lammpstrj 
	./a.out

simple.lammpstrj:
	lmp_serial < simple.input

.PHONY: clean

clean:
	rm -f a.out log.lammps simple.lammpstrj
