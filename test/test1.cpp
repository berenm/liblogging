/**
 * @file
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt
 */

#include <logging/logging.hpp>

#include <corefungi.hpp>

namespace test_module {
  namespace logging {
    static constexpr char const module_name[] = "log/module/test";
  }

  static void test() {
    __trace() << "debug message";
    __debug() << "debug message";
    __info() << "info message";
    __notice() << "notice message";
    __warn() << "warn message";
    __error() << "error message";
    __fatal() << "fatal message";
  }

}

int main(int arg_count, char** arg_values) {
  corefungi::init(arg_count, arg_values);

  __trace() << "debug message";
  __debug() << "debug message";
  __info() << "info message";
  __notice() << "notice message";
  __warn() << "warn message";
  __error() << "error message";
  __fatal() << "fatal message";

  std::vector< std::string > v = {
    "this", "is", "a", "vector", "of", "strings"
  };
  __info() << v << v;

  test_module::test();

  return 0;
}
