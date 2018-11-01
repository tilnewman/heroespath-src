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
#include "misc/config-file.hpp"
#include "misc/log-macros.hpp"
#include "misc/real.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/gui-images.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/text-info.hpp"
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
            },
            true)
        , titleCachedTexture_("media-images-title-blacksymbol")
        , titleSprite_(titleCachedTexture_.Get())
        , resumeButtonUPtr_(std::make_unique<gui::MainMenuButton>(
              stage::Stage::Load,
              gui::ImageTextEntity::Callback_t::IHandlerPtr_t(this),
              sfutil::ScreenRatioToPixelsHoriz(0.305f)))
        , createButtonUPtr_(std::make_unique<gui::MainMenuButton>(
              stage::Stage::Character,
              gui::ImageTextEntity::Callback_t::IHandlerPtr_t(this),
              sfutil::ScreenRatioToPixelsHoriz(0.43f)))
        , settingsButtonUPtr_(std::make_unique<gui::MainMenuButton>(
              stage::Stage::Settings,
              gui::ImageTextEntity::Callback_t::IHandlerPtr_t(this),
              sfutil::ScreenRatioToPixelsHoriz(0.201f)))
        , creditsButtonUPtr_(std::make_unique<gui::MainMenuButton>(
              stage::Stage::Credits,
              gui::ImageTextEntity::Callback_t::IHandlerPtr_t(this),
              sfutil::ScreenRatioToPixelsHoriz(0.177f)))
        , exitButtonUPtr_(std::make_unique<gui::MainMenuButton>(
              stage::Stage::Exit,
              gui::ImageTextEntity::Callback_t::IHandlerPtr_t(this),
              sfutil::ScreenRatioToPixelsHoriz(0.114f)))
        , ouroborosUPtr_(std::make_unique<gui::Ouroboros>("MainMenu's"))
        , bottomSymbol_()
        , background_()
    {}

    MainMenuStage::~MainMenuStage() { StageBase::ClearAllEntities(); }

    void MainMenuStage::Setup()
    {
        // title image
        const auto TITLE_SCALE { sfutil::MapByRes(0.5f, 3.0f) };
        titleSprite_.setScale(TITLE_SCALE, TITLE_SCALE);

        titleSprite_.setPosition(
            sfutil::DisplayCenterHoriz(titleSprite_.getGlobalBounds().width),
            sfutil::MapByRes(20.0f, 188.0f));

        // Ouroboros
        EntityAdd(ouroborosUPtr_.get());

        const auto BETWEEN_BUTTONS_SPACER { sfutil::ScreenRatioToPixelsVert(0.01f) };

        const auto BUTTONS_HEIGHT { (BETWEEN_BUTTONS_SPACER * 4.0f)
                                    + resumeButtonUPtr_->GetEntityRegion().height
                                    + createButtonUPtr_->GetEntityRegion().height
                                    + settingsButtonUPtr_->GetEntityRegion().height
                                    + creditsButtonUPtr_->GetEntityRegion().height
                                    + exitButtonUPtr_->GetEntityRegion().height };

        const auto BUTTONS_POS_VERT_CENTER {
            sfutil::Bottom(titleSprite_.getGlobalBounds())
            + ((bottomSymbol_.Region().top - sfutil::Bottom(titleSprite_.getGlobalBounds())) * 0.5f)
        };

        const auto FIRST_BUTTON_POS_TOP { BUTTONS_POS_VERT_CENTER - (BUTTONS_HEIGHT * 0.5f) };

        resumeButtonUPtr_->SetEntityPos(
            sfutil::DisplayCenterHoriz(resumeButtonUPtr_->GetEntityRegion().width),
            FIRST_BUTTON_POS_TOP);

        createButtonUPtr_->SetEntityPos(
            sfutil::DisplayCenterHoriz(createButtonUPtr_->GetEntityRegion().width),
            sfutil::Bottom(resumeButtonUPtr_->GetEntityRegion()) + BETWEEN_BUTTONS_SPACER);

        settingsButtonUPtr_->SetEntityPos(
            sfutil::DisplayCenterHoriz(settingsButtonUPtr_->GetEntityRegion().width),
            sfutil::Bottom(createButtonUPtr_->GetEntityRegion()) + BETWEEN_BUTTONS_SPACER);

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
                delete NEXT_GAMESTATE_PTR.Ptr();
            }

            return (GAMESTATE_PVEC.empty() == false);
        }() };

        if (ARE_THERE_GAMES_TO_LOAD == false)
        {
            resumeButtonUPtr_->SetMouseState(gui::MouseState::Disabled);
        }

        EntityAdd(resumeButtonUPtr_.get());
        EntityAdd(createButtonUPtr_.get());
        EntityAdd(settingsButtonUPtr_.get());
        EntityAdd(creditsButtonUPtr_.get());
        EntityAdd(exitButtonUPtr_.get());
    }

    void MainMenuStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        target.draw(background_, STATES);
        target.draw(titleSprite_, STATES);
        target.draw(bottomSymbol_, STATES);
        StageBase::Draw(target, STATES);
    }

    bool MainMenuStage::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if (StageBase::KeyRelease(KEY_EVENT))
        {
            return true;
        }
        else if (KEY_EVENT.code == sf::Keyboard::M)
        {
            createButtonUPtr_->SetMouseState(gui::MouseState::Over);
            gui::SoundManager::Instance()->PlaySfx_Keypress();
            TransitionTo(stage::Stage::Menu);
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
