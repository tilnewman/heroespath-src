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

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/gui/gui-elements.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/tile.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/sound-manager.hpp"

#include "game/game-data-file.hpp"
#include "game/log-macros.hpp"
#include "game/loop-manager.hpp"
#include "game/state/game-state.hpp"
#include "game/state/game-state-factory.hpp"

#include "misc/real.hpp"


namespace game
{
namespace stage
{

    MainMenuStage::MainMenuStage()
    :
        Stage              ("MainMenu"),
        SCREEN_WIDTH_      (sfml_util::Display::Instance()->GetWinWidth()),
        SCREEN_HEIGHT_     (sfml_util::Display::Instance()->GetWinHeight()),
        SCREEN_RECT_       (0.0f, 0.0f, sfml_util::Display::Instance()->GetWinWidth(), sfml_util::Display::Instance()->GetWinHeight()),
        BUTTON_SCALE_      (sfml_util::MapByRes(1.0f, 3.0f)),
        titleTexture_      (),
        titleSprite_       (),
        gradient_          (),
        resumeButtonSPtr_  ( new main_menu_buttons::ResumeButton(0.0f, 0.0f, BUTTON_SCALE_, false) ),
        createButtonSPtr_  ( new main_menu_buttons::CreateCharactersButton(0.0f, 0.0f, BUTTON_SCALE_, false) ),
        settingsButtonSPtr_( new main_menu_buttons::SettingsButton(0.0f, 0.0f, BUTTON_SCALE_, false) ),
        creditsButtonSPtr_ ( new main_menu_buttons::CreditsButton(0.0f, 0.0f, BUTTON_SCALE_, false) ),
        exitButtonSPtr_    ( new main_menu_buttons::ExitButton(0.0f, 0.0f, BUTTON_SCALE_, false) ),
        ouroborosUPtr_     (),
        bottomSymbol_      (),
        backgroundImage_   ("media-images-backgrounds-tile-darkknot")
    {}


    MainMenuStage::~MainMenuStage()
    {
        ClearAllEntities();
    }


    bool MainMenuStage::HandleCallback(const sfml_util::gui::callback::FourStateButtonCallbackPackage_t & PACKAGE)
    {

        if (PACKAGE.PTR_ == resumeButtonSPtr_.get())
        {
            LoopManager::Instance()->Goto_LoadGameMenu();
            return true;
        }

        return false;
    }


    void MainMenuStage::Setup()
    {
        //title image
        sfml_util::LoadImageOrTexture<sf::Texture>(titleTexture_, GameDataFile::Instance()->GetMediaPath("media-images-title-blacksymbol"));
        titleTexture_.setSmooth(true);
        titleSprite_.setTexture(titleTexture_);
        const float TITLE_IMAGE_SCALE(sfml_util::MapByRes(0.5f, 3.0f));
        titleSprite_.setScale(TITLE_IMAGE_SCALE, TITLE_IMAGE_SCALE);
        titleSprite_.setPosition((SCREEN_WIDTH_ * 0.5f) - (titleSprite_.getGlobalBounds().width * 0.5f), sfml_util::MapByRes(20.0f, 188.0f));

        //gradient
        gradient_.Setup(SCREEN_RECT_, sfml_util::GradientInfo(sf::Color(0,0,0,200), sfml_util::Corner::TopLeft | sfml_util::Corner::BottomRight) );

        //Ouroboros
        ouroborosUPtr_ = std::make_unique<game::Ouroboros>("MainMenu's");
        EntityAdd(ouroborosUPtr_.get());

        //buttons
        resumeButtonSPtr_->SetScaleToRes();
        createButtonSPtr_->SetScaleToRes();
        settingsButtonSPtr_->SetScaleToRes();
        creditsButtonSPtr_->SetScaleToRes();
        exitButtonSPtr_->SetScaleToRes();
        //
        const float SPACE_BETWEEN_BUTTONS(sfml_util::MapByRes(8.0f, 50.0f));
        const float TITLE_TO_BUTTONS_SPACER(sfml_util::MapByRes(160.0f, 1000.0f));
        //
        resumeButtonSPtr_->SetEntityPos  ((SCREEN_WIDTH_ * 0.5f) - (resumeButtonSPtr_->GetEntityRegion().width   * 0.5f), titleSprite_.getGlobalBounds().top + titleSprite_.getGlobalBounds().height + TITLE_TO_BUTTONS_SPACER);
        createButtonSPtr_->SetEntityPos  ((SCREEN_WIDTH_ * 0.5f) - (createButtonSPtr_->GetEntityRegion().width   * 0.5f), resumeButtonSPtr_->GetEntityRegion().top   + resumeButtonSPtr_->GetEntityRegion().height   + SPACE_BETWEEN_BUTTONS);
        settingsButtonSPtr_->SetEntityPos((SCREEN_WIDTH_ * 0.5f) - (settingsButtonSPtr_->GetEntityRegion().width * 0.5f), createButtonSPtr_->GetEntityRegion().top   + createButtonSPtr_->GetEntityRegion().height   + SPACE_BETWEEN_BUTTONS);
        creditsButtonSPtr_->SetEntityPos ((SCREEN_WIDTH_ * 0.5f) - (creditsButtonSPtr_->GetEntityRegion().width  * 0.5f), settingsButtonSPtr_->GetEntityRegion().top + settingsButtonSPtr_->GetEntityRegion().height + SPACE_BETWEEN_BUTTONS);
        exitButtonSPtr_->SetEntityPos    ((SCREEN_WIDTH_ * 0.5f) - (exitButtonSPtr_->GetEntityRegion().width     * 0.5f), creditsButtonSPtr_->GetEntityRegion().top  + creditsButtonSPtr_->GetEntityRegion().height  + SPACE_BETWEEN_BUTTONS);
        //
        EntityAdd(resumeButtonSPtr_.get());
        EntityAdd(createButtonSPtr_.get());
        EntityAdd(settingsButtonSPtr_.get());
        EntityAdd(creditsButtonSPtr_.get());
        EntityAdd(exitButtonSPtr_.get());
        //
        resumeButtonSPtr_->SetCallbackHandler(this);

        //determine if there are saved games to load
        auto const GAMESTATE_PSET{ state::GameStateFactory::Instance()->LoadAllGames() };
        if (GAMESTATE_PSET.empty())
        {
            resumeButtonSPtr_->SetIsDisabled(true);
        }
        else
        {
            //free all of the loaded games
            for (auto const NEXT_GAMESTATE_PTR : GAMESTATE_PSET)
            {
                delete NEXT_GAMESTATE_PTR;
            }
        }
    }


    void MainMenuStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        target.draw(backgroundImage_, STATES);
        target.draw(gradient_);
        target.draw(titleSprite_, STATES);
        bottomSymbol_.Draw(target, STATES);
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
            createButtonSPtr_->SetMouseState(sfml_util::MouseState::Over);
            sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::Switch).PlayRandom();
            LoopManager::Instance()->Goto_CharacterCreation();
            return true;
        }
        else if (KEY_EVENT.code == sf::Keyboard::S)
        {
            settingsButtonSPtr_->SetMouseState(sfml_util::MouseState::Over);
            sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::Switch).PlayRandom();
            LoopManager::Instance()->Goto_Settings();
            return true;
        }
        else if (KEY_EVENT.code == sf::Keyboard::C)
        {
            creditsButtonSPtr_->SetMouseState(sfml_util::MouseState::Over);
            sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::Switch).PlayRandom();
            LoopManager::Instance()->Goto_Credits();
            return true;
        }
        else if (KEY_EVENT.code == sf::Keyboard::E)
        {
            exitButtonSPtr_->SetMouseState(sfml_util::MouseState::Over);
            sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::Switch).PlayRandom();
            LoopManager::Instance()->Goto_Exit();
            return true;
        }
        else if (KEY_EVENT.code == sf::Keyboard::R)
        {
            if (false == resumeButtonSPtr_->IsDisabled())
            {
                resumeButtonSPtr_->SetMouseState(sfml_util::MouseState::Over);
                sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::Switch).PlayRandom();
                LoopManager::Instance()->Goto_LoadGameMenu();
                return true;
            }
        }

        return false;
    }

}
}
