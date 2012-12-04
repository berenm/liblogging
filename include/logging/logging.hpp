/**
 * @file
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
 *
 * @date 19 déc. 2009
 * @todo comment
 */

#ifndef LOGGER_HXX_
#define LOGGER_HXX_

#include <boost/thread.hpp>
#include <boost/log/core.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/sources/channel_feature.hpp>
#include <boost/log/filters.hpp>
#include <boost/log/keywords/severity.hpp>
#include <boost/log/keywords/channel.hpp>
#include <boost/log/formatters.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/trivial.hpp>
#include <ostream>

namespace logging {
  namespace blg = boost::log;
  namespace bls = boost::log::sources;
  namespace blf = boost::log::formatters;
  namespace blt = boost::log::filters;
  namespace blk = boost::log::keywords;

  class level {
    private:
      size_t l;

    public:
      level() : l(0) {}
      level(size_t const l) : l(l) {}

      friend std::ostream& operator<<(std::ostream& s, logging::level const& l);
      bool operator>=(level const& o) const { return this->l >= o.l; }
      operator size_t() const { return this->l; }

      static const level trace;
      static const level debug;
      static const level info;
      static const level notice;
      static const level warning;
      static const level error;
      static const level fatal;
  };

  BOOST_LOG_DECLARE_LOGGER_MT(logger_class, (bls::severity< logging::level > )(bls::channel< std::string > ));
  BOOST_LOG_GLOBAL_LOGGER(logger, logger_class);

  template< typename Context >
  class logger_maker {
    private:
      static inline boost::log::aux::record_pump< logging::logger_class > make(logging::level const& level, std::string const& module=Context::module) {
        return boost::log::aux::make_pump_stream(logging::logger::get(), logging::logger::get().open_record((boost::log::keywords::severity = level, boost::log::keywords::channel = module)));
      }

    public:
      static inline boost::log::aux::record_pump< logging::logger_class > trace() { return logger_maker::make(logging::level::trace); }
      static inline boost::log::aux::record_pump< logging::logger_class > debug() { return logger_maker::make(logging::level::debug); }
      static inline boost::log::aux::record_pump< logging::logger_class > info() { return logger_maker::make(logging::level::info); }
      static inline boost::log::aux::record_pump< logging::logger_class > notice() { return logger_maker::make(logging::level::notice); }
      static inline boost::log::aux::record_pump< logging::logger_class > warning() { return logger_maker::make(logging::level::warning); }
      static inline boost::log::aux::record_pump< logging::logger_class > error() { return logger_maker::make(logging::level::error); }
      static inline boost::log::aux::record_pump< logging::logger_class > fatal() { return logger_maker::make(logging::level::fatal); }
  };

  struct default_context {
    static std::string const module;
  };

  typedef logger_maker< default_context > log;

} // namespace logging

#define __fatal()  logging::log::fatal().stream()
#define __error()  logging::log::error().stream()
#define __warn()   logging::log::warning().stream()
#define __info()   logging::log::info().stream()
#define __notice() logging::log::notice().stream()
#define __debug()  logging::log::debug().stream()
#define __trace()  logging::log::trace().stream()

#endif /* LOGGER_HXX_ */
