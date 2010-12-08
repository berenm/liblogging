/**
 * @file
 * @date 21 nov. 2009
 * @todo comment
 */

#include "logging/logging.hpp"

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

  char const* dump_level(severity_level lvl) {
    switch (lvl) {
      case debug:
        return ANSI_FG(ANSI_NORMAL, ANSI_GREEN) "[debug  ]";
      case info:
        return ANSI_FG(ANSI_FAINT, ANSI_BLUE) "[info   ]";
      case warning:
        return ANSI_FG(ANSI_NORMAL, ANSI_RED) "[warning]";
      case error:
        return ANSI_FG(ANSI_FAINT, ANSI_MAGENTA) "[ERROR  ]";
      case fatal:
        return ANSI_FG(ANSI_BRIGHT, ANSI_RED) "[FATAL  ]";
      default:
        return ANSI_FG(ANSI_NORMAL, ANSI_BLACK) "";
    }
  }

  void init() {

    logging::add_common_attributes();

    boost::shared_ptr< logging::core > core = logging::core::get();

    // Create a backend and attach a couple of streams to it
    ::boost::shared_ptr< sinks::text_ostream_backend > backend = boost::make_shared< sinks::text_ostream_backend >();
    backend->add_stream(::boost::shared_ptr< std::ostream >(&::std::clog, logging::empty_deleter()));
    backend->set_formatter(fmt::format(ANSI_CLEAR() "%1%: %2% %3% %4%" ANSI_CLEAR())
        % fmt::attr< unsigned int >("LineID", keywords::format = "%08x")
        % fmt::date_time< ::boost::posix_time::ptime >("TimeStamp")
        % fmt::attr< ::logging::severity_level >("Severity") % fmt::message());

    // Enable auto-flushing after each log record written
    backend->auto_flush(true);

    // Wrap it into the frontend and register in the core.
    // The backend requires synchronization in the frontend.
    typedef sinks::synchronous_sink< sinks::text_ostream_backend > sink_t;
    boost::shared_ptr< sink_t > sink(new sink_t(backend));

    core->add_sink(sink);
  }

} // namespace logging
