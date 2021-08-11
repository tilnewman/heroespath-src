// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// main-menu-stage.cpp
//
#include "main-menu-stage.hpp"

#include "game/game-state-factory.hpp"
#include "game/game-state.hpp"
#include "gui/display.hpp"
#include "gui/font-manager.hpp"
#include "gui/gui-images.hpp"
#include "gui/sound-manager.hpp"
#include "gui/text-info.hpp"
#include "misc/config-file.hpp"
#include "misc/log-macros.hpp"
#include "misc/real.hpp"
#include "sfutil/center.hpp"
#include "sfutil/display.hpp"
#include "sfutil/position.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace heroespath
{
namespace stage
{

    MainMenuStage::MainMenuStage()
        : StageBase(
            "MainMenu",
            {
                gui::GuiFont::Default,
                gui::GuiFont::System,
                gui::GuiFont::Number,
            })
        , BUTTON_HEIGHT_SCREEN_RATIO(gui::MainMenuButton::DEFAULT_HEIGHT_SCREEN_SIZE_RATIO_ * 1.2f)
        , stageTitle_(gui::MenuImage::Title, true, 0.2275f, 0.68f, -0.0082f)
        , resumeButtonUPtr_(std::make_unique<gui::MainMenuButton>(
              gui::MenuImage::Resume,
              stage::Stage::Load,
              gui::ImageTextEntity::Callback_t::IHandlerPtr_t(this),
              sf::Vector2f(),
              true,
              true,
              BUTTON_HEIGHT_SCREEN_RATIO))
        , newGameButtonUPtr_(std::make_unique<gui::MainMenuButton>(
              gui::MenuImage::New,
              stage::Stage::Character,
              gui::ImageTextEntity::Callback_t::IHandlerPtr_t(this),
              sf::Vector2f(),
              true,
              false,
              BUTTON_HEIGHT_SCREEN_RATIO))
        , settingsButtonUPtr_(std::make_unique<gui::MainMenuButton>(
              gui::MenuImage::Settings,
              stage::Stage::Settings,
              gui::ImageTextEntity::Callback_t::IHandlerPtr_t(this),
              sf::Vector2f(),
              true,
              false,
              BUTTON_HEIGHT_SCREEN_RATIO))
        , creditsButtonUPtr_(std::make_unique<gui::MainMenuButton>(
              gui::MenuImage::Credits,
              stage::Stage::Credits,
              gui::ImageTextEntity::Callback_t::IHandlerPtr_t(this),
              sf::Vector2f(),
              true,
              false,
              BUTTON_HEIGHT_SCREEN_RATIO))
        , exitButtonUPtr_(std::make_unique<gui::MainMenuButton>(
              gui::MenuImage::Exit,
              stage::Stage::Exit,
              gui::ImageTextEntity::Callback_t::IHandlerPtr_t(this),
              sf::Vector2f(),
              true,
              false,
              BUTTON_HEIGHT_SCREEN_RATIO))
        , ouroborosUPtr_(std::make_unique<gui::Ouroboros>("MainMenu's"))
        , bottomSymbol_()
        , background_()
    {}

    MainMenuStage::~MainMenuStage() { StageBase::ClearAllEntities(); }

    void MainMenuStage::Setup()
    {
        // position menu buttons
        const auto BETWEEN_BUTTONS_SPACER { sfutil::ScreenRatioToPixelsVert(0.0f) };

        const auto BUTTONS_HEIGHT { (BETWEEN_BUTTONS_SPACER * 4.0f)
                                    + resumeButtonUPtr_->GetEntityRegion().height
                                    + newGameButtonUPtr_->GetEntityRegion().height
                                    + settingsButtonUPtr_->GetEntityRegion().height
                                    + creditsButtonUPtr_->GetEntityRegion().height
                                    + exitButtonUPtr_->GetEntityRegion().height };

        const auto TITLE_POS_BOTTOM { sfutil::Bottom(stageTitle_.Region()) };

        const auto BUTTONS_POS_VERT_CENTER {
            TITLE_POS_BOTTOM + ((bottomSymbol_.Region().top - TITLE_POS_BOTTOM) * 0.5f)
        };

        const auto FIRST_BUTTON_POS_TOP { BUTTONS_POS_VERT_CENTER - (BUTTONS_HEIGHT * 0.5f) };

        newGameButtonUPtr_->SetEntityPos(
            sfutil::DisplayCenterHoriz(newGameButtonUPtr_->GetEntityRegion().width),
            FIRST_BUTTON_POS_TOP);

        resumeButtonUPtr_->SetEntityPos(
            sfutil::DisplayCenterHoriz(resumeButtonUPtr_->GetEntityRegion().width),
            sfutil::Bottom(newGameButtonUPtr_->GetEntityRegion()) + BETWEEN_BUTTONS_SPACER);

        settingsButtonUPtr_->SetEntityPos(
            sfutil::DisplayCenterHoriz(settingsButtonUPtr_->GetEntityRegion().width),
            sfutil::Bottom(resumeButtonUPtr_->GetEntityRegion()) + BETWEEN_BUTTONS_SPACER);

        creditsButtonUPtr_->SetEntityPos(
            sfutil::DisplayCenterHoriz(creditsButtonUPtr_->GetEntityRegion().width),
            sfutil::Bottom(settingsButtonUPtr_->GetEntityRegion()) + BETWEEN_BUTTONS_SPACER);

        exitButtonUPtr_->SetEntityPos(
            sfutil::DisplayCenterHoriz(exitButtonUPtr_->GetEntityRegion().width),
            sfutil::Bottom(creditsButtonUPtr_->GetEntityRegion()) + BETWEEN_BUTTONS_SPACER);

        const auto ARE_THERE_GAMES_TO_LOAD { []() {
            // TODO this is wasteful in the extreme, need to add GameStateFactory::FindGameToLoad()
            // that does not create all games in order to find just one
            const auto GAMESTATE_PVEC { game::GameStateFactory::Instance()->LoadAllGames() };
            for (const auto & NEXT_GAMESTATE_PTR : GAMESTATE_PVEC)
            {
                delete NEXT_GAMESTATE_PTR;
            }

            return (GAMESTATE_PVEC.empty() == false);
        }() };

        if (ARE_THERE_GAMES_TO_LOAD == false)
        {
            resumeButtonUPtr_->SetMouseState(gui::MouseState::Disabled);
        }

        EntityAdd(resumeButtonUPtr_);
        EntityAdd(newGameButtonUPtr_);
        EntityAdd(settingsButtonUPtr_);
        EntityAdd(creditsButtonUPtr_);
        EntityAdd(exitButtonUPtr_);

        // position ouroboros
        const auto BUTTON_VERT_CENTER_DIFF { (BUTTONS_POS_VERT_CENTER - sfutil::Center().y) };
        ouroborosUPtr_->MoveEntityPos(0.0f, BUTTON_VERT_CENTER_DIFF);
        EntityAdd(ouroborosUPtr_);
    }

    void MainMenuStage::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(background_, states);
        target.draw(stageTitle_, states);
        target.draw(bottomSymbol_, states);
        StageBase::draw(target, states);
    }

    bool MainMenuStage::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if (StageBase::KeyRelease(KEY_EVENT))
        {
            return true;
        }
        else if (KEY_EVENT.code == sf::Keyboard::N)
        {
            newGameButtonUPtr_->SetMouseState(gui::MouseState::Over);
            gui::SoundManager::Instance()->PlaySfx_Keypress();
            TransitionTo(stage::Stage::Character);
            return true;
        }
        else if (KEY_EVENT.code == sf::Keyboard::S)
        {
            settingsButtonUPtr_->SetMouseState(gui::MouseState::Over);
            gui::SoundManager::Instance()->PlaySfx_Keypress();
            TransitionTo(stage::Stage::Settings);
            return true;
        }
        else if (KEY_EVENT.code == sf::Keyboard::C)
        {
            creditsButtonUPtr_->SetMouseState(gui::MouseState::Over);
            gui::SoundManager::Instance()->PlaySfx_Keypress();
            TransitionTo(stage::Stage::Credits);
            return true;
        }
        else if ((KEY_EVENT.code == sf::Keyboard::Escape) || (KEY_EVENT.code == sf::Keyboard::E))
        {
            if (resumeButtonUPtr_->GetMouseState() == gui::MouseState::Over)
            {
                resumeButtonUPtr_->SetMouseState(gui::MouseState::Up);
            }

            newGameButtonUPtr_->SetMouseState(gui::MouseState::Up);
            settingsButtonUPtr_->SetMouseState(gui::MouseState::Up);
            creditsButtonUPtr_->SetMouseState(gui::MouseState::Up);

            exitButtonUPtr_->SetMouseState(gui::MouseState::Over);

            gui::SoundManager::Instance()->PlaySfx_Keypress();
            TransitionTo(stage::Stage::Exit);
            return true;
        }
        else if (KEY_EVENT.code == sf::Keyboard::R)
        {
            if (resumeButtonUPtr_->GetMouseState() != gui::MouseState::Disabled)
            {
                resumeButtonUPtr_->SetMouseState(gui::MouseState::Over);
                gui::SoundManager::Instance()->PlaySfx_Keypress();
                TransitionTo(stage::Stage::Load);
                return true;
            }
        }

        return false;
    }

} // namespace stage
} // namespace heroespath
