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
#ifndef HEROESPATH_SFMLUTIL_ICALLBACKHANDLER_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_ICALLBACKHANDLER_HPP_INCLUDED
//
// i-callback-handler.hpp
//  Pure virtual interface for all callback handler objects
//
#include "misc/assertlogandthrow.hpp"

#include <string>

namespace heroespath
{
namespace sfml_util
{
    namespace callback
    {

        // For some reason Package_t would not work as a pointer on IcallbackHandler below.
        //(grumble) zTn 2016-10-20
        template <typename T>
        struct PtrWrapper
        {
            PtrWrapper(const T * const PACKAGE_PTR)
                : PTR_(PACKAGE_PTR)
            {
                M_ASSERT_OR_LOGANDTHROW_SS(
                    (PTR_ != nullptr),
                    "sfml_util::callback::PtrWrapper(PACKAGE_PTR) was given a null PACKAGE_PTR.");
            }

            PtrWrapper(const PtrWrapper & PW)
                : // The lifetime of this object is not managed by this class.
                  // Usage is short-term observation only, so ptr copying is safe.
                PTR_(PW.PTR_)
            {
                M_ASSERT_OR_LOGANDTHROW_SS(
                    (PTR_ != nullptr),
                    "sfml_util::callback::PtrWrapper(PtrWrapper) copy-constructor "
                        << "was given a null PACKAGE_PTR.");
            }

            PtrWrapper & operator=(const PtrWrapper & PW)
            {
                if (&PW != this)
                {
                    // See copy constructor comment regarding copying this pointer.
                    PTR_ = PW.PTR_;
                }

                return *this;
            }

            virtual ~PtrWrapper() {}

            const T * const PTR_;
        };

        // the standard callback handler interface
        template <typename Package_t, typename Return_t = void>
        struct ICallbackHandler
        {
            virtual ~ICallbackHandler() {}
            virtual const std::string HandlerName() const = 0;
            virtual Return_t HandleCallback(const Package_t &) = 0;
        };
    } // namespace callback
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_ICALLBACKHANDLER_HPP_INCLUDED
