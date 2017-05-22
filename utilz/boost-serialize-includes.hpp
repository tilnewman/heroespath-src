#ifndef UTILZ_BOOSTSERIALIZEINCLUDES_HPP_INCLUDED
#define UTILZ_BOOSTSERIALIZEINCLUDES_HPP_INCLUDED
//
// boost-serialize-includes.hpp
//

//suppress warnings that are safe to ignore in boost
#include "utilz/platform.hpp"
#ifdef PLATFORMDETECT__APPLE_OS
#pragma GCC diagnostic ignored "-Wundef"
#endif

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/base_object.hpp>

#ifdef PLATFORMDETECT__APPLE_OS
#pragma GCC diagnostic warning "-Wundef"
#endif

#endif //UTILZ_BOOSTSERIALIZEINCLUDES_HPP_INCLUDED
