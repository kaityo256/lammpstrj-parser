#include <cassert>
#include <cmath>
#include <cstdio>
#include <iomanip>
#include <lammpstrj/lammpstrj.hpp>
#include <sstream>
#include <string>

class LocalDensityCalculator {

private:
  std::string filename_;
  const double mesh_size_;
  lammpstrj::SystemInfo system_info_;
  LocalDensityCalculator(const double mesh_size, const std::string &filename, const lammpstrj::SystemInfo &si)
      : system_info_(si), mesh_size_(mesh_size) {
    filename_ = filename;
  }

public:
  static std::unique_ptr<LocalDensityCalculator> create(const double mesh_size, const std::string &filename) {
    auto si = lammpstrj::read_info(filename);
    if (!si) {
      std::cerr << "Error: Could not read file: " << filename << std::endl;
      return nullptr;
    }
    return std::unique_ptr<LocalDensityCalculator>(new LocalDensityCalculator(mesh_size, filename, *si));
  }

  void calc_temperature(const std::unique_ptr<lammpstrj::SystemInfo> &si,
                        std::vector<lammpstrj::Atom> &atoms) {
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
  }

  void write_vtk(const std::string &filename, int nx, int ny, int nz, const std::vector<double> &data) {
    if (data.size() != static_cast<size_t>(nx * ny * nz)) {
      std::cerr << "Error: data size does not match grid dimensions." << std::endl;
      return;
    }

    std::ofstream ofs(filename);
    if (!ofs) {
      std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
      return;
    }

    ofs << "# vtk DataFile Version 1.0\n";
    ofs << "test\n";
    ofs << "ASCII\n";
    ofs << "DATASET STRUCTURED_POINTS\n";
    ofs << "DIMENSIONS " << nx << " " << ny << " " << nz << "\n";
    ofs << "ORIGIN 0.0 0.0 0.0\n";
    ofs << "SPACING 1.0 1.0 1.0\n";
    ofs << "\n";
    ofs << "POINT_DATA " << nx * ny * nz << "\n";
    ofs << "\n";
    ofs << "SCALARS intensity float\n";
    ofs << "LOOKUP_TABLE default\n";

    for (double v : data) {
      ofs << v << "\n";
    }

    ofs.close();
    std::cout << filename << std::endl;
  }

  void calc_density(const std::unique_ptr<lammpstrj::SystemInfo> &si,
                    std::vector<lammpstrj::Atom> &atoms) {
    // セル数（切り上げ）
    const int nx = static_cast<int>(std::ceil(si->LX / mesh_size_));
    const int ny = static_cast<int>(std::ceil(si->LY / mesh_size_));
    const int nz = static_cast<int>(std::ceil(si->LZ / mesh_size_));
    const int total_cells = nx * ny * nz;

    // 実際のセルサイズ
    const double mx = si->LX / static_cast<double>(nx);
    const double my = si->LY / static_cast<double>(ny);
    const double mz = si->LZ / static_cast<double>(nz);
    const double mx_inv = 1.0 / mx;
    const double my_inv = 1.0 / my;
    const double mz_inv = 1.0 / mz;
    // 初期化
    std::vector<double> density;
    density.assign(total_cells, 0.0);

    // 各原子を対応するセルに割り当ててカウント
    for (const auto &atom : atoms) {
      int ix = static_cast<int>(atom.x * mx_inv);
      int iy = static_cast<int>(atom.y * my_inv);
      int iz = static_cast<int>(atom.z * mz_inv);

      int index = ix + nx * (iy + ny * iz);
      density[index] += 1.0;
    }

    // 密度に変換（個数密度: 個数 / セル体積）
    const double cell_volume = mx * my * mz;
    for (auto &d : density) {
      d /= cell_volume;
    }
    static int index = 0;
    std::ostringstream oss;
    oss << "density." << std::setfill('0') << std::setw(4) << index << ".vtk";
    index++;
    std::string filename = oss.str();
    write_vtk(filename, nx, ny, nz, density);
  }

  void calculate() {
    lammpstrj::for_each_frame(filename_,
                              [this](const std::unique_ptr<lammpstrj::SystemInfo> &si, std::vector<lammpstrj::Atom> &atoms) {
                                this->calc_density(si, atoms);
                              });
  }
};

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " input.lammpstrj" << std::endl;
    return 1;
  }
  double mesh_size = 2.0;
  std::string filename = argv[1];
  auto calculator = LocalDensityCalculator::create(mesh_size, filename);
  if (!calculator) {
    std::cerr << "Failed to create LocalDensityCalculator." << std::endl;
    return 1;
  }
  calculator->calculate();
}

/*
       0            1   -3.0307637            0   -1.5311387   -2.1519302
    1000    1.0058772   -3.3139654            0   -1.8055268 -0.026187652
    2000   0.86811971    -3.472529            0    -2.170675  -0.17591555
    3000   0.81875479   -3.7134688            0   -2.4856437   -0.1176149
    4000   0.79822012   -3.9687192            0   -2.7716884  -0.15368024
    5000    0.7709941   -4.1997123            0   -3.0435102 -0.032310683
    6000   0.75421466   -4.4145754            0   -3.2835363  -0.10211006
    7000   0.73497865    -4.589255            0   -3.4870626  -0.10635509
    8000    0.7107305   -4.7152801            0   -3.6494509 -0.095841943
    9000   0.68184167   -4.7800265            0   -3.7575197  -0.13537357
   10000   0.67612267   -4.8089435            0   -3.7950131 -0.056874083
   */