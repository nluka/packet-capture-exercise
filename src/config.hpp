#ifndef CONFIG_HPP
#define CONFIG_HPP

#define SANITY_CHECK_PACKET_TYPE_IS_SEQUENTIAL 1

#define IMPLEMENTATION 1
// 1 - std::vector for streamCounters and pendingStreamMsgs
// 2 - std::unordered_map for streamCounters and pendingStreamMsgs
// 3 - std::array for streamCounters and pendingStreamMsgs

#endif // CONFIG_HPP
