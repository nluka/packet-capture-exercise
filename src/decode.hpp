#ifndef PARSE_HPP
#define PARSE_HPP

#include <sstream>
#include <vector>

std::stringstream decode_packet_captures(
  std::vector<char> const &packetCaptures
);

#endif // PARSE_HPP
