#include <cstdio>
#include <lammpstrj/lammpstrj.hpp>

int main() {
  SystemInfo si = read_info("simple.lammpstrj");
  printf("(LX, LY, LZ) = (%f, %f, %f)\n", si.LX, si.LY, si.LZ);
}