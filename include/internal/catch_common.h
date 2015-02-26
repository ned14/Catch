/*
 *  Created by Phil on 29/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_COMMON_H_INCLUDED
#define TWOBLUECUBES_CATCH_COMMON_H_INCLUDED

#define INTERNAL_CATCH_UNIQUE_NAME_LINE2( name, line ) name##line
#define INTERNAL_CATCH_UNIQUE_NAME_LINE( name, line ) INTERNAL_CATCH_UNIQUE_NAME_LINE2( name, line )
#define INTERNAL_CATCH_UNIQUE_NAME( name ) INTERNAL_CATCH_UNIQUE_NAME_LINE( name, __LINE__ )

// __COUNTER__ was introduced in VS 7 (VS.NET 2002), GCC 4.3 and clang 3.0
#if (defined(_MSC_VER) && (_MSC_VER >= 1300)) \
    || (defined(__GNUC__) && (__GNUC__ >= 4) && (__GNUC_MINOR__ >= 3)) \
    || (defined(__clang__) && (__clang_major__ >= 3))
# undef INTERNAL_CATCH_UNIQUE_NAME
# define INTERNAL_CATCH_UNIQUE_NAME( name ) INTERNAL_CATCH_UNIQUE_NAME_LINE( name, __COUNTER__ )
#endif

#define INTERNAL_CATCH_STRINGIFY2( expr ) #expr
#define INTERNAL_CATCH_STRINGIFY( expr ) INTERNAL_CATCH_STRINGIFY2( expr )

#ifndef CATCH_CONFIG_STL_MUTEX
#  include <mutex>
#  include <atomic>
// The rigmarole below is to work around lack of thread safe static initialisation on <= VS2013
#  define CATCH_CONFIG_STL_MUTEX(v) inline std::mutex &__global_lock_##v() \
  { \
    static std::mutex lock; \
    return lock; \
  } \
  inline std::mutex &global_lock_##v() \
  { \
    static std::atomic<int> c; \
    while(2!=c.load(std::memory_order_acquire)) \
    { \
      int expected=0; \
      if(c.compare_exchange_weak(expected, 1, std::memory_order_acquire, std::memory_order_consume)) \
      { \
        __global_lock_##v(); \
        c.store(2, std::memory_order_release); \
        break; \
      } \
    } \
    return __global_lock_##v(); \
  }
#  define CATCH_CONFIG_STL_MUTEX_HOLD(v) std::lock_guard<std::mutex> ___g(global_lock_##v());
#endif
// We define a total of two global locks, one to serialise checks, the other to serialise stdout/stderr redirection
// The other lock is defined in catch_runner_impl.hpp
CATCH_CONFIG_STL_MUTEX(internal_catch)

#include <sstream>
#include <stdexcept>
#include <algorithm>

#include "catch_compiler_capabilities.h"

namespace Catch {

    class NonCopyable {
#ifdef CATCH_CPP11_OR_GREATER
        NonCopyable( NonCopyable const& )              = delete;
        NonCopyable( NonCopyable && )                  = delete;
        NonCopyable& operator = ( NonCopyable const& ) = delete;
        NonCopyable& operator = ( NonCopyable && )     = delete;
#else
        NonCopyable( NonCopyable const& info );
        NonCopyable& operator = ( NonCopyable const& );
#endif

    protected:
        NonCopyable() {}
        virtual ~NonCopyable();
    };

    class SafeBool {
    public:
        typedef void (SafeBool::*type)() const;

        static type makeSafe( bool value ) {
            return value ? &SafeBool::trueValue : 0;
        }
    private:
        void trueValue() const {}
    };

    template<typename ContainerT>
    inline void deleteAll( ContainerT& container ) {
        typename ContainerT::const_iterator it = container.begin();
        typename ContainerT::const_iterator itEnd = container.end();
        for(; it != itEnd; ++it )
            delete *it;
    }
    template<typename AssociativeContainerT>
    inline void deleteAllValues( AssociativeContainerT& container ) {
        typename AssociativeContainerT::const_iterator it = container.begin();
        typename AssociativeContainerT::const_iterator itEnd = container.end();
        for(; it != itEnd; ++it )
            delete it->second;
    }

    bool startsWith( std::string const& s, std::string const& prefix );
    bool endsWith( std::string const& s, std::string const& suffix );
    bool contains( std::string const& s, std::string const& infix );
    void toLowerInPlace( std::string& s );
    std::string toLower( std::string const& s );
    std::string trim( std::string const& str );
    bool replaceInPlace( std::string& str, std::string const& replaceThis, std::string const& withThis );

    struct pluralise {
        pluralise( std::size_t count, std::string const& label );

        friend std::ostream& operator << ( std::ostream& os, pluralise const& pluraliser );

        std::size_t m_count;
        std::string m_label;
    };

    struct SourceLineInfo {

        SourceLineInfo();
        SourceLineInfo( char const* _file, std::size_t _line );
        SourceLineInfo( SourceLineInfo const& other );
#  ifdef CATCH_CPP11_OR_GREATER
        SourceLineInfo( SourceLineInfo && )                  = default;
        SourceLineInfo& operator = ( SourceLineInfo const& ) = default;
        SourceLineInfo& operator = ( SourceLineInfo && )     = default;
#  endif
        bool empty() const;
        bool operator == ( SourceLineInfo const& other ) const;

        std::string file;
        std::size_t line;
    };

    std::ostream& operator << ( std::ostream& os, SourceLineInfo const& info );

    // This is just here to avoid compiler warnings with macro constants and boolean literals
    inline bool isTrue( bool value ){ return value; }
    inline bool alwaysTrue() { return true; }
    inline bool alwaysFalse() { return false; }

    void throwLogicError( std::string const& message, SourceLineInfo const& locationInfo );

    // Use this in variadic streaming macros to allow
    //    >> +StreamEndStop
    // as well as
    //    >> stuff +StreamEndStop
    struct StreamEndStop {
        std::string operator+() {
            return std::string();
        }
    };
    template<typename T>
    T const& operator + ( T const& value, StreamEndStop ) {
        return value;
    }
}

#define CATCH_INTERNAL_LINEINFO ::Catch::SourceLineInfo( __FILE__, static_cast<std::size_t>( __LINE__ ) )
#define CATCH_INTERNAL_ERROR( msg ) ::Catch::throwLogicError( msg, CATCH_INTERNAL_LINEINFO );

#endif // TWOBLUECUBES_CATCH_COMMON_H_INCLUDED

