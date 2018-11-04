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
#include "game/game-state-factory.hpp"
#include "game/game-state.hpp"
#include "game/world.hpp"
#include "gui/display.hpp"
#include "gui/gui-images.hpp"
#include "gui/ouroboros.hpp"
#include "gui/sound-manager.hpp"
#include "misc/config-file.hpp"
#include "misc/real.hpp"
#include "popup/popup-manager.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace heroespath
{
namespace stage
{

    LoadGameStage::LoadGameStage()
        : StageBase(
            "LoadGameMenu",
            { gui::GuiFont::Default,
              gui::GuiFont::System,
              gui::GuiFont::SystemCondensed,
              gui::GuiFont::Number,
              gui::GuiFont::Handwriting },
            true)
        , stageTitle_("media-images-buttons-mainmenu-load-normal")
        , backgroundBox_(
              "LoadGameStage'sBackground",
              StageRegion(),
              gui::BoxEntityInfo(gui::CachedTexture(
                  "media-images-backgrounds-tile-darkknot",
                  gui::ImageOpt::Default | gui::ImageOpt::Repeated)))
        , backButtonUPtr_(std::make_unique<gui::MainMenuButton>(
              stage::Stage::Previous,
              gui::ImageTextEntity::Callback_t::IHandlerPtr_t(this),
              1.0f,
              sf::Vector2f(200.0f, sfutil::DisplaySize().y - 100.0f)))
        , screenSizeV_(sfutil::DisplaySize())
        , gsListBoxRect_(
              ((screenSizeV_.x * 0.1f) - 85.0f) + sfutil::SpacerOld(800.0f),
              610.0f,
              (screenSizeV_.y * 0.5f),
              ((screenSizeV_.y * 0.5f) - ((screenSizeV_.y * 0.5f) * 0.5f)) + 50.0f)
        , gsListBoxBGColor_(sfutil::color::Orange - sf::Color(100, 100, 100, 220))
        , gsListBoxFocusColors_(
              sfutil::color::Orange,
              gsListBoxBGColor_,
              sfutil::color::Orange - gui::FocusColors::DEFAULT_OFFSET_COLOR_,
              gsListBoxBGColor_ - sf::Color(40, 40, 40, 0))
        , gsListBoxUPtr_()
        , locTextRegionUPtr_()
        , charTextRegionUVec_()
        , charLabelTextRegionUPtr_()
        , ouroborosUPtr_(std::make_unique<gui::Ouroboros>("LoadGameStage's"))
        , gamestatePVec_()
    {}

    LoadGameStage::~LoadGameStage()
    {
        StageBase::ClearAllEntities();

        for (const auto & GAME_STATE_PTR : gamestatePVec_)
        {
            delete GAME_STATE_PTR;
        }

        gamestatePVec_.clear();
    }

    const std::string LoadGameStage::HandleCallback(
        const GameListBox_t::Callback_t::Packet_t &, const std::string & PACKET_DESCRIPTION)
    {
        // TODO Handle selection of a game to load and then load it,
        // including a call to all creatures StoreItemsInWarehouseAfterLoad(),
        // and Party::Add/or re-add pointers using player::CharacterWarehouse::Store(),
        // and call gamestatePSet_.erase(ptr)

        SetupGameInfoDisplay();

        return MakeCallbackHandlerMessage(
            PACKET_DESCRIPTION, "game listbox callback NOT HANDLED TODO");
    }

    void LoadGameStage::Setup()
    {
        EntityAdd(ouroborosUPtr_);
        EntityAdd(backButtonUPtr_);

        // hand all GameState objects to the ListBox
        // TODO this is wasteful in the extreme, need a GameStateFactory::LoadAllSavedGameProfiles()
        // function that doesn't actually load every game but a vector of profiles to use instead.
        gamestatePVec_ = game::GameStateFactory::Instance()->LoadAllGames();

        for (const auto & GAMESTATE_PTR : gamestatePVec_)
        {
            std::ostringstream ss;
            ss << "Last Saved " << GAMESTATE_PTR->DateTimeOfLastSave().date.year << "-"
               << GAMESTATE_PTR->DateTimeOfLastSave().date.month << "-"
               << GAMESTATE_PTR->DateTimeOfLastSave().date.day << "  "
               << GAMESTATE_PTR->DateTimeOfLastSave().time.hours << ":"
               << GAMESTATE_PTR->DateTimeOfLastSave().time.minutes << ":"
               << GAMESTATE_PTR->DateTimeOfLastSave().time.seconds;

            const gui::TextInfo TEXT_INFO(
                ss.str(), gui::GuiFont::System, gui::FontManager::Instance()->Size_Normal());

            gsListBoxUPtr_->Append(
                std::make_unique<gui::ListElement<game::GameStatePtr_t>>(GAMESTATE_PTR, TEXT_INFO));
        }

        gui::BoxEntityInfo gsListBoxInfo;
        gsListBoxInfo.SetupBorder(true);
        gsListBoxInfo.focus_colors = gsListBoxFocusColors_;
        gsListBoxInfo.SetupColor(gsListBoxBGColor_);

        gsListBoxUPtr_ = std::make_unique<GameListBox_t>(
            "LoadGameStage'sGame",
            misc::MakeNotNull(this),
            misc::MakeNotNull(this),
            gui::ListBoxPacket(gsListBoxRect_, gsListBoxInfo, sfutil::color::Orange));

        EntityAdd(gsListBoxUPtr_);
        SetupGameInfoDisplay();

        SetFocus(gsListBoxUPtr_);
    }

    void LoadGameStage::SetupGameInfoDisplay()
    {
        // free any existing TextRegion objects
        for (const auto & NEXT_TEXTREGION_UPTR : charTextRegionUVec_)
        {
            EntityRemove(NEXT_TEXTREGION_UPTR);
        }

        charTextRegionUVec_.clear();

        if (gsListBoxUPtr_->Empty())
        {
            return;
        }

        const auto GAMESTATE_PTR { gsListBoxUPtr_->Selection()->Element() };

        gui::TextInfo descTextInfo(
            "", gui::GuiFont::System, gui::FontManager::Instance()->Size_Smallish());

        // establish positions
        const auto CHAR_LIST_POS_LEFT { sfutil::Right(gsListBoxRect_) + 75.0f };
        const auto CHAR_LIST_POS_TOP { gsListBoxRect_.top + 100.0f };

        // setup location text
        if (!locTextRegionUPtr_)
        {
            locTextRegionUPtr_ = std::make_unique<gui::TextRegion>("LoadGameLocation");
            EntityAdd(locTextRegionUPtr_);
        }

        descTextInfo.text = std::string("Location:        ")
                                .append(GAMESTATE_PTR->GetWorld().GetMaps().Current().Name());

        const sf::FloatRect LOC_TEXT_RECT(
            CHAR_LIST_POS_LEFT, CHAR_LIST_POS_TOP - 35.0f, 0.0f, 0.0f);

        locTextRegionUPtr_->Setup(descTextInfo, LOC_TEXT_RECT);

        // setup character list label text
        if (!charLabelTextRegionUPtr_)
        {
            charLabelTextRegionUPtr_ = std::make_unique<gui::TextRegion>("CharacterListLabel");

            EntityAdd(charLabelTextRegionUPtr_);
        }

        descTextInfo.text = "Characters:";

        const sf::FloatRect CHAR_TEXT_RECT(
            CHAR_LIST_POS_LEFT, CHAR_LIST_POS_TOP - 5.0f, 0.0f, 0.0f);

        charLabelTextRegionUPtr_->Setup(descTextInfo, CHAR_TEXT_RECT);

        // setup characters list
        const auto CHAR_PVEC { GAMESTATE_PTR->Party().Characters() };
        auto posY { CHAR_LIST_POS_TOP + 30.0f };

        const auto NUM_CHARS { CHAR_PVEC.size() };
        for (std::size_t i(0); i < NUM_CHARS; ++i)
        {
            std::ostringstream ss;
            ss << "CharList_" << i << "_" << CHAR_PVEC[i]->Name();
            const auto TEXT_REGION_ENTITY_NAME { ss.str() };

            ss.str("");
            ss << CHAR_PVEC[i]->Name() << ", " << CHAR_PVEC[i]->RoleName()
               << ", rank: " << CHAR_PVEC[i]->Rank();

            descTextInfo.text = ss.str();

            const sf::FloatRect RECT(CHAR_LIST_POS_LEFT + 25.0f, posY, 0.0f, 0.0f);

            auto textRegionUPtr { std::make_unique<gui::TextRegion>(
                TEXT_REGION_ENTITY_NAME, descTextInfo, RECT) };

            EntityAdd(textRegionUPtr);
            posY += textRegionUPtr->GetEntityRegion().height - 25.0f;
            charTextRegionUVec_.emplace_back(std::move(textRegionUPtr));
        }
    }

    void LoadGameStage::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(backgroundBox_, states);
        target.draw(stageTitle_, states);
        StageBase::draw(target, states);
    }

    bool LoadGameStage::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if (StageBase::KeyRelease(KEY_EVENT))
        {
            return true;
        }
        else if (KEY_EVENT.code == sf::Keyboard::B)
        {
            backButtonUPtr_->SetMouseState(gui::MouseState::Over);
            gui::SoundManager::Instance()->PlaySfx_Keypress();
            TransitionTo(stage::Stage::Menu);
            return true;
        }
        else
        {
            return false;
        }
    }

} // namespace stage
} // namespace heroespath
