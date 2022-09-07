#include <filesystem>

#include "process.hpp"
#include "test.hpp"
#include "util.hpp"

int main() {
  test::set_verbose_mode(false);
  test::set_indentation("  ");
  test::use_stdout(true);

  SETUP_SUITE("testing")

  auto const testCase = [&s](std::string const name) {
    std::string const
      inputFilePathname = name + ".packets",
      outputFilePathname = name + ".output",
      expectedFilePathname = name + ".expected"
    ;

    util::crash_if_file_not_found(inputFilePathname.c_str());
    std::ifstream inputFile(inputFilePathname, std::ios::binary);
    util::crash_if_file_not_open(inputFile, inputFilePathname.c_str());

    auto const packetCaptures = extract_packet_captures_from_file(
      inputFile, inputFilePathname.c_str()
    );

    std::string const output = process_packet_captures(packetCaptures).str();

    std::ofstream outputFile(outputFilePathname);
    util::crash_if_file_not_open(outputFile, outputFilePathname.c_str());
    outputFile << output;

    util::crash_if_file_not_found(expectedFilePathname.c_str());
    auto const expectedFileSizeInBytes = std::filesystem::file_size(
      expectedFilePathname
    );
    std::ifstream expectedFile(expectedFilePathname);
    std::string expected{};
    expected.reserve(expectedFileSizeInBytes);
    std::getline(expectedFile, expected, '\0');

    s.assert("all-complete", output == expected);
  };

  testCase("all-complete"); // scenario 1
  testCase("2-consec-partials"); // scenario 2B + 3 (SYSTEM_EVENT)
  testCase("3-consec-partials"); // scenario 2A + 3 (EXECUTED)

  testCase("partials-ladder"); // all scenarios
  // s0 partial ACCEPTED len=34
  // s1 partial CANCELED len=1
  // s2 partial REPLACED len=81
  // s0 empty ACCEPTED len=0
  // s2 partial REPLACED len=1 (final)
  // s1 partial CANCELED len=30 (final)
  // s0 partial ACCEPTED len=34 (final)

  test::evaluate_suites();

  return static_cast<int>(s.fails());
}
