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
#include <boost/shared_ptr.hpp>

#include <boost/filesystem.hpp>

#include <iostream>
#include <fstream>

namespace logging {

#define ANSI_ESCAPE() "\033["

#define ANSI_CLEAR() ANSI_ESCAPE() "0m"

#define ANSI_NORMAL() ANSI_ESCAPE() "0;"
#define ANSI_BRIGHT() ANSI_ESCAPE() "1;"
#define ANSI_FAINT()  ANSI_ESCAPE() "2;"
#define ANSI_ITALIC() ANSI_ESCAPE() "3;"

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

  BOOST_LOG_GLOBAL_LOGGER_INIT(logger, logger_t) {
    logging::add_common_attributes();

    auto filter_debug(flt::attr< ::logging::severity_level >("Severity") == ::logging::severity_level::debug);
    auto filter_info(flt::attr< ::logging::severity_level >("Severity") == ::logging::severity_level::info);
    auto filter_warning(flt::attr< ::logging::severity_level >("Severity") == ::logging::severity_level::warning);
    auto filter_error(flt::attr< ::logging::severity_level >("Severity") == ::logging::severity_level::error);
    auto filter_fatal(flt::attr< ::logging::severity_level >("Severity") == ::logging::severity_level::fatal);

    auto format_ansi_debug   = fmt::format(ANSI_FG(ANSI_FAINT, ANSI_GREEN) "[debug  ]");
    auto format_ansi_info    = fmt::format(ANSI_FG(ANSI_NORMAL, ANSI_BLUE) "[info   ]");
    auto format_ansi_warning = fmt::format(ANSI_FG(ANSI_NORMAL, ANSI_YELLOW) "[warning]");
    auto format_ansi_error   = fmt::format(ANSI_FG(ANSI_NORMAL, ANSI_RED) "[ERROR  ]");
    auto format_ansi_fatal   = fmt::format(ANSI_BG(ANSI_NORMAL, ANSI_RED) ANSI_FG(ANSI_BRIGHT, ANSI_BLACK) "[FATAL  ]");

    auto format_debug   = fmt::format("[debug  ]");
    auto format_info    = fmt::format("[info   ]");
    auto format_warning = fmt::format("[warning]");
    auto format_error   = fmt::format("[ERROR  ]");
    auto format_fatal   = fmt::format("[FATAL  ]");

    auto ansi_level_format = fmt::if_(filter_debug)[format_ansi_debug].else_[fmt::if_(filter_info)[format_ansi_info].else_[fmt::if_(filter_warning)[format_ansi_warning].else_[fmt::if_(filter_error)[format_ansi_error].else_[fmt::if_(filter_fatal)[format_ansi_fatal]]]]];
    auto level_format      = fmt::if_(filter_debug)[format_debug].else_[fmt::if_(filter_info)[format_info].else_[fmt::if_(filter_warning)[format_warning].else_[fmt::if_(filter_error)[format_error].else_[fmt::if_(filter_fatal)[format_fatal]]]]];

    bool        is_a_tty = ::logging::detail::isatty(::std::clog);
    char const* format   = (is_a_tty ? ANSI_CLEAR() "%1%: %2% %3% %4%" ANSI_CLEAR() : "%1%: %2% %3% %4%");
    logging::init_log_to_console(::std::clog,
                                 keywords::format = fmt::format(format)
                                                    % fmt::attr< unsigned int >("LineID", keywords::format = "%08x") % fmt::date_time
                                                    < ::boost::posix_time::ptime
                                                    >("TimeStamp") % (is_a_tty ? ansi_level_format : level_format) % fmt::message(),
                                 keywords::auto_flush = true,
                                 keywords::filter = flt::attr< ::logging::severity_level >("Severity")
                                                    >= ::logging::severity_level::debug);
    logging::init_log_to_file("application.log",
                              keywords::format = fmt::format("%1%: %2% %3% %4%")
                                                 % fmt::attr< unsigned int >("LineID", keywords::format = "%08x") % fmt::date_time
                                                 < ::boost::posix_time::ptime >("TimeStamp") % level_format % fmt::message(),
                              keywords::rotation_size = 10 * 1024 * 1024);

    boost::shared_ptr< logging::core > core = logging::core::get();

    return logger_t(keywords::severity = ::logging::severity_level::info);
  }

} // namespace logging
