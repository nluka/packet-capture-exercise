#ifndef PROCESS_HPP
#define PROCESS_HPP

#include <fstream>
#include <sstream>
#include <vector>

std::vector<char> extract_packet_captures_from_file(
  std::ifstream &file,
  char const *filePathname
);

std::stringstream process_packet_captures(
  std::vector<char> const &packetCaptures
);

#endif // PROCESS_HPP
