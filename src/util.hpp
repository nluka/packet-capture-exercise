#ifndef UTIL_HPP
#define UTIL_HPP

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "exit.hpp"

namespace util {

std::uint16_t byteswap_uint16(std::uint16_t val);
std::uint32_t byteswap_uint32(std::uint32_t val);
std::uint64_t byteswap_uint64(std::uint64_t val);

std::fstream open_file(char const *pathname, int flags);

} // namespace util

#endif // UTIL_HPP
