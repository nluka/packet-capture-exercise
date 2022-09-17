#ifndef UTIL_HPP
#define UTIL_HPP

#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

namespace util {

std::uint16_t byteswap_uint16(std::uint16_t val);
std::uint32_t byteswap_uint32(std::uint32_t val);

std::fstream open_file(char const *pathname, int flags);
std::vector<char> extract_bin_file_contents(char const *pathname);
std::string extract_txt_file_contents(char const *pathname);

} // namespace util

#endif // UTIL_HPP
