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
    static constexpr char const module_name[] = "module";
  }

  static void test() {
    __trace() << "trace message";
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

  __trace() << "trace message";
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

  auto const& t = std::make_tuple("this is ", 1, std::vector< char > {'t','u','p','l','e'});
  __warn() << t << " " << t;

  test_module::test();

  return 0;
}
