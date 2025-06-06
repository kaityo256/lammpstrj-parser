#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

struct Atom {
  int type;
  double x, y, z;
};

struct SimulationInfo {
  int atoms;         // 原子数
  double LX, LY, LZ; // シミュレーションボックスのサイズ
};

SimulationInfo read_info(const std::string filename) {
  std::ifstream file(filename);
  SimulationInfo info;

  if (!file.is_open()) {
    std::cerr << "ファイルを開けませんでした: " << filename << std::endl;
    return info; // 初期値を返す
  }

  std::string line;
  bool atoms_found = false;
  bool box_found = false;

  while (std::getline(file, line)) {
    if (!atoms_found &&
        line.find("ITEM: NUMBER OF ATOMS") != std::string::npos) {
      // 次の行に原子数がある
      std::getline(file, line);
      info.atoms = std::stoi(line);
      atoms_found = true;
    }

    if (!box_found && line.find("ITEM: BOX BOUNDS") != std::string::npos) {
      // 次の3行でボックスサイズ情報
      double x_min, x_max, y_min, y_max, z_min, z_max;
      std::getline(file, line);
      std::istringstream(line) >> x_min >> x_max;
      info.LX = x_max - x_min; // X方向のサイズ

      std::getline(file, line);
      std::istringstream(line) >> y_min >> y_max;
      info.LY = y_max - y_min; // Y方向のサイズ

      std::getline(file, line);
      std::istringstream(line) >> z_min >> z_max;
      info.LZ = z_max - z_min; // Z方向のサイズ

      box_found = true;
    }

    // 必要な情報を取得したら処理を終了
    if (atoms_found && box_found) {
      break;
    }
  }

  file.close();
  return info;
}