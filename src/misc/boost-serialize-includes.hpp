// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_BOOSTSERIALIZEINCLUDES_HPP_INCLUDED
#define HEROESPATH_MISC_BOOSTSERIALIZEINCLUDES_HPP_INCLUDED
//
// boost-serialize-includes.hpp
//

// suppress warnings that are safe to ignore in boost
//#include "misc/platform.hpp"
//#ifdef HEROESPATH_PLATFORM_DETECTED_IS_APPLE
//#pragma GCC diagnostic ignored "-Wundef"
//#endif

#include <boost/serialization/serialization.hpp>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/unique_ptr.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/vector.hpp>

//#ifdef HEROESPATH_PLATFORM_DETECTED_IS_APPLE
//#pragma GCC diagnostic warning "-Wundef"
//#endif

#endif // HEROESPATH_MISC_BOOSTSERIALIZEINCLUDES_HPP_INCLUDED
