#include <array>
#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <functional>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <unordered_map>

#include "config.hpp"
#include "packet.hpp"
#include "parse.hpp"
#include "stream.hpp"

std::vector<char> extract_packet_captures_from_file(
  std::fstream &file,
  char const *const filePathname
) {
  auto const fileSizeInBytes = std::filesystem::file_size(filePathname);
  std::vector<char> vec(fileSizeInBytes);
  file.read(vec.data(), fileSizeInBytes);
  return vec;
}

std::stringstream parse_packet_captures(
  std::vector<char> const &packetCaptures
) {
  std::vector<stream::Counters> streamCounters(UINT16_MAX + 1);
  std::vector<stream::PendingMsg> pendingStreamMsgs(UINT16_MAX + 1);

  auto const processMsg = [&packetCaptures, &streamCounters](
    std::uint16_t const streamId,
    packet::msg::Type const msgType,
    char const *const msg,
    std::size_t const packetPos
  ) {
    #if SANITY_CHECK_PACKET_TYPE_IS_SEQUENTIAL
    packet::msg::validate_is_sequential(msg, packetPos);
    #endif

    switch (msgType) {
      using packet::msg::Type;

      case Type::ACCEPTED:
        ++streamCounters[streamId].m_acceptedMessages;
        break;
      case Type::CANCELED:
        ++streamCounters[streamId].m_canceledMessages;
        break;
      case Type::EXECUTED: {
        std::uint32_t const executedShares = packet::read_uint32(msg + 26);
        streamCounters[streamId].m_executedShares += executedShares;
        ++streamCounters[streamId].m_executedMessages;
        break;
      }
      case Type::REPLACED:
        ++streamCounters[streamId].m_replacedMessages;
        break;
      case Type::SYSTEM_EVENT:
        ++streamCounters[streamId].m_systemEventMessages;
        break;
      default: {
        std::stringstream err{};
        err << "packet starting at byte " << packetPos
            << " has invalid message type of " << static_cast<char>(msgType);
        throw std::runtime_error(err.str());
      }
    }
  };

  std::size_t
    numPacketsChecked = 0,
    numEmptyPackets = 0,
    packetSize = 0
  ;

  for (
    std::size_t pos = 0;
    pos < packetCaptures.size();
    pos += packetSize, ++numPacketsChecked
  ) {
    char const *const header = packetCaptures.data() + pos;
    std::uint16_t const streamId = packet::read_uint16(header);
    std::uint32_t const packetLen = packet::read_uint32(header + 2);

    packetSize = static_cast<std::size_t>(6) + packetLen;

    if (packetLen == 0) {
      // no message data, discard this packet
      ++numEmptyPackets;
      continue;
    }

    /*
      when parsing a non-empty packet, there are 4 possible scenarios:

      1. complete packet
      2. first partial packet for a stream
        A. neither message length nor type are known (packet length == 1)
        B. message length (and thus type) are known (packet length >= 2)
      3. non-first partial packet for a stream
          message length (and thus type) are known because we ignore empty
          packets, as such we have at least 2 bytes to work with

      the main challenge is to determine which scenario we are in...
    */

    char const *const msg = header + 6;

    bool const streamHasPendingMsg = !(pendingStreamMsgs[streamId].is_empty());

    if (streamHasPendingMsg) {
      // we are in scenario 3

      auto &pendingStreamMsg = pendingStreamMsgs[streamId];
      pendingStreamMsg.push_chunk(msg, msg + packetLen - 1);

      if (pendingStreamMsg.m_msgType == packet::msg::Type::NOT_YET_KNOWN) {
        // message type may not have been known previously if we received just
        // 1 byte (scenario 2A), but it is definitely known now

        auto const msgSoFar = pendingStreamMsg.extract();
        std::uint16_t const msgLen = packet::read_uint16(msgSoFar.data());
        auto const msgType = packet::msg::deduce_type_from_msg_len(msgLen);
        pendingStreamMsg.m_msgType = msgType;
      }

      if (pendingStreamMsg.is_complete()) {
        auto const completeMsg = pendingStreamMsg.extract();
        processMsg(streamId, pendingStreamMsg.m_msgType, completeMsg.data(), pos);
        pendingStreamMsg.reset();
      }

      continue;
    }

    if (packetLen == 1) {
      // we are in scneario 2A

      auto &pendingStreamMsg = pendingStreamMsgs[streamId];
      pendingStreamMsg.m_msgType = packet::msg::Type::NOT_YET_KNOWN;
      pendingStreamMsg.push_chunk(msg, msg);

      continue;
    }

    std::uint16_t const msgLen = packet::read_uint16(msg);

    if (packetLen == msgLen + static_cast<size_t>(2)) {
      // we are in scenario 1

      auto const msgType = static_cast<packet::msg::Type>(*(msg + 3));
      processMsg(streamId, msgType, msg, pos);
    } else {
      // we are in scenario 2B

      auto &pendingStreamMsg = pendingStreamMsgs[streamId];
      pendingStreamMsg.push_chunk(msg, msg + packetLen - 1);
      auto const msgType = packet::msg::deduce_type_from_msg_len(msgLen);
      if (msgType == packet::msg::Type::NIL) {
        throw std::runtime_error("nil");
      }
      pendingStreamMsg.m_msgType = msgType;
    }
  }

  stream::Counters totals{};
  std::stringstream output{};
  std::uint8_t allZeroes[sizeof(stream::Counters)] { 0 };

  for (std::size_t streamId = 0; streamId <= UINT16_MAX; ++streamId) {
    auto const &counters = streamCounters[streamId];

    totals.m_acceptedMessages += counters.m_acceptedMessages;
    totals.m_systemEventMessages += counters.m_systemEventMessages;
    totals.m_replacedMessages += counters.m_replacedMessages;
    totals.m_canceledMessages += counters.m_canceledMessages;
    totals.m_executedMessages += counters.m_executedMessages;
    totals.m_executedShares += counters.m_executedShares;

    bool const areAnyCountersNonZero =
    #if 1
      std::memcmp(&counters, allZeroes, sizeof(stream::Counters)) != 0;
    #else
      counters.m_acceptedMessages > 0 ||
      counters.m_systemEventMessages > 0 ||
      counters.m_replacedMessages > 0 ||
      counters.m_canceledMessages > 0 ||
      counters.m_executedMessages > 0 ||
      counters.m_executedShares > 0;
    #endif

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
    << "Packets parsed: " << numPacketsChecked << '\n'
    << "Empty packets: " << numEmptyPackets << '\n';

  return output;
}
