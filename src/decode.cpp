#include <array>
#include <bit>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <stdexcept>

#include "decode.hpp"
#include "util.hpp"

using namespace std;

template <typename Ty, Ty (*big_to_little_endian_converter)(Ty)>
Ty read_packet_bytes(char const *const src) {
  Ty val;

  memcpy(&val, src, sizeof(val));

  if constexpr (endian::native == endian::little) {
    val = big_to_little_endian_converter(val);
  }

  return val;
}

namespace packet {
  uint16_t read_uint16(char const *const firstByte) {
    return read_packet_bytes<uint16_t, util::byteswap_uint16>(firstByte);
  }
  uint32_t read_uint32(char const *const firstByte) {
    return read_packet_bytes<uint32_t, util::byteswap_uint32>(firstByte);
  }
} // namespace packet

stringstream decode_packet_captures(vector<char> const &packetCaptures) {
  enum class MessageType : char {
    ACCEPTED = 'A',
    CANCELED = 'C',
    EXECUTED = 'E',
    SYSTEM_EVENT = 'S',
    REPLACED = 'U',
  };

  struct Counters {
    uint32_t
      m_acceptedMessages = 0,
      m_canceledMessages = 0,
      m_replacedMessages = 0,
      m_systemEventMessages = 0,
      m_executedMessages = 0,
      m_executedShares = 0;
  };

  vector<Counters> streamCounters(UINT16_MAX + 1);
  Counters totals{};
  // array<vector<char>, UINT16_MAX + 1> partialBuffers{};
  vector<vector<char>> partialBuffers(UINT16_MAX + 1);

  auto const processMsg = [&packetCaptures, &streamCounters, &totals](
    uint16_t const streamId,
    MessageType const msgType,
    char const *const msg
  ) {
    switch (msgType) {
      case MessageType::ACCEPTED:
        ++streamCounters[streamId].m_acceptedMessages;
        ++totals.m_acceptedMessages;
        break;
      case MessageType::CANCELED:
        ++streamCounters[streamId].m_canceledMessages;
        ++totals.m_canceledMessages;
        break;
      case MessageType::EXECUTED: {
        uint32_t const executedShares = packet::read_uint32(msg + 26);
        streamCounters[streamId].m_executedShares += executedShares;
        totals.m_executedShares += executedShares;
        ++streamCounters[streamId].m_executedMessages;
        ++totals.m_executedMessages;
        break;
      }
      case MessageType::REPLACED:
        ++streamCounters[streamId].m_replacedMessages;
        ++totals.m_replacedMessages;
        break;
      case MessageType::SYSTEM_EVENT:
        ++streamCounters[streamId].m_systemEventMessages;
        ++totals.m_systemEventMessages;
        break;
      default:
        throw runtime_error("bad message type");
    }
  };

  auto const processPartialPacket = [&partialBuffers, &processMsg](
    uint16_t const streamId,
    uint32_t const packetLen,
    char const *const msg
  ) {
    auto &buffer = partialBuffers[streamId];

    // append this partial message to stream's buffer
    buffer.reserve(buffer.size() + packetLen);
    buffer.insert(buffer.end(), msg, msg + packetLen);

    bool const haveMsgLenAndType = buffer.size() >= 4;
    if (haveMsgLenAndType) {
      // the message may have been completed with this packet,
      // process it if that's the case

      uint16_t const msgLen = packet::read_uint16(buffer.data());
      size_t const completeMsgPacketLen = msgLen + 2;

      if (buffer.size() >= completeMsgPacketLen) {
        auto const msgType = static_cast<MessageType>(*(buffer.data() + 3));
        processMsg(streamId, msgType, buffer.data());
        buffer.erase(buffer.begin(), buffer.begin() + completeMsgPacketLen);
      }
    }
  };

  size_t numPacketsDecoded = 0, numEmptyPackets = 0, packetSize = 0;

  for (
    size_t pos = 0;
    pos < packetCaptures.size();
    pos += packetSize, ++numPacketsDecoded
  ) {
    char const *const header = packetCaptures.data() + pos;
    uint16_t const streamId = packet::read_uint16(header);
    uint32_t const packetLen = packet::read_uint32(header + 2);

    packetSize = static_cast<size_t>(6) + packetLen;

    if (packetLen == 0) {
      ++numEmptyPackets;
    } else {
      char const *const msg = header + 6;
      bool const streamHasPendingMsg = !partialBuffers[streamId].empty();
      if (streamHasPendingMsg || packetLen == 1) {
        processPartialPacket(streamId, packetLen, msg);
      } else {
        uint16_t const msgLen = packet::read_uint16(msg);
        bool const packetIsComplete = msgLen == (packetLen - 2);
        if (packetIsComplete) {
          auto const msgType = static_cast<MessageType>(*(msg + 3));
          processMsg(streamId, msgType, msg);
        } else {
          processPartialPacket(streamId, packetLen, msg);
        }
      }
    }
  }

  stringstream output{};
  uint8_t const allZeroes[sizeof(Counters)]{ 0 };

  for (size_t streamId = 0; streamId <= UINT16_MAX; ++streamId) {
    auto const &counters = streamCounters[streamId];

    bool const areAnyCountersNonZero = memcmp(
      &counters, allZeroes, sizeof(Counters)
    ) != 0;

    if (areAnyCountersNonZero) {
      output
        << "Stream " << streamId << ":\n"
        << "  Accepted: " << counters.m_acceptedMessages << " messages\n"
        << "  System Event: " << counters.m_systemEventMessages << " messages\n"
        << "  Replaced: " << counters.m_replacedMessages << " messages\n"
        << "  Canceled: " << counters.m_canceledMessages << " messages\n"
        << "  Executed: " << counters.m_executedMessages << " messages, "
          << counters.m_executedShares << " shares\n";
    }
  }

  output
    << "Totals:\n"
    << "  Accepted: " << totals.m_acceptedMessages << " messages\n"
    << "  System Event: " << totals.m_systemEventMessages << " messages\n"
    << "  Replaced: " << totals.m_replacedMessages << " messages\n"
    << "  Canceled: " << totals.m_canceledMessages << " messages\n"
    << "  Executed: " << totals.m_executedMessages << " messages, "
      << totals.m_executedShares << " shares\n"
    << "Num packets decoded: " << numPacketsDecoded << '\n'
    << "Num empty packets: " << numEmptyPackets << '\n';

  return output;
}
