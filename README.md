# lammpstrj.hpp

**lammpstrj.hpp** is a single-header C++14 library for parsing LAMMPS trajectory (`*.lammpstrj`) files.  
It provides a simple and efficient interface to access system information and analyze simulation frames.

## Features

- Header-only, single-file library
- Reads basic simulation box info (box size and number of atoms)
- Provides a per-frame callback interface for analysis
- Written in standard C++14
- Lightweight and dependency-free
- MIT License

## Getting Started

### 1. Include the header

This library is provided as a single-header C++14 library located in:

```sh
include/lammpstrj/lammpstrj.hpp
```

You can add this repository as a Git submodule to your own project:
```sh
git submodule add https://github.com/kaityo256/lammpstrj-parser external/lammpstrj-parser
```

Then, add the include directory to your compiler’s include path. For example, with `g++`:

```sh
g++ -Iexternal/lammpstrj-parser/include ...
```

Now you can include the header as:
```cpp
#include <lammpstrj/lammpstrj.hpp>
```

This approach keeps your dependencies organized and makes updates easy via submodules.

### 2. Read Simulation Box Info

You can obtain the number of atoms and the box size from the first frame using:
```cpp
auto si = lammpstrj::read_info("trajectory.lammpstrj");
printf("(LX, LY, LZ) = (%f, %f, %f)\n", si->LX, si->LY, si->LZ);
printf("N = %d\n", si->atoms);
```

The return type is a std::unique_ptr to the following structure:
```cpp
struct SystemInfo {
  int atoms;         // Number of atoms
  double LX, LY, LZ; // Size of the simulation box
};
```

### 3. Analyze Frames with a Callback

You can pass a lambda or function to `for_each_frame` to analyze each frame:
```cpp
using FrameCallback = std::function<void(const std::unique_ptr<lammpstrj::SystemInfo>&, std::vector<lammpstrj::Atom>&)>;

void lammpstrj::for_each_frame(const std::string& filename, FrameCallback callback);
```

Example: Compute kinetic energy per frame (proportional to temperature):
```cpp
lammpstrj::FrameCallback calc_temperature = [](const auto& si, const auto& atoms) {
  static int index = 0;
  double e = 0.0;
  for (auto& a : atoms) {
    e += a.vx * a.vx + a.vy * a.vy + a.vz * a.vz;
  }
  e /= static_cast<double>(si->atoms);
  e /= 3.0;
  printf("%d %f\n", index * 100, e);
  index++;
};

lammpstrj::for_each_frame("trajectory.lammpstrj", calc_temperature);
```

## Requirements

C++14 or later

## License

This library is licensed under the MIT License. See the[LICENSE](LICENSE) file for details.
