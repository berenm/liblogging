==========================
liblogging utility library
==========================
.. image:: https://secure.travis-ci.org/berenm/liblogging.png?branch=master
    :alt: Build Status
    :target: https://travis-ci.org/berenm/liblogging

USAGE
`````````````````````
.. code:: cpp

  // 1. include the logging/logging.hpp header.
  #include <logging/logging.hpp>

  // 2. define module names in your namespaces.
  namespace foo {
  namespace core {

    namespace logging { static constexpr char const module_name[] = "foo.core" }

  }
  }

  // 3. use the logging macros, the closest logging::module_name symbol will be used.
  namespace foo {
  namespace core {

    void bar() {
      __fatal() << "bar is not implemented!";
    }

  }
  }

CONFIGURATION
`````````````````````
For now, there is none. Might be possible later on, but the purpose is to have nice logs without configuration.


PORTABILITY
`````````````````````
I develop on GNU/Linux, and this is the main platform.

I've done some tests on Windows, and it required a few tweaks to compile, but mostly because Visual Studio compiler doesn't know enough about C++11. I may add support at some point but that is not a priority. Patches accepted.

Once compiled, it works fine as well, ANSI codes are not supported natively, but the support can be added using ansicon_. Download a pre-built ansicon from the download page, extract it somewhere and make the .exe and .dll executable. Then run ``ansicon -i`` from there and you are done.

.. _ansicon: https://github.com/adoxa/ansicon

COPYING INFORMATION
`````````````````````

 Distributed under the Boost Software License, Version 1.0.

 See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt
