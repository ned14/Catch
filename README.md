This is a fork of [Phil Nash's header only CATCH C++ automated test case framework](https://github.com/philsquared/Catch), only with added thread safety. I've also replaced how the unit tests unique names are generated, his uses \__LINE__, mine uses \__COUNTER__ where the compiler provides that. The \__COUNTER__ method provides much superior uniqueness of naming.

Thread safety has been naively implemented using via CATCH_CONFIG_STL_MUTEX and CATCH_CONFIG_STL_MUTEX_HOLD (these default to the C++ 11 std::mutex, but boost::mutex could go in here easily enough for older compilers), so you'll need a compiler with those. Any recent GCC, clang or MSVC will do. I don't claim I've made the entire library perfectly thread safe, but your basic checks and requires are thread safe, as is unit test stdout/stderr redirection. It works well enough for me, good enough until CATCH gains proper thread support.

Build status (on Travis CI) [![Build Status](https://travis-ci.org/ned14/Catch-ThreadSafe.png)](https://travis-ci.org/ned14/Catch-ThreadSafe)

<a href="https://raw.githubusercontent.com/ned14/Catch-ThreadSafe/develop/single_include/catch.hpp">[The latest, single header, version can be downloaded directly using this link]</a>

## What's the Catch?

Catch stands for C++ Automated Test Cases in Headers and is a multi-paradigm automated test framework for C++ and Objective-C (and, maybe, C). It is implemented entirely in a set of header files, but is packaged up as a single header for extra convenience.

## How to use it
This documentation comprises these three parts:

* [Why do we need yet another C++ Test Framework?](docs/why-catch.md)
* [Tutorial](docs/tutorial.md) - getting started
* [Reference section](docs/Readme.md) - all the details

The documentation will continue until morale improves

## More
* Issues and bugs can be raised on the [Issue tracker on GitHub](https://github.com/philsquared/Catch/issues) except if it regards thread safety,
in which case [use this issue tracker instead](https://github.com/ned14/Catch-ThreadSafe/issues).
