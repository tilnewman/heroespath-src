// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_CALLBACK_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_CALLBACK_HPP_INCLUDED
//
// callback.hpp
//
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "popup/popup-response.hpp"

namespace heroespath
{
namespace sfml_util
{

    // Responsible for defining all the types that are handy to have when working with callbacks
    template <typename T>
    struct Callback
    {
        Callback() = delete;
        ~Callback() = delete;

        Callback(const Callback &) = delete;
        Callback(Callback &&) = delete;
        Callback & operator=(const Callback &) = delete;
        Callback & operator=(Callback &&) = delete;

        using Packet_t = T;
        using PacketPtr_t = misc::NotNull<Packet_t *>;

        // Responsible for declaring the interface of a callback handler. (typically a
        // Stage) A callback is required to pass a non-null pointer to whatever initiated
        // the callback.
        struct IHandler_t
        {
            // returns true if the callback was identified and handled (changed something)
            virtual bool HandleCallback(const PacketPtr_t & CALLBACK_PACKET_PTR) = 0;

        protected:
            ~IHandler_t() {}
        };

        using IHandlerPtr_t = misc::NotNull<IHandler_t *>;
        using IHandlerPtrOpt_t = boost::optional<IHandlerPtr_t>;
    };

    using PopupCallback_t = Callback<const popup::PopupResponse>;

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_ICALLBACK_HANDLER_HPP_INCLUDED
