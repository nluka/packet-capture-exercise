#include <filesystem>

#include "util.hpp"

#ifdef _MSC_VER
#define MICROSOFT_COMPILER 1
#elif __GNUC__
#define GXX_COMPILER 1
#endif

#ifdef MICROSOFT_COMPILER
#include <cstdlib>
#endif

std::uint16_t util::byteswap_uint16(std::uint16_t const val) {
  #if MICROSOFT_COMPILER
  static_assert(sizeof(unsigned short) == sizeof(std::uint16_t));
  return _byteswap_ushort(val);
  #elif GXX_COMPILER
  return __builtin_bswap16(val);
  #else
  #error "unsupported compiler"
  #endif
}

std::uint32_t util::byteswap_uint32(std::uint32_t const val) {
  #if MICROSOFT_COMPILER
  static_assert(sizeof(unsigned long) == sizeof(std::uint32_t));
  return _byteswap_ulong(val);
  #elif GXX_COMPILER
  return __builtin_bswap32(val);
  #else
  #error "unsupported compiler"
  #endif
}

std::uint64_t util::byteswap_uint64(std::uint64_t const val) {
  #if MICROSOFT_COMPILER
  static_assert(sizeof(unsigned long long) == sizeof(std::uint64_t));
  return _byteswap_uint64(val);
  #elif GXX_COMPILER
  return __builtin_bswap64(val);
  #else
  #error "unsupported compiler"
  #endif
}

void util::crash_if_file_not_found(char const *const filePathname) {
  if (!std::filesystem::exists(filePathname)) {
    std::cerr << "fatal: file `" << filePathname << "` not found\n";
    EXIT(ExitCode::FILE_NOT_FOUND);
  }
}
