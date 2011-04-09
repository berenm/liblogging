/**
 * @file
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
 *
 * @date 21 nov. 2009
 * @todo comment
 */

#include "logging/logging.hpp"
#include "logging/detail/fileno.hpp"

#include <boost/log/sinks.hpp>
#include <boost/log/utility/empty_deleter.hpp>
#include <boost/log/attributes.hpp>

#include <boost/log/utility/init/to_console.hpp>
#include <boost/log/utility/init/to_file.hpp>
#include <boost/log/utility/init/common_attributes.hpp>

#include <boost/date_time.hpp>

#include <iostream>
#include <fstream>

namespace logging {

#define ANSI_ESCAPE() "\033["

#define ANSI_CLEAR() ANSI_ESCAPE() "0m"

#define ANSI_NORMAL()  ANSI_ESCAPE() "0;"
#define ANSI_BRIGHT()  ANSI_ESCAPE() "1;"
#define ANSI_FAINT()   ANSI_ESCAPE() "2;"
#define ANSI_ITALIC()  ANSI_ESCAPE() "3;"

#define ANSI_FG(mode_m, color_m) mode_m() "3" color_m() "m"
#define ANSI_BG(mode_m, color_m) mode_m() "4" color_m() "m"

#define ANSI_BLACK()   "0"
#define ANSI_RED()     "1"
#define ANSI_GREEN()   "2"
#define ANSI_YELLOW()  "3"
#define ANSI_BLUE()    "4"
#define ANSI_MAGENTA() "5"
#define ANSI_CYAN()    "6"
#define ANSI_WHITE()   "7"

  static inline char const* dump_level(::logging::severity_level const level_in, bool const ansi_mode_in) {
    if (ansi_mode_in) {
      switch (level_in) {
        case ::logging::severity_level::debug:
          return ANSI_FG(ANSI_NORMAL, ANSI_GREEN) "[debug  ]";
        case ::logging::severity_level::info:
          return ANSI_FG(ANSI_FAINT, ANSI_BLUE) "[info   ]";
        case ::logging::severity_level::warning:
          return ANSI_FG(ANSI_NORMAL, ANSI_RED) "[warning]";
        case ::logging::severity_level::error:
          return ANSI_FG(ANSI_FAINT, ANSI_MAGENTA) "[ERROR  ]";
        case ::logging::severity_level::fatal:
          return ANSI_FG(ANSI_BRIGHT, ANSI_RED) "[FATAL  ]";
        default:
          return ANSI_FG(ANSI_NORMAL, ANSI_BLACK) "";
      }
    } else {
      switch (level_in) {
        case ::logging::severity_level::debug:
          return "[debug  ]";
        case ::logging::severity_level::info:
          return "[info   ]";
        case ::logging::severity_level::warning:
          return "[warning]";
        case ::logging::severity_level::error:
          return "[ERROR  ]";
        case ::logging::severity_level::fatal:
          return "[FATAL  ]";
        default:
          return "";
      }
    }

    return "";
  }

  template< >
  ::std::wostream& operator<<< wchar_t > (::std::wostream& stream_in, ::logging::severity_level const level_in) {
    return stream_in << dump_level(level_in, ::logging::detail::isatty(::std::clog));
  }

  template< >
  ::std::ostream& operator<<< char > (::std::ostream& stream_in, ::logging::severity_level const level_in) {
    return stream_in << dump_level(level_in, ::logging::detail::isatty(::std::clog));
  }

  BOOST_LOG_GLOBAL_LOGGER_INIT(logger, logger_t) {
    logging::add_common_attributes();

    boost::shared_ptr< logging::core > core = logging::core::get();

    bool is_a_tty = ::logging::detail::isatty(::std::clog);
    char const* format = (is_a_tty ? ANSI_CLEAR() "%1%: %2% %3% %4%" ANSI_CLEAR() : "%1%: %2% %3% %4%");
    // Create a backend and attach a couple of streams to it
    ::boost::shared_ptr< sinks::text_ostream_backend > backend = boost::make_shared< sinks::text_ostream_backend >();
    backend->add_stream(::boost::shared_ptr< std::ostream >(&::std::clog, logging::empty_deleter()));
    backend->set_formatter(fmt::format(format) % fmt::attr< unsigned int >("LineID", keywords::format = "%08x")
        % fmt::date_time< ::boost::posix_time::ptime >("TimeStamp")
        % fmt::attr< ::logging::severity_level >("Severity") % fmt::message());

    // Enable auto-flushing after each log record written
    backend->auto_flush(true);

    // Wrap it into the frontend and register in the core.
    // The backend requires synchronization in the frontend.
    typedef sinks::synchronous_sink< sinks::text_ostream_backend > sink_t;
    boost::shared_ptr< sink_t > sink(new sink_t(backend));

    core->add_sink(sink);

    return logger_t(keywords::severity = ::logging::severity_level::info);
  }

} // namespace logging
