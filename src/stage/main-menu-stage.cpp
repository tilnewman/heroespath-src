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

#include "game/game-data-file.hpp"
#include "game/game-state-factory.hpp"
#include "game/game-state.hpp"
#include "game/loop-manager.hpp"
#include "log/log-macros.hpp"
#include "misc/real.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/gui/gui-images.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/sfml-util-center.hpp"
#include "sfml-util/sfml-util-display.hpp"
#include "sfml-util/sfml-util-position.hpp"
#include "sfml-util/sound-manager.hpp"

namespace heroespath
{
namespace stage
{

    MainMenuStage::MainMenuStage()
        : Stage(
              "MainMenu",
              {
                  sfml_util::GuiFont::Default,
                  sfml_util::GuiFont::System,
                  sfml_util::GuiFont::Number,
              },
              true)
        , titleCachedTexture_("media-images-title-blacksymbol")
        , titleSprite_(titleCachedTexture_.Get())
        , resumeButtonUPtr_(std::make_unique<sfml_util::MainMenuButton>(
              sfml_util::LoopState::Load,
              sfml_util::ImageTextEntity::Callback_t::IHandlerPtr_t(this),
              sfml_util::ScreenRatioToPixelsHoriz(0.305f)))
        , createButtonUPtr_(std::make_unique<sfml_util::MainMenuButton>(
              sfml_util::LoopState::Character,
              sfml_util::ImageTextEntity::Callback_t::IHandlerPtr_t(this),
              sfml_util::ScreenRatioToPixelsHoriz(0.43f)))
        , settingsButtonUPtr_(std::make_unique<sfml_util::MainMenuButton>(
              sfml_util::LoopState::Settings,
              sfml_util::ImageTextEntity::Callback_t::IHandlerPtr_t(this),
              sfml_util::ScreenRatioToPixelsHoriz(0.201f)))
        , creditsButtonUPtr_(std::make_unique<sfml_util::MainMenuButton>(
              sfml_util::LoopState::Credits,
              sfml_util::ImageTextEntity::Callback_t::IHandlerPtr_t(this),
              sfml_util::ScreenRatioToPixelsHoriz(0.177f)))
        , exitButtonUPtr_(std::make_unique<sfml_util::MainMenuButton>(
              sfml_util::LoopState::Exit,
              sfml_util::ImageTextEntity::Callback_t::IHandlerPtr_t(this),
              sfml_util::ScreenRatioToPixelsHoriz(0.114f)))
        , ouroborosUPtr_(std::make_unique<sfml_util::Ouroboros>("MainMenu's"))
        , bottomSymbol_()
        , background_()
    {}

    MainMenuStage::~MainMenuStage() { Stage::ClearAllEntities(); }

    void MainMenuStage::Setup()
    {
        // title image
        const auto TITLE_SCALE { sfml_util::MapByRes(0.5f, 3.0f) };
        titleSprite_.setScale(TITLE_SCALE, TITLE_SCALE);

        titleSprite_.setPosition(
            sfml_util::DisplayCenterHoriz(titleSprite_.getGlobalBounds().width),
            sfml_util::MapByRes(20.0f, 188.0f));

        // Ouroboros
        EntityAdd(ouroborosUPtr_.get());

        auto const BETWEEN_BUTTONS_SPACER { sfml_util::ScreenRatioToPixelsVert(0.01f) };

        const auto BUTTONS_HEIGHT { (BETWEEN_BUTTONS_SPACER * 4.0f)
                                    + resumeButtonUPtr_->GetEntityRegion().height
                                    + createButtonUPtr_->GetEntityRegion().height
                                    + settingsButtonUPtr_->GetEntityRegion().height
                                    + creditsButtonUPtr_->GetEntityRegion().height
                                    + exitButtonUPtr_->GetEntityRegion().height };

        const auto BUTTONS_POS_VERT_CENTER {
            sfml_util::Bottom(titleSprite_.getGlobalBounds())
            + ((bottomSymbol_.Region().top - sfml_util::Bottom(titleSprite_.getGlobalBounds()))
               * 0.5f)
        };

        const auto FIRST_BUTTON_POS_TOP { BUTTONS_POS_VERT_CENTER - (BUTTONS_HEIGHT * 0.5f) };

        resumeButtonUPtr_->SetEntityPos(
            sfml_util::DisplayCenterHoriz(resumeButtonUPtr_->GetEntityRegion().width),
            FIRST_BUTTON_POS_TOP);

        createButtonUPtr_->SetEntityPos(
            sfml_util::DisplayCenterHoriz(createButtonUPtr_->GetEntityRegion().width),
            sfml_util::Bottom(resumeButtonUPtr_->GetEntityRegion()) + BETWEEN_BUTTONS_SPACER);

        settingsButtonUPtr_->SetEntityPos(
            sfml_util::DisplayCenterHoriz(settingsButtonUPtr_->GetEntityRegion().width),
            sfml_util::Bottom(createButtonUPtr_->GetEntityRegion()) + BETWEEN_BUTTONS_SPACER);

        creditsButtonUPtr_->SetEntityPos(
            sfml_util::DisplayCenterHoriz(creditsButtonUPtr_->GetEntityRegion().width),
            sfml_util::Bottom(settingsButtonUPtr_->GetEntityRegion()) + BETWEEN_BUTTONS_SPACER);

        exitButtonUPtr_->SetEntityPos(
            sfml_util::DisplayCenterHoriz(exitButtonUPtr_->GetEntityRegion().width),
            sfml_util::Bottom(creditsButtonUPtr_->GetEntityRegion()) + BETWEEN_BUTTONS_SPACER);

        auto const ARE_THERE_GAMES_TO_LOAD { []() {
            // TODO this is wasteful in the extreme, need to add GameStateFactory::FindGameToLoad()
            // that does not create all games in order to find just one
            auto const GAMESTATE_PVEC { game::GameStateFactory::Instance()->LoadAllGames() };
            for (auto const & NEXT_GAMESTATE_PTR : GAMESTATE_PVEC)
            {
                delete NEXT_GAMESTATE_PTR.Ptr();
            }

            return (GAMESTATE_PVEC.empty() == false);
        }() };

        if (ARE_THERE_GAMES_TO_LOAD == false)
        {
            resumeButtonUPtr_->SetMouseState(sfml_util::MouseState::Disabled);
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
        Stage::Draw(target, STATES);
    }

    bool MainMenuStage::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if (Stage::KeyRelease(KEY_EVENT))
        {
            return true;
        }
        else if (KEY_EVENT.code == sf::Keyboard::M)
        {
            createButtonUPtr_->SetMouseState(sfml_util::MouseState::Over);
            sfml_util::SoundManager::Instance()->PlaySfx_Keypress();
            game::LoopManager::Instance()->TransitionTo_CharacterCreation();
            return true;
        }
        else if (KEY_EVENT.code == sf::Keyboard::S)
        {
            settingsButtonUPtr_->SetMouseState(sfml_util::MouseState::Over);
            sfml_util::SoundManager::Instance()->PlaySfx_Keypress();
            game::LoopManager::Instance()->TransitionTo_Settings();
            return true;
        }
        else if (KEY_EVENT.code == sf::Keyboard::C)
        {
            creditsButtonUPtr_->SetMouseState(sfml_util::MouseState::Over);
            sfml_util::SoundManager::Instance()->PlaySfx_Keypress();
            game::LoopManager::Instance()->TransitionTo_Credits();
            return true;
        }
        else if ((KEY_EVENT.code == sf::Keyboard::Escape) || (KEY_EVENT.code == sf::Keyboard::E))
        {
            exitButtonUPtr_->SetMouseState(sfml_util::MouseState::Over);
            sfml_util::SoundManager::Instance()->PlaySfx_Keypress();
            game::LoopManager::Instance()->TransitionTo_Exit();
            return true;
        }
        else if (KEY_EVENT.code == sf::Keyboard::R)
        {
            if (resumeButtonUPtr_->GetMouseState() != sfml_util::MouseState::Disabled)
            {
                resumeButtonUPtr_->SetMouseState(sfml_util::MouseState::Over);
                sfml_util::SoundManager::Instance()->PlaySfx_Keypress();
                game::LoopManager::Instance()->TransitionTo_LoadGameMenu();
                return true;
            }
        }

        return false;
    }
} // namespace stage
} // namespace heroespath
