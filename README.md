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

### Example

```cpp
#include <cstdio>
#include <lammpstrj/lammpstrj.hpp>

int main() {
  lammpstrj::SystemInfo si = lammpstrj::read_info("simple.lammpstrj");
  printf("(LX, LY, LZ) = (%f, %f, %f)\n", si.LX, si.LY, si.LZ);
  printf("N = %d\n", si.atoms);
}
```

## License

This project is licensed under the MIT License.
