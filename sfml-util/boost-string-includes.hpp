#ifndef SFMLUTIL_BOOSTSTRINGINCLUDES_HPP_INCLUDED
#define SFMLUTIL_BOOSTSTRINGINCLUDES_HPP_INCLUDED
//
// boost-string-includes.hpp
//

//suppress warnings that are safe to ignore in boost
#include "sfml-util/platform.hpp"
#ifdef SFMLUTIL_PLATFORMDETECT__APPLE_OS
#pragma GCC diagnostic ignored "-Wundef"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#endif

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/algorithm.hpp>

#ifdef SFMLUTIL_PLATFORMDETECT__APPLE_OS
#pragma GCC diagnostic warning "-Wswitch-enum"
#pragma GCC diagnostic warning "-Wundef"
#endif

#endif //SFMLUTIL_BOOSTSTRINGINCLUDES_HPP_INCLUDED
