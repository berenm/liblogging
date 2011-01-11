/**
 * @file
 * @date 11 janv. 2011
 * @todo comment
 */

#include <logging/logging.hpp>

int main(int arg_count, char** arg_values) {
  __debug
    << "debug message";
  __info
    << "info message";
  __warn
    << "warn message";
  __error
    << "error message";
  __fatal
    << "fatal message";

  //  ::logging::logger::get().set_attributes();

  __debugM (main)
    << "debug message";
  __infoM(module1)
    << "info message";
  __warnM(opengl)
    << "warn message";
  __errorM()
    << "error message";
  __fatalM("test")
    << "fatal message";

  return 0;
}
