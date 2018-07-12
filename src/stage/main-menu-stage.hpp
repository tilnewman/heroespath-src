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
//  A Stage class that displays the rolling credits of the app.
//
#include "popup/i-popup-callback.hpp"
#include "sfml-util/gradient.hpp"
#include "sfml-util/gui/background-image.hpp"
#include "sfml-util/gui/box.hpp"
#include "sfml-util/gui/main-menu-buttons.hpp"
#include "sfml-util/horiz-symbol.hpp"
#include "sfml-util/ouroboros.hpp"
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-system.hpp"
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
        , public sfml_util::gui::callback::IFourStateButtonCallbackHandler_t
    {
    public:
        MainMenuStage(const MainMenuStage &) = delete;
        MainMenuStage(MainMenuStage &&) = delete;
        MainMenuStage & operator=(const MainMenuStage &) = delete;
        MainMenuStage & operator=(MainMenuStage &&) = delete;

    public:
        MainMenuStage();
        virtual ~MainMenuStage();

        const std::string HandlerName() const override { return GetStageName(); }

        bool HandleCallback(
            const sfml_util::gui::callback::FourStateButtonCallbackPackage_t &) override;

        void Setup() override;
        void Draw(sf::RenderTarget & target, const sf::RenderStates & STATES) override;
        bool KeyRelease(const sf::Event::KeyEvent &) override;

    private:
        const float BUTTON_SCALE_;
        //
        sfml_util::CachedTexture titleCachedTexture_;
        sf::Sprite titleSprite_;
        sfml_util::GradientRect gradient_;
        sfml_util::gui::MainMenuButtonUPtr_t resumeButtonUPtr_;
        sfml_util::gui::MainMenuButtonUPtr_t createButtonUPtr_;
        sfml_util::gui::MainMenuButtonUPtr_t settingsButtonUPtr_;
        sfml_util::gui::MainMenuButtonUPtr_t creditsButtonUPtr_;
        sfml_util::gui::MainMenuButtonUPtr_t exitButtonUPtr_;
        sfml_util::OuroborosUPtr_t ouroborosUPtr_;
        sfml_util::BottomSymbol bottomSymbol_;
        sfml_util::gui::BackgroundImage backgroundImage_;
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_MAINMENUSTAGE_HPP_INCLUDED
