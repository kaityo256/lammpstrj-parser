# lammpstrj

A lightweight single-header C++ library for parsing LAMMPS trajectory files (`*.lammpstrj`).

## Features

- Minimal, dependency-free design
- Parses basic information such as:
  - Number of atoms
  - Simulation box size (LX, LY, LZ)
- MIT licensed

## Getting Started

### Installation

This is a single-header library. Simply copy the header file into your project and include it:

### Examples

```cpp
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
```

## License

This project is licensed under the MIT License.
