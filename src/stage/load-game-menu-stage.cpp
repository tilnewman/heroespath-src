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
#include "sfml-util/gui/list-box-item-factory.hpp"
#include "sfml-util/gui/list-box-item.hpp"
#include "sfml-util/gui/text-region.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/ouroboros.hpp"
#include "sfml-util/sfml-util.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/tile.hpp"

#include <list>
#include <string>

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
        , SCREEN_WIDTH_(sfml_util::Display::Instance()->GetWinWidth())
        , SCREEN_HEIGHT_(sfml_util::Display::Instance()->GetWinHeight())
        , mainMenuTitle_("resume_button_normal.png")
        , backgroundImage_("media-images-backgrounds-tile-darkknot")
        , backButtonUPtr_()
        , gsListBoxUPtr_()
        , locTextRegionUPtr_()
        , charTextRegionUVec_()
        , charLabelTextRegionUPtr_()
        , gsListBoxPosLeft_(0.0f)
        , gsListBoxPosTop_(0.0f)
        , gsListBoxPosWidth_(0.0f)
        , gsListBoxPosHeight_(0.0f)
        , ouroborosUPtr_()
        , bottomSymbol_()
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
        const sfml_util::gui::callback::FourStateButtonCallbackPackage_t & PACKAGE)
    {
        if (PACKAGE.PTR_ == backButtonUPtr_.get())
        {
            game::LoopManager::Instance()->TransitionTo_MainMenu();
            return true;
        }

        return false;
    }

    bool LoadGameStage::HandleCallback(const sfml_util::gui::callback::ListBoxEventPackage &)
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
        // ouroboros
        ouroborosUPtr_ = std::make_unique<sfml_util::Ouroboros>("LoadGameStage's");
        EntityAdd(ouroborosUPtr_.get());

        // back button
        auto const BUTTONS_PATH{ game::GameDataFile::Instance()->GetMediaPath(
            "media-images-buttons-mainmenu-dir") };

        backButtonUPtr_ = std::make_unique<sfml_util::gui::FourStateButton>(
            "Back",
            200.0f,
            SCREEN_HEIGHT_ - 100.0f,
            std::string(BUTTONS_PATH).append("back_button_normal.png"),
            "",
            std::string(BUTTONS_PATH).append("back_button_lit.png"));

        backButtonUPtr_->SetCallbackHandler(this);
        EntityAdd(backButtonUPtr_.get());

        // GameState ListBox
        //
        // determine position on screen
        gsListBoxPosLeft_ = ((SCREEN_WIDTH_ * 0.1f) - 85.0f) + sfml_util::SpacerOld(800.0f);
        gsListBoxPosWidth_ = 610.0f;
        gsListBoxPosHeight_ = SCREEN_HEIGHT_ * 0.5f;
        gsListBoxPosTop_ = ((SCREEN_HEIGHT_ * 0.5f) - (gsListBoxPosHeight_ * 0.5f)) + 50.0f;

        sf::FloatRect GS_LB_RECT(
            gsListBoxPosLeft_, gsListBoxPosTop_, gsListBoxPosWidth_, gsListBoxPosHeight_);

        // establish the boxing options
        auto const BG_COLOR{ sfml_util::FontManager::Color_Orange()
                             - sf::Color(100, 100, 100, 220) };

        const sfml_util::gui::BackgroundInfo BG_INFO(BG_COLOR);

        const sfml_util::gui::box::Info BOX_INFO(
            1,
            true,
            GS_LB_RECT,
            sfml_util::gui::ColorSet(
                sfml_util::FontManager::Color_Orange(),
                BG_COLOR,
                sfml_util::FontManager::Color_Orange()
                    - sfml_util::gui::ColorSet::DEFAULT_OFFSET_COLOR_,
                BG_COLOR - sf::Color(40, 40, 40, 0)),
            BG_INFO);

        // create the ListBox
        gsListBoxUPtr_ = std::make_unique<sfml_util::gui::ListBox>(
            "GameStateToLoad",
            GS_LB_RECT,
            sfml_util::IStagePtr_t(this),
            BOX_INFO,
            sfml_util::FontManager::Color_Orange(),
            sfml_util::gui::callback::IListBoxCallbackHandlerPtr_t(this));

        // hand all GameState objects to the ListBox
        // TODO this is wasteful in the extreme, need a GameStateFactory::LoadAllSavedGameProfiles()
        // function that doesn't actually load every game but a vector of profiles to use instead.
        gamestatePVec_ = game::GameStateFactory::Instance()->LoadAllGames();

        sfml_util::gui::ListBoxItemFactory listBoxItemFactory;
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

            gsListBoxUPtr_->Add(
                listBoxItemFactory.Make(gsListBoxUPtr_->GetEntityName(), TEXT_INFO, GAMESTATE_PTR));
        }

        EntityAdd(gsListBoxUPtr_.get());

        SetupGameInfoDisplay();
    }

    void LoadGameStage::SetupGameInfoDisplay()
    {
        // free any existing TextRegion objects
        for (auto const & NEXT_TEXTREGION_UPTR : charTextRegionUVec_)
        {
            EntityRemove(NEXT_TEXTREGION_UPTR.get());
        }

        charTextRegionUVec_.clear();

        auto const SELECTED_ITEM_PTR_OPT{ gsListBoxUPtr_->Selected() };
        if (!SELECTED_ITEM_PTR_OPT | !SELECTED_ITEM_PTR_OPT.value()->GameStatePtrOpt())
        {
            return;
        }

        auto const GAMESTATE_PTR{ SELECTED_ITEM_PTR_OPT.value()->GameStatePtrOpt().value() };

        sfml_util::gui::TextInfo descTextInfo(
            "",
            sfml_util::FontManager::Instance()->GetFont(sfml_util::Font::System),
            sfml_util::FontManager::Instance()->Size_Smallish());

        // establish positions
        auto const CHAR_LIST_POS_LEFT{ gsListBoxPosLeft_ + gsListBoxPosWidth_ + 75.0f };
        auto const CHAR_LIST_POS_TOP{ gsListBoxPosTop_ + 100.0f };

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
        auto const CHAR_PVEC{ GAMESTATE_PTR->Party().Characters() };
        auto posY{ CHAR_LIST_POS_TOP + 30.0f };

        auto const NUM_CHARS{ CHAR_PVEC.size() };
        for (std::size_t i(0); i < NUM_CHARS; ++i)
        {
            std::ostringstream ss;
            ss << "CharList_" << i << "_" << CHAR_PVEC[i]->Name();
            auto const TEXT_REGION_ENTITY_NAME{ ss.str() };

            ss.str("");
            ss << CHAR_PVEC[i]->Name() << ", " << CHAR_PVEC[i]->RoleName()
               << ", rank: " << CHAR_PVEC[i]->Rank();

            descTextInfo.text = ss.str();

            const sf::FloatRect RECT(CHAR_LIST_POS_LEFT + 25.0f, posY, 0.0f, 0.0f);

            auto textRegionUPtr{ std::make_unique<sfml_util::gui::TextRegion>(
                TEXT_REGION_ENTITY_NAME, descTextInfo, RECT) };

            EntityAdd(textRegionUPtr.get());
            posY += textRegionUPtr->GetEntityRegion().height - 25.0f;
            charTextRegionUVec_.emplace_back(std::move(textRegionUPtr));
        }
    }

    void LoadGameStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        target.draw(backgroundImage_, STATES);
        target.draw(mainMenuTitle_, STATES);
        target.draw(bottomSymbol_, STATES);
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
