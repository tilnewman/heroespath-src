// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_POPUP_IPOOPUPCALLBACK_HPP_INCLUDED
#define HEROESPATH_POPUP_IPOOPUPCALLBACK_HPP_INCLUDED
//
// i-popup-callback.hpp
//  Interface for all objects that can handle popup button responses.
//
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "popup/popup-response.hpp"
#include "sfml-util/i-callback-handler.hpp"

namespace heroespath
{
namespace popup
{

    // Returns true if Loop's popup callback handler pointer should be reset to null.
    // This means you must return false if one popup will replace another.
    using IPopupHandler_t = sfml_util::callback::ICallbackHandler<PopupResponse, bool>;
    using IPopupHandlerPtr_t = misc::NotNull<IPopupHandler_t *>;
    using IPopupHandlerPtrOpt_t = boost::optional<IPopupHandlerPtr_t>;

} // namespace popup
} // namespace heroespath

#endif // HEROESPATH_POPUP_IPOOPUPCALLBACK_HPP_INCLUDED
