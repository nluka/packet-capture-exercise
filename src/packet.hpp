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
    std::size_t max_complete_length() {
      return 82;
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

    std::size_t complete_length(msg::Type);

    Type get_type_from_msg_len(std::uint16_t msgLen);

    // represents a partial message which cannot be processed until
    // more data arrives.
    class Pending {

      // represents a partial packet's message,
      // 2 or more of which create a complete message that can be processed.
      struct Chunk {
        char const *m_msgFirstByte;
        char const *m_msgLastByte;
      };

      // using basic_string for SSO
      std::basic_string<Chunk> m_chunks{};

    public:
      msg::Type m_msgType = msg::Type::NIL;

      void push_chunk(char const *msgFirstByte, char const *msgLastByte);
      std::array<char, msg::max_complete_length()> extract() const noexcept;
      bool is_empty() const noexcept;
      bool is_complete() const;
      void reset() noexcept;
    };

  } // namespace msg

} // namespace packet

#endif // PACKET_HPP
