/**
 * @file
 * @date 21 nov. 2009
 * @todo comment
 */

#include "logging/logging.hpp"
#include <boost/logging/format.hpp>
#include <boost/logging/format/formatter/tags.hpp>
#include <boost/logging/format/formatter/high_precision_time.hpp>

namespace logging {
  namespace blf = boost::logging::formatter;

  BOOST_DEFINE_LOG_FILTER(log_filter, finder::filter )
  BOOST_DEFINE_LOG(logger, finder::logger)

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

#define ANSI_BOOST_LOG_STR(ansi_code_m, string) \
  (const ::boost::logging::char_type*) ::boost::logging::ansi_unicode_char_holder ( \
        ansi_code_m string, \
        L"" ansi_code_m string )

  const ::boost::logging::char_type* dump_custom_levels::dump(::boost::logging::level::type lvl) {
    using namespace ::boost::logging::level;
    switch (lvl) {
      case debug:
        return ANSI_BOOST_LOG_STR( ANSI_FG(ANSI_NORMAL, ANSI_GREEN), "[debug] ");
      case info:
        return ANSI_BOOST_LOG_STR(ANSI_FG(ANSI_FAINT, ANSI_BLUE), "[info]  ");
      case warning:
        return ANSI_BOOST_LOG_STR(ANSI_FG(ANSI_NORMAL, ANSI_RED), "[warn]  ");
      case error:
        return ANSI_BOOST_LOG_STR(ANSI_FG(ANSI_FAINT, ANSI_MAGENTA), "[ERROR] ");
      case fatal:
        return ANSI_BOOST_LOG_STR(ANSI_FG(ANSI_BRIGHT, ANSI_RED), "[FATAL] ");
      default:
        return ANSI_BOOST_LOG_STR(ANSI_FG(ANSI_NORMAL, ANSI_BLACK), "");
    }
  }

#undef ANSI_BOOST_LOG_STR

  void init() {
    namespace bl = boost::logging;
    namespace bld = boost::logging::destination;
    namespace blf = boost::logging::formatter;
    namespace blft = boost::logging::formatter::tag;
    //         add formatters and destinations
    //         That is, how the message is to be formatted and where should it be written to

    blf::named_spacer_t< blf::do_convert_format::append > append(
//        " (%idx%) %fileline% %function% [T%thread_id%]"
        ANSI_CLEAR());
    append.add("idx", blf::idx_t< blf::do_convert_format::append >());
    append.add("thread_id", blft::thread_id_t< ::std::basic_ostringstream< bl::char_type >,
        blf::do_convert_format::append >()); // thread_id tag
    append.add("fileline", blft::file_line_t< blf::do_convert_format::append >());
    append.add("function", blft::function_t< blf::do_convert_format::append >());

    blf::named_spacer prepend(ANSI_CLEAR() "%time% %level%%module%: ");
    prepend.add("time", blf::high_precision_time("$hh:$mm:$ss.$micro")); // time tag
    prepend.add("module", blf::tag::module());
    prepend.add("level", blf::tag::level());

    logger()->writer().add_formatter(append); // file/line tag
    logger()->writer().add_formatter(prepend); // file/line tag

    logger()->writer().add_formatter(blf::append_newline());
    logger()->writer().add_destination(bld::cout());
    //  g_l()->writer().add_destination( destination::file("out.txt") );
    logger()->mark_as_initialized();
  }

} // namespace logging
