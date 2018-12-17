// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_MAINMENUBUTTONS_HPP_INCLUDED
#define HEROESPATH_GUI_MAINMENUBUTTONS_HPP_INCLUDED
//
// main-menu-buttons.hpp
//
#include "gui/image-text-entity.hpp"
#include "gui/menu-image-enum.hpp"
#include "stage/stage-enum.hpp"

#include <memory>
#include <vector>

namespace heroespath
{
namespace gui
{

    // Responsible for implementing an ImageTextEntity that behaves like a one of the standard
    // menu stage buttons such as Back/Next/etc.
    class MainMenuButton : public gui::ImageTextEntity
    {
    public:
        explicit MainMenuButton(
            const MenuImage::Enum MENU_IMAGE = MenuImage::Count,
            const stage::Stage::Enum TRANSITION_TO = stage::Stage::Count,
            const ImageTextEntity::Callback_t::IHandlerPtrOpt_t & CALLBACK_HANDLER_PTR_OPT
            = boost::none,
            const sf::Vector2f & POS_V = sf::Vector2f(),
            const bool WILL_TRANSITION_STAGE = true,
            const bool WILL_INCLUDE_DISABLED = false,
            const float HEIGHT_SCREEN_RATIO = DEFAULT_HEIGHT_SCREEN_SIZE_RATIO_);

        virtual ~MainMenuButton() = default;

        MainMenuButton(const MainMenuButton &) = delete;
        MainMenuButton(MainMenuButton &&) = delete;
        MainMenuButton & operator=(const MainMenuButton &) = delete;
        MainMenuButton & operator=(MainMenuButton &&) = delete;

        void PretendClicked(const sf::Vector2f & FAKE_MOUSE_POS_V = sf::Vector2f(-1.0f, -1.0f))
        {
            OnClick(FAKE_MOUSE_POS_V);
        }

    protected:
        void OnClick(const sf::Vector2f &) override;

        static const MouseImageInfo MakeMouseImageInfo(
            const MenuImage::Enum MENU_IMAGE,
            const bool WILL_INCLUDE_DISABLED,
            const float HEIGHT_SCREEN_RATIO,
            const sf::Vector2f & POS_V);

    public:
        static const float DEFAULT_HEIGHT_SCREEN_SIZE_RATIO_;
        static const sf::Uint8 DISABLED_COLOR_ALPHA_;

    private:
        stage::Stage::Enum transitionTo_;
        bool willTransitionStage_;
    };

    using MainMenuButtonUPtr_t = std::unique_ptr<MainMenuButton>;
    using MainMenuButtonUVec_t = std::vector<MainMenuButtonUPtr_t>;

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_MAINMENUBUTTONS_HPP_INCLUDED
