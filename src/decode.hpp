#ifndef DECODE_HPP
#define DECODE_HPP

#include <sstream>
#include <vector>

std::stringstream decode_packet_captures(
  std::vector<char> const &packetCaptures
);

#endif // DECODE_HPP
