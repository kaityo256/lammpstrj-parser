#include <cstdio>
#include <lammpstrj/lammpstrj.hpp>
#include <memory>

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

  lammpstrj::FrameCallback calc_temperature = [](const auto &si, const auto &atoms) {
    static int index = 0;
    double e = 0.0;
    for (auto &a : atoms) {
      e += a.vx * a.vx;
      e += a.vy * a.vy;
      e += a.vz * a.vz;
    }
    e /= static_cast<double>(si->atoms);
    e /= 3.0;
    printf("%d %f\n", index * 100, e);

    index++;
  };

  lammpstrj::for_each_frame(filename, calc_temperature);
}
