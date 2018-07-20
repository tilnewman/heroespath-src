// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// load-game-menu-stage.cpp
//
#include "load-game-menu-stage.hpp"

#include "creature/creature.hpp"
#include "creature/player-party.hpp"
#include "game/game-data-file.hpp"
#include "game/game-state-factory.hpp"
#include "game/game-state.hpp"
#include "game/loop-manager.hpp"
#include "game/world.hpp"
#include "misc/real.hpp"
#include "popup/popup-manager.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/gui/gui-elements.hpp"
#include "sfml-util/ouroboros.hpp"
#include "sfml-util/sfml-util.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/tile.hpp"

namespace heroespath
{
namespace stage
{

    LoadGameStage::LoadGameStage()
        : Stage(
              "LoadGameMenu",
              { sfml_util::Font::Default,
                sfml_util::Font::System,
                sfml_util::Font::SystemCondensed,
                sfml_util::Font::Number,
                sfml_util::Font::Handwriting },
              true)
        , stageTitle_("media-images-buttons-mainmenu-load-normal")
        , backgroundImage_("media-images-backgrounds-tile-darkknot")
        , backButtonUPtr_(std::make_unique<sfml_util::gui::MainMenuButton>(
              sfml_util::LoopState::Previous,
              sfml_util::gui::callback::IFourStateButtonCallbackHandlerPtrOpt_t(this),
              1.0f,
              sf::Vector2f(200.0f, sfml_util::DisplaySize().y - 100.0f)))
        , screenSizeV_(sfml_util::DisplaySize())
        , gsListBoxRect_(
              ((screenSizeV_.x * 0.1f) - 85.0f) + sfml_util::SpacerOld(800.0f),
              610.0f,
              (screenSizeV_.y * 0.5f),
              ((screenSizeV_.y * 0.5f) - ((screenSizeV_.y * 0.5f) * 0.5f)) + 50.0f)
        , gsListBoxBGColor_(sfml_util::Colors::Orange - sf::Color(100, 100, 100, 220))
        , gsListBoxColorSet_(
              sfml_util::Colors::Orange,
              gsListBoxBGColor_,
              sfml_util::Colors::Orange - sfml_util::gui::ColorSet::DEFAULT_OFFSET_COLOR_,
              gsListBoxBGColor_ - sf::Color(40, 40, 40, 0))
        , gsListBoxInfo_(
              1,
              true,
              gsListBoxRect_,
              gsListBoxColorSet_,
              sfml_util::gui::BackgroundInfo(gsListBoxBGColor_))
        , gsListBoxUPtr_(
              std::make_unique<sfml_util::gui::ListBox<LoadGameStage, game::GameStatePtr_t>>(
                  "GameStateToLoad",
                  this,
                  sfml_util::gui::ListBoxPacket(gsListBoxInfo_, sfml_util::Colors::Orange)))
        , locTextRegionUPtr_()
        , charTextRegionUVec_()
        , charLabelTextRegionUPtr_()
        , ouroborosUPtr_(std::make_unique<sfml_util::Ouroboros>("LoadGameStage's"))
        , gamestatePVec_()
    {}

    LoadGameStage::~LoadGameStage()
    {
        Stage::ClearAllEntities();

        for (auto const & GAME_STATE_PTR : gamestatePVec_)
        {
            delete GAME_STATE_PTR.Ptr();
        }

        gamestatePVec_.clear();
    }

    bool LoadGameStage::HandleCallback(
        const sfml_util::gui::callback::ListBoxEventPackage<LoadGameStage, game::GameStatePtr_t> &)
    {
        // TODO Handle selection of a game to load and then load it,
        // including a call to all creatures StoreItemsInWarehouseAfterLoad(),
        // and Party::Add/or re-add pointers using player::CharacterWarehouse::Store(),
        // and call gamestatePSet_.erase(ptr)

        SetupGameInfoDisplay();
        return true;
    }

    void LoadGameStage::Setup()
    {
        EntityAdd(ouroborosUPtr_.get());
        EntityAdd(backButtonUPtr_.get());

        // hand all GameState objects to the ListBox
        // TODO this is wasteful in the extreme, need a GameStateFactory::LoadAllSavedGameProfiles()
        // function that doesn't actually load every game but a vector of profiles to use instead.
        gamestatePVec_ = game::GameStateFactory::Instance()->LoadAllGames();

        for (auto const & GAMESTATE_PTR : gamestatePVec_)
        {
            std::ostringstream ss;
            ss << "Last Saved " << GAMESTATE_PTR->DateTimeOfLastSave().date.year << "-"
               << GAMESTATE_PTR->DateTimeOfLastSave().date.month << "-"
               << GAMESTATE_PTR->DateTimeOfLastSave().date.day << "  "
               << GAMESTATE_PTR->DateTimeOfLastSave().time.hours << ":"
               << GAMESTATE_PTR->DateTimeOfLastSave().time.minutes << ":"
               << GAMESTATE_PTR->DateTimeOfLastSave().time.seconds;

            const sfml_util::gui::TextInfo TEXT_INFO(
                ss.str(), sfml_util::FontManager::Instance()->GetFont(sfml_util::Font::System));

            gsListBoxUPtr_->Append(
                std::make_unique<sfml_util::gui::ListElement<game::GameStatePtr_t>>(
                    GAMESTATE_PTR, TEXT_INFO));
        }

        EntityAdd(gsListBoxUPtr_.get());
        SetupGameInfoDisplay();

        SetFocus(gsListBoxUPtr_.get());
        // gsListBoxUPtr_->SilentExerciseHack();
    }

    void LoadGameStage::SetupGameInfoDisplay()
    {
        // free any existing TextRegion objects
        for (auto const & NEXT_TEXTREGION_UPTR : charTextRegionUVec_)
        {
            EntityRemove(NEXT_TEXTREGION_UPTR.get());
        }

        charTextRegionUVec_.clear();

        if (gsListBoxUPtr_->Empty())
        {
            return;
        }

        auto const GAMESTATE_PTR { gsListBoxUPtr_->Selection()->Element() };

        sfml_util::gui::TextInfo descTextInfo(
            "",
            sfml_util::FontManager::Instance()->GetFont(sfml_util::Font::System),
            sfml_util::FontManager::Instance()->Size_Smallish());

        // establish positions
        auto const CHAR_LIST_POS_LEFT { sfml_util::Right(gsListBoxRect_) + 75.0f };
        auto const CHAR_LIST_POS_TOP { gsListBoxRect_.top + 100.0f };

        // setup location text
        if (!locTextRegionUPtr_)
        {
            locTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>("LoadGameLocation");
            EntityAdd(locTextRegionUPtr_.get());
        }

        descTextInfo.text = std::string("Location:        ")
                                .append(GAMESTATE_PTR->GetWorld().GetMaps().Current().Name());

        const sf::FloatRect LOC_TEXT_RECT(
            CHAR_LIST_POS_LEFT, CHAR_LIST_POS_TOP - 35.0f, 0.0f, 0.0f);

        locTextRegionUPtr_->Setup(descTextInfo, LOC_TEXT_RECT);

        // setup character list label text
        if (!charLabelTextRegionUPtr_)
        {
            charLabelTextRegionUPtr_
                = std::make_unique<sfml_util::gui::TextRegion>("CharacterListLabel");

            EntityAdd(charLabelTextRegionUPtr_.get());
        }

        descTextInfo.text = "Characters:";

        const sf::FloatRect CHAR_TEXT_RECT(
            CHAR_LIST_POS_LEFT, CHAR_LIST_POS_TOP - 5.0f, 0.0f, 0.0f);

        charLabelTextRegionUPtr_->Setup(descTextInfo, CHAR_TEXT_RECT);

        // setup characters list
        auto const CHAR_PVEC { GAMESTATE_PTR->Party().Characters() };
        auto posY { CHAR_LIST_POS_TOP + 30.0f };

        auto const NUM_CHARS { CHAR_PVEC.size() };
        for (std::size_t i(0); i < NUM_CHARS; ++i)
        {
            std::ostringstream ss;
            ss << "CharList_" << i << "_" << CHAR_PVEC[i]->Name();
            auto const TEXT_REGION_ENTITY_NAME { ss.str() };

            ss.str("");
            ss << CHAR_PVEC[i]->Name() << ", " << CHAR_PVEC[i]->RoleName()
               << ", rank: " << CHAR_PVEC[i]->Rank();

            descTextInfo.text = ss.str();

            const sf::FloatRect RECT(CHAR_LIST_POS_LEFT + 25.0f, posY, 0.0f, 0.0f);

            auto textRegionUPtr { std::make_unique<sfml_util::gui::TextRegion>(
                TEXT_REGION_ENTITY_NAME, descTextInfo, RECT) };

            EntityAdd(textRegionUPtr.get());
            posY += textRegionUPtr->GetEntityRegion().height - 25.0f;
            charTextRegionUVec_.emplace_back(std::move(textRegionUPtr));
        }
    }

    void LoadGameStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        target.draw(backgroundImage_, STATES);
        target.draw(stageTitle_, STATES);
        Stage::Draw(target, STATES);
    }

    bool LoadGameStage::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if (Stage::KeyRelease(KEY_EVENT))
        {
            return true;
        }
        else if (KEY_EVENT.code == sf::Keyboard::B)
        {
            backButtonUPtr_->SetMouseState(sfml_util::MouseState::Over);
            sfml_util::SoundManager::Instance()->PlaySfx_Keypress();
            game::LoopManager::Instance()->TransitionTo_MainMenu();
            return true;
        }
        else
        {
            return false;
        }
    }

} // namespace stage
} // namespace heroespath
