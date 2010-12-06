/**
 * @file
 * @date 21 nov. 2009
 * @todo comment
 */

#include "logging/logging.hpp"

#include <boost/log/sinks.hpp>
#include <boost/log/utility/empty_deleter.hpp>

#include <iostream>
#include <fstream>

namespace logging {
  namespace logging = ::boost::log;
  namespace sinks = ::boost::log::sinks;
  namespace src = ::boost::log::sources;
  namespace fmt = ::boost::log::formatters;
  namespace flt = ::boost::log::filters;
  namespace attrs = ::boost::log::attributes;
  namespace keywords = ::boost::log::keywords;

  //#define ANSI_ESCAPE() "\033["
  //
  //#define ANSI_CLEAR() ANSI_ESCAPE() "0m"
  //
  //#define ANSI_NORMAL()  ANSI_ESCAPE() "0;"
  //#define ANSI_BRIGHT()  ANSI_ESCAPE() "1;"
  //#define ANSI_FAINT()   ANSI_ESCAPE() "2;"
  //#define ANSI_ITALIC()  ANSI_ESCAPE() "3;"
  //
  //#define ANSI_FG(mode_m, color_m) mode_m() "3" color_m() "m"
  //#define ANSI_BG(mode_m, color_m) mode_m() "4" color_m() "m"
  //
  //#define ANSI_BLACK()   "0"
  //#define ANSI_RED()     "1"
  //#define ANSI_GREEN()   "2"
  //#define ANSI_YELLOW()  "3"
  //#define ANSI_BLUE()    "4"
  //#define ANSI_MAGENTA() "5"
  //#define ANSI_CYAN()    "6"
  //#define ANSI_WHITE()   "7"
  //
  //#define ANSI_BOOST_LOG_STR(ansi_code_m, string) \
//  (const ::boost::logging::char_type*) ::boost::logging::ansi_unicode_char_holder ( \
//        ansi_code_m string, \
//        L"" ansi_code_m string )

  //  const ::boost::logging::char_type* dump_custom_levels::dump(::boost::logging::level::type lvl) {
  //    using namespace ::boost::logging::level;
  //    switch (lvl) {
  //      case debug:
  //      return ANSI_BOOST_LOG_STR( ANSI_FG(ANSI_NORMAL, ANSI_GREEN), "[debug] ");
  //      case info:
  //      return ANSI_BOOST_LOG_STR(ANSI_FG(ANSI_FAINT, ANSI_BLUE), "[info]  ");
  //      case warning:
  //      return ANSI_BOOST_LOG_STR(ANSI_FG(ANSI_NORMAL, ANSI_RED), "[warn]  ");
  //      case error:
  //      return ANSI_BOOST_LOG_STR(ANSI_FG(ANSI_FAINT, ANSI_MAGENTA), "[ERROR] ");
  //      case fatal:
  //      return ANSI_BOOST_LOG_STR(ANSI_FG(ANSI_BRIGHT, ANSI_RED), "[FATAL] ");
  //      default:
  //      return ANSI_BOOST_LOG_STR(ANSI_FG(ANSI_NORMAL, ANSI_BLACK), "");
  //    }
  //  }
  //
  //#undef ANSI_BOOST_LOG_STR

  void init() {

    // Construct the sink
    typedef sinks::synchronous_sink< sinks::text_ostream_backend > text_sink;
    ::boost::shared_ptr< text_sink > sink_ptr = ::boost::make_shared< text_sink >();

    // Add a stream to write log to
    sink_ptr->locked_backend()->add_stream(::boost::make_shared< ::std::ofstream >("console.log"));

    // Register the sink in the logging core
    logging::core::get()->add_sink(sink_ptr);

    // We have to provide an empty deleter to avoid destroying the global stream object
    ::boost::shared_ptr< ::std::ostream > stream_ptr(&::std::clog, logging::empty_deleter());
    sink_ptr->locked_backend()->add_stream(stream_ptr);

  }

} // namespace logging
