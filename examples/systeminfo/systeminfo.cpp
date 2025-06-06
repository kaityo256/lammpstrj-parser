#include <cstdio>
#include <lammpstrj/lammpstrj.hpp>

int main() {
  lammpstrj::SystemInfo si = lammpstrj::read_info("simple.lammpstrj");
  printf("(LX, LY, LZ) = (%f, %f, %f)\n", si.LX, si.LY, si.LZ);
  printf("N = %d\n", si.atoms);
}