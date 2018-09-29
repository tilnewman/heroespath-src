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
#include "sfml-util/gui/box-entity.hpp"
#include "sfml-util/gui/image-text-entity.hpp"
#include "sfml-util/gui/main-menu-background.hpp"
#include "sfml-util/gui/main-menu-buttons.hpp"
#include "sfml-util/horiz-symbol.hpp"
#include "sfml-util/ouroboros.hpp"
#include "sfml-util/stage.hpp"

#include <memory>
#include <string>

namespace heroespath
{
namespace stage
{

    // Responsible for displaying and managing mouse-clicks of five menu options:
    // Resume Game, Create Characters, Settings, Credits, Exit
    class MainMenuStage
        : public sfml_util::Stage
        , public sfml_util::ImageTextEntity::Callback_t::IHandler_t
    {
    public:
        MainMenuStage(const MainMenuStage &) = delete;
        MainMenuStage(MainMenuStage &&) = delete;
        MainMenuStage & operator=(const MainMenuStage &) = delete;
        MainMenuStage & operator=(MainMenuStage &&) = delete;

        MainMenuStage();
        virtual ~MainMenuStage();

        bool HandleCallback(const sfml_util::ImageTextEntity::Callback_t::PacketPtr_t &) override
        {
            return false;
        }

        void Setup() override;
        void Draw(sf::RenderTarget & target, const sf::RenderStates & STATES) override;
        bool KeyRelease(const sf::Event::KeyEvent &) override;

    private:
        sfml_util::CachedTexture titleCachedTexture_;
        sf::Sprite titleSprite_;
        sfml_util::MainMenuButtonUPtr_t resumeButtonUPtr_;
        sfml_util::MainMenuButtonUPtr_t createButtonUPtr_;
        sfml_util::MainMenuButtonUPtr_t settingsButtonUPtr_;
        sfml_util::MainMenuButtonUPtr_t creditsButtonUPtr_;
        sfml_util::MainMenuButtonUPtr_t exitButtonUPtr_;
        sfml_util::OuroborosUPtr_t ouroborosUPtr_;
        sfml_util::BottomSymbol bottomSymbol_;
        sfml_util::MainMenuBackground background_;
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_MAINMENUSTAGE_HPP_INCLUDED
