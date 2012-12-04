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

#include <boost/log/sinks.hpp>
#include <boost/log/utility/empty_deleter.hpp>
#include <boost/log/attributes.hpp>

#include <boost/log/utility/init/to_console.hpp>
#include <boost/log/utility/init/to_file.hpp>
#include <boost/log/utility/init/common_attributes.hpp>
#include <boost/log/formatters/char_decorator.hpp>

#include <boost/date_time.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/filesystem.hpp>
#include <boost/preprocessor.hpp>

#include <iostream>
#include <fstream>

#include <unistd.h>
#include <cstdio>

namespace logging {

#define __ansi(code_m)         "\e[" code_m "m"
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

  static const char* const level_names[] = {
    "[trace  ]",
    "[debug  ]",
    "[info   ]",
    "[notice ]",
    "[Warning]",
    "[ERROR  ]",
    "[FATAL  ]",
  };

  std::ostream& operator<<(std::ostream& s, logging::level const& l) {
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

  std::string const default_context::module = "main";

  template< typename >
  struct ansi_decorator_traits;

  template< >
  struct ansi_decorator_traits< char > {
    static boost::iterator_range< const char* const* > get_patterns() {
      return boost::make_iterator_range(level_names);

    }

#define __ansi_trace()   __ansi(__ansi_faint())
#define __ansi_debug()   __ansi(__ansi_color(fg, normal, green) ";" __ansi_faint())
#define __ansi_info()    __ansi(__ansi_color(fg, normal, blue))
#define __ansi_notice()  __ansi(__ansi_color(fg, normal, cyan))
#define __ansi_warning() __ansi(__ansi_color(fg, normal, yellow))
#define __ansi_error()   __ansi(__ansi_color(fg, normal, red))
#define __ansi_fatal()   __ansi(__ansi_color(fg, bright, black) ";" __ansi_color(bg, normal, red) ";" __ansi_bold())

    static boost::iterator_range< const char* const* > get_replacements() {
      static const char* const replacements[] = {
        __ansi_trace() "[trace  ]",
        __ansi_debug() "[debug  ]",
        __ansi_info() "[info   ]",
        __ansi_notice() "[notice ]",
        __ansi_warning() "[Warning]",
        __ansi_error() "[ERROR  ]",
        __ansi_fatal() "[FATAL  ]",
      };

      return boost::make_iterator_range(replacements);
    }

  };

  struct fmt_ansi_decorator_gen {
    template< typename FormatterT >
    blf::fmt_char_decorator< FormatterT > operator[](FormatterT const& fmt) const {
      typedef blf::fmt_char_decorator< FormatterT >                  decorator;
      typedef ansi_decorator_traits< typename decorator::char_type > traits_t;

      return decorator(fmt, traits_t::get_patterns(), traits_t::get_replacements());
    }

  };

  fmt_ansi_decorator_gen const ansi_level = {};

  BOOST_LOG_GLOBAL_LOGGER_INIT(logger, logger_class) {
    blg::add_common_attributes();

    bool const is_a_tty = isatty(fileno(stderr));

    if (is_a_tty) {
      blg::init_log_to_console(std::clog,
                               blk::format = blf::format(__ansi_line("%1%: %2% %3% %4%: %5%"))
                                             % blf::attr< uint32_t >("LineID", blk::format = "%08x")
                                             % blf::date_time< boost::posix_time::ptime >("TimeStamp")
                                             % ansi_level[blf::attr < logging::level > ("Severity")]
                                             % blf::attr< std::string >("Channel")
                                             % blf::message(),
                               blk::auto_flush = true,
                               blk::filter = blt::attr< logging::level >("Severity") >= logging::level::trace);
    } else {
      blg::init_log_to_console(std::clog,
                               blk::format = blf::format("%1%: %2% %3% %4%: %5%")
                                             % blf::attr< uint32_t >("LineID", blk::format = "%08x")
                                             % blf::date_time< boost::posix_time::ptime >("TimeStamp")
                                             % blf::attr< logging::level >("Severity")
                                             % blf::attr< std::string >("Channel")
                                             % blf::message(),
                               blk::auto_flush = true,
                               blk::filter = blt::attr< logging::level >("Severity") >= logging::level::trace);
    }

    // blg::init_log_to_file("application.log",
    //                       blk::format = blf::format("%1%: %2% %3% %4%: %5%")
    //                                     % blf::attr< uint32_t >("LineID", blk::format = "%08x")
    //                                     % blf::date_time< boost::posix_time::ptime >("TimeStamp")
    //                                     % blf::attr< logging::level >("Severity")
    //                                     % blf::attr< std::string >("Channel")
    //                                     % blf::message(),
    //                       blk::rotation_size = 10 * 1024 * 1024);

    boost::shared_ptr< blg::core > core = blg::core::get();

    return logger_class(blk::severity = logging::level::info);
  }

} // namespace logging
