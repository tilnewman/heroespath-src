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
#ifndef APPBASE_BOOSTINC_HPP
#define APPBASE_BOOSTINC_HPP
//
// boostinc.hpp
//  This class helps collect boost includes where
//  certain warnings can more easily be prevented.
//

//prevent warnings that can be ignored
#if defined(WIN32) || defined(_WIN32) || defined(__WINDOWS__)
#pragma warning(disable : 4244)
#pragma warning(disable : 4512)
#elif defined(__APPLE__) || defined(__MACH__)
#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Wnested-anon-types"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wundef"
#endif


#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/random.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

#if defined(WIN32) || defined(_WIN32) || defined(__WINDOWS__)
#pragma warning(default : 4244)
#pragma warning(default : 4512)
#elif defined(__APPLE__) || defined(__MACH__)
#pragma GCC diagnostic warning "-Wswitch-enum"
#pragma GCC diagnostic warning "-Wnested-anon-types"
#pragma GCC diagnostic warning "-Wunused-parameter"
#pragma GCC diagnostic warning "-Wshadow"
#pragma GCC diagnostic warning "-Wundef"
#endif


#endif //ZUI_BOOST_INC_HPP
