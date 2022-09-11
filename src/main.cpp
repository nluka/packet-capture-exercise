#include <filesystem>
#include <fstream>
#include <iostream>

#include "exit.hpp"
#include "parse.hpp"
#include "util.hpp"

int main(int const argc, char const *const *const argv) {
  if (argc < 2 || argc > 3) {
    std::cerr << "usage: <packet_captures_file> [<out_file>]\n";
    EXIT(ExitCode::WRONG_NUM_OF_ARGS);
  }

  try {

    char const *const packetCapturesFilePathname = argv[1];

    auto packetCapturesFile = util::open_file(
      packetCapturesFilePathname,
      std::ios_base::in | std::ios::binary
    );

    auto const packetCaptures = extract_packet_captures_from_file(
      packetCapturesFile,
      packetCapturesFilePathname
    );

    auto const output = parse_packet_captures(packetCaptures).str();

    // if <out_file> was specified, dump output there,
    // otherwise dump output to console.
    if (argc == 3) {
      char const *const outFilePathname = argv[2];
      auto outFile = util::open_file(
        outFilePathname,
        std::ios_base::out | std::ios::binary
      );
      outFile.write(output.c_str(), output.size());
    } else {
      std::cout << output;
    }

  } catch (std::exception const &err) {
    std::cerr << "fatal: " << err.what() << '\n';
    EXIT(ExitCode::UNCAUGHT_EXCEPTION);
  }

  return static_cast<int>(ExitCode::SUCCESS);
}
