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
#include "interact/interaction-button-enum.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {
        class TextButton;
        using TextButtonPtr_t = misc::NotNull<TextButton *>;
        using TextButtonPtrOpt_t = boost::optional<TextButtonPtr_t>;
    } // namespace gui
} // namespace sfml_util
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

        bool DoPointersMatch(const sfml_util::gui::TextButtonPtr_t P) const
        {
            return (P == ptrOpt_);
        }

        sfml_util::gui::TextButtonUPtr_t
            Make(const sfml_util::gui::callback::ITextButtonCallbackHandlerPtr_t);

    private:
        Buttons::Enum whichButton_;
        sfml_util::gui::TextButtonPtrOpt_t ptrOpt_;
    };

    using ButtonVec_t = std::vector<Button>;

} // namespace interact
} // namespace heroespath

#endif // HEROESPATH_MAP_INTERACT_HPP_INCLUDED
