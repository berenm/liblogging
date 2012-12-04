/**
 * @file
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
 *
 * @date 11 janv. 2011
 * @todo comment
 */

#include <logging/logging.hpp>

namespace test_module {
  namespace logging {
    struct context {
      static std::string const module;
    };

    std::string const context::module = "log/module/test";

    typedef::logging::logger_maker< context > log;
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
  __trace() << "debug message";
  __debug() << "debug message";
  __info() << "info message";
  __notice() << "notice message";
  __warn() << "warn message";
  __error() << "error message";
  __fatal() << "fatal message";

  test_module::test();

  return 0;
}
