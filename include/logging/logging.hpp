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

  static constexpr char module_name[] = "main";

} // namespace logging

#define __fatal()  BOOST_LOG_CHANNEL_SEV(::logging::logger::get(), logging::module_name, ::logging::level::fatal)
#define __error()  BOOST_LOG_CHANNEL_SEV(::logging::logger::get(), logging::module_name, ::logging::level::error)
#define __warn()   BOOST_LOG_CHANNEL_SEV(::logging::logger::get(), logging::module_name, ::logging::level::warning)
#define __info()   BOOST_LOG_CHANNEL_SEV(::logging::logger::get(), logging::module_name, ::logging::level::info)
#define __notice() BOOST_LOG_CHANNEL_SEV(::logging::logger::get(), logging::module_name, ::logging::level::notice)
#define __debug()  BOOST_LOG_CHANNEL_SEV(::logging::logger::get(), logging::module_name, ::logging::level::debug)
#define __trace()  BOOST_LOG_CHANNEL_SEV(::logging::logger::get(), logging::module_name, ::logging::level::trace)

#endif // ifndef __LOGGING_LOGGING_HPP__
