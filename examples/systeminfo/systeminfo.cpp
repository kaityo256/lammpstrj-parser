#include <cstdio>
#include <lammpstrj/lammpstrj.hpp>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " input.lammpstrj" << std::endl;
    return 1;
  }

  std::string filename = argv[1];
  auto si = lammpstrj::read_info(filename);
  if (!si) {
    std::cerr << "Error: Could not read file: " << filename << std::endl;
    return 1;
  }

  printf("(LX, LY, LZ) = (%f, %f, %f)\n", si->LX, si->LY, si->LZ);
  printf("N = %d\n", si->atoms);
  return 0;
}
