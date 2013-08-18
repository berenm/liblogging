/**
 * @file
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef __LOGGING_LOGGING_HPP__
#define __LOGGING_LOGGING_HPP__

#include <vector>
#include <functional>
#include <iostream>

#include <logging/ostream.hpp>

namespace liblog {
  enum class level {
    flush,
    trace,
    debug,
    info,
    notice,
    warning,
    error,
    fatal,
  };

  std::ostream& operator<<(std::ostream& s, liblog::level const& l);

  struct logstream {
    static liblog::level verbosity();

    logstream(char const* module_name, liblog::level const& level) :
      enabled { level >= logstream::verbosity() }
    {
      if (!this->enabled) return;

      this->callbacks.reserve(20);
      (*this) << module_name << " " << level << ": ";
    }

    ~logstream() {
      if (!this->enabled) return;

      (*this) << level::flush;
      for (auto const& f : callbacks) { f(std::clog); }
    }

    template< typename T >
    logstream const& operator<<(T&& v) const {
      if (this->enabled) 
        this->callbacks.emplace_back([v](std::ostream& s) -> std::ostream& { return s << v; });

      return *this;
    }

    logstream const& operator<<(std::ostream& (f) (std::ostream &)) const {
      if (this->enabled)
        this->callbacks.emplace_back(std::move(f));

      return *this;
    }

    typedef std::function< std::ostream& (std::ostream&) > callback_type;
    typedef std::vector< callback_type >                   callbacks_type;

    bool const             enabled;
    callbacks_type mutable callbacks;
  };

} // namespace liblog

namespace logging {
  static constexpr char const module_name[] = "main";
} // namespace logging

#define __fatal()  liblog::logstream{logging::module_name, liblog::level::fatal}
#define __error()  liblog::logstream{logging::module_name, liblog::level::error}
#define __warn()   liblog::logstream{logging::module_name, liblog::level::warning}
#define __info()   liblog::logstream{logging::module_name, liblog::level::info}
#define __notice() liblog::logstream{logging::module_name, liblog::level::notice}
#define __debug()  liblog::logstream{logging::module_name, liblog::level::debug}
#define __trace()  liblog::logstream{logging::module_name, liblog::level::trace}

#endif // ifndef __LOGGING_LOGGING_HPP__
