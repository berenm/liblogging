/**
 * @file
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
 *
 * @date 11 janv. 2011
 * @todo comment
 */

#ifndef LOGGING_DETAIL_FILENO_HPP_
#define LOGGING_DETAIL_FILENO_HPP_

#include <iosfwd>

namespace logging {
  namespace detail {

    template< typename char_t, typename traits_t >
    int fileno(::std::basic_ios< char_t, traits_t > const& stream_in);

    template< typename char_t, typename traits_t >
    bool isatty(::std::basic_ios< char_t, traits_t > const& stream_in);

  } // namespace detail
} // namespace logging

#endif /* LOGGING_DETAIL_FILENO_HPP_ */
