// lammpstrj.hpp
//
// Copyright (c) 2025 H. Watanabe
//
// This file is licensed under the MIT License.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace lammpstrj {

struct Atom {
  int type = 0;
  int id = 0;
  double x = 0.0, y = 0.0, z = 0.0;
  double vx = 0.0, vy = 0.0, vz = 0.0;
};

struct SystemInfo {
  int atoms; // Number of atoms
  double x_min, x_max, y_min, y_max, z_min, z_max;
  double LX, LY, LZ; // Size of the simulation box
};

std::unique_ptr<SystemInfo> read_info(const std::string filename) {
  std::ifstream file(filename);
  auto si = std::make_unique<SystemInfo>();

  if (!file.is_open()) {
    return nullptr;
  }

  std::string line;
  bool atoms_found = false;
  bool box_found = false;

  while (std::getline(file, line)) {
    if (!atoms_found &&
        line.find("ITEM: NUMBER OF ATOMS") != std::string::npos) {
      std::getline(file, line);
      si->atoms = std::stoi(line);
      atoms_found = true;
    }

    if (!box_found && line.find("ITEM: BOX BOUNDS") != std::string::npos) {
      // The next 3 lines contain the box size information
      std::getline(file, line);
      std::istringstream(line) >> si->x_min >> si->x_max;
      si->LX = si->x_max - si->x_min; // Size in X direction

      std::getline(file, line);
      std::istringstream(line) >> si->y_min >> si->y_max;
      si->LY = si->y_max - si->y_min; // Size in Y direction

      std::getline(file, line);
      std::istringstream(line) >> si->z_min >> si->z_max;
      si->LZ = si->z_max - si->z_min; // Size in Z direction

      box_found = true;
    }

    // Exit loop once required information has been found
    if (atoms_found && box_found) {
      break;
    }
  }

  file.close();
  return si;
}

using FrameCallback = std::function<void(const std::unique_ptr<lammpstrj::SystemInfo> &,
                                         std::vector<lammpstrj::Atom> &)>;

void for_each_frame(const std::string &filename, std::function<void(const std::unique_ptr<SystemInfo> &is, std::vector<lammpstrj::Atom> &)> callback) {

  auto si = read_info(filename);
  const int N = si->atoms;

  std::ifstream fin(filename);
  if (!fin.is_open()) {
    std::cerr << "Error: Cannot open file " << filename << std::endl;
    std::abort();
  }

  std::string line;
  bool in_atoms_section = false;
  std::vector<std::string> fields;
  std::unordered_map<std::string, int> field_indices;

  std::vector<Atom> atoms;

  while (std::getline(fin, line)) {
    if (line.find("ITEM: TIMESTEP") != std::string::npos) {
      atoms.assign(N, Atom());
      fields.clear();
      field_indices.clear();
      in_atoms_section = false;
    } else if (line.find("ITEM: ATOMS") != std::string::npos) {
      std::istringstream ss(line);
      std::string token;
      ss >> token >> token; // skip "ITEM: ATOMS"
      while (ss >> token) {
        fields.push_back(token);
      }

      // idが存在するか確認
      if (std::find(fields.begin(), fields.end(), "id") == fields.end()) {
        std::cerr << "Error: id field is required in ATOMS section." << std::endl;
        std::abort();
      }

      // 各フィールドのインデックスを記録
      for (size_t i = 0; i < fields.size(); ++i) {
        field_indices[fields[i]] = static_cast<int>(i);
      }

      in_atoms_section = true;

      for (int i = 0; i < N; ++i) {
        if (!std::getline(fin, line)) {
          std::cerr << "Error: Unexpected end of file while reading atom data." << std::endl;
          std::abort();
        }

        std::istringstream datastream(line);
        std::vector<std::string> tokens;
        std::string val;
        while (datastream >> val)
          tokens.push_back(val);

        if ((int)tokens.size() < (int)fields.size()) {
          std::cerr << "Error: Not enough fields in ATOMS data line." << std::endl;
          std::abort();
        }

        Atom atom;
        int id = std::stoi(tokens[field_indices["id"]]);
        atom.id = id;

        if (field_indices.count("type")) atom.type = std::stoi(tokens[field_indices["type"]]);

        // x, y, z（without scaling）
        if (field_indices.count("x")) atom.x = std::stod(tokens[field_indices["x"]]);
        if (field_indices.count("y")) atom.y = std::stod(tokens[field_indices["y"]]);
        if (field_indices.count("z")) atom.z = std::stod(tokens[field_indices["z"]]);

        // xs, ys, zs（with scaling)
        if (field_indices.count("xs")) atom.x = std::stod(tokens[field_indices["xs"]]) * si->LX;
        if (field_indices.count("ys")) atom.y = std::stod(tokens[field_indices["ys"]]) * si->LY;
        if (field_indices.count("zs")) atom.z = std::stod(tokens[field_indices["zs"]]) * si->LZ;

        // Periodic boundary correction
        if (atom.x < 0) atom.x += si->LX;
        if (atom.x > si->LX) atom.x -= si->LX;
        if (atom.y < 0) atom.y += si->LY;
        if (atom.y > si->LY) atom.y -= si->LY;
        if (atom.z < 0) atom.z += si->LZ;
        if (atom.z > si->LZ) atom.z -= si->LZ;

        if (field_indices.count("vx")) atom.vx = std::stod(tokens[field_indices["vx"]]);
        if (field_indices.count("vy")) atom.vy = std::stod(tokens[field_indices["vy"]]);
        if (field_indices.count("vz")) atom.vz = std::stod(tokens[field_indices["vz"]]);

        if (id < 1 || id > N) {
          std::cerr << "Error: Invalid atom id " << id << std::endl;
          std::abort();
        }

        atoms[id - 1] = atom;
      }

      callback(si, atoms);
    }
  }
}

} // namespace lammpstrj
