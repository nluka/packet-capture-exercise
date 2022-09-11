#ifndef STREAM_HPP
#define STREAM_HPP

#include <array>
#include <cstdint>
#include <cstring>

#include "packet.hpp"

namespace stream {

struct Counters {
  std::uint32_t
    m_acceptedMessages = 0,
    m_canceledMessages = 0,
    m_replacedMessages = 0,
    m_systemEventMessages = 0,
    m_executedMessages = 0,
    m_executedShares = 0;
};

// represents a partial message which cannot be processed until
// more data arrives.
class PendingMsg {
public:
  typedef std::array<char, packet::msg::max_len() + 2> content_t;

  packet::msg::Type m_msgType = packet::msg::Type::NIL;

  void push_chunk(char const *msgFirstByte, char const *msgLastByte);
  content_t extract() const noexcept;
  bool is_empty() const noexcept;
  bool is_complete() const;
  void reset() noexcept;

private:
  // represents a partial packet's message,
  // 2 or more of which create a complete message that can be processed.
  struct Chunk {
    char const *m_msgFirstByte;
    char const *m_msgLastByte;
  };

  // using basic_string for SSO
  std::basic_string<Chunk> m_chunks{};
};

} // namespace stream

#endif // STREAM_HPP
