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
// load-game-menu-stage.cpp
//
#include "load-game-menu-stage.hpp"

#include "sfml-util/display.hpp"
#include "sfml-util/gui/gui-elements.hpp"
#include "sfml-util/gui/list-box-item.hpp"
#include "sfml-util/gui/text-region.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/ouroboros.hpp"
#include "sfml-util/sfml-util.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/tile.hpp"

#include "popup/popup-manager.hpp"

#include "game/game-data-file.hpp"
#include "game/loop-manager.hpp"
#include "player/character.hpp"
#include "player/party.hpp"
#include "state/game-state-factory.hpp"
#include "state/game-state.hpp"
#include "state/world.hpp"

#include "misc/real.hpp"

#include <list>
#include <string>

namespace heroespath
{
namespace stage
{

    LoadGameStage::LoadGameStage()
        : Stage("LoadGameMenu")
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
        , gamestatePSet_()
    {}

    LoadGameStage::~LoadGameStage()
    {
        ClearAllEntities();

        for (auto & nextGameStatePtr : gamestatePSet_)
        {
            delete nextGameStatePtr;
        }

        gamestatePSet_.clear();
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
        gsListBoxPosLeft_ = ((SCREEN_WIDTH_ * 0.1f) - 85.0f) + sfml_util::MapByRes(0.0f, 800.0f);
        gsListBoxPosWidth_ = 610.0f;
        gsListBoxPosHeight_ = SCREEN_HEIGHT_ * 0.5f;
        gsListBoxPosTop_ = ((SCREEN_HEIGHT_ * 0.5f) - (gsListBoxPosHeight_ * 0.5f)) + 50.0f;

        sf::FloatRect GS_LB_RECT(
            gsListBoxPosLeft_, gsListBoxPosTop_, gsListBoxPosWidth_, gsListBoxPosHeight_);

        // hand all GameState objects to the ListBox
        gamestatePSet_ = state::GameStateFactory::Instance()->LoadAllGames();
        sfml_util::gui::ListBoxItemSVec_t listBoxItemSVec;
        std::size_t gameStateCount(0);

        for (auto const NEXT_GAMESTATE_PTR : gamestatePSet_)
        {
            std::ostringstream ss;
            ss << "Started "
               /*TODO MAP NAME INSTEAD OF STARTED DATE*/
               << NEXT_GAMESTATE_PTR->DateTimeStarted().date.year << "-"
               << NEXT_GAMESTATE_PTR->DateTimeStarted().date.month << "-"
               << NEXT_GAMESTATE_PTR->DateTimeStarted().date.day << ", Saved "
               << NEXT_GAMESTATE_PTR->DateTimeOfLastSave().date.year << "-"
               << NEXT_GAMESTATE_PTR->DateTimeOfLastSave().date.month << "-"
               << NEXT_GAMESTATE_PTR->DateTimeOfLastSave().date.day << "  "
               << NEXT_GAMESTATE_PTR->DateTimeOfLastSave().time.hours << ":"
               << NEXT_GAMESTATE_PTR->DateTimeOfLastSave().time.minutes << ":"
               << NEXT_GAMESTATE_PTR->DateTimeOfLastSave().time.seconds;

            const sfml_util::gui::TextInfo TEXT_INFO(
                ss.str(),
                sfml_util::FontManager::Instance()->Font_Default2(),
                28,
                sf::Color::White,
                sfml_util::Justified::Left);

            ss.str("");
            ss << ++gameStateCount;

            auto const LBI_SPTR = std::make_shared<sfml_util::gui::ListBoxItem>(
                ss.str(), TEXT_INFO, NEXT_GAMESTATE_PTR);

            listBoxItemSVec.push_back(LBI_SPTR);
        }

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

        // reate the ListBox
        gsListBoxUPtr_ = std::make_unique<sfml_util::gui::ListBox>(
            "GameStateToLoad",
            GS_LB_RECT,
            listBoxItemSVec,
            this,
            10.0f,
            6.0f,
            BOX_INFO,
            sfml_util::FontManager::Color_Orange(),
            this);

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

        // establish which item is selected and get the player list from that
        // GameState's Party object
        if (gsListBoxUPtr_->Empty())
        {
            return;
        }

        auto listBoxItemSPtr{ gsListBoxUPtr_->Selected() };
        M_ASSERT_OR_LOGANDTHROW_SS(
            (listBoxItemSPtr.get() != nullptr),
            "LoadGameStage::SetupGameInfoDisplay() The ListBox was not empty but GetSelected()"
                << " returned a nullptr.");

        auto gameStatePtr(listBoxItemSPtr->GAMESTATE_CPTR);
        M_ASSERT_OR_LOGANDTHROW_SS(
            (gameStatePtr != nullptr),
            "LoadGameStage::SetupGameInfoDisplay() The ListBox was not empty but GetSelected()"
                << " returned a GAMESTATE_CPTR that was null.");

        sfml_util::gui::TextInfo descTextInfo(
            "",
            sfml_util::FontManager::Instance()->Font_Default2(),
            26,
            sf::Color::White,
            sfml_util::Justified::Left);

        // establish positions
        auto const CHAR_LIST_POS_LEFT{ gsListBoxPosLeft_ + gsListBoxPosWidth_ + 75.0f };
        auto const CHAR_LIST_POS_TOP{ gsListBoxPosTop_ + 100.0f };

        // setup location text
        if (locTextRegionUPtr_.get() == nullptr)
        {
            locTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>("LoadGameLocation");

            EntityAdd(locTextRegionUPtr_.get());
        }

        descTextInfo.text = std::string("Location:        ")
                                .append(gameStatePtr->World().GetMaps().Current().Name());

        const sf::FloatRect LOC_TEXT_RECT(
            CHAR_LIST_POS_LEFT, CHAR_LIST_POS_TOP - 35.0f, 0.0f, 0.0f);

        locTextRegionUPtr_->Setup(descTextInfo, LOC_TEXT_RECT);

        // setup character list label text
        if (charLabelTextRegionUPtr_.get() == nullptr)
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
        auto const CHAR_PVEC{ gameStatePtr->Party().Characters() };
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
            charTextRegionUVec_.push_back(std::move(textRegionUPtr));
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
}
}
