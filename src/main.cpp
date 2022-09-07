#include <filesystem>
#include <fstream>
#include <iostream>

#include "exit.hpp"
#include "process.hpp"
#include "util.hpp"

int main(int const argc, char const *const *const argv) {
  if (argc < 2 || argc > 3) {
    std::cerr << "usage: <packet_captures_file> [<out_file>]\n";
    EXIT(ExitCode::WRONG_NUM_OF_ARGS);
  }

  try {

    char const *const packetCapturesFilePathname = argv[1];

    util::crash_if_file_not_found(packetCapturesFilePathname);

    std::ifstream packetCapturesFile(
      packetCapturesFilePathname,
      std::ios::binary
    );

    util::crash_if_file_not_open(packetCapturesFile, packetCapturesFilePathname);

    auto const packetCaptures = extract_packet_captures_from_file(
      packetCapturesFile,
      packetCapturesFilePathname
    );

    auto const output = process_packet_captures(packetCaptures).str();

    // if <out_file> was specified, dump output there,
    // otherwise dump output to console.
    if (argc == 3) {
      char const *const outFilePathname = argv[2];
      std::ofstream outFile(outFilePathname);
      util::crash_if_file_not_open(outFile, outFilePathname);
      outFile << output;
    } else {
      std::cout << output;
    }

  } catch (std::exception const &err) {
    std::cerr << "fatal: " << err.what() << '\n';
    EXIT(ExitCode::UNCAUGHT_EXCEPTION);
  }

  return static_cast<int>(ExitCode::SUCCESS);
}
