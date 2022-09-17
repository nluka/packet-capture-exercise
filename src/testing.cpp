#include <fstream>
#include <string>

#include "decode.hpp"
#include "exit.hpp"
#include "test.hpp"
#include "util.hpp"

using namespace std;

int main(int const argc, char const *const *const argv) {
  if (argc != 3) {
    cerr << "usage: <fabricated_data_path> <real_data_path>\n";
    EXIT(ExitCode::WRONG_NUM_OF_ARGS);
  }

  try {

  test::set_verbose_mode(false);
  test::set_indentation("  ");
  test::use_stdout(true);

  {
    SETUP_SUITE("testing")

    auto const testCase = [&s](string const &name) {
      vector<char> const packetCaptures =
        util::extract_bin_file_contents((name + ".packets").c_str());

      string const output = decode_packet_captures(packetCaptures).str();

      {
        fstream outFile = util::open_file(
          (name + ".output").c_str(), ios_base::out
        );
        outFile.write(output.c_str(), output.size());
      }

      string const expected =
        util::extract_txt_file_contents((name + ".expected").c_str());

      s.assert(name.c_str(), output == expected);
    };

    string const fabricatedDataPath = argv[1];
    string const realDataPath = argv[2];

    testCase(fabricatedDataPath + "/all-complete");
    // s0 complete ACCEPTED
    // s1 complete CANCELED
    // s0 complete EXECUTED
    // s1 complete REPLACED
    // s0 complete SYSTEM_EVENT

    testCase(fabricatedDataPath + "/2-consec-partials");
    // s0 partial EXECUTED len=28 (start msg)
    // s0 partial EXECUTED len=15 (start msg)

    testCase(fabricatedDataPath + "/3-consec-partials");
    // s0 partial SYSTEM_EVENT len=1 (start msg)
    // s0 partial SYSTEM_EVENT len=2 (continue msg)
    // s0 partial SYSTEM_EVENT len=10 (end msg)

    testCase(fabricatedDataPath + "/1stream-2msgs-4partials");
    // s0 partial SYSTEM_EVENT len=7 (start msg A)
    // s0 partial SYSTEM_EVENT len=6 (end msg A)
    // s0 partial SYSTEM_EVENT len=10 (start msg B)
    // s0 partial SYSTEM_EVENT len=3 (end msg B)

    testCase(fabricatedDataPath + "/msg-end-and-start-in-partial");
    // s0 partial SYSTEM_EVENT len=10 (start msg A)
    // s0 partial SYSTEM_EVENT len=6 (finish msg A, start msg B)
    // s0 partial SYSTEM_EVENT len=10 (end msg B)

    testCase(fabricatedDataPath + "/partials-ladder");
    // s0 partial ACCEPTED len=34 (start msg A)
    // s1 partial CANCELED len=1 (start msg B)
    // s2 partial REPLACED len=81 (start msg C)
    // s0 empty ACCEPTED len=0
    // s2 partial REPLACED len=1 (end msg C)
    // s1 partial CANCELED len=30 (end msg B)
    // s0 partial ACCEPTED len=34 (end msg A)

    // the real deal
    testCase(realDataPath + "/OUCHLMM2.incoming");
  }

  test::evaluate_suites();

  return static_cast<int>(test::assertions_failed());

  } catch (exception const &err) {
    cerr << "fatal: " << err.what() << '\n';
    EXIT(ExitCode::UNCAUGHT_EXCEPTION);
  }
}
