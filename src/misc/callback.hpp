// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_CALLBACK_HPP_INCLUDED
#define HEROESPATH_MISC_CALLBACK_HPP_INCLUDED
//
// callback.hpp
//
#include "misc/boost-optional-that-throws.hpp"
#include "misc/log-macros.hpp"
#include "misc/not-null.hpp"

namespace heroespath
{
namespace popup
{
    struct PopupResponse;
}
namespace misc
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

        // Responsible for declaring the interface of a callback handler. (typically a
        // Stage) A callback is required to pass an information packet to the handler.
        struct IHandler_t
        {
            // returns a description (including PACKET_DESCRIPTION) if anything meaningful was done
            virtual const std::string
                HandleCallback(const Packet_t & PACKET, const std::string & PACKET_DESCRIPTION)
                = 0;

        protected:
            virtual ~IHandler_t() = default;
        };

        using IHandlerPtr_t = misc::NotNull<IHandler_t *>;
        using IHandlerPtrOpt_t = boost::optional<IHandlerPtr_t>;

        static void HandleAndLog(
            IHandler_t & handler, const Packet_t & PACKET, const std::string & PACKET_DESCRIPTION)
        {
            const std::string CALLBACK_HANDLER_MESSAGE { handler.HandleCallback(
                PACKET, PACKET_DESCRIPTION) };

            if (CALLBACK_HANDLER_MESSAGE.empty() == false)
            {
                M_HP_LOG(CALLBACK_HANDLER_MESSAGE);
            }
        }
    };

    using PopupCallback_t = Callback<popup::PopupResponse>;

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_CALLBACK_HPP_INCLUDED
