#include <array>
#include <bit>
#include <cstring>
#include <exception>
#include <numeric>
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

std::size_t msg::complete_length(Type const msgType) {
  switch (msgType) {
    case Type::ACCEPTED: return static_cast<std::size_t>(68);
    case Type::CANCELED: return static_cast<std::size_t>(31);
    case Type::EXECUTED: return static_cast<std::size_t>(43);
    case Type::REPLACED: return static_cast<std::size_t>(82);
    case Type::SYSTEM_EVENT: return static_cast<std::size_t>(13);
    default: throw std::runtime_error("packet::msg::expected_length failed - invalid Type");
  }
}

msg::Type msg::get_type_from_msg_len(std::uint16_t const msgLen) {
  switch (msgLen) {
    case 66: return Type::ACCEPTED;
    case 29: return Type::CANCELED;
    case 41: return Type::EXECUTED;
    case 80: return Type::REPLACED;
    case 11: return Type::SYSTEM_EVENT;
    default: return Type::NIL;
  }
}

void msg::Pending::push_chunk(
  char const *const msgFirstByte,
  char const *const msgLastByte
) {
  Chunk newChunk{msgFirstByte, msgLastByte};
  m_chunks.push_back(newChunk);
}

std::array<char, msg::max_complete_length()> msg::Pending::extract() const noexcept {
  std::array<char, packet::msg::max_complete_length()> body{};

  for (
    std::size_t i = 0, pos = 0;
    i < m_chunks.size();
    ++i
  ) {
    auto const [msgFirstByte, msgLastByte] = m_chunks[i];
    std::size_t const msgLen = msgLastByte - msgFirstByte + 1;
    if (msgLen > 0) {
      std::memcpy(body.data() + pos, msgFirstByte, msgLen);
      pos += msgLen;
    }
  }

  return body;
}

bool msg::Pending::is_empty() const noexcept {
  return m_msgType == msg::Type::NIL;
}

bool msg::Pending::is_complete() const {
  if (m_msgType < Type::ACCEPTED) {
    return false;
  }

  std::size_t const bytesAccumulated = [this]() {
    std::size_t sum = 0;
    for (auto const [msgFirstByte, msgLastByte] : m_chunks) {
      std::size_t const chunkLen = msgLastByte - msgFirstByte + 1;
      sum += chunkLen;
    }
    return sum;
  }();

  auto const expectedMsgLength = msg::complete_length(m_msgType);

  if (bytesAccumulated > expectedMsgLength) {
    throw std::runtime_error("accumulated too many bytes for message type");
  } else {
    return bytesAccumulated == expectedMsgLength;
  }
}

void msg::Pending::reset() noexcept {
  m_msgType = msg::Type::NIL;
  m_chunks.clear();
}
