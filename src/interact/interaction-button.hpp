// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_INTERACT_BUTTON_HPP_INCLUDED
#define HEROESPATH_INTERACT_BUTTON_HPP_INCLUDED
//
// interact-button-enum.hpp
//
#include "gui/text-button.hpp"
#include "interact/interaction-button-enum.hpp"
#include "misc/not-null.hpp"

#include <SFML/Window/Keyboard.hpp>

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace interact
{

    // Responsible for wrapping all the information needed to implement a
    // button used by interactions.
    class Button
    {
    public:
        explicit Button(const Buttons::Enum WHICH_BUTTON = Buttons::Count);

        Buttons::Enum Which() const { return whichButton_; }

        const std::string Name() const { return Buttons::ToString(whichButton_); }

        sf::Keyboard::Key Key() const { return Buttons::Key(whichButton_); }

        bool IsSameButton(const gui::TextButton * const OTHER_BUTTON_PTR) const
        {
            if (ptrOpt_)
            {
                return (OTHER_BUTTON_PTR == ptrOpt_.value());
            }
            else
            {
                return false;
            }
        }

        gui::TextButtonUPtr_t Make(const gui::TextButton::Callback_t::IHandlerPtrOpt_t &);

    private:
        Buttons::Enum whichButton_;
        gui::TextButtonPtrOpt_t ptrOpt_;
    };

    using ButtonVec_t = std::vector<Button>;

} // namespace interact
} // namespace heroespath

#endif // HEROESPATH_MAP_INTERACT_HPP_INCLUDED
