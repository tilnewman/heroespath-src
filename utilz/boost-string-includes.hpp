#ifndef UTILZ_BOOSTSTRINGINCLUDES_HPP_INCLUDED
#define UTILZ_BOOSTSTRINGINCLUDES_HPP_INCLUDED
//
// boost-string-includes.hpp
//

//suppress warnings that are safe to ignore in boost
#include "utilz/platform.hpp"
#ifdef PLATFORMDETECT__APPLE_OS
#pragma GCC diagnostic ignored "-Wundef"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#endif

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/algorithm.hpp>

#ifdef PLATFORMDETECT__APPLE_OS
#pragma GCC diagnostic warning "-Wswitch-enum"
#pragma GCC diagnostic warning "-Wundef"
#endif

#endif //UTILZ_BOOSTSTRINGINCLUDES_HPP_INCLUDED
