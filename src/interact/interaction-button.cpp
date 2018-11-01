// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// interaction-button.cpp
//
#include "interaction-button.hpp"

#include "sfml-util/text-button.hpp"
#include "sfutil/keyboard.hpp"

namespace heroespath
{
namespace interact
{

    Button::Button(const Buttons::Enum WHICH_BUTTON)
        : whichButton_(WHICH_BUTTON)
        , ptrOpt_()
    {}

    gui::TextButtonUPtr_t
        Button::Make(const gui::TextButton::Callback_t::IHandlerPtrOpt_t & CALLBACK_HANDLER_PTR_OPT)
    {
        const auto KEY { Key() };

        const auto DISPLAYED_NAME { [&]() {
            if ((sf::Keyboard::KeyCount == KEY) || (sf::Keyboard::Unknown == KEY))
            {
                return Name();
            }
            else
            {
                return Name() + "(" + sfutil::ToString(KEY) + ")";
            }
        }() };

        auto uptr { std::make_unique<gui::TextButton>(
            Name(),
            0.0f,
            0.0f,
            gui::MouseTextInfo::Make_InteractionButtonSet(DISPLAYED_NAME),
            CALLBACK_HANDLER_PTR_OPT) };

        ptrOpt_ = uptr.get();
        return uptr;
    }

} // namespace interact
} // namespace heroespath
