/**
 * @file
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt
 */

#include "logging/logging.hpp"

#include <corefungi.hpp>

#include <boost/log/utility/setup.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/expressions.hpp>

#include <boost/date_time.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/filesystem.hpp>
#include <boost/preprocessor.hpp>

#include <iostream>
#include <fstream>

#include <unistd.h>
#include <cstdio>

namespace logging {
  namespace cfg = ::corefungi;

#define __ansi(code_m)         "\033[" code_m "m"
#define __ansi_line(message_m) __ansi("0") message_m __ansi("0")

#define __ansi_color_black   "0"
#define __ansi_color_red     "1"
#define __ansi_color_green   "2"
#define __ansi_color_yellow  "3"
#define __ansi_color_blue    "4"
#define __ansi_color_magenta "5"
#define __ansi_color_cyan    "6"
#define __ansi_color_white   "7"

#define __ansi_color_foreground_normal(color_m) "3" BOOST_PP_CAT(__ansi_color_, color_m)
#define __ansi_color_background_normal(color_m) "4" BOOST_PP_CAT(__ansi_color_, color_m)
#define __ansi_color_foreground_bright(color_m) "9" BOOST_PP_CAT(__ansi_color_, color_m)
#define __ansi_color_background_bright(color_m) "10" BOOST_PP_CAT(__ansi_color_, color_m)
#define __ansi_color_fg_normal(color_m)         __ansi_color_foreground_normal(color_m)
#define __ansi_color_fg_bright(color_m)         __ansi_color_foreground_bright(color_m)
#define __ansi_color_bg_normal(color_m)         __ansi_color_background_normal(color_m)
#define __ansi_color_bg_bright(color_m)         __ansi_color_background_bright(color_m)

#define __ansi_color(side_m, style_m, color_m) BOOST_PP_CAT(__ansi_color_, BOOST_PP_CAT(side_m, BOOST_PP_CAT(_, style_m))) (color_m)

#define __ansi_normal()     "0"
#define __ansi_bold()       "1"
#define __ansi_faint()      "2"
#define __ansi_italic()     "3"
#define __ansi_underlined() "4"
#define __ansi_negative()   "7"
#define __ansi_crossed()    "9"

  static char const* const level_names[] = {
    "[trace  ]",
    "[debug  ]",
    "[info   ]",
    "[notice ]",
    "[Warning]",
    "[ERROR  ]",
    "[FATAL  ]",
  };

  template< typename S >
  S& operator<<(S& s, logging::level const& l) {
    s << level_names[l.l];

    return s;
  }

  const level level::trace   = level(0);
  const level level::debug   = level(1);
  const level level::info    = level(2);
  const level level::notice  = level(3);
  const level level::warning = level(4);
  const level level::error   = level(5);
  const level level::fatal   = level(6);

#define __ansi_trace()   __ansi(__ansi_faint())
#define __ansi_debug()   __ansi(__ansi_color(fg, normal, green) ";" __ansi_faint())
#define __ansi_info()    __ansi(__ansi_color(fg, normal, blue))
#define __ansi_notice()  __ansi(__ansi_color(fg, normal, cyan))
#define __ansi_warning() __ansi(__ansi_color(fg, normal, yellow))
#define __ansi_error()   __ansi(__ansi_color(fg, normal, red))
#define __ansi_fatal()   __ansi(__ansi_color(fg, bright, black) ";" __ansi_color(bg, normal, red) ";" __ansi_bold())

  static char const* const ansi_level_names[] = {
    __ansi_trace() "[trace  ]",
    __ansi_debug() "[debug  ]",
    __ansi_info() "[info   ]",
    __ansi_notice() "[notice ]",
    __ansi_warning() "[Warning]",
    __ansi_error() "[ERROR  ]",
    __ansi_fatal() "[FATAL  ]",
  };

  BOOST_LOG_ATTRIBUTE_KEYWORD(severity_, "Severity", logging::level)
  BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp_, "TimeStamp", boost::posix_time::ptime)
  BOOST_LOG_ATTRIBUTE_KEYWORD(channel_, "Channel", std::string)
  BOOST_LOG_ATTRIBUTE_KEYWORD(line_id_, "LineID", uint32_t)
  BOOST_LOG_ATTRIBUTE_KEYWORD(scope_, "Scope", bla::named_scope::value_type)

  static level verbosity() {
    switch (cfg::get("log.level")[0]) {
      case 't':

        return level::trace;

      case 'd':

        return level::debug;

      case 'i':

        return level::info;

      case 'n':

        return level::notice;

      case 'w':

        return level::warning;

      case 'e':

        return level::error;

      case 'f':

        return level::fatal;
    }
  }

  static cfg::sprout const o = {
    "Logging options", {
      { "log.level",   "the logging verbosity", cfg::default_ = "info", cfg::short_name = "l" }
    }
  };

  BOOST_LOG_GLOBAL_LOGGER_INIT(logger, logger_class) {
    blg::add_common_attributes();

    bool const is_a_tty = isatty(fileno(stderr));

    if (is_a_tty) {
      blg::add_console_log(std::clog,
                           blk::format = ble::format(__ansi_line("%1% (%2%) %3% %4%: %5%"))
                                         % timestamp_
                                         % ble::c_decor[ble::stream << std::hex << std::setw(8) << std::setfill('0') << line_id_ << std::dec << std::setfill(' ')]
                                         % ble::char_decor(level_names, ansi_level_names)[ble::stream << severity_]
                                         % channel_
                                         % ble::message,
                           blk::auto_flush = true,
                           blk::filter = severity_ >= logging::verbosity());
    } else {
      blg::add_console_log(std::clog,
                           blk::format = ble::format("%1% (%2%) %3% %4%: %5%")
                                         % timestamp_
                                         % ble::c_decor[ble::stream << std::hex << std::setw(8) << std::setfill('0') << line_id_ << std::dec << std::setfill(' ')]
                                         % severity_
                                         % channel_
                                         % ble::message,
                           blk::auto_flush = true,
                           blk::filter = severity_ >= logging::verbosity());
    }

    // blg::init_log_to_file("application.log",
    // blk::format = ble::format("%1%: %2% %3% %4%: %5%")
    // % ble::attr< uint32_t >("LineID", blk::format = "%08x")
    // % timestamp_
    // % ble::attr< logging::level >("Severity")
    // % channel_
    // % ble::message,
    // blk::rotation_size = 10 * 1024 * 1024);

    boost::shared_ptr< blg::core > core = blg::core::get();

    return logger_class(blk::severity = logging::level::info);
  }

} // namespace logging
