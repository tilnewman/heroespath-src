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
// text-button.cpp
//
#include "text-button.hpp"

#include "sfml-util/gui/mouse-text-info.hpp"

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        TextButton::TextButton(
            const std::string & NAME, callback::ITextButtonCallbackHandler_t * callbackHandlerPtr)
            : GuiText(std::string(NAME).append("_TextButton"))
            , callbackHandlerPtr_(callbackHandlerPtr)
        {}

        TextButton::TextButton(
            const std::string & NAME,
            const float POS_LEFT,
            const float POS_TOP,
            const MouseTextInfo & MOUSE_TEXT_INFO,
            callback::ITextButtonCallbackHandler_t * callbackHandlerPtr)
            : GuiText(std::string(NAME).append("_TextButton"), POS_LEFT, POS_TOP, MOUSE_TEXT_INFO)
            , callbackHandlerPtr_(callbackHandlerPtr)
        {
            Setup(POS_LEFT, POS_TOP, MOUSE_TEXT_INFO, callbackHandlerPtr);
        }

        TextButton::~TextButton() = default;

        void TextButton::Setup(
            const float POS_LEFT,
            const float POS_TOP,
            const MouseTextInfo & MOUSE_TEXT_INFO,
            callback::ITextButtonCallbackHandler_t * callbackHandlerPtr)
        {
            GuiText::Setup(MOUSE_TEXT_INFO.up.text, POS_LEFT, POS_TOP, MOUSE_TEXT_INFO);
            callbackHandlerPtr_ = callbackHandlerPtr;
        }

        void TextButton::OnClick(const sf::Vector2f &)
        {
            if (nullptr != callbackHandlerPtr_)
            {
                callbackHandlerPtr_->HandleCallback(this);
            }
        }
    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
