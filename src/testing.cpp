#include <filesystem>
#include <fstream>

#include "parse.hpp"
#include "test.hpp"
#include "util.hpp"

int main(int const argc, char const *const *const argv) {
  if (argc != 3) {
    std::cerr << "usage: <fabricated_data_path> <real_data_path>\n";
    EXIT(ExitCode::WRONG_NUM_OF_ARGS);
  }

  // configure test module:
  {
    using namespace test;
    set_verbose_mode(false);
    set_indentation("  ");
    use_stdout(true);
  }

  {
    using namespace std;
    SETUP_SUITE("testing")

    auto const testCase = [&s](string const &name) {
      string const
        packetFilePathname = name + ".packets",
        outputFilePathname = name + ".output",
        expectedFilePathname = name + ".expected"
      ;

      fstream packetFile = util::open_file(
        packetFilePathname.c_str(),
        ios_base::in | ios::binary
      );
      fstream outFile = util::open_file(
        outputFilePathname.c_str(),
        ios_base::out
      );
      fstream expectedFile = util::open_file(
        expectedFilePathname.c_str(),
        ios_base::in
      );
      size_t const expectedFileSize = filesystem::file_size(
        expectedFilePathname
      );

      auto const packetCaptures = extract_packet_captures_from_file(
        packetFile, packetFilePathname.c_str()
      );
      string const output = parse_packet_captures(packetCaptures).str();
      outFile.write(output.c_str(), output.size());

      string expected{};
      expected.reserve(expectedFileSize);
      std::getline(expectedFile, expected, '\0');
      expected.erase( // remove any \r characters
        std::remove(expected.begin(), expected.end(), '\r'),
        expected.end()
      );

      s.assert(name.c_str(), output == expected);
    };

    string const fabricatedDataPath = argv[1];
    string const realDataPath = argv[2];

    // scenario 1, all message types
    testCase(fabricatedDataPath + "/all-complete");

    // scenario 2B + 3, SYSTEM_EVENT
    testCase(fabricatedDataPath + "/2-consec-partials");

    // scenario 2A + 3, EXECUTED
    testCase(fabricatedDataPath + "/3-consec-partials");

    // all scenarios
    testCase(fabricatedDataPath + "/partials-ladder");
    // s0 partial ACCEPTED len=34
    // s1 partial CANCELED len=1
    // s2 partial REPLACED len=81
    // s0 empty ACCEPTED len=0
    // s2 partial REPLACED len=1 (final)
    // s1 partial CANCELED len=30 (final)
    // s0 partial ACCEPTED len=34 (final)

    // the real deal
    testCase(realDataPath + "/OUCHLMM2.incoming");

    // the real deal, but 100 times the data
    testCase(realDataPath + "/100x");
  }

  test::evaluate_suites();

  return static_cast<int>(test::assertions_failed());
}
