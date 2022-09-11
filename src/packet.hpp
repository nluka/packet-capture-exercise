#ifndef PACKET_HPP
#define PACKET_HPP

#include <string>
#include <cstdint>

namespace packet {

std::uint16_t read_uint16(char const *firstByte);
std::uint32_t read_uint32(char const *firstByte);
std::uint64_t read_uint64(char const *firstByte);

namespace msg {

  void validate_is_sequential(char const *msg, std::size_t packetPos);

  constexpr
  std::size_t max_len() {
    return 80;
  }

  enum class Type : char {
    NIL = '\0',
    NOT_YET_KNOWN,
    ACCEPTED = 'A',
    CANCELED = 'C',
    EXECUTED = 'E',
    REPLACED = 'R',
    SYSTEM_EVENT = 'S',
  };

  std::size_t deduce_len_from_type(msg::Type);

  Type deduce_type_from_msg_len(std::uint16_t msgLen);

} // namespace msg

} // namespace packet

#endif // PACKET_HPP
