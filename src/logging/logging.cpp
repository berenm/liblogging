/**
 * @file
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt
 */

#include "logging/logging.hpp"

#include <corefungi.hpp>

#include <boost/preprocessor.hpp>

#include <iostream>

#include <unistd.h>
#include <cstdio>

namespace liblog {
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
#define __ansi_color_default "9"

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

#define __ansi_trace()   __ansi(__ansi_faint())
#define __ansi_debug()   __ansi(__ansi_color(fg, normal, green) ";" __ansi_faint())
#define __ansi_info()    __ansi(__ansi_color(fg, normal, blue))
#define __ansi_notice()  __ansi(__ansi_color(fg, normal, cyan))
#define __ansi_warning() __ansi(__ansi_color(fg, normal, yellow))
#define __ansi_error()   __ansi(__ansi_color(fg, normal, red))
#define __ansi_fatal()   __ansi(__ansi_color(bg, bright, default) ";" __ansi_color(fg, normal, red) ";" __ansi_negative() ";" __ansi_bold())

  static constexpr char const* const level_names[] = {
    "\n",
    "trace",
    "debug",
    "info",
    "notice",
    "Warning",
    "ERROR",
    "FATAL",
  };

  static constexpr char const* const ansi_level_names[] = {
    __ansi("0")     "\n",
    __ansi_trace()   "❧",
    __ansi_debug()   "❡",
    __ansi_info()    "✔",
    __ansi_notice()  "❢",
    __ansi_warning() "▲",
    __ansi_error()   "✖",
    __ansi_fatal()   "⚡",
  };

  static cfg::sprout const o = {
    "Logging options", {
      { "log.level",   "the logging verbosity", cfg::short_name = "l", cfg::of_type< std::vector< std::string > >() }
    }
  };

  std::ostream& operator<<(std::ostream& s, liblog::level const& l) {
    static const bool is_atty = isatty(fileno(stderr));

    if (is_atty)
      s << ansi_level_names[static_cast< int8_t >(l)];
    else
      s << level_names[static_cast< int8_t >(l)];

    return s;
  }

  liblog::level logstream::verbosity() {
    static const char verbosity = cfg::get("log.level.#0")[0];

    switch (verbosity) {
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
      default:
        return level::info;
    }
  }

} // namespace liblog
