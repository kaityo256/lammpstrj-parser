#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace lammpstrj {

struct Atom {
  int type;
  double x, y, z;
};

struct SystemInfo {
  int atoms;         // Number of atoms
  double LX, LY, LZ; // Size of the simulation box
};

SystemInfo read_info(const std::string filename) {
  std::ifstream file(filename);
  SystemInfo info{};

  if (!file.is_open()) {
    std::cerr << "Failed to open file: " << filename << std::endl;
    return info;
  }

  std::string line;
  bool atoms_found = false;
  bool box_found = false;

  while (std::getline(file, line)) {
    if (!atoms_found &&
        line.find("ITEM: NUMBER OF ATOMS") != std::string::npos) {
      std::getline(file, line);
      info.atoms = std::stoi(line);
      atoms_found = true;
    }

    if (!box_found && line.find("ITEM: BOX BOUNDS") != std::string::npos) {
      // The next 3 lines contain the box size information
      double x_min, x_max, y_min, y_max, z_min, z_max;
      std::getline(file, line);
      std::istringstream(line) >> x_min >> x_max;
      info.LX = x_max - x_min; // Size in X direction

      std::getline(file, line);
      std::istringstream(line) >> y_min >> y_max;
      info.LY = y_max - y_min; // Size in Y direction

      std::getline(file, line);
      std::istringstream(line) >> z_min >> z_max;
      info.LZ = z_max - z_min; // Size in Z direction

      box_found = true;
    }

    // Exit loop once required information has been found
    if (atoms_found && box_found) {
      break;
    }
  }

  file.close();
  return info;
}

} // namespace lammpstrj
