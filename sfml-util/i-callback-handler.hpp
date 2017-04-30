#ifndef APPBASE_SFMLUTIL_ICALLBACKHANDLER_INCLUDED
#define APPBASE_SFMLUTIL_ICALLBACKHANDLER_INCLUDED
//
// i-callback-handler.hpp
//  Pure virtual interface for all callback handler objects
//
#include "heroespath/assertlogandthrow.hpp"
#include <string>


namespace sfml_util
{
namespace callback
{

    //for some reason Package_t would not work as a pointer on IcallbackHandler below...grumble zTn 2016-10-20
    template<typename T>
    struct PtrWrapper
    {
        PtrWrapper(const T * const PACKAGE_PTR)
        :
            PTR_(PACKAGE_PTR)
        {
            M_ASSERT_OR_LOGANDTHROW_SS((PTR_ != nullptr), "sfml_util::callback::PtrWrapper(PACKAGE_PTR) was given a null PACKAGE_PTR.");
        }

        PtrWrapper(const PtrWrapper & PW)
        :
            PTR_(PW.PTR_)
        {
            M_ASSERT_OR_LOGANDTHROW_SS((PTR_ != nullptr), "sfml_util::callback::PtrWrapper(PtrWrapper) copy-constructor was given a null PACKAGE_PTR.");
        }

        PtrWrapper operator=(const PtrWrapper & PW)
        {
            if (& PW != this)
            {
                PTR_ = PW.PTR_;
            }

            return * this;
        }

        virtual ~PtrWrapper() {}

        const T * const PTR_;
    };


    //the standard callback handler interface
    template<typename Package_t, typename Return_t = void>
    struct ICallbackHandler
    {
        virtual ~ICallbackHandler() {}
        virtual const std::string HandlerName() const = 0;
        virtual Return_t HandleCallback(const Package_t &) = 0;
    };

}
}
#endif //APPBASE_SFMLUTIL_ICALLBACKHANDLER_INCLUDED
