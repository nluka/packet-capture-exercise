#include <stdexcept>

#include "stream.hpp"

using namespace stream;
using std::size_t;

void PendingMsg::push_chunk(
  char const *const msgFirstByte,
  char const *const msgLastByte
) {
  Chunk newChunk{msgFirstByte, msgLastByte};
  m_chunks.push_back(newChunk);
}

PendingMsg::content_t PendingMsg::extract() const noexcept {
  content_t content{};

  for (
    std::size_t i = 0, pos = 0;
    i < m_chunks.size();
    ++i
  ) {
    auto const [msgFirstByte, msgLastByte] = m_chunks[i];
    std::size_t const msgLen = msgLastByte - msgFirstByte + 1;
    if (msgLen > 0) {
      std::memcpy(content.data() + pos, msgFirstByte, msgLen);
      pos += msgLen;
    }
  }

  return content;
}

bool PendingMsg::is_empty() const noexcept {
  return m_msgType == packet::msg::Type::NIL;
}

bool PendingMsg::is_complete() const {
  if (m_msgType < packet::msg::Type::ACCEPTED) {
    return false;
  }

  size_t bytesAccumulated = 0;
  for (auto const [msgFirstByte, msgLastByte] : m_chunks) {
    size_t const chunkLen = msgLastByte - msgFirstByte + 1;
    bytesAccumulated += chunkLen;
  }

  // payload refers to the combination of message length (2 bytes)
  // and the message content (message length bytes)
  auto const payloadLen = packet::msg::deduce_len_from_type(m_msgType) + 2;

  if (bytesAccumulated > payloadLen) {
    throw std::runtime_error("accumulated too many bytes for message type");
  } else {
    return bytesAccumulated == payloadLen;
  }
}

void PendingMsg::reset() noexcept {
  m_msgType = packet::msg::Type::NIL;
  m_chunks.clear();
}
