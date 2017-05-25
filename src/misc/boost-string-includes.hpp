///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef MISC_BOOSTSTRINGINCLUDES_HPP_INCLUDED
#define MISC_BOOSTSTRINGINCLUDES_HPP_INCLUDED
//
// boost-string-includes.hpp
//

//suppress warnings that are safe to ignore in boost
#include "misc/platform.hpp"
#ifdef PLATFORM_DETECTED_IS_APPLE
#pragma GCC diagnostic ignored "-Wundef"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#endif

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/algorithm.hpp>

#ifdef PLATFORM_DETECTED_IS_APPLE
#pragma GCC diagnostic warning "-Wswitch-enum"
#pragma GCC diagnostic warning "-Wundef"
#endif

#endif //MISC_BOOSTSTRINGINCLUDES_HPP_INCLUDED