#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace lammpstrj {

struct Atom {
  int type;
  double x, y, z;
  double vx, vy, vz;
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

void for_each_frame(const std::string &filename,
                    std::function<void(const std::vector<Atom> &)> callback) {
  SystemInfo si = read_info(filename);
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Failed to open file: " << filename << std::endl;
    return;
  }

  std::string line;
  std::vector<std::string> current_labels;
  std::vector<Atom> atoms;
  bool reading_atoms = false;
  int atom_count = 0;

  while (std::getline(file, line)) {
    if (line.find("ITEM: TIMESTEP") != std::string::npos) {
      // フレームが始まる → 前のフレームがあれば callback を呼ぶ
      if (!atoms.empty()) {
        callback(atoms);
        atoms.clear();
      }
      reading_atoms = false;
      atom_count = 0;
    } else if (line.find("ITEM: ATOMS") != std::string::npos) {
      // 項目のラベルを取得
      current_labels.clear();
      std::istringstream iss(line);
      std::string word;
      while (iss >> word) {
        if (word != "ITEM:" && word != "ATOMS") {
          current_labels.push_back(word);
        }
      }
      reading_atoms = true;
    } else if (reading_atoms && atom_count < si.atoms) {
      std::istringstream iss(line);
      Atom atom;
      std::unordered_map<std::string, double *> value_map = {
          {"x", &atom.x}, {"y", &atom.y}, {"z", &atom.z}, {"vx", &atom.vx}, {"vy", &atom.vy}, {"vz", &atom.vz}};

      std::string id_token; // idは使わないが読み飛ばす必要あり
      iss >> id_token;

      for (const std::string &label : current_labels) {
        if (label == "id") {
          continue; // 既に読み取った
        }

        double val;
        iss >> val;
        auto it = value_map.find(label);
        if (it != value_map.end()) {
          *(it->second) = val;
        }
      }

      // 座標のスケーリングと周期境界補正
      atom.x *= si.LX;
      atom.y *= si.LY;
      atom.z *= si.LZ;

      if (atom.x < 0.0) atom.x += si.LX;
      if (atom.x > si.LX) atom.x -= si.LX;
      if (atom.y < 0.0) atom.y += si.LY;
      if (atom.y > si.LY) atom.y -= si.LY;
      if (atom.z < 0.0) atom.z += si.LZ;
      if (atom.z > si.LZ) atom.z -= si.LZ;

      atoms.push_back(atom);
      ++atom_count;
    }
  }

  // 最後のフレームが残っていれば callback を呼ぶ
  if (!atoms.empty()) {
    callback(atoms);
  }

  file.close();
}

} // namespace lammpstrj
