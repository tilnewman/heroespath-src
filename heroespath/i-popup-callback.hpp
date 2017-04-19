#ifndef APPBASE_SFMLUTIL_IPOOPUPCALLBACK_INCLUDED
#define APPBASE_SFMLUTIL_IPOOPUPCALLBACK_INCLUDED
//
// i-popup-callback.hpp
//  Interface for all objects that can handle popup button responses.
//
#include <string>
#include "sfml-util/i-callback-handler.hpp"
#include "heroespath/popup-response.hpp"


namespace heroespath
{
namespace callback
{
    //returns true if Loop's popup callback handler pointer should be reset to null
    using IPopupHandler_t = sfml_util::callback::ICallbackHandler<PopupResponse, bool>;
}
}
#endif //APPBASE_SFMLUTIL_IPOOPUPCALLBACK_INCLUDED
