#include <cstdio>
#include <lammpstrj/lammpstrj.hpp>

int main() {
  lammpstrj::SystemInfo si = lammpstrj::read_info("temperature.lammpstrj");
  printf("(LX, LY, LZ) = (%f, %f, %f)\n", si.LX, si.LY, si.LZ);
  printf("N = %d\n", si.atoms);

  auto calc_temperature = [](const lammpstrj::SystemInfo &si, const std::vector<lammpstrj::Atom> &atoms) {
    static int index = 0;
    double e = 0.0;
    for (auto &a : atoms) {
      e += a.vx * a.vx;
      e += a.vy * a.vy;
      e += a.vz * a.vz;
    }
    e /= static_cast<double>(si.atoms);
    e /= 3.0;
    printf("%d %f\n", index * 100, e);

    index++;
  };

  lammpstrj::for_each_frame("temperature.lammpstrj", calc_temperature);
}
