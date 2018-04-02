// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
//
// main-menu-stage.cpp
//
#include "main-menu-stage.hpp"

#include "game/game-data-file.hpp"
#include "game/loop-manager.hpp"
#include "log/log-macros.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/gui/gui-elements.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/sfml-util.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/tile.hpp"
#include "state/game-state-factory.hpp"
#include "state/game-state.hpp"

#include "misc/real.hpp"

namespace heroespath
{
namespace stage
{

    MainMenuStage::MainMenuStage()
        : Stage("MainMenu")
        , SCREEN_WIDTH_(sfml_util::Display::Instance()->GetWinWidth())
        , SCREEN_HEIGHT_(sfml_util::Display::Instance()->GetWinHeight())
        , SCREEN_RECT_(
              0.0f,
              0.0f,
              sfml_util::Display::Instance()->GetWinWidth(),
              sfml_util::Display::Instance()->GetWinHeight())
        , BUTTON_SCALE_(sfml_util::MapByRes(1.0f, 3.0f))
        , titleTexture_()
        , titleSprite_()
        , gradient_()
        , resumeButtonUPtr_(std::make_unique<sfml_util::main_menu_buttons::ResumeButton>(
              0.0f, 0.0f, BUTTON_SCALE_, false))
        , createButtonUPtr_(std::make_unique<sfml_util::main_menu_buttons::CreateCharactersButton>(
              0.0f, 0.0f, BUTTON_SCALE_, false))
        , settingsButtonUPtr_(std::make_unique<sfml_util::main_menu_buttons::SettingsButton>(
              0.0f, 0.0f, BUTTON_SCALE_, false))
        , creditsButtonUPtr_(std::make_unique<sfml_util::main_menu_buttons::CreditsButton>(
              0.0f, 0.0f, BUTTON_SCALE_, false))
        , exitButtonUPtr_(std::make_unique<sfml_util::main_menu_buttons::ExitButton>(
              0.0f, 0.0f, BUTTON_SCALE_, false))
        , ouroborosUPtr_()
        , bottomSymbol_()
        , backgroundImage_("media-images-backgrounds-tile-darkknot")
    {}

    MainMenuStage::~MainMenuStage() { Stage::ClearAllEntities(); }

    bool MainMenuStage::HandleCallback(
        const sfml_util::gui::callback::FourStateButtonCallbackPackage_t & PACKAGE)
    {

        if (PACKAGE.PTR_ == resumeButtonUPtr_.get())
        {
            game::LoopManager::Instance()->TransitionTo_LoadGameMenu();
            return true;
        }

        return false;
    }

    void MainMenuStage::Setup()
    {
        // title image
        sfml_util::LoadTexture(
            titleTexture_,
            game::GameDataFile::Instance()->GetMediaPath("media-images-title-blacksymbol"));

        titleSprite_.setTexture(titleTexture_);
        const float TITLE_IMAGE_SCALE(sfml_util::MapByRes(0.5f, 3.0f));
        titleSprite_.setScale(TITLE_IMAGE_SCALE, TITLE_IMAGE_SCALE);

        titleSprite_.setPosition(
            (SCREEN_WIDTH_ * 0.5f) - (titleSprite_.getGlobalBounds().width * 0.5f),
            sfml_util::MapByRes(20.0f, 188.0f));

        // gradient
        gradient_.Setup(
            SCREEN_RECT_,
            sfml_util::GradientInfo(
                sf::Color(0, 0, 0, 200),
                sfml_util::Corner::TopLeft | sfml_util::Corner::BottomRight));

        // Ouroboros
        ouroborosUPtr_ = std::make_unique<sfml_util::Ouroboros>("MainMenu's");
        EntityAdd(ouroborosUPtr_.get());

        // buttons
        resumeButtonUPtr_->SetScaleToRes();
        createButtonUPtr_->SetScaleToRes();
        settingsButtonUPtr_->SetScaleToRes();
        creditsButtonUPtr_->SetScaleToRes();
        exitButtonUPtr_->SetScaleToRes();

        auto const SPACE_BETWEEN_BUTTONS{ sfml_util::MapByRes(8.0f, 50.0f) };
        auto const TITLE_TO_BUTTONS_SPACER{ sfml_util::MapByRes(160.0f, 1000.0f) };

        resumeButtonUPtr_->SetEntityPos(
            (SCREEN_WIDTH_ * 0.5f) - (resumeButtonUPtr_->GetEntityRegion().width * 0.5f),
            titleSprite_.getGlobalBounds().top + titleSprite_.getGlobalBounds().height
                + TITLE_TO_BUTTONS_SPACER);

        createButtonUPtr_->SetEntityPos(
            (SCREEN_WIDTH_ * 0.5f) - (createButtonUPtr_->GetEntityRegion().width * 0.5f),
            resumeButtonUPtr_->GetEntityRegion().top + resumeButtonUPtr_->GetEntityRegion().height
                + SPACE_BETWEEN_BUTTONS);

        settingsButtonUPtr_->SetEntityPos(
            (SCREEN_WIDTH_ * 0.5f) - (settingsButtonUPtr_->GetEntityRegion().width * 0.5f),
            createButtonUPtr_->GetEntityRegion().top + createButtonUPtr_->GetEntityRegion().height
                + SPACE_BETWEEN_BUTTONS);

        creditsButtonUPtr_->SetEntityPos(
            (SCREEN_WIDTH_ * 0.5f) - (creditsButtonUPtr_->GetEntityRegion().width * 0.5f),
            settingsButtonUPtr_->GetEntityRegion().top
                + settingsButtonUPtr_->GetEntityRegion().height + SPACE_BETWEEN_BUTTONS);

        exitButtonUPtr_->SetEntityPos(
            (SCREEN_WIDTH_ * 0.5f) - (exitButtonUPtr_->GetEntityRegion().width * 0.5f),
            creditsButtonUPtr_->GetEntityRegion().top + creditsButtonUPtr_->GetEntityRegion().height
                + SPACE_BETWEEN_BUTTONS);

        resumeButtonUPtr_->SetCallbackHandler(this);

        auto const ARE_THERE_GAMES_TO_LOAD{ []() {
            // TODO this is wasteful in the extreme, need to add GameStateFactory::FindGameToLoad()
            // that does not create all games in order to find just one
            auto const GAMESTATE_PVEC{ state::GameStateFactory::Instance()->LoadAllGames() };
            for (auto const & NEXT_GAMESTATE_PTR : GAMESTATE_PVEC)
            {
                delete NEXT_GAMESTATE_PTR.Ptr();
            }

            return (GAMESTATE_PVEC.empty() == false);
        }() };

        resumeButtonUPtr_->SetIsDisabled(ARE_THERE_GAMES_TO_LOAD == false);

        EntityAdd(resumeButtonUPtr_.get());
        EntityAdd(createButtonUPtr_.get());
        EntityAdd(settingsButtonUPtr_.get());
        EntityAdd(creditsButtonUPtr_.get());
        EntityAdd(exitButtonUPtr_.get());
    }

    void MainMenuStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        target.draw(backgroundImage_, STATES);
        target.draw(gradient_);
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
            if (false == resumeButtonUPtr_->IsDisabled())
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
