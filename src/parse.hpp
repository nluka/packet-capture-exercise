#ifndef PARSE_HPP
#define PARSE_HPP

#include <fstream>
#include <sstream>
#include <vector>

std::vector<char> extract_packet_captures_from_file(
  std::fstream &file,
  char const *filePathname
);

std::stringstream parse_packet_captures(
  std::vector<char> const &packetCaptures
);

#endif // PARSE_HPP
