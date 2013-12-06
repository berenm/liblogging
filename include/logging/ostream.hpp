/**
 * @file
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef __LOGGING_OSTREAM_HPP__
#define __LOGGING_OSTREAM_HPP__

#include <cstdint>

#include <logging/logging.hpp>

#include <vector>
#include <queue>
#include <list>
#include <forward_list>
#include <stack>
#include <valarray>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <complex>
#include <ostream>
#include <array>
#include <bitset>
#include <tuple>
#include <functional>

namespace liblog {
  namespace detail {

    struct sequence_container;
    struct associative_container;
    struct collection_container;

    template< typename T > struct container_type;

    template< typename T, typename C = typename container_type< T >::type >
    struct delimiters;

    template< typename T >
    struct delimiters< T, void > {
      static constexpr char const* const begine = "";
      static constexpr char const* const ende = "";
    };

    template< >
    struct delimiters< std::string, void > {
      static constexpr char const* const begine = "\"";
      static constexpr char const* const ende = "\"";
    };

    template< > struct delimiters< char const*, void > : delimiters< std::string, void > {};
    template< > struct delimiters< char*, void > : delimiters< std::string, void > {};

    template< >
    struct delimiters< char, void > {
      static constexpr char const* const begine = "\'";
      static constexpr char const* const ende = "\'";
    };

    template< typename  T >
    struct delimiters< T, sequence_container > {
      static constexpr char const begin     = '[';
      static constexpr char const end       = ']';
      static constexpr char const separator = ',';
    };

    template< typename T >
    struct delimiters< T, associative_container > {
      static constexpr char const begin     = '{';
      static constexpr char const end       = '}';
      static constexpr char const separator = ',';
    };

    template< typename T >
    struct delimiters< T, collection_container > {
      static constexpr char const begin     = '(';
      static constexpr char const end       = ')';
      static constexpr char const separator = ',';
    };

#define STL_OSTREAM_DECLARE_CONTAINER_TYPE(container_m, type_m)                      \
  template< typename ... Types > struct container_type< container_m< Types ... > > { \
    typedef  type_m type;                                                            \
  }

    STL_OSTREAM_DECLARE_CONTAINER_TYPE(std::deque, sequence_container);
    STL_OSTREAM_DECLARE_CONTAINER_TYPE(std::list, sequence_container);
    STL_OSTREAM_DECLARE_CONTAINER_TYPE(std::forward_list, sequence_container);
    STL_OSTREAM_DECLARE_CONTAINER_TYPE(std::vector, sequence_container);

    STL_OSTREAM_DECLARE_CONTAINER_TYPE(std::stack, sequence_container);
    STL_OSTREAM_DECLARE_CONTAINER_TYPE(std::queue, sequence_container);
    STL_OSTREAM_DECLARE_CONTAINER_TYPE(std::priority_queue, sequence_container);

    STL_OSTREAM_DECLARE_CONTAINER_TYPE(std::valarray, sequence_container);

    STL_OSTREAM_DECLARE_CONTAINER_TYPE(std::set, associative_container);
    STL_OSTREAM_DECLARE_CONTAINER_TYPE(std::multiset, associative_container);
    STL_OSTREAM_DECLARE_CONTAINER_TYPE(std::map, associative_container);
    STL_OSTREAM_DECLARE_CONTAINER_TYPE(std::multimap, associative_container);
    STL_OSTREAM_DECLARE_CONTAINER_TYPE(std::unordered_set, associative_container);
    STL_OSTREAM_DECLARE_CONTAINER_TYPE(std::unordered_multiset, associative_container);
    STL_OSTREAM_DECLARE_CONTAINER_TYPE(std::unordered_map, associative_container);
    STL_OSTREAM_DECLARE_CONTAINER_TYPE(std::unordered_multimap, associative_container);

    STL_OSTREAM_DECLARE_CONTAINER_TYPE(std::tuple, collection_container);
    STL_OSTREAM_DECLARE_CONTAINER_TYPE(std::pair, collection_container);
    STL_OSTREAM_DECLARE_CONTAINER_TYPE(std::complex, collection_container);

    template< class T, std::size_t N > struct container_type< std::array< T, N > > {
      typedef  sequence_container type;
    };

    // STL_OSTREAM_DECLARE_CONTAINER_TYPE(std::bitset, sequence_container);

#undef STL_OSTREAM_DECLARE_CONTAINER_TYPE

    template< std::size_t const N, typename ... Types >
    struct tuple_printer {
      typedef std::tuple< Types ... > tuple_type;

      template< typename S >
      static inline void print(S& s, std::tuple< Types ... > const& t) {
        typedef typename std::decay< typename std::tuple_element< N, tuple_type >::type >::type value_type;

        detail::tuple_printer< N - 1, Types ... >::print(s, t);

        s << detail::delimiters< tuple_type >::separator << ' ';
        s << detail::delimiters< value_type, void >::begine << std::get< N >(t) << detail::delimiters< value_type, void >::ende;
      }

    };

    template< typename ... Types >
    struct tuple_printer< 0, Types ... > {
      typedef std::tuple< Types ... > tuple_type;

      template< typename S >
      static inline void print(S& s, tuple_type const& t) {
        typedef typename std::decay< typename std::tuple_element< 0, tuple_type >::type >::type value_type;

        s << detail::delimiters< value_type, void >::begine << std::get< 0 >(t) << detail::delimiters< value_type, void >::ende;
      }

    };
  }

  template< typename C, typename T = typename C::value_type, typename Type = typename detail::container_type< C >::type >
  static inline std::ostream& operator<<(std::ostream& s, C const& v);

  template< typename K, typename V >
  static inline std::ostream& operator<<(std::ostream& s, std::pair< K, V > const& p);

  template< typename ... Types >
  static inline std::ostream& operator<<(std::ostream& s, std::tuple< Types ... > const& t);

  template< class R, class ... Args >
  static inline std::ostream& operator<<(std::ostream& s, std::function< R(Args ...) > const& f);

  template< typename C, typename T, typename Type >
  static inline std::ostream& operator<<(std::ostream& s, C const& v) {
    if (std::begin(v) == std::end(v)) {
      s << detail::delimiters< C >::begin << detail::delimiters< C >::end;

      return s;
    }

    s << detail::delimiters< C >::begin;
    T const& f = *std::begin(v);
    for (T const& t : v) {
      if (&t != &f)
        s << detail::delimiters< C >::separator << ' ';
      s << detail::delimiters< T, void >::begine << t << detail::delimiters< T, void >::ende;
    }
    s << detail::delimiters< C >::end;

    return s;
  }

  template< typename K, typename V >
  static inline std::ostream& operator<<(std::ostream& s, std::pair< K, V > const& p) {
    s << p.first << ": " << p.second;

    return s;
  }

  template< typename ... Types >
  static inline std::ostream& operator<<(std::ostream& s, std::tuple< Types ... > const& t) {
    s << detail::delimiters< std::tuple< Types ... > >::begin;
    detail::tuple_printer< sizeof ... (Types) -1, Types ... >::print(s, t);
    s << detail::delimiters< std::tuple< Types ... > >::end;

    return s;
  }

  template< class R, class ... Args >
  static inline std::ostream& operator<<(std::ostream& s, std::function< R(Args ...) > const& f) {
    s << reinterpret_cast< void const* >(*f.template target< R (*)(Args ...) >());

    return s;
  }

}

#endif // ifndef __LOGGING_OSTREAM_HPP__
