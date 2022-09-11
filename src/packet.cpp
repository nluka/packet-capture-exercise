#include <array>
#include <bit>
#include <cstring>
#include <numeric>
#include <stdexcept>
#include <string>
#include <sstream>

#include "packet.hpp"
#include "util.hpp"

template <typename Ty, Ty (*big_to_little_endian_converter)(Ty)>
Ty read_packet_bytes(char const *const src) {
  Ty val;

  std::memcpy(&val, src, sizeof(val));

  if constexpr (std::endian::native == std::endian::little) {
    val = big_to_little_endian_converter(val);
  }

  return val;
}

std::uint16_t packet::read_uint16(char const *const src) {
  return read_packet_bytes<std::uint16_t, util::byteswap_uint16>(src);
}
std::uint32_t packet::read_uint32(char const *const src) {
  return read_packet_bytes<std::uint32_t, util::byteswap_uint32>(src);
}
std::uint64_t packet::read_uint64(char const *const src) {
  return read_packet_bytes<std::uint64_t, util::byteswap_uint64>(src);
}

using namespace packet;

void msg::validate_is_sequential(
  char const *const msg,
  std::size_t const packetPos
) {
  char const packetType = *(msg + 2);
  if (packetType != 'S') {
    std::stringstream err{};
    err << "packet starting at byte "
        << packetPos << " is non-sequential";
    throw std::runtime_error(err.str());
  }
}

std::size_t msg::deduce_len_from_type(Type const msgType) {
  switch (msgType) {
    case Type::ACCEPTED: return static_cast<std::size_t>(66);
    case Type::CANCELED: return static_cast<std::size_t>(29);
    case Type::EXECUTED: return static_cast<std::size_t>(41);
    case Type::REPLACED: return static_cast<std::size_t>(80);
    case Type::SYSTEM_EVENT: return static_cast<std::size_t>(11);
    default: throw std::runtime_error("packet::msg::expected_length failed - invalid Type");
  }
}

msg::Type msg::deduce_type_from_msg_len(std::uint16_t const msgLen) {
  switch (msgLen) {
    case 66: return Type::ACCEPTED;
    case 29: return Type::CANCELED;
    case 41: return Type::EXECUTED;
    case 80: return Type::REPLACED;
    case 11: return Type::SYSTEM_EVENT;
    default: return Type::NIL;
  }
}
