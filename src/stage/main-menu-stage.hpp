// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MAINMENUSTAGE_HPP_INCLUDED
#define HEROESPATH_MAINMENUSTAGE_HPP_INCLUDED
//
// credits-stage.hpp
//
#include "gui/box-entity.hpp"
#include "gui/horiz-symbol.hpp"
#include "gui/image-text-entity.hpp"
#include "gui/main-menu-background.hpp"
#include "gui/main-menu-buttons.hpp"
#include "gui/ouroboros.hpp"
#include "stage/stage-base.hpp"

#include <memory>
#include <string>

namespace heroespath
{
namespace stage
{

    // Responsible for displaying and managing mouse-clicks of five menu options:
    // Resume Game, Create Characters, Settings, Credits, Exit
    class MainMenuStage
        : public stage::StageBase
        , public gui::ImageTextEntity::Callback_t::IHandler_t
    {
    public:
        MainMenuStage(const MainMenuStage &) = delete;
        MainMenuStage(MainMenuStage &&) = delete;
        MainMenuStage & operator=(const MainMenuStage &) = delete;
        MainMenuStage & operator=(MainMenuStage &&) = delete;

        MainMenuStage();
        virtual ~MainMenuStage();

        bool HandleCallback(const gui::ImageTextEntity::Callback_t::PacketPtr_t) override
        {
            return false;
        }

        void Setup() override;
        void draw(sf::RenderTarget &, sf::RenderStates) const override;
        bool KeyRelease(const sf::Event::KeyEvent &) override;

    private:
        gui::CachedTexture titleCachedTexture_;
        sf::Sprite titleSprite_;
        gui::MainMenuButtonUPtr_t resumeButtonUPtr_;
        gui::MainMenuButtonUPtr_t createButtonUPtr_;
        gui::MainMenuButtonUPtr_t settingsButtonUPtr_;
        gui::MainMenuButtonUPtr_t creditsButtonUPtr_;
        gui::MainMenuButtonUPtr_t exitButtonUPtr_;
        gui::OuroborosUPtr_t ouroborosUPtr_;
        gui::BottomSymbol bottomSymbol_;
        gui::MainMenuBackground background_;
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_MAINMENUSTAGE_HPP_INCLUDED
