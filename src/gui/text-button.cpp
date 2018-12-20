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
namespace gui
{

    TextButton::TextButton(
        const std::string & NAME, const Callback_t::IHandlerPtrOpt_t CALLBACK_HANDLER_PTR_OPT)
        : TextEntity(std::string(NAME).append("_TextButton"))
        , callbackHandlerPtrOpt_(CALLBACK_HANDLER_PTR_OPT)
    {}

    TextButton::TextButton(
        const std::string & NAME,
        const sf::Vector2f & POS_V,
        const MouseTextInfo & MOUSE_TEXT_INFO,
        const Callback_t::IHandlerPtrOpt_t CALLBACK_HANDLER_PTR_OPT)
        : TextEntity(std::string(NAME).append("_TextButton"), POS_V.x, POS_V.y, MOUSE_TEXT_INFO)
        , callbackHandlerPtrOpt_(CALLBACK_HANDLER_PTR_OPT)
    {
        // no need to call setup
    }

    TextButton::~TextButton() = default;

    void TextButton::Setup(
        const sf::Vector2f & POS_V,
        const MouseTextInfo & MOUSE_TEXT_INFO,
        const Callback_t::IHandlerPtrOpt_t CALLBACK_HANDLER_PTR_OPT)
    {
        TextEntity::Setup(POS_V.x, POS_V.y, MOUSE_TEXT_INFO);
        callbackHandlerPtrOpt_ = CALLBACK_HANDLER_PTR_OPT;
    }

    void TextButton::OnClick(const sf::Vector2f &)
    {
        if (callbackHandlerPtrOpt_)
        {
            std::ostringstream ss;
            ss << "TextButton(" << GetEntityName() << "\", on-click)";

            Callback_t::HandleAndLog(*callbackHandlerPtrOpt_.value(), *this, ss.str());
        }
    }

} // namespace gui
} // namespace heroespath
