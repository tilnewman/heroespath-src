#ifndef APPBASE_COMMON_BOOSTSERIALIZEINCLUDES_HPP_INCLUDED
#define APPBASE_COMMON_BOOSTSERIALIZEINCLUDES_HPP_INCLUDED
//
// boost-serialize-includes.hpp
//

//suppress warnings that are safe to ignore in boost
#include "sfml-util/platform.hpp"
#ifdef SFMLUTIL_PLATFORMDETECT__APPLE_OS
#pragma GCC diagnostic ignored "-Wundef"
#endif

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/base_object.hpp>

#ifdef SFMLUTIL_PLATFORMDETECT__APPLE_OS
#pragma GCC diagnostic warning "-Wundef"
#endif

#endif //APPBASE_COMMON_BOOSTSERIALIZEINCLUDES_HPP_INCLUDED
