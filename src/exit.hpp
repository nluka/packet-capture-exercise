#ifndef EXIT_HPP
#define EXIT_HPP

enum class ExitCode : int {
  SUCCESS = 0,
  WRONG_NUM_OF_ARGS,
  FILE_NOT_FOUND,
  FILE_OPEN_FAILED,
  UNCAUGHT_EXCEPTION,
  UNEXPECTED_PACKET_TYPE,
};

#define EXIT(code) std::exit(static_cast<int>(code))

#endif // EXIT_HPP
