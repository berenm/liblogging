/**
 * @file
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef __LOGGING_LOGGING_HPP__
#define __LOGGING_LOGGING_HPP__

#include <boost/thread.hpp>
#include <boost/log/common.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>

#include "logging/ostream.hpp"

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

  struct log {
    log(logging::level const& level, std::string const& module) :
      logger(logging::logger::get()),
      record(this->logger.open_record((boost::log::keywords::severity = level, boost::log::keywords::channel = module)))
    {}

    logging::logger_class& logger;
    blg::record            record;

    typedef boost::log::aux::record_pump< logging::logger_class > pump_type;

    inline pump_type pump() { return boost::log::aux::make_record_pump(this->logger, this->record); }
  };

  static constexpr char module_name[] = "main";

} // namespace logging

#define __fatal()  ::logging::log(::logging::level::fatal, logging::module_name).pump().stream()
#define __error()  ::logging::log(::logging::level::error, logging::module_name).pump().stream()
#define __warn()   ::logging::log(::logging::level::warning, logging::module_name).pump().stream()
#define __info()   ::logging::log(::logging::level::info, logging::module_name).pump().stream()
#define __notice() ::logging::log(::logging::level::notice, logging::module_name).pump().stream()
#define __debug()  ::logging::log(::logging::level::debug, logging::module_name).pump().stream()
#define __trace()  ::logging::log(::logging::level::trace, logging::module_name).pump().stream()

#endif // ifndef __LOGGING_LOGGING_HPP__
