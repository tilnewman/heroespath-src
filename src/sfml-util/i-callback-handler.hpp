// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
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

            virtual ~PtrWrapper() = default;

            const T * const PTR_;
        };

        // the standard callback handler interface
        template <typename Package_t, typename Return_t = void>
        struct ICallbackHandler
        {
            virtual ~ICallbackHandler() = default;
            virtual const std::string HandlerName() const = 0;
            virtual Return_t HandleCallback(const Package_t &) = 0;
            virtual bool WillAllowMousePosStateChange() const { return true; }
        };

    } // namespace callback
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_ICALLBACKHANDLER_HPP_INCLUDED
