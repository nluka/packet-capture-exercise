#include <bit>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <vector>

#include "exitcode.hpp"

int main(int const argc, char const *const *const argv) {
  if (argc != 2) {
    std::puts("usage: <packet_capture_file>");
    std::exit((int)ExitCode::WRONG_NUM_OF_ARGS);
  }

  try { // top-level exception safety net

  std::vector<char> const packetCapture = [argv](){
    char const *const fPathname = argv[1];

    std::ifstream fPacketCapture(fPathname);
    if (!fPacketCapture.is_open()) {
      std::cerr << "fatal: failed to open file `" << fPathname << "`\n";
      std::exit((int)ExitCode::FILE_OPEN_FAILED);
    }

    // extract entire file - this may not be reasonable depending on the
    // file size, but for the sake of this exercise I will assume the file is
    // not exceptionally large and will easily fit in memory.
    std::size_t const fileSizeInBytes = std::filesystem::file_size(fPathname);
    std::vector<char> vec(fileSizeInBytes);
    fPacketCapture.read(vec.data(), fileSizeInBytes);

    return vec;
  }();

  // end of top-level exception safety net
  } catch (std::exception const &err) {
    std::cerr << "fatal: " << err.what() << '\n';
    std::exit((int)ExitCode::UNCAUGHT_EXEPTION);
  }

  return (int)ExitCode::SUCCESS;
}
