// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// interaction-button.cpp
//
#include "interaction-button.hpp"
#include "sfml-util/sfml-util.hpp"

namespace heroespath
{
namespace interact
{

    Button::Button(const std::string & NAME, const sf::Keyboard::Key KEY)
        : name_(NAME)
        , key_(KEY)
        , ptr_(nullptr)
    {}

    sfml_util::gui::TextButtonUPtr_t
        Button::Make(sfml_util::gui::callback::ITextButtonCallbackHandler_t * callbackHandlerPtr)
    {
        auto const DISPLAYED_NAME{ [&]() {
            if ((sf::Keyboard::KeyCount == key_) || (sf::Keyboard::Unknown == key_))
            {
                return name_;
            }
            else
            {
                return name_ + "(" + sfml_util::KeyCodeToString(key_) + ")";
            }
        }() };

        auto uptr{ std::make_unique<sfml_util::gui::TextButton>(
            name_,
            0.0f,
            0.0f,
            sfml_util::gui::MouseTextInfo::Make_InteractionButtonSet(DISPLAYED_NAME),
            callbackHandlerPtr) };

        ptr_ = uptr.get();
        return uptr;
    }
} // namespace interact
} // namespace heroespath
