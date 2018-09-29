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

namespace heroespath
{
namespace sfml_util
{

    TextButton::TextButton(
        const std::string & NAME, const Callback_t::IHandlerPtrOpt_t CALLBACK_HANDLER_PTR_OPT)
        : TextEntity(std::string(NAME).append("_TextButton"))
        , callbackHandlerPtrOpt_(CALLBACK_HANDLER_PTR_OPT)
    {}

    TextButton::TextButton(
        const std::string & NAME,
        const float POS_LEFT,
        const float POS_TOP,
        const MouseTextInfo & MOUSE_TEXT_INFO,
        const Callback_t::IHandlerPtrOpt_t CALLBACK_HANDLER_PTR_OPT)
        : TextEntity(std::string(NAME).append("_TextButton"), POS_LEFT, POS_TOP, MOUSE_TEXT_INFO)
        , callbackHandlerPtrOpt_(CALLBACK_HANDLER_PTR_OPT)
    {
        Setup(POS_LEFT, POS_TOP, MOUSE_TEXT_INFO, CALLBACK_HANDLER_PTR_OPT);
    }

    TextButton::~TextButton() = default;

    void TextButton::Setup(
        const float POS_LEFT,
        const float POS_TOP,
        const MouseTextInfo & MOUSE_TEXT_INFO,
        const Callback_t::IHandlerPtrOpt_t CALLBACK_HANDLER_PTR_OPT)
    {
        TextEntity::Setup(POS_LEFT, POS_TOP, MOUSE_TEXT_INFO);
        callbackHandlerPtrOpt_ = CALLBACK_HANDLER_PTR_OPT;
    }

    void TextButton::OnClick(const sf::Vector2f &)
    {
        if (callbackHandlerPtrOpt_)
        {
            callbackHandlerPtrOpt_.value()->HandleCallback(this);
        }
    }

} // namespace sfml_util
} // namespace heroespath
