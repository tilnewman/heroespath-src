// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
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
            const std::string & NAME,
            const callback::ITextButtonCallbackHandlerPtr_t CALLBACK_HANLER_PTR)
            : GuiText(std::string(NAME).append("_TextButton"))
            , callbackHandlerPtr_(CALLBACK_HANLER_PTR)
        {}

        TextButton::TextButton(
            const std::string & NAME,
            const float POS_LEFT,
            const float POS_TOP,
            const MouseTextInfo & MOUSE_TEXT_INFO,
            const callback::ITextButtonCallbackHandlerPtr_t CALLBACK_HANLER_PTR)
            : GuiText(std::string(NAME).append("_TextButton"), POS_LEFT, POS_TOP, MOUSE_TEXT_INFO)
            , callbackHandlerPtr_(CALLBACK_HANLER_PTR)
        {
            Setup(POS_LEFT, POS_TOP, MOUSE_TEXT_INFO, CALLBACK_HANLER_PTR);
        }

        TextButton::~TextButton() = default;

        void TextButton::Setup(
            const float POS_LEFT,
            const float POS_TOP,
            const MouseTextInfo & MOUSE_TEXT_INFO,
            const callback::ITextButtonCallbackHandlerPtr_t CALLBACK_HANLER_PTR)
        {
            GuiText::Setup(MOUSE_TEXT_INFO.up.text, POS_LEFT, POS_TOP, MOUSE_TEXT_INFO);
            callbackHandlerPtr_ = CALLBACK_HANLER_PTR;
        }

        void TextButton::OnClick(const sf::Vector2f &)
        {
            callbackHandlerPtr_->HandleCallback(this);
        }

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
