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
#ifndef SFMLUTIL_IPOOPUPCALLBACK_INCLUDED
#define SFMLUTIL_IPOOPUPCALLBACK_INCLUDED
//
// i-popup-callback.hpp
//  Interface for all objects that can handle popup button responses.
//
#include "sfml-util/i-callback-handler.hpp"

#include "game/popup-response.hpp"

#include <string>


namespace game
{
namespace callback
{

    //Returns true if Loop's popup callback handler pointer should be reset to null.
    //This means you must return false if one popup will replace another.
    using IPopupHandler_t = sfml_util::callback::ICallbackHandler<PopupResponse, bool>;

}
}

#endif //SFMLUTIL_IPOOPUPCALLBACK_INCLUDED
