/**
 * @date 19 déc. 2009
 * @todo comment
 */

#ifndef LOGGER_HXX_
#define LOGGER_HXX_

#include <boost/log/core.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/filters.hpp>
#include <boost/log/keywords/severity.hpp>
#include <boost/log/formatters.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/trivial.hpp>

namespace logging {
  namespace logging = ::boost::log;
  namespace sinks = ::boost::log::sinks;
  namespace src = ::boost::log::sources;
  namespace fmt = ::boost::log::formatters;
  namespace flt = ::boost::log::filters;
  namespace attrs = ::boost::log::attributes;
  namespace keywords = ::boost::log::keywords;

  //! Trivial severity levels
  enum class severity_level {
    trace, debug, info, warning, error, fatal
  };

  char const* dump_level(::logging::severity_level lvl);

  template< typename char_t, typename traits_t >
  ::std::basic_ostream< char_t, traits_t >& operator<<(::std::basic_ostream< char_t, traits_t >& stream_in,
                                                       ::logging::severity_level const level_in);

  //  typedef src::severity_logger_mt< severity_level > logger_t;
  BOOST_LOG_DECLARE_LOGGER_MT(logger_t, ( src::severity< ::logging::severity_level > ));
  BOOST_LOG_GLOBAL_LOGGER(logger, logger_t)

#define __logML(module_m, level_m) \
            BOOST_LOG_STREAM_WITH_PARAMS(::logging::logger::get(), (::boost::log::keywords::severity = ::logging::severity_level::level_m)) << "[" #module_m "] "

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

#endif /* LOGGER_HXX_ */
