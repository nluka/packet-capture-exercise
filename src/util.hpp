#ifndef UTIL_HPP
#define UTIL_HPP

#include <cstdint>
#include <fstream>
#include <iostream>

#include "exit.hpp"

namespace util {

std::uint16_t byteswap_uint16(std::uint16_t val);
std::uint32_t byteswap_uint32(std::uint32_t val);
std::uint64_t byteswap_uint64(std::uint64_t val);

void crash_if_file_not_found(char const *pathname);

template <typename FileStreamTy>
void crash_if_file_not_open(
  FileStreamTy const &file,
  char const *const pathname
) {
  if (!file.is_open()) {
    std::cerr << "fatal: unable to open file `" << pathname << "`\n";
    EXIT(ExitCode::FILE_OPEN_FAILED);
  }
}

} // namespace util

#endif // UTIL_HPP
