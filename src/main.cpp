#include <iostream>
#include <stdexcept>
#include <vector>

#include "decode.hpp"
#include "exit.hpp"
#include "util.hpp"

using namespace std;

int main(int const argc, char const *const *const argv) {
  if (argc < 2 || argc > 3) {
    cerr << "usage: <packet_captures_file> [<out_file>]\n";
    EXIT(ExitCode::WRONG_NUM_OF_ARGS);
  }

  try {

    vector<char> const packetCaptures = util::extract_bin_file_contents(argv[1]);

    string const output = decode_packet_captures(packetCaptures).str();

    // if <out_file> was specified, dump output there,
    // otherwise dump output to console.
    if (argc == 3) {
      char const *const outFilePathname = argv[2];
      fstream outFile =
        util::open_file(outFilePathname, ios_base::out | ios::binary);
      outFile.write(output.c_str(), output.size());
    } else {
      cout << output;
    }

    return static_cast<int>(ExitCode::SUCCESS);

  } catch (exception const &err) {
    cerr << "fatal: " << err.what() << '\n';
    EXIT(ExitCode::UNCAUGHT_EXCEPTION);
  }
}
