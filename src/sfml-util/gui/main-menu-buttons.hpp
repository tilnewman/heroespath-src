// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_GUI_MAINMENUBUTTONS_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GUI_MAINMENUBUTTONS_HPP_INCLUDED
//
// main-menu-buttons.hpp
//
#include "sfml-util/gui/four-state-button.hpp"
#include "sfml-util/loop-state-enum.hpp"

#include <memory>
#include <vector>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        // Responsible for defaulting a FourStateButton to be a main menu button
        class MainMenuButton : public sfml_util::gui::FourStateButton
        {
        public:
            MainMenuButton(const MainMenuButton &) = delete;
            MainMenuButton(MainMenuButton &&) = delete;
            MainMenuButton & operator=(const MainMenuButton &) = delete;
            MainMenuButton & operator=(MainMenuButton &&) = delete;

            explicit MainMenuButton(
                const LoopState::Enum TRANSITION_TO = LoopState::Count,
                const callback::IFourStateButtonCallbackHandlerPtrOpt_t CALLBACK_HANDLER_PTR_OPT
                = boost::none,
                const float SCALE = 1.0f,
                const sf::Vector2f & POS_V = sf::Vector2f(0.0f, 0.0f));

            virtual ~MainMenuButton() = default;

        protected:
            void OnClick(const sf::Vector2f &) override;

            static const ButtonStateImageKeys
                MakeButtonStateImageKeys(const LoopState::Enum TRANSITION_TO);

        private:
            sfml_util::LoopState::Enum transitionTo_;
        };

        using MainMenuButtonUPtr_t = std::unique_ptr<MainMenuButton>;
        using MainMenuButtonUVec_t = std::vector<MainMenuButtonUPtr_t>;

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUI_MAINMENUBUTTONS_HPP_INCLUDED