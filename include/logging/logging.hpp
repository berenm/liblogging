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
#include <boost/log/common.hpp>
#include <ostream>

namespace logging {
  namespace blg = boost::log;
  namespace bls = boost::log::sources;
  namespace ble = boost::log::expressions;
  namespace blk = boost::log::keywords;
  namespace bla = boost::log::attributes;

  class level {
    private:
      size_t l;

    public:
      level() : l(0) {}
      level(size_t const l) : l(l) {}

      template< typename S > friend S& operator<<(S& s, logging::level const& l);
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

  typedef bls::severity_channel_logger_mt< logging::level > logger_class;
  BOOST_LOG_GLOBAL_LOGGER(logger, logger_class);

  template< typename Context >
  class logger_maker {
    private:
      logger_maker(logging::level const& level, std::string const& module=Context::module) :
        logger(logging::logger::get()),
        record(this->logger.open_record((boost::log::keywords::severity = level, boost::log::keywords::channel = module)))
      {}

      logging::logger_class& logger;
      blg::record            record;

      typedef boost::log::aux::record_pump< logging::logger_class > pump_type;

    public:
      static inline logger_maker trace() { return logger_maker(logging::level::trace); }
      static inline logger_maker debug() { return logger_maker(logging::level::debug); }
      static inline logger_maker info() { return logger_maker(logging::level::info); }
      static inline logger_maker notice() { return logger_maker(logging::level::notice); }
      static inline logger_maker warning() { return logger_maker(logging::level::warning); }
      static inline logger_maker error() { return logger_maker(logging::level::error); }
      static inline logger_maker fatal() { return logger_maker(logging::level::fatal); }

      inline pump_type pump() { return boost::log::aux::make_record_pump(this->logger, this->record); }
  };

  struct default_context {
    static std::string const module;
  };

  typedef logger_maker< default_context > log;

} // namespace logging

#define __fatal()  logging::log::fatal().pump().stream()
#define __error()  logging::log::error().pump().stream()
#define __warn()   logging::log::warning().pump().stream()
#define __info()   logging::log::info().pump().stream()
#define __notice() logging::log::notice().pump().stream()
#define __debug()  logging::log::debug().pump().stream()
#define __trace()  logging::log::trace().pump().stream()

#endif /* LOGGER_HXX_ */
