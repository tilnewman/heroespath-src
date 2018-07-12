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
#include "sfml-util/gui/gui-elements.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/sfml-util.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/tile.hpp"

namespace heroespath
{
namespace stage
{

    MainMenuStage::MainMenuStage()
        : Stage(
              "MainMenu",
              {
                  sfml_util::Font::Default,
                  sfml_util::Font::System,
              },
              true)
        , BUTTON_SCALE_(sfml_util::MapByRes(1.0f, 3.0f))
        , titleCachedTexture_("media-images-title-blacksymbol")
        , titleSprite_(titleCachedTexture_.Get())
        , gradient_(
              sfml_util::DisplayRect(),
              sfml_util::GradientInfo(
                  sf::Color(0, 0, 0, 200),
                  sfml_util::Corner::TopLeft | sfml_util::Corner::BottomRight))
        , resumeButtonUPtr_(std::make_unique<sfml_util::gui::MainMenuButton>(
              sfml_util::LoopState::Load,
              sfml_util::gui::callback::IFourStateButtonCallbackHandlerPtrOpt_t(this),
              BUTTON_SCALE_))
        , createButtonUPtr_(std::make_unique<sfml_util::gui::MainMenuButton>(
              sfml_util::LoopState::Character,
              sfml_util::gui::callback::IFourStateButtonCallbackHandlerPtrOpt_t(this),
              BUTTON_SCALE_))
        , settingsButtonUPtr_(std::make_unique<sfml_util::gui::MainMenuButton>(
              sfml_util::LoopState::Settings,
              sfml_util::gui::callback::IFourStateButtonCallbackHandlerPtrOpt_t(this),
              BUTTON_SCALE_))
        , creditsButtonUPtr_(std::make_unique<sfml_util::gui::MainMenuButton>(
              sfml_util::LoopState::Credits,
              sfml_util::gui::callback::IFourStateButtonCallbackHandlerPtrOpt_t(this),
              BUTTON_SCALE_))
        , exitButtonUPtr_(std::make_unique<sfml_util::gui::MainMenuButton>(
              sfml_util::LoopState::Exit,
              sfml_util::gui::callback::IFourStateButtonCallbackHandlerPtrOpt_t(this),
              BUTTON_SCALE_))
        , ouroborosUPtr_(std::make_unique<sfml_util::Ouroboros>("MainMenu's"))
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
        sfml_util::ScaleSize(titleSprite_, sfml_util::MapByRes(0.5f, 3.0f));

        titleSprite_.setPosition(
            sfml_util::DisplayCenterHoriz(titleSprite_.getGlobalBounds().width),
            sfml_util::MapByRes(20.0f, 188.0f));

        // Ouroboros
        EntityAdd(ouroborosUPtr_.get());

        // buttons
        resumeButtonUPtr_->SetScaleToRes();
        createButtonUPtr_->SetScaleToRes();
        settingsButtonUPtr_->SetScaleToRes();
        creditsButtonUPtr_->SetScaleToRes();
        exitButtonUPtr_->SetScaleToRes();

        auto const SPACE_BETWEEN_BUTTONS { sfml_util::MapByRes(8.0f, 50.0f) };
        auto const TITLE_TO_BUTTONS_SPACER { sfml_util::MapByRes(160.0f, 1000.0f) };

        resumeButtonUPtr_->SetEntityPos(
            sfml_util::DisplayCenterHoriz(resumeButtonUPtr_->GetEntityRegion().width),
            sfml_util::Bottom(titleSprite_.getGlobalBounds()) + TITLE_TO_BUTTONS_SPACER);

        createButtonUPtr_->SetEntityPos(
            sfml_util::DisplayCenterHoriz(createButtonUPtr_->GetEntityRegion().width),
            sfml_util::Bottom(resumeButtonUPtr_->GetEntityRegion()) + SPACE_BETWEEN_BUTTONS);

        settingsButtonUPtr_->SetEntityPos(
            sfml_util::DisplayCenterHoriz(settingsButtonUPtr_->GetEntityRegion().width),
            sfml_util::Bottom(createButtonUPtr_->GetEntityRegion()) + SPACE_BETWEEN_BUTTONS);

        creditsButtonUPtr_->SetEntityPos(
            sfml_util::DisplayCenterHoriz(creditsButtonUPtr_->GetEntityRegion().width),
            sfml_util::Bottom(settingsButtonUPtr_->GetEntityRegion()) + SPACE_BETWEEN_BUTTONS);

        exitButtonUPtr_->SetEntityPos(
            sfml_util::DisplayCenterHoriz(exitButtonUPtr_->GetEntityRegion().width),
            sfml_util::Bottom(creditsButtonUPtr_->GetEntityRegion()) + SPACE_BETWEEN_BUTTONS);

        resumeButtonUPtr_->SetCallbackHandler(this);

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
