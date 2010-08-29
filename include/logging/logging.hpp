/**
 * @date 19 déc. 2009
 * @todo comment
 */

#ifndef LOGGER_HXX_
#define LOGGER_HXX_

#define BOOST_LOG_USE_BOOST_THREADS

#include <boost/logging/format_fwd.hpp>
#include <boost/logging/writer/on_dedicated_thread.hpp>

#include <boost/logging/format_ts.hpp>
#include <boost/logging/format/formatter/named_spacer.hpp>

#include <boost/preprocessor.hpp>

namespace logging {
  namespace bl = ::boost::logging;
  namespace blt = ::boost::logging::tag;
  namespace blo = ::boost::logging::optimize;
  namespace blsu = ::boost::logging::scenario::usage;
  namespace blsufc = ::boost::logging::scenario::usage::filter_::change;
  namespace blsufl = ::boost::logging::scenario::usage::filter_::level;
  namespace blsulc = ::boost::logging::scenario::usage::logger_::change;
  namespace blsulf = ::boost::logging::scenario::usage::logger_::favor;
  namespace blsulg = ::boost::logging::scenario::usage::logger_::gather;

  struct dump_custom_levels {
      static const ::boost::logging::char_type * dump(::boost::logging::level::type lvl);
  };
}

namespace boost {
  namespace logging {
    template< > struct dump_level< override > {
        typedef ::logging::dump_custom_levels type;
    };
  }
}

#include <boost/logging/format/formatter/tags.hpp>

namespace logging {
  typedef blt::holder< blo::cache_string_one_str< >, blt::file_line, blt::thread_id, blt::time, blt::function,
      blt::module, blt::level > log_string;
} // namespace logging

BOOST_LOG_FORMAT_MSG(::logging::log_string)

namespace logging {
  typedef blsu::use< blsufc::set_once_when_multiple_threads, blsufl::use_levels,
      blsulc::set_once_when_multiple_threads, blsulf::speed, blsulg::ostream_like > finder;
  BOOST_DECLARE_LOG_FILTER(log_filter, finder::filter )
  BOOST_DECLARE_LOG(logger, finder::logger)

  void init();

} // namespace logging

#define __fatalM(module_m)  __logML(module_m,fatal)
#define __fatal             __logML(main,fatal)
#define __errorM(module_m)  __logML(module_m,error)
#define __error             __logML(main,error)
#define __warnM(module_m)   __logML(module_m,warning)
#define __warn              __logML(main,warning)
#define __infoM(module_m)   __logML(module_m,info)
#define __info              __logML(main,info)
#define __debugM(module_m)  __logML(module_m,debug)
#define __debug             __logML(main,debug)

#define __logL(level)       __logML(main,level)

#define __logML(module_m, level_m) BOOST_LOG_USE_LOG_IF_LEVEL(::logging::logger(), ::logging::log_filter(), level_m) \
  .set_tag(BOOST_LOG_TAG_FILELINE) \
  .set_tag(BOOST_LOG_TAG_FUNCTION) \
  .set_tag(BOOST_LOG_TAG_LEVEL(level_m)) \
  .set_tag(BOOST_LOG_TAG(module)( # module_m)) \

#endif /* LOGGER_HXX_ */
