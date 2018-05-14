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
// inventory-stage.hpp
//
#include "inventory-stage.hpp"

#include "combat/combat-sound-effects.hpp"
#include "combat/fight.hpp"
#include "creature/algorithms.hpp"
#include "creature/condition.hpp"
#include "creature/creature.hpp"
#include "game/game-data-file.hpp"
#include "game/game.hpp"
#include "game/loop-manager.hpp"
#include "item/item-warehouse.hpp"
#include "item/item.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/real.hpp"
#include "player/party.hpp"
#include "popup/popup-stage-char-select.hpp"
#include "popup/popup-stage-inventory-prompt.hpp"
#include "popup/popup-stage-musicsheet.hpp"
#include "popup/popup-stage-num-select.hpp"
#include "popup/popup-stage-spellbook.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/gui/box.hpp"
#include "sfml-util/gui/creature-image-loader.hpp"
#include "sfml-util/gui/item-image-loader.hpp"
#include "sfml-util/gui/list-box-item.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/gui/text-region.hpp"
#include "sfml-util/gui/title-image-loader.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/ouroboros.hpp"
#include "sfml-util/sfml-util.hpp"
#include "sfml-util/song-animation.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/sparkle-animation.hpp"
#include "song/song.hpp"
#include "spell/spell.hpp"
#include "state/game-state.hpp"

#include <algorithm>
#include <string>
#include <vector>

namespace heroespath
{
namespace stage
{

    const float InventoryStage::VIEW_CHANGE_SLIDER_SPEED_{ 4.0f };
    const float InventoryStage::VIEW_CHANGE_BETWEEN_TIME_SEC_{ 0.5f };
    const float InventoryStage::CREATURE_IMAGE_RIGHT_PAD_{ 10.0f };
    const float InventoryStage::DETAILVIEW_SLIDER_SPEED_{ 4.0f };
    const float InventoryStage::DETAILVIEW_TIMER_DURATION_SEC_{ 2.33f };
    const float InventoryStage::DETAILVIEW_COLOR_ALPHA_START_{ 20.0f };
    const float InventoryStage::DETAILVIEW_COLOR_ALPHA_END_{ 220.0f };
    const float InventoryStage::DETAILVIEW_INNER_PAD_{ 20.0f };

    const std::string InventoryStage::POPUP_NAME_GIVE_{ "InventoryStage'sGivePopupName" };

    const std::string InventoryStage::POPUP_NAME_CHAR_SELECT_{
        "InventoryStage'sCharacterSelectPopupName"
    };

    const std::string InventoryStage::POPUP_NAME_NUMBER_SELECT_{
        "InventoryStage'sPopupNumberSelectPopupName"
    };

    const std::string InventoryStage::POPUP_NAME_CONTENTSELECTION_{
        "InventoryStage'sShareGatherSelectionPopupName"
    };

    const std::string InventoryStage::POPUP_NAME_DROPCONFIRM_{
        "InventoryStage'sDropItemConfirmationPopupName"
    };

    const std::string InventoryStage::POPUP_NAME_SPELLBOOK_{ "InventoryStage'sSpellbookPopupName" };

    const std::string InventoryStage::POPUP_NAME_SPELL_RESULT_{
        "InventoryStage'sSpellResultPopupName"
    };

    const std::string InventoryStage::POPUP_NAME_MUSICSHEET_{
        "InventoryStage'sMusicSheetPopupName"
    };

    const std::string InventoryStage::POPUP_NAME_SONG_RESULT_{
        "InventoryStage'sSongResultPopupName"
    };

    InventoryStage::InventoryStage(
        const creature::CreaturePtr_t TURN_CREATURE_PTR,
        const creature::CreaturePtr_t INVENTORY_CREATURE_PTR,
        const game::Phase::Enum CURRENT_PHASE)
        : Stage(
              "Inventory",
              0.0f,
              0.0f,
              sfml_util::Display::Instance()->GetWinWidth(),
              sfml_util::Display::Instance()->GetWinHeight())
        , SCREEN_WIDTH_(sfml_util::Display::Instance()->GetWinWidth())
        , SCREEN_HEIGHT_(sfml_util::Display::Instance()->GetWinHeight())
        , INNER_PAD_(sfml_util::MapByRes(10.0f, 40.0f))
        , INNER_RECT_(
              INNER_PAD_,
              INNER_PAD_,
              SCREEN_WIDTH_ - (2.0f * INNER_PAD_),
              SCREEN_HEIGHT_ - (2.0f * INNER_PAD_))
        , CREATURE_IMAGE_POS_LEFT_(INNER_RECT_.left + sfml_util::MapByRes(35.0f, 100.0f))
        , CREATURE_IMAGE_SCALE_(sfml_util::MapByRes(0.75f, 3.25f))
        , CREATURE_IMAGE_HEIGHT_MAX_(
              sfml_util::gui::CreatureImageLoader::MaxDimmension() * CREATURE_IMAGE_SCALE_)
        , LISTBOX_HEIGHT_REDUCTION_(sfml_util::MapByRes(100.0f, 400.0f))
        , LISTBOX_SCREEN_EDGE_MARGIN_(sfml_util::MapByRes(35.0f, 100.0f))
        , LISTBOX_BETWEEN_SPACER_(sfml_util::MapByRes(65.0f, 200.0f))
        , LISTBOX_WIDTH_(
              ((INNER_RECT_.width - (2.0f * LISTBOX_SCREEN_EDGE_MARGIN_)) - LISTBOX_BETWEEN_SPACER_)
              * 0.5f)
        , OUT_OF_SIGHT_POS_((LISTBOX_WIDTH_ + sfml_util::MapByRes(100.0f, 300.0f)) * -1.0f)
        , FIRST_LISTBOX_POS_LEFT_(INNER_RECT_.left + LISTBOX_SCREEN_EDGE_MARGIN_)
        , SECOND_LISTBOX_POS_LEFT_(
              FIRST_LISTBOX_POS_LEFT_ + LISTBOX_WIDTH_ + LISTBOX_BETWEEN_SPACER_)
        , STATS_POS_LEFT_(
              (INNER_RECT_.left + INNER_RECT_.width) - sfml_util::MapByRes(275.0f, 1200.0f))
        , LISTBOX_COLOR_IMAGE_(sf::Color(255, 255, 255, 190))
        , LISTBOX_COLOR_LINE_(sfml_util::FontManager::Color_GrayDark())
        , LISTBOX_COLOR_FG_(LISTBOX_COLOR_LINE_)
        , LISTBOX_COLOR_BG_(sfml_util::FontManager::Color_Orange() - sf::Color(100, 100, 100, 220))
        , LISTBOX_COLOR_TITLE_(sfml_util::FontManager::Color_Orange() - sf::Color(130, 130, 130, 0))
        , DESCBOX_TEXT_COLOR_(LISTBOX_COLOR_TITLE_ - sf::Color(50, 50, 50, 0))
        , DESCBOX_TEXT_SIZE_(sfml_util::FontManager::Instance()->Size_Largeish())
        , LISTBOX_COLORSET_(LISTBOX_COLOR_FG_, LISTBOX_COLOR_BG_)
        , LISTBOX_BG_INFO_(LISTBOX_COLOR_BG_)
        , mainMenuTitle_("inventory_button.png", true, 1.0f, 0.75f)
        , CREATURE_IMAGE_POS_TOP_(mainMenuTitle_.Bottom(false))
        , LISTBOX_POS_TOP_(
              (CREATURE_IMAGE_POS_TOP_ + CREATURE_IMAGE_HEIGHT_MAX_)
              + (LISTBOX_HEIGHT_REDUCTION_ * 0.5f))
        , LISTBOX_HEIGHT_((SCREEN_HEIGHT_ - LISTBOX_POS_TOP_) - LISTBOX_HEIGHT_REDUCTION_)
        , LISTBOX_REGION_(
              FIRST_LISTBOX_POS_LEFT_, LISTBOX_POS_TOP_, LISTBOX_WIDTH_, LISTBOX_HEIGHT_)
        , DESCBOX_REGION_(
              SECOND_LISTBOX_POS_LEFT_, LISTBOX_POS_TOP_, LISTBOX_WIDTH_, LISTBOX_HEIGHT_)
        , DESCBOX_MARGIN_(15.0f)
        , DESCBOX_MARGINS_(DESCBOX_MARGIN_, DESCBOX_MARGIN_, DESCBOX_MARGIN_, DESCBOX_MARGIN_)
        , DETAILVIEW_WIDTH_(SCREEN_WIDTH_ * 0.75f)
        , DETAILVIEW_HEIGHT_(SCREEN_HEIGHT_ * 0.85f)
        , DETAILVIEW_POS_LEFT_((SCREEN_WIDTH_ * 0.5f) - (DETAILVIEW_WIDTH_ * 0.5f))
        , DETAILVIEW_POS_TOP_(sfml_util::MapByRes(35.0f, 100.0f))
        , SORT_ICON_SCALE_(sfml_util::MapByRes(0.1f, 0.35f))
        , SORT_ICON_SPACER_(sfml_util::MapByRes(22.0f, 75.0f))
        , SORT_ICON_POS_TOP_(
              (LISTBOX_POS_TOP_ - (127.0f * SORT_ICON_SCALE_)) - sfml_util::MapByRes(10.0f, 20.0f))
        , SORT_ICON_COLOR_(sf::Color(255, 255, 255, 127))
        , listBoxItemTextInfo_(
              " ",
              sfml_util::FontManager::Instance()->Font_Default2(),
              sfml_util::FontManager::Instance()->Size_Smallish(),
              sfml_util::FontManager::Color_GrayDarker(),
              sfml_util::Justified::Left)
        , creaturePtr_(INVENTORY_CREATURE_PTR)
        , bottomSymbol_(0.75f, true, sf::Color::White)
        , paperBgTexture_()
        , paperBgSprite_()
        , ouroborosUPtr_(std::make_unique<sfml_util::Ouroboros>("InventoryStage's", true))
        , creatureTexture_()
        , creatureSprite_()
        , view_(ViewType::Items)
        , characterViewMap_()
        , isSliderAnimating_(false)
        , isSlidingLeft_(false)
        , isViewForcedToItems_(false)
        , viewToChangeTo_(ViewType::Count)
        , sliderAnimTimerSec_(VIEW_CHANGE_BETWEEN_TIME_SEC_ + 1.0f)
        , detailsPosLeft_(0.0f)
        , centerPosLeft_(0.0f)
        , isImageSliding_(false)
        , isDetailsSliding_(false)
        , isCenterSliding_(false)
        , isStatsSliding_(false)
        , isListBoxSliding_(false)
        , isDescBoxSliding_(false)
        , isImageSlidingDone_(true)
        , isDetailsSlidingDone_(true)
        , isCenterSlidingDone_(true)
        , isStatsSlidingDone_(true)
        , isListBoxSlidingDone_(true)
        , isDescBoxSlidingDone_(true)
        , hasImageChanged_(false)
        , hasDetailsChanged_(false)
        , hasCenterChanged_(false)
        , hasStatsChanged_(false)
        , hasListBoxChanged_(false)
        , hasDescBoxChanged_(false)
        , imageSlider_(VIEW_CHANGE_SLIDER_SPEED_)
        , detailsSlider_(VIEW_CHANGE_SLIDER_SPEED_)
        , centerSlider_(VIEW_CHANGE_SLIDER_SPEED_)
        , statsSlider_(VIEW_CHANGE_SLIDER_SPEED_)
        , listBoxSlider_(VIEW_CHANGE_SLIDER_SPEED_)
        , descBoxSlider_(VIEW_CHANGE_SLIDER_SPEED_)
        , detailsTextRegionUPtr_()
        , statsTextRegionUPtr_()
        , eqTitleTextRegionUPtr_()
        , unEqTitleTextRegionUPtr_()
        , equippedListBoxUPtr_()
        , unEquipListBoxUPtr_()
        , insTextRegionUPtr_()
        , descTextRegionUPtr_()
        , descBoxUPtr_()
        , centerTextRegionUPtr_()
        , backButtonUPtr_()
        , itemsButtonUPtr_()
        , titlesButtonUPtr_()
        , condsButtonUPtr_()
        , spellsButtonUPtr_()
        , giveButtonUPtr_()
        , shareButtonUPtr_()
        , gatherButtonUPtr_()
        , equipButtonUPtr_()
        , unequipButtonUPtr_()
        , dropButtonUPtr_()
        , sortButtonNameTexture_()
        , sortButtonPriceTexture_()
        , sortButtonWeightTexture_()
        , eqSortButtonNameUPtr_()
        , eqSortButtonPriceUPtr_()
        , eqSortButtonWeightUPtr_()
        , unEqSortButtonNameUPtr_()
        , unEqSortButtonPriceUPtr_()
        , unEqSortButtonWeightUPtr_()
        , isSortReversedEqName_(false)
        , isSortReversedEqPrice_(false)
        , isSortReversedEqWeight_(false)
        , isSortReversedUneqName_(false)
        , isSortReversedUneqPrice_(false)
        , isSortReversedUneqWeight_(false)
        , actionType_(ActionType::Count)
        , contentType_(ContentType::Count)
        , listBoxItemToGiveSPtr_()
        , creatureToGiveToPtrOpt_(boost::none)
        , itemToDropSPtr_()
        , isDetailViewFadingIn_(false)
        , isDetailViewFadingOut_(false)
        , isDetailViewDoneFading_(false)
        , isAchievementDisplaying_(false)
        , hasMouseMoved_(false)
        , isWaitingOnPopup_(false)
        , detailViewTimerSec_(0.0f)
        , detailViewSliderRatio_(0.0f)
        , mousePosV_(0.0f, 0.0f)
        , detailViewSourceRect_(sfml_util::gui::ListBox::ERROR_RECT_)
        , detailViewQuads_(sf::Quads, 4)
        , detailViewSprite_()
        , detailViewTexture_()
        , detailViewTextUPtr_()
        , detailViewSlider_(DETAILVIEW_SLIDER_SPEED_)
        , spellBeingCastPtrOpt_(boost::none)
        , songBeingPlayedPtrOpt_(boost::none)
        , turnActionInfo_()
        , fightResult_()
        , creatureEffectIndex_(0)
        , hitInfoIndex_(0)
        , combatSoundEffectsUPtr_(std::make_unique<combat::CombatSoundEffects>())
        , sparkleAnimUPtr_()
        , songAnimUPtr_()
        , turnCreaturePtr_(TURN_CREATURE_PTR)
        , currentPhase_(CURRENT_PHASE)
        , hasTakenActionSpellOrSong_(false)
    {
        sfml_util::SoundManager::Instance()->MusicStart(sfml_util::music::Inventory);
    }

    InventoryStage::~InventoryStage()
    {
        sfml_util::SoundManager::Instance()->MusicStart(sfml_util::music::Inventory);
        Stage::ClearAllEntities();
    }

    bool InventoryStage::HandleCallback(
        const sfml_util::gui::callback::ListBoxEventPackage & PACKAGE)
    {
        if (PACKAGE.package.PTR_ == nullptr)
        {
            return false;
        }

        if ((PACKAGE.gui_event == sfml_util::GuiEvent::Click)
            || (PACKAGE.gui_event == sfml_util::GuiEvent::SelectionChange)
            || (PACKAGE.keypress_event.code == sf::Keyboard::Up)
            || (PACKAGE.keypress_event.code == sf::Keyboard::Down))
        {
            SetDescBoxTextFromListBoxItem(PACKAGE.package.PTR_->Selected());
        }
        else if (
            (PACKAGE.gui_event == sfml_util::GuiEvent::DoubleClick)
            || (PACKAGE.keypress_event.code == sf::Keyboard::Return))
        {
            if (view_ == ViewType::Items)
            {
                if (PACKAGE.package.PTR_ == equippedListBoxUPtr_.get())
                {
                    return HandleUnequipRequest();
                }
                else if (PACKAGE.package.PTR_ == unEquipListBoxUPtr_.get())
                {
                    return HandleEquipRequest();
                }
            }
        }

        return false;
    }

    bool InventoryStage::HandleCallback(
        const sfml_util::gui::callback::FourStateButtonCallbackPackage_t & PACKAGE)
    {
        if (isSliderAnimating_)
        {
            return false;
        }

        if (PACKAGE.PTR_ == backButtonUPtr_.get())
        {
            return HandleBack();
        }

        if (PACKAGE.PTR_ == itemsButtonUPtr_.get())
        {
            return HandleViewChange(ViewType::Items);
        }

        if (PACKAGE.PTR_ == titlesButtonUPtr_.get())
        {
            return HandleViewChange(ViewType::Titles);
        }

        if (PACKAGE.PTR_ == condsButtonUPtr_.get())
        {
            return HandleViewChange(ViewType::Conditions);
        }

        if (PACKAGE.PTR_ == spellsButtonUPtr_.get())
        {
            return HandleSpellsOrSongs();
        }

        if (PACKAGE.PTR_ == giveButtonUPtr_.get())
        {
            return HandleGiveRequestInitial();
        }

        if (PACKAGE.PTR_ == shareButtonUPtr_.get())
        {
            return HandleShare();
        }

        if (PACKAGE.PTR_ == gatherButtonUPtr_.get())
        {
            return HandleGather();
        }

        if (PACKAGE.PTR_ == equipButtonUPtr_.get())
        {
            return HandleEquipActual();
        }

        if (PACKAGE.PTR_ == unequipButtonUPtr_.get())
        {
            return HandleUnequipActual();
        }

        if (PACKAGE.PTR_ == dropButtonUPtr_.get())
        {
            return HandleDropRequest();
        }

        if (PACKAGE.PTR_ == eqSortButtonNameUPtr_.get())
        {
            SortByName(*equippedListBoxUPtr_, isSortReversedEqName_);
            return true;
        }

        if (PACKAGE.PTR_ == eqSortButtonPriceUPtr_.get())
        {
            SortByPrice(*equippedListBoxUPtr_, isSortReversedEqPrice_);
            return true;
        }

        if (PACKAGE.PTR_ == eqSortButtonWeightUPtr_.get())
        {
            SortByWeight(*equippedListBoxUPtr_, isSortReversedEqWeight_);
            return true;
        }

        if (PACKAGE.PTR_ == unEqSortButtonNameUPtr_.get())
        {
            SortByName(*unEquipListBoxUPtr_, isSortReversedUneqName_);
            return true;
        }

        if (PACKAGE.PTR_ == unEqSortButtonPriceUPtr_.get())
        {
            SortByPrice(*unEquipListBoxUPtr_, isSortReversedUneqPrice_);
            return true;
        }

        if (PACKAGE.PTR_ == unEqSortButtonWeightUPtr_.get())
        {
            SortByWeight(*unEquipListBoxUPtr_, isSortReversedUneqWeight_);
            return true;
        }

        return false;
    }

    bool InventoryStage::HandleCallback(const popup::PopupResponse & POPUP_RESPONSE)
    {
        isWaitingOnPopup_ = false;

        if (POPUP_RESPONSE.Info().Name() == POPUP_NAME_SONG_RESULT_)
        {
            return HandleSong_Step2_DisplayResults();
        }
        else if (POPUP_RESPONSE.Info().Name() == POPUP_NAME_SPELL_RESULT_)
        {
            return HandleCast_Step3_DisplayResults();
        }
        else if (
            (POPUP_RESPONSE.Info().Name() == POPUP_NAME_DROPCONFIRM_)
            && (POPUP_RESPONSE.Response() == popup::ResponseTypes::Yes))
        {
            return HandleDropActual();
        }
        else if (
            (POPUP_RESPONSE.Info().Name() == POPUP_NAME_GIVE_)
            && (POPUP_RESPONSE.Response() == popup::ResponseTypes::Select))
        {
            if (POPUP_RESPONSE.Selection() == popup::PopupInfo::ContentNum_Item())
            {
                return HandleGiveRequestItems();
            }
            else if (POPUP_RESPONSE.Selection() == popup::PopupInfo::ContentNum_Coins())
            {
                return HandleGiveRequestCoins();
            }
            else if (POPUP_RESPONSE.Selection() == popup::PopupInfo::ContentNum_Gems())
            {
                return HandleGiveRequestGems();
            }
            else if (POPUP_RESPONSE.Selection() == popup::PopupInfo::ContentNum_MeteorShards())
            {
                return HandleGiveRequestMeteorShards();
            }
        }
        else if (
            (POPUP_RESPONSE.Info().Name() == POPUP_NAME_CHAR_SELECT_)
            && (POPUP_RESPONSE.Response() == popup::ResponseTypes::Select))
        {
            creatureToGiveToPtrOpt_
                = game::Game::Instance()->State().Party().GetAtOrderPos(POPUP_RESPONSE.Selection());

            auto const CREATURE_TO_GIVE_TO_PTR{ creatureToGiveToPtrOpt_.value() };

            if (spellBeingCastPtrOpt_)
            {
                creature::CreaturePVec_t spellTargetCreaturePVec;
                spellTargetCreaturePVec.emplace_back(CREATURE_TO_GIVE_TO_PTR);

                HandleCast_Step2_PerformSpell(
                    spellBeingCastPtrOpt_.value(), spellTargetCreaturePVec);

                return true;
            }

            switch (contentType_)
            {
                case ContentType::Item:
                {
                    return HandleGiveActualItems(CREATURE_TO_GIVE_TO_PTR);
                }
                case ContentType::Coins:
                {
                    return HandleGiveActualCoins(CREATURE_TO_GIVE_TO_PTR);
                }
                case ContentType::Gems:
                {
                    return HandleGiveActualGems(CREATURE_TO_GIVE_TO_PTR);
                }
                case ContentType::MeteorShards:
                {
                    return HandleGiveActualMeteorShards(CREATURE_TO_GIVE_TO_PTR);
                }
                case ContentType::Count:
                default:
                {
                    return true;
                }
            }
        }
        else if (
            (POPUP_RESPONSE.Info().Name() == POPUP_NAME_NUMBER_SELECT_)
            && (POPUP_RESPONSE.Response() == popup::ResponseTypes::Select)
            && creatureToGiveToPtrOpt_)
        {
            auto const CREATURE_TO_GIVE_TO_PTR{ creatureToGiveToPtrOpt_.value() };

            switch (contentType_)
            {
                case ContentType::MeteorShards:
                {
                    HandleMeteorShardsGive(POPUP_RESPONSE.Selection(), CREATURE_TO_GIVE_TO_PTR);
                    return false;
                }
                case ContentType::Coins:
                {
                    HandleCoinsGive(POPUP_RESPONSE.Selection(), CREATURE_TO_GIVE_TO_PTR);
                    return false;
                }
                case ContentType::Gems:
                {
                    HandleGemsGive(POPUP_RESPONSE.Selection(), CREATURE_TO_GIVE_TO_PTR);
                    return false;
                }
                case ContentType::Item:
                case ContentType::Count:
                default:
                {
                    return true;
                }
            }
        }
        else if (
            (POPUP_RESPONSE.Info().Name() == POPUP_NAME_CONTENTSELECTION_)
            && (POPUP_RESPONSE.Response() == popup::ResponseTypes::Select))
        {
            if (POPUP_RESPONSE.Selection() == popup::PopupInfo::ContentNum_Coins())
            {
                if (ActionType::Gather == actionType_)
                {
                    HandleCoinsGather(true);
                }
                else if (ActionType::Share == actionType_)
                {
                    HandleCoinsShare();
                }

                return false;
            }
            else if (POPUP_RESPONSE.Selection() == popup::PopupInfo::ContentNum_Gems())
            {
                if (ActionType::Gather == actionType_)
                {
                    HandleGemsGather(true);
                }
                else if (ActionType::Share == actionType_)
                {
                    HandleGemsShare();
                }

                return false;
            }
            else if (POPUP_RESPONSE.Selection() == popup::PopupInfo::ContentNum_MeteorShards())
            {
                if (ActionType::Gather == actionType_)
                {
                    HandleMeteorShardsGather(true);
                }
                else if (ActionType::Share == actionType_)
                {
                    HandleMeteorShardsShare();
                }

                return false;
            }
        }
        else if (
            (POPUP_RESPONSE.Info().Name() == POPUP_NAME_SPELLBOOK_)
            && (POPUP_RESPONSE.Response() == popup::ResponseTypes::Select))
        {
            const spell::SpellPVec_t SPELLS_PVEC{ creaturePtr_->SpellsPVec() };

            auto const RESPONSE_SELECTION_INDEX{ POPUP_RESPONSE.Selection() };

            M_ASSERT_OR_LOGANDTHROW_SS(
                (RESPONSE_SELECTION_INDEX < SPELLS_PVEC.size()),
                "stage::InventoryStage::HandleCallback(SPELL, selection="
                    << POPUP_RESPONSE.Selection()
                    << ") Selection was greater than SpellPVec.size=" << SPELLS_PVEC.size());

            auto const SPELL_PTR{ SPELLS_PVEC.at(RESPONSE_SELECTION_INDEX) };

            creaturePtr_->LastSpellCastNum(RESPONSE_SELECTION_INDEX);
            return HandleCast_Step1_TargetSelection(SPELL_PTR);
        }
        else if (
            (POPUP_RESPONSE.Info().Name() == POPUP_NAME_MUSICSHEET_)
            && (POPUP_RESPONSE.Response() == popup::ResponseTypes::Select))
        {
            auto const SONGS_PVEC{ creaturePtr_->SongsPVec() };
            auto const RESPONSE_SELECTION_INDEX{ POPUP_RESPONSE.Selection() };

            M_ASSERT_OR_LOGANDTHROW_SS(
                (RESPONSE_SELECTION_INDEX < SONGS_PVEC.size()),
                "stage::InventoryStage::HandleCallback(SONG, selection="
                    << POPUP_RESPONSE.Selection()
                    << ") Selection was greater than SongPVec.size=" << SONGS_PVEC.size());

            creaturePtr_->LastSongPlayedNum(RESPONSE_SELECTION_INDEX);
            return HandleSong_Step1_Play(SONGS_PVEC.at(RESPONSE_SELECTION_INDEX));
        }

        return false;
    }

    void InventoryStage::Setup()
    {
        EntityAdd(ouroborosUPtr_.get());

        Setup_PaperBackground();
        Setup_InstructionText();
        Setup_CreatureImage();
        Setup_CreatureDetails(true);
        Setup_CreatureStats();
        Setup_CenterText();
        Setup_ListBox();
        Setup_DescBox(false);
        Setup_FirstListBoxTitle();
        Setup_DescBoxTitle();
        Setup_MenuButtons();
        Setup_SortButtons();
        Setup_ButtonMouseoverText();
        ForceSelectionAndDrawOfListBox();
    }

    void InventoryStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        target.draw(paperBgSprite_, STATES);
        target.draw(mainMenuTitle_, STATES);
        target.draw(bottomSymbol_, STATES);
        target.draw(creatureSprite_, STATES);
        Stage::Draw(target, STATES);

        // Always draw because it is a fast operation and will
        // be fully transparent when should not be drawn.
        target.draw(detailViewQuads_, STATES);

        if (detailViewTextUPtr_)
        {
            sf::RenderStates statesBlendAdd{ STATES };
            statesBlendAdd.blendMode = sf::BlendAdd;
            target.draw(detailViewSprite_, statesBlendAdd);

            detailViewTextUPtr_->draw(target, STATES);
        }

        auto newBlendModeStates{ STATES };
        newBlendModeStates.blendMode = sf::BlendAdd;

        if (sparkleAnimUPtr_)
        {
            target.draw(*sparkleAnimUPtr_, newBlendModeStates);
        }

        if (songAnimUPtr_)
        {
            target.draw(*songAnimUPtr_, newBlendModeStates);
        }
    }

    bool InventoryStage::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if ((isSliderAnimating_ == false) && (IsDetailViewFadingOrVisible() == false)
            && (game::LoopManager::Instance()->IsFading() == false))
        {
            sfml_util::SoundManager::Instance()->PlaySfx_Keypress();

            if (KEY_EVENT.code == sf::Keyboard::A)
            {
                return HandleAchievementDisplay();
            }

            if ((KEY_EVENT.code == sf::Keyboard::B) || (KEY_EVENT.code == sf::Keyboard::Escape))
            {
                return HandleBack();
            }

            if (KEY_EVENT.code == sf::Keyboard::I)
            {
                return HandleViewChange(ViewType::Items);
            }

            if (KEY_EVENT.code == sf::Keyboard::T)
            {
                return HandleViewChange(ViewType::Titles);
            }

            if (KEY_EVENT.code == sf::Keyboard::C)
            {
                return HandleViewChange(ViewType::Conditions);
            }

            if (KEY_EVENT.code == sf::Keyboard::S)
            {
                return HandleSpellsOrSongs();
            }

            if (KEY_EVENT.code == sf::Keyboard::G)
            {
                return HandleGiveRequestInitial();
            }

            if (KEY_EVENT.code == sf::Keyboard::H)
            {
                return HandleShare();
            }

            if (KEY_EVENT.code == sf::Keyboard::R)
            {
                return HandleGather();
            }

            if (KEY_EVENT.code == sf::Keyboard::E)
            {
                return HandleEquipActual();
            }

            if (KEY_EVENT.code == sf::Keyboard::U)
            {
                return HandleUnequipActual();
            }

            if (KEY_EVENT.code == sf::Keyboard::D)
            {
                return HandleDropRequest();
            }

            if (KEY_EVENT.code == sf::Keyboard::Left)
            {
                return HandlePlayerChangeBeforeOrAfter(false);
            }

            if (KEY_EVENT.code == sf::Keyboard::Right)
            {
                return HandlePlayerChangeBeforeOrAfter(true);
            }

            if (KEY_EVENT.code == sf::Keyboard::Num1)
            {
                return HandlePlayerChangeIndex(0);
            }

            if (KEY_EVENT.code == sf::Keyboard::Num2)
            {
                return HandlePlayerChangeIndex(1);
            }

            if (KEY_EVENT.code == sf::Keyboard::Num3)
            {
                return HandlePlayerChangeIndex(2);
            }

            if (KEY_EVENT.code == sf::Keyboard::Num4)
            {
                return HandlePlayerChangeIndex(3);
            }

            if (KEY_EVENT.code == sf::Keyboard::Num5)
            {
                return HandlePlayerChangeIndex(4);
            }

            if (KEY_EVENT.code == sf::Keyboard::Num6)
            {
                return HandlePlayerChangeIndex(5);
            }
        }

        if (isDetailViewDoneFading_)
        {
            StartDetailViewFadeOutTasks();
        }

        if (isDetailViewDoneFading_)
        {
            StartDetailViewFadeOutTasks();
        }
        else
        {
            detailViewTimerSec_ = 0.0f;
        }

        return Stage::KeyRelease(KEY_EVENT);
    }

    void InventoryStage::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        if (isSliderAnimating_)
        {
            sliderAnimTimerSec_ += ELAPSED_TIME_SECONDS;

            auto const CUT_OFF_TIME_SEC{ 100.0f };

            if ((sliderAnimTimerSec_ > VIEW_CHANGE_BETWEEN_TIME_SEC_)
                && (sliderAnimTimerSec_ < CUT_OFF_TIME_SEC))
            {
                sliderAnimTimerSec_ = 0.0f;

                if (ViewType::Count == viewToChangeTo_)
                {
                    UpdateTime_ViewChangeNone(CUT_OFF_TIME_SEC);
                }
                else
                {
                    UpdateTime_ViewChangeNormal(CUT_OFF_TIME_SEC);
                }
            }

            if (isImageSliding_)
            {
                UpdateTime_SlideCharacterImage(ELAPSED_TIME_SECONDS);
            }

            if (isDetailsSliding_)
            {
                UpdateTime_SlideDetailText(ELAPSED_TIME_SECONDS);
            }

            if (isCenterSliding_)
            {
                UpdateTime_SlideCenterText(ELAPSED_TIME_SECONDS);
            }

            if (isStatsSliding_)
            {
                UpdateTime_SlideStatusText(ELAPSED_TIME_SECONDS);
            }

            if (isListBoxSliding_)
            {
                UpdateTime_SlideListBox(ELAPSED_TIME_SECONDS);
            }

            if (isDescBoxSliding_)
            {
                UpdateTime_SlideDescBox(ELAPSED_TIME_SECONDS);
            }

            if (ViewType::Count == viewToChangeTo_)
            {
                if (isImageSlidingDone_ && isDetailsSlidingDone_ && isCenterSlidingDone_
                    && isStatsSlidingDone_ && isListBoxSlidingDone_ && isDescBoxSlidingDone_)
                {
                    AfterChangingViewTasks();
                }
            }
            else
            {
                if (isListBoxSlidingDone_ && isDescBoxSlidingDone_)
                {
                    AfterChangingViewTasks();
                }
            }
        }
        else
        {
            if (hasMouseMoved_ && (IsDetailViewFadingOrVisible() == false)
                && (game::LoopManager::Instance()->IsFading() == false)
                && GetItemMouseIsOver(mousePosV_) && (false == isWaitingOnPopup_))
            {
                UpdateTime_DetailView(ELAPSED_TIME_SECONDS);
            }

            if (isDetailViewFadingIn_ || isDetailViewFadingOut_)
            {
                UpdateTime_DetailViewFade(ELAPSED_TIME_SECONDS);
            }

            UpdateTime_SparkleAnimation(ELAPSED_TIME_SECONDS);
            UpdateTime_SongAnimation(ELAPSED_TIME_SECONDS);
        }

        Stage::UpdateTime(ELAPSED_TIME_SECONDS);
    }

    void InventoryStage::UpdateMousePos(const sf::Vector2i & NEW_MOUSE_POS)
    {
        auto const NEW_MOUSE_POS_F{ sfml_util::ConvertVector<int, float>(NEW_MOUSE_POS) };
        hasMouseMoved_ = true;
        HandleDetailViewMouseInterrupt(NEW_MOUSE_POS_F);
        Stage::UpdateMousePos(NEW_MOUSE_POS);
    }

    void InventoryStage::UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V)
    {
        HandleDetailViewMouseInterrupt(MOUSE_POS_V);
        Stage::UpdateMouseDown(MOUSE_POS_V);

        if (IfMouseDownIsOnDisabledButtonPopupRejection(backButtonUPtr_, MOUSE_POS_V))
            return;

        if (IfMouseDownIsOnDisabledButtonPopupRejection(itemsButtonUPtr_, MOUSE_POS_V))
            return;

        if (IfMouseDownIsOnDisabledButtonPopupRejection(titlesButtonUPtr_, MOUSE_POS_V))
            return;

        if (IfMouseDownIsOnDisabledButtonPopupRejection(condsButtonUPtr_, MOUSE_POS_V))
            return;

        if (IfMouseDownIsOnDisabledButtonPopupRejection(spellsButtonUPtr_, MOUSE_POS_V))
            return;

        if (IfMouseDownIsOnDisabledButtonPopupRejection(giveButtonUPtr_, MOUSE_POS_V))
            return;

        if (IfMouseDownIsOnDisabledButtonPopupRejection(shareButtonUPtr_, MOUSE_POS_V))
            return;

        if (IfMouseDownIsOnDisabledButtonPopupRejection(gatherButtonUPtr_, MOUSE_POS_V))
            return;

        if (IfMouseDownIsOnDisabledButtonPopupRejection(equipButtonUPtr_, MOUSE_POS_V))
            return;

        if (IfMouseDownIsOnDisabledButtonPopupRejection(unequipButtonUPtr_, MOUSE_POS_V))
            return;

        if (IfMouseDownIsOnDisabledButtonPopupRejection(dropButtonUPtr_, MOUSE_POS_V))
            return;
    }

    void InventoryStage::Setup_PaperBackground()
    {
        sfml_util::LoadTexture(
            paperBgTexture_,
            game::GameDataFile::Instance()->GetMediaPath("media-images-backgrounds-paper-2"));

        paperBgSprite_.setTexture(paperBgTexture_);

        paperBgSprite_.setScale(
            SCREEN_WIDTH_ / static_cast<float>(paperBgTexture_.getSize().x),
            SCREEN_HEIGHT_ / static_cast<float>(paperBgTexture_.getSize().y));

        paperBgSprite_.setPosition(0.0f, 0.0f);
    }

    void InventoryStage::Setup_InstructionText()
    {
        const sfml_util::gui::TextInfo INSTR_TEXT_INFO(
            "(use arrows or numbers to change characters, press 'a' to see achievements)",
            sfml_util::FontManager::Instance()->Font_Default1(),
            sfml_util::FontManager::Instance()->Size_Small(),
            sfml_util::FontManager::Color_GrayDark(),
            sf::BlendAlpha,
            sf::Text::Italic,
            sfml_util::Justified::Left);

        insTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "InventoryStage'sInstruction",
            INSTR_TEXT_INFO,
            sf::FloatRect(0.0f, mainMenuTitle_.Bottom(false) - 10.0f, 0.0f, 0.0f));

        insTextRegionUPtr_->SetEntityPos(
            (SCREEN_WIDTH_ * 0.5f) - (insTextRegionUPtr_->GetEntityRegion().width * 0.5f) + 93.0f,
            insTextRegionUPtr_->GetEntityPos().y);

        EntityAdd(insTextRegionUPtr_.get());
    }

    void InventoryStage::Setup_CreatureImage()
    {
        sfml_util::gui::CreatureImageLoader creatureImageLoader;
        creatureImageLoader.GetImage(creatureTexture_, creaturePtr_);
        sfml_util::Invert(creatureTexture_);
        sfml_util::Mask(creatureTexture_, sf::Color::White);
        creatureSprite_.setTexture(creatureTexture_, true);
        creatureSprite_.setPosition(CREATURE_IMAGE_POS_LEFT_, CREATURE_IMAGE_POS_TOP_);
        creatureSprite_.setColor(sf::Color(255, 255, 255, 127));
        creatureSprite_.setScale(CREATURE_IMAGE_SCALE_, CREATURE_IMAGE_SCALE_);
    }

    void InventoryStage::Setup_CreatureDetails(const bool WILL_UPDATE_POSITION)
    {
        std::ostringstream ss;
        ss << "Character # "
           << game::Game::Instance()->State().Party().GetOrderNum(creaturePtr_) + 1 << "\n"
           << creaturePtr_->Name() << "\n"
           << creaturePtr_->SexName() << "\n"
           << creaturePtr_->RaceName();

        if (creaturePtr_->IsBeast())
        {
            if (creaturePtr_->Race() != creature::race::Wolfen)
            {
                ss << ", " << creaturePtr_->RoleName();
            }

            ss << " " << creaturePtr_->RankClassName() << "\n";
        }
        else
        {
            ss << " " << creaturePtr_->RankClassName() << " " << creaturePtr_->RoleName() << "\n";
        }

        ss << "Rank:  " << creaturePtr_->Rank() << "\n"
           << "Experience: " << creaturePtr_->Exp() << "\n"
           << "Health:  " << creaturePtr_->HealthCurrent() << "/" << creaturePtr_->HealthNormal()
           << " " << creaturePtr_->HealthRatio() << "%\n"
           << "Condition:  " << creaturePtr_->ConditionNames(4) << "\n"
           << "\n";

        const sfml_util::gui::TextInfo DETAILS_TEXT_INFO(
            ss.str(),
            sfml_util::FontManager::Instance()->Font_Default1(),
            sfml_util::FontManager::Instance()->Size_Smallish(),
            sfml_util::FontManager::Color_GrayDarker(),
            sfml_util::Justified::Left);

        sf::FloatRect detailsTextRect(
            CREATURE_IMAGE_POS_LEFT_ + creatureSprite_.getGlobalBounds().width
                + CREATURE_IMAGE_RIGHT_PAD_,
            creatureSprite_.getPosition().y + CREATURE_IMAGE_RIGHT_PAD_,
            0.0f,
            0.0f);

        if ((WILL_UPDATE_POSITION == false) && (detailsTextRegionUPtr_))
        {
            detailsTextRect.left = detailsTextRegionUPtr_->GetEntityPos().x;
        }

        detailsPosLeft_ = detailsTextRect.left;

        if (!detailsTextRegionUPtr_)
        {
            detailsTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "InventoryStage'sDetails", DETAILS_TEXT_INFO, detailsTextRect);

            EntityAdd(detailsTextRegionUPtr_.get());
        }
        else
        {
            detailsTextRegionUPtr_->SetText(ss.str());
        }

        detailsTextRegionUPtr_->SetEntityPos(
            detailsPosLeft_, detailsTextRegionUPtr_->GetEntityPos().y);
    }

    void InventoryStage::Setup_CreatureStats()
    {
        auto const STR_MOD_STR{ creaturePtr_->TraitModifiedString(stats::Traits::Strength, true) };

        auto const ACC_MOD_STR{ creaturePtr_->TraitModifiedString(stats::Traits::Accuracy, true) };

        auto const CHA_MOD_STR{ creaturePtr_->TraitModifiedString(stats::Traits::Charm, true) };

        auto const LCK_MOD_STR{ creaturePtr_->TraitModifiedString(stats::Traits::Luck, true) };

        auto const SPD_MOD_STR{ creaturePtr_->TraitModifiedString(stats::Traits::Speed, true) };

        auto const INT_MOD_STR{ creaturePtr_->TraitModifiedString(
            stats::Traits::Intelligence, true) };

        std::ostringstream ss;
        ss << "Strength:       " << creaturePtr_->Strength() << " " << STR_MOD_STR << "\n"
           << "Accuracy:      " << creaturePtr_->Accuracy() << " " << ACC_MOD_STR << "\n"
           << "Charm:          " << creaturePtr_->Charm() << " " << CHA_MOD_STR << "\n"
           << "Luck:             " << creaturePtr_->Luck() << " " << LCK_MOD_STR << "\n"
           << "Speed:            " << creaturePtr_->Speed() << " " << SPD_MOD_STR << "\n"
           << "Intelligence:   " << creaturePtr_->Intelligence() << " " << INT_MOD_STR << "\n"
           << "\n \n ";

        const sfml_util::gui::TextInfo STATS_TEXT_INFO(
            ss.str(),
            sfml_util::FontManager::Instance()->Font_Typical(),
            sfml_util::FontManager::Instance()->Size_Normal(),
            sfml_util::FontManager::Color_GrayDarker(),
            sfml_util::Justified::Left);

        if (!statsTextRegionUPtr_)
        {
            const sf::FloatRect STATS_TEXT_RECT(
                STATS_POS_LEFT_, mainMenuTitle_.Bottom(false) + 20.0f, 0.0f, 0.0f);

            statsTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "InventoryStage'sStats", STATS_TEXT_INFO, STATS_TEXT_RECT);

            EntityAdd(statsTextRegionUPtr_.get());
        }
        else
        {
            statsTextRegionUPtr_->SetText(ss.str());
        }
    }

    void InventoryStage::Setup_CenterText()
    {
        auto const & INVENTORY{ creaturePtr_->Inventory() };

        std::ostringstream ss;
        ss << "Coins:  " << INVENTORY.Coins() << "\n"
           << "Gems:  " << INVENTORY.Gems() << "\n"
           << "Meteor Shards:  " << INVENTORY.MeteorShards() << "\n"
           << "Mana:  " << creaturePtr_->Mana() << "/" << creaturePtr_->ManaNormal() << "\n"
           << "Weight: " << INVENTORY.Weight() << "/" << creaturePtr_->WeightCanCarry() << "\n"
           << "\n \n ";

        const sfml_util::gui::TextInfo CENTER_TEXT_INFO(
            ss.str(),
            sfml_util::FontManager::Instance()->Font_Typical(),
            sfml_util::FontManager::Instance()->Size_Normal(),
            sfml_util::FontManager::Color_GrayDarker(),
            sfml_util::Justified::Left);

        const bool WAS_ALREADY_INSTANTIATED{ centerTextRegionUPtr_ };

        if (WAS_ALREADY_INSTANTIATED)
        {
            centerTextRegionUPtr_->SetText(ss.str());
        }
        else
        {
            const sf::FloatRect CENTER_TEXT_RECT(
                0.0f, mainMenuTitle_.Bottom(true) + sfml_util::MapByRes(5.0f, 30.0f), 0.0f, 0.0f);

            centerTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "InventoryStage'sCenter", CENTER_TEXT_INFO, CENTER_TEXT_RECT);

            EntityAdd(centerTextRegionUPtr_.get());
        }

        const float POS_LEFT{ ((WAS_ALREADY_INSTANTIATED == false) && creaturePtr_->IsBeast())
                                  ? (SCREEN_WIDTH_ + 1.0f)
                                  : (SCREEN_WIDTH_ * 0.5f) - sfml_util::MapByRes(100.0f, 300.0f) };

        centerTextRegionUPtr_->SetEntityPos(POS_LEFT, centerTextRegionUPtr_->GetEntityPos().y);
        centerPosLeft_ = POS_LEFT;
    }

    void InventoryStage::Setup_ListBox()
    {
        const sfml_util::gui::box::Info LISTBOX_BOX_INFO(
            1, true, LISTBOX_REGION_, LISTBOX_COLORSET_, LISTBOX_BG_INFO_);

        sfml_util::gui::ListBoxItemSVec_t listBoxItemsSVec;

        // establish which view to use
        ViewType viewToUse(viewToChangeTo_);

        if (ViewType::Count == viewToUse)
        {
            viewToUse = view_;
        }

        if (isViewForcedToItems_)
        {
            viewToUse = ViewType::Items;
        }

        switch (viewToUse)
        {
            case ViewType::Conditions:
            {
                for (auto const & NEXT_CONDITION_PTR : creaturePtr_->ConditionsPVec())
                {
                    listBoxItemTextInfo_.text = NEXT_CONDITION_PTR->Name();

                    auto const LISTBOXITEM_SPTR = std::make_shared<sfml_util::gui::ListBoxItem>(
                        NEXT_CONDITION_PTR->Name() + "_ConditionsListBoxEntry",
                        listBoxItemTextInfo_,
                        NEXT_CONDITION_PTR);

                    listBoxItemsSVec.emplace_back(LISTBOXITEM_SPTR);
                }
                break;
            }
            case ViewType::Items:
            {
                for (auto const & NEXT_ITEM_SPTR : creaturePtr_->Inventory().ItemsEquipped())
                {
                    listBoxItemTextInfo_.text = NEXT_ITEM_SPTR->Name();

                    auto const LISTBOXITEM_SPTR = std::make_shared<sfml_util::gui::ListBoxItem>(
                        NEXT_ITEM_SPTR->Name() + "_EquippedItemsListBoxEntry",
                        listBoxItemTextInfo_,
                        NEXT_ITEM_SPTR);

                    listBoxItemsSVec.emplace_back(LISTBOXITEM_SPTR);
                }
                break;
            }
            case ViewType::Spells:
            {
                for (auto const & NEXT_SPELL_PTR : creaturePtr_->SpellsPVec())
                {
                    listBoxItemTextInfo_.text = NEXT_SPELL_PTR->Name();

                    auto const LISTBOXITEM_SPTR = std::make_shared<sfml_util::gui::ListBoxItem>(
                        NEXT_SPELL_PTR->Name() + "_SpellsListBoxEntry",
                        listBoxItemTextInfo_,
                        NEXT_SPELL_PTR);

                    listBoxItemsSVec.emplace_back(LISTBOXITEM_SPTR);
                }
                break;
            }
            case ViewType::Titles:
            case ViewType::Count:
            default:
            {
                for (auto const & NEXT_TITLE_PTR : creaturePtr_->TitlesPVec())
                {
                    listBoxItemTextInfo_.text = NEXT_TITLE_PTR->Name();

                    auto const LISTBOXITEM_SPTR = std::make_shared<sfml_util::gui::ListBoxItem>(
                        NEXT_TITLE_PTR->Name() + "_TitlesListBoxEntry",
                        listBoxItemTextInfo_,
                        NEXT_TITLE_PTR);

                    listBoxItemsSVec.emplace_back(LISTBOXITEM_SPTR);
                }
                break;
            }
        }

        const bool IS_ALREADY_INSTANTIATED{ equippedListBoxUPtr_ };

        if (IS_ALREADY_INSTANTIATED)
        {
            EntityRemove(equippedListBoxUPtr_.get());
        }

        equippedListBoxUPtr_ = std::make_unique<sfml_util::gui::ListBox>(
            "InventoryStage'sLeftListBox",
            LISTBOX_REGION_,
            listBoxItemsSVec,
            sfml_util::IStagePtr_t(this),
            10.0f,
            6.0f,
            LISTBOX_BOX_INFO,
            LISTBOX_COLOR_LINE_,
            sfml_util::gui::callback::IListBoxCallbackHandlerPtr_t(this));

        EntityAdd(equippedListBoxUPtr_.get());

        equippedListBoxUPtr_->SelectedIndex(0);
        equippedListBoxUPtr_->ImageColor(LISTBOX_COLOR_IMAGE_);
        equippedListBoxUPtr_->WillPlaySoundEffects(false);
    }

    void InventoryStage::Setup_DescBox(const bool WILL_MOVE_OFFSCREEN)
    {
        sfml_util::gui::ListBoxItemSVec_t unEquipItemsSVec;
        for (auto const & NEXT_ITEM_SPTR : creaturePtr_->Inventory().Items())
        {
            std::ostringstream itemEntryNameSS;
            itemEntryNameSS << NEXT_ITEM_SPTR->Name() << " "
                            << "UnEquippedItemsListBoxEntry";

            listBoxItemTextInfo_.text = NEXT_ITEM_SPTR->Name();

            auto const EQUIPPED_LISTBOXITEM_SPTR = std::make_shared<sfml_util::gui::ListBoxItem>(
                itemEntryNameSS.str(), listBoxItemTextInfo_, NEXT_ITEM_SPTR);

            unEquipItemsSVec.emplace_back(EQUIPPED_LISTBOXITEM_SPTR);
        }

        const sfml_util::gui::box::Info LISTBOX_BOX_INFO(
            1, true, DESCBOX_REGION_, LISTBOX_COLORSET_, LISTBOX_BG_INFO_);

        const bool IS_EQ_ALREADY_INSTANTIATED{ unEquipListBoxUPtr_ };

        sf::Vector2f origPosV(OUT_OF_SIGHT_POS_, OUT_OF_SIGHT_POS_);

        if (IS_EQ_ALREADY_INSTANTIATED)
        {
            origPosV = unEquipListBoxUPtr_->GetEntityPos();
            EntityRemove(unEquipListBoxUPtr_.get());
        }

        unEquipListBoxUPtr_ = std::make_unique<sfml_util::gui::ListBox>(
            "InventoryStage'sUnEquipped",
            DESCBOX_REGION_,
            unEquipItemsSVec,
            sfml_util::IStagePtr_t(this),
            10.0f,
            6.0f,
            LISTBOX_BOX_INFO,
            LISTBOX_COLOR_LINE_,
            sfml_util::gui::callback::IListBoxCallbackHandlerPtr_t(this));

        EntityAdd(unEquipListBoxUPtr_.get());

        unEquipListBoxUPtr_->SelectedIndex(0);
        unEquipListBoxUPtr_->ImageColor(LISTBOX_COLOR_IMAGE_);
        unEquipListBoxUPtr_->WillPlaySoundEffects(false);

        if (IS_EQ_ALREADY_INSTANTIATED)
        {
            if (WILL_MOVE_OFFSCREEN)
            {
                unEquipListBoxUPtr_->SetEntityPos(
                    SCREEN_WIDTH_ + 1.0f, unEquipListBoxUPtr_->GetEntityPos().y);
            }
            else
            {
                unEquipListBoxUPtr_->SetEntityPos(origPosV.x, origPosV.y);
            }
        }

        if (!descBoxUPtr_)
        {
            descBoxUPtr_ = std::make_unique<sfml_util::gui::box::Box>(
                "InventoryStage'sDesc", LISTBOX_BOX_INFO);

            EntityAdd(descBoxUPtr_.get());
        }

        sfml_util::gui::TextInfo descTextInfo(listBoxItemTextInfo_);
        descTextInfo.color = DESCBOX_TEXT_COLOR_;
        descTextInfo.text = " ";

        const bool IS_DTR_ALREADY_INSTANTIATED(descTextRegionUPtr_);

        if (IS_DTR_ALREADY_INSTANTIATED)
        {
            EntityRemove(descTextRegionUPtr_.get());
        }

        descTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "InventoryStage'sDescription",
            descTextInfo,
            DESCBOX_REGION_,
            sfml_util::gui::TextRegion::DEFAULT_NO_RESIZE_,
            sfml_util::gui::box::Info(),
            DESCBOX_MARGINS_);

        EntityAdd(descTextRegionUPtr_.get());

        descTextRegionUPtr_->SetEntityPos(
            SCREEN_WIDTH_ + 1.0f, descTextRegionUPtr_->GetEntityPos().y);

        descBoxUPtr_->SetEntityPos(SCREEN_WIDTH_ + 1.0f, descBoxUPtr_->GetEntityPos().y);
    }

    void InventoryStage::Setup_MenuButtons()
    {
        Setup_MenuButton(backButtonUPtr_, "(B)ack", 0.085f);
        Setup_MenuButton(itemsButtonUPtr_, "(I)tems", 0.155f);
        Setup_MenuButton(titlesButtonUPtr_, "(T)itles", 0.225f);
        Setup_MenuButton(condsButtonUPtr_, "(C)onditions", 0.34f);
        Setup_MenuButton(spellsButtonUPtr_, "(S)pells", 0.42f);
        Setup_MenuButton(giveButtonUPtr_, "(G)ive", 0.49f);
        Setup_MenuButton(shareButtonUPtr_, "S(h)are", 0.57f);
        Setup_MenuButton(gatherButtonUPtr_, "Gathe(r)", 0.667f);
        Setup_MenuButton(equipButtonUPtr_, "(E)quip", 0.76f);
        Setup_MenuButton(unequipButtonUPtr_, "(U)nequip", 0.865f);
        Setup_MenuButton(dropButtonUPtr_, "(D)rop", 0.94f);
    }

    void InventoryStage::Setup_SortButtons()
    {
        // load images
        sfml_util::LoadTexture(
            sortButtonNameTexture_,
            game::GameDataFile::Instance()->GetMediaPath("media-images-misc-abc"));

        sfml_util::LoadTexture(
            sortButtonPriceTexture_,
            game::GameDataFile::Instance()->GetMediaPath("media-images-misc-money-bag"));

        sfml_util::LoadTexture(
            sortButtonWeightTexture_,
            game::GameDataFile::Instance()->GetMediaPath("media-images-misc-weight"));

        // setup sprites
        sf::Sprite eqNameSprite;
        sf::Sprite eqPriceSprite;
        sf::Sprite eqWeightSprite;
        sf::Sprite unEqNameSprite;
        sf::Sprite unEqPriceSprite;
        sf::Sprite unEqWeightSprite;

        eqNameSprite.setTexture(sortButtonNameTexture_, true);
        eqPriceSprite.setTexture(sortButtonPriceTexture_, true);
        eqWeightSprite.setTexture(sortButtonWeightTexture_, true);
        unEqNameSprite.setTexture(sortButtonNameTexture_, true);
        unEqPriceSprite.setTexture(sortButtonPriceTexture_, true);
        unEqWeightSprite.setTexture(sortButtonWeightTexture_, true);

        eqNameSprite.setScale(SORT_ICON_SCALE_, SORT_ICON_SCALE_);
        eqPriceSprite.setScale(SORT_ICON_SCALE_, SORT_ICON_SCALE_);
        eqWeightSprite.setScale(SORT_ICON_SCALE_, SORT_ICON_SCALE_);
        unEqNameSprite.setScale(SORT_ICON_SCALE_, SORT_ICON_SCALE_);
        unEqPriceSprite.setScale(SORT_ICON_SCALE_, SORT_ICON_SCALE_);
        unEqWeightSprite.setScale(SORT_ICON_SCALE_, SORT_ICON_SCALE_);

        eqNameSprite.setColor(SORT_ICON_COLOR_);
        eqPriceSprite.setColor(SORT_ICON_COLOR_);
        eqWeightSprite.setColor(SORT_ICON_COLOR_);
        unEqNameSprite.setColor(SORT_ICON_COLOR_);
        unEqPriceSprite.setColor(SORT_ICON_COLOR_);
        unEqWeightSprite.setColor(SORT_ICON_COLOR_);

        auto const THREE_ICONS_WIDTH{ eqNameSprite.getGlobalBounds().width
                                      + eqPriceSprite.getGlobalBounds().width
                                      + eqWeightSprite.getGlobalBounds().width };

        auto const POS_HORIZ_NAME_EQ{ ((FIRST_LISTBOX_POS_LEFT_ + LISTBOX_WIDTH_)
                                       - THREE_ICONS_WIDTH)
                                      - (SORT_ICON_SPACER_ * 2.25f) };

        auto const POS_HORIZ_NAME_UNEQ{ ((SECOND_LISTBOX_POS_LEFT_ + LISTBOX_WIDTH_)
                                         - THREE_ICONS_WIDTH)
                                        - (SORT_ICON_SPACER_ * 2.25f) };

        auto const POS_VERT_NAME{ SORT_ICON_POS_TOP_
                                  - (eqNameSprite.getGlobalBounds().height * 0.5f) };

        auto const POS_HORIZ_PRICE_EQ{ POS_HORIZ_NAME_EQ + eqNameSprite.getGlobalBounds().width
                                       + SORT_ICON_SPACER_ };

        auto const POS_HORIZ_PRICE_UNEQ{
            POS_HORIZ_NAME_UNEQ + unEqNameSprite.getGlobalBounds().width + SORT_ICON_SPACER_
        };

        auto const POS_VERT_PRICE{ SORT_ICON_POS_TOP_
                                   - (eqPriceSprite.getGlobalBounds().height * 0.5f) };

        auto const POS_HORIZ_WEIGHT_EQ{ POS_HORIZ_PRICE_EQ + eqPriceSprite.getGlobalBounds().width
                                        + SORT_ICON_SPACER_ };

        auto const POS_HORIZ_WEIGHT_UNEQ{
            POS_HORIZ_PRICE_UNEQ + unEqPriceSprite.getGlobalBounds().width + SORT_ICON_SPACER_
        };

        auto const POS_VERT_WEIGHT{ SORT_ICON_POS_TOP_
                                    - (eqWeightSprite.getGlobalBounds().height * 0.5f) };

        eqNameSprite.setPosition(POS_HORIZ_NAME_EQ, POS_VERT_NAME);
        eqPriceSprite.setPosition(POS_HORIZ_PRICE_EQ, POS_VERT_PRICE);
        eqWeightSprite.setPosition(POS_HORIZ_WEIGHT_EQ, POS_VERT_WEIGHT);
        unEqNameSprite.setPosition(POS_HORIZ_NAME_UNEQ, POS_VERT_NAME);
        unEqPriceSprite.setPosition(POS_HORIZ_PRICE_UNEQ, POS_VERT_PRICE);
        unEqWeightSprite.setPosition(POS_HORIZ_WEIGHT_UNEQ, POS_VERT_WEIGHT);

        // setup actual buttons
        Setup_SortButton(
            "InventoryStage's_EquippedListbox_SortByName_", eqSortButtonNameUPtr_, eqNameSprite);

        Setup_SortButton(
            "InventoryStage's_EquippedListbox_SortByPrice_", eqSortButtonPriceUPtr_, eqPriceSprite);

        Setup_SortButton(
            "InventoryStage's_EquippedListbox_SortByWeight_",
            eqSortButtonWeightUPtr_,
            eqWeightSprite);

        Setup_SortButton(
            "InventoryStage's_UnequippedListbox_SortByName_",
            unEqSortButtonNameUPtr_,
            unEqNameSprite);

        Setup_SortButton(
            "InventoryStage's_UnequippedListbox_SortByPrice_",
            unEqSortButtonPriceUPtr_,
            unEqPriceSprite);

        Setup_SortButton(
            "InventoryStage's_UnequippedListbox_SortByWeight_",
            unEqSortButtonWeightUPtr_,
            unEqWeightSprite);
    }

    void InventoryStage::Setup_SortButton(
        const std::string & NAME,
        sfml_util::gui::FourStateButtonUPtr_t & sortButtonUPtr,
        sf::Sprite & sprite)
    {
        sortButtonUPtr = std::make_unique<sfml_util::gui::FourStateButton>(
            NAME, sprite.getPosition().x, sprite.getPosition().y, *sprite.getTexture(), true);

        sortButtonUPtr->Color(sprite.getColor());
        sortButtonUPtr->Scale(sprite.getScale().x);
        sortButtonUPtr->SetCallbackHandler(this);

        EntityAdd(sortButtonUPtr.get());
    }

    void InventoryStage::Setup_FirstListBoxTitle()
    {
        std::string titleText("");
        switch (view_)
        {
            case ViewType::Conditions:
            {
                titleText = "Conditions";
                break;
            }
            case ViewType::Items:
            {
                titleText = "Equipped Items";
                break;
            }
            case ViewType::Spells:
            {
                titleText = "Spells";
                break;
            }
            case ViewType::Titles:
            case ViewType::Count:
            default:
            {
                titleText = "Titles";
                break;
            }
        }

        const sfml_util::gui::TextInfo LISTBOX_TEXT_INFO(
            titleText,
            sfml_util::FontManager::Instance()->Font_Default2(),
            sfml_util::FontManager::Instance()->Size_Normal(),
            LISTBOX_COLOR_TITLE_,
            sfml_util::Justified::Center);

        const sf::FloatRect LISTBOX_TEXT_RECT(FIRST_LISTBOX_POS_LEFT_, 0.0f, LISTBOX_WIDTH_, 0.0f);

        if (!eqTitleTextRegionUPtr_)
        {
            eqTitleTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "InventoryStage'sEquippedListBoxTitle", LISTBOX_TEXT_INFO, LISTBOX_TEXT_RECT);

            EntityAdd(eqTitleTextRegionUPtr_.get());
        }
        else
        {
            eqTitleTextRegionUPtr_->Setup(LISTBOX_TEXT_INFO, LISTBOX_TEXT_RECT);
        }

        eqTitleTextRegionUPtr_->SetEntityPos(
            eqTitleTextRegionUPtr_->GetEntityPos().x,
            LISTBOX_POS_TOP_ - eqTitleTextRegionUPtr_->GetEntityRegion().height);
    }

    void InventoryStage::Setup_DescBoxTitle()
    {
        auto const TITLETEXT{ (ViewType::Items == view_) ? "Unequipped Items" : "Description" };

        const sfml_util::gui::TextInfo DESC_TEXT_INFO(
            TITLETEXT,
            sfml_util::FontManager::Instance()->Font_Default2(),
            sfml_util::FontManager::Instance()->Size_Normal(),
            LISTBOX_COLOR_TITLE_,
            sfml_util::Justified::Center);

        const sf::FloatRect DESC_TEXT_RECT(SECOND_LISTBOX_POS_LEFT_, 0.0f, LISTBOX_WIDTH_, 0.0f);

        if (!unEqTitleTextRegionUPtr_)
        {
            unEqTitleTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "InventoryStage'sUnequippedListBoxTitle", DESC_TEXT_INFO, DESC_TEXT_RECT);

            EntityAdd(unEqTitleTextRegionUPtr_.get());
        }
        else
        {
            unEqTitleTextRegionUPtr_->Setup(DESC_TEXT_INFO, DESC_TEXT_RECT);
        }

        unEqTitleTextRegionUPtr_->SetEntityPos(
            unEqTitleTextRegionUPtr_->GetEntityPos().x,
            LISTBOX_POS_TOP_ - unEqTitleTextRegionUPtr_->GetEntityRegion().height);
    }

    void InventoryStage::Setup_MenuButton(
        sfml_util::gui::FourStateButtonUPtr_t & buttonUPtr,
        const std::string & TEXT,
        const float HORIZ_OFFSET_MULT)
    {
        sfml_util::gui::TextInfo textInfo(
            TEXT,
            sfml_util::FontManager::Instance()->Font_Default2(),
            sfml_util::FontManager::Instance()->Size_Largeish(),
            sf::Color::Black,
            sfml_util::Justified::Left);

        auto const COLOR_DISABLED{ sf::Color(0, 0, 0, 180) };
        auto const COLOR_OVER{ sfml_util::FontManager::Color_GrayDarker() };
        auto const COLOR_DOWN{ COLOR_DISABLED };

        sfml_util::gui::TextInfo textInfoDisabled(textInfo);
        textInfoDisabled.color = COLOR_DISABLED;
        textInfoDisabled.text = TEXT;

        const sfml_util::gui::MouseTextInfo MOUSE_TEXT_INFO{ textInfo, COLOR_DOWN, COLOR_OVER };

        buttonUPtr = std::make_unique<sfml_util::gui::FourStateButton>(
            "InventoryStage's" + TEXT,
            0.0f,
            0.0f,
            "",
            "",
            "",
            "",
            MOUSE_TEXT_INFO,
            textInfoDisabled);

        buttonUPtr->SetEntityPos(
            (INNER_RECT_.left + (HORIZ_OFFSET_MULT * INNER_RECT_.width))
                - buttonUPtr->GetEntityRegion().width,
            bottomSymbol_.Middle() - (buttonUPtr->GetEntityRegion().height * 0.5f));

        buttonUPtr->SetCallbackHandler(this);
        EntityAdd(buttonUPtr.get());
    }

    void InventoryStage::Setup_ButtonMouseoverText()
    {
        backButtonUPtr_->SetIsDisabled(false);

        backButtonUPtr_->SetMouseHoverText("Click here or press 'b' to return to previous screen.");

        if (ViewType::Items == view_)
        {
            itemsButtonUPtr_->SetIsDisabled(true);
            itemsButtonUPtr_->SetMouseHoverText("Already viewing items.");
        }
        else
        {
            itemsButtonUPtr_->SetIsDisabled(false);
            itemsButtonUPtr_->SetMouseHoverText("Click here or press 'i' to view items.");
        }

        if (ViewType::Spells == view_)
        {
            auto const ROLE_ENUM{ creaturePtr_->Role() };
            if (ROLE_ENUM == creature::role::Bard)
            {
                spellsButtonUPtr_->SetText("(S)ongs");
                spellsButtonUPtr_->SetIsDisabled(true);

                spellsButtonUPtr_->SetMouseHoverText("Already viewing songs.");
            }
            else if (
                (ROLE_ENUM == creature::role::Cleric) || (ROLE_ENUM == creature::role::Sorcerer))
            {
                spellsButtonUPtr_->SetText("(S)pells");
                spellsButtonUPtr_->SetIsDisabled(true);

                spellsButtonUPtr_->SetMouseHoverText("Already viewing spells.");
            }
        }
        else
        {
            auto const ROLE_ENUM{ creaturePtr_->Role() };
            if (ROLE_ENUM == creature::role::Bard)
            {
                spellsButtonUPtr_->SetText("(S)ongs");
                spellsButtonUPtr_->SetIsDisabled(false);

                spellsButtonUPtr_->SetMouseHoverText(
                    "Click here or press 's' to view and play magical songs.");
            }
            else if (
                (ROLE_ENUM == creature::role::Cleric) || (ROLE_ENUM == creature::role::Sorcerer))
            {
                spellsButtonUPtr_->SetText("(S)pells");
                spellsButtonUPtr_->SetIsDisabled(false);

                spellsButtonUPtr_->SetMouseHoverText(
                    "Click here or press 's' to view and cast spells.");
            }
        }

        if (creaturePtr_->Titles().empty())
        {
            titlesButtonUPtr_->SetIsDisabled(true);

            titlesButtonUPtr_->SetMouseHoverText(
                creaturePtr_->Name() + " does not have any titles.");
        }
        else if (ViewType::Titles == view_)
        {
            titlesButtonUPtr_->SetIsDisabled(true);
            titlesButtonUPtr_->SetMouseHoverText("Already viewing titles.");
        }
        else
        {
            titlesButtonUPtr_->SetIsDisabled(false);
            titlesButtonUPtr_->SetMouseHoverText("Click here or press 't' to view titles.");
        }

        if (ViewType::Conditions == view_)
        {
            condsButtonUPtr_->SetIsDisabled(true);
            condsButtonUPtr_->SetMouseHoverText("Already viewing conditions.");
        }
        else
        {
            condsButtonUPtr_->SetIsDisabled(false);
            condsButtonUPtr_->SetMouseHoverText("Click here or press 'c' to view conditions.");
        }

        if (creaturePtr_->IsBeast())
        {
            giveButtonUPtr_->SetIsDisabled(true);
            shareButtonUPtr_->SetIsDisabled(true);
            gatherButtonUPtr_->SetIsDisabled(true);

            const std::string DENY_MSG{
                "Beasts cannot carry items, coins, gems, or Meteor Shards."
            };

            giveButtonUPtr_->SetMouseHoverText(DENY_MSG);
            shareButtonUPtr_->SetMouseHoverText(DENY_MSG);
            gatherButtonUPtr_->SetMouseHoverText(DENY_MSG);
        }
        else
        {
            giveButtonUPtr_->SetIsDisabled(false);
            shareButtonUPtr_->SetIsDisabled(false);
            gatherButtonUPtr_->SetIsDisabled(false);

            giveButtonUPtr_->SetMouseHoverText(
                "Click here or press 'g' to give something to another character.");

            shareButtonUPtr_->SetMouseHoverText(
                "Click here or press 'h' to share something between all characters.");

            gatherButtonUPtr_->SetMouseHoverText(
                "Click here or press 'r' to gather all of something from all other characters.");
        }

        if (ViewType::Items == view_)
        {
            auto selectedItemSPtr{ unEquipListBoxUPtr_->Selected() };

            if (selectedItemSPtr && selectedItemSPtr->ITEM_PTR_OPT
                && (selectedItemSPtr->ITEM_PTR_OPT->Obj().Name().empty() == false))
            {
                equipButtonUPtr_->SetIsDisabled(false);

                equipButtonUPtr_->SetMouseHoverText(
                    "Click here or press 'e' to equip the "
                    + unEquipListBoxUPtr_->Selected()->ITEM_PTR_OPT->Obj().Name() + ".");

                dropButtonUPtr_->SetIsDisabled(false);

                dropButtonUPtr_->SetMouseHoverText(
                    "(Click here or press 'd' to drop the "
                    + unEquipListBoxUPtr_->Selected()->ITEM_PTR_OPT->Obj().Name() + ")");
            }
            else
            {
                equipButtonUPtr_->SetIsDisabled(true);

                equipButtonUPtr_->SetMouseHoverText(
                    "There is no unequipped item selected to equip.");

                dropButtonUPtr_->SetIsDisabled(true);

                dropButtonUPtr_->SetMouseHoverText("There is no unequipped item selected to drop.");
            }
        }
        else if (creaturePtr_->IsBeast())
        {
            equipButtonUPtr_->SetIsDisabled(true);
            equipButtonUPtr_->SetMouseHoverText("Beasts cannot carry, equip, or drop items.");
            dropButtonUPtr_->SetIsDisabled(true);
            dropButtonUPtr_->SetMouseHoverText("Beasts cannot carry, equip, or drop items.");
        }
        else
        {
            equipButtonUPtr_->SetIsDisabled(true);

            equipButtonUPtr_->SetMouseHoverText("Click 'Item' or press 'i' to switch to the items "
                                                "view if you want to equip an item.");

            dropButtonUPtr_->SetIsDisabled(true);

            dropButtonUPtr_->SetMouseHoverText("Click 'Item' or press 'i' to switch to the items "
                                               "view if you want to drop an item.");
        }

        if (ViewType::Items == view_)
        {
            if ((equippedListBoxUPtr_->Selected().get() != nullptr)
                && equippedListBoxUPtr_->Selected()->ITEM_PTR_OPT
                && (equippedListBoxUPtr_->Selected()->ITEM_PTR_OPT->Obj().Name().empty() == false))
            {
                unequipButtonUPtr_->SetIsDisabled(false);

                unequipButtonUPtr_->SetMouseHoverText(
                    "Click here or press 'u' to unequip the "
                    + equippedListBoxUPtr_->Selected()->ITEM_PTR_OPT->Obj().Name() + ".");
            }
            else
            {
                unequipButtonUPtr_->SetIsDisabled(true);
                unequipButtonUPtr_->SetMouseHoverText("There is no item to unequip.");
            }
        }
        else if (creaturePtr_->IsBeast())
        {
            unequipButtonUPtr_->SetIsDisabled(true);
            unequipButtonUPtr_->SetMouseHoverText("Beasts cannot carry, equip, or drop items.");
        }
        else
        {
            unequipButtonUPtr_->SetIsDisabled(true);

            unequipButtonUPtr_->SetMouseHoverText("Click 'Item' or press 'i' to switch to the "
                                                  "items view if you want to unequip an item.");
        }
    }

    void InventoryStage::UpdateTime_ViewChangeNone(const float CUT_OFF_TIME_SEC)
    {
        if (isSlidingLeft_)
        {
            if ((false == isImageSlidingDone_) && (false == isImageSliding_))
            {
                isImageSliding_ = true;
            }

            if ((false == isDetailsSlidingDone_) && (false == isDetailsSliding_))
            {
                isDetailsSliding_ = true;
            }

            if ((false == isCenterSlidingDone_) && (false == isCenterSliding_))
            {
                isCenterSliding_ = true;
            }

            if ((false == isListBoxSlidingDone_) && (false == isListBoxSliding_))
            {
                isListBoxSliding_ = true;
            }

            if ((false == isStatsSlidingDone_) && (false == isStatsSliding_))
            {
                isStatsSliding_ = true;
                isDescBoxSliding_ = true;
                sliderAnimTimerSec_ = CUT_OFF_TIME_SEC + 1.0f;
            }
        }
        else
        {
            if ((false == isStatsSlidingDone_) && (false == isStatsSliding_))
            {
                isStatsSliding_ = true;
                isDescBoxSliding_ = true;
            }

            if ((false == isCenterSlidingDone_) && (false == isCenterSliding_))
            {
                isCenterSliding_ = true;
            }

            if ((false == isDetailsSlidingDone_) && (false == isDetailsSliding_))
            {
                isDetailsSliding_ = true;
            }

            if ((false == isListBoxSlidingDone_) && (false == isListBoxSliding_))
            {
                isListBoxSliding_ = true;
            }

            if ((false == isImageSlidingDone_) && (false == isImageSliding_))
            {
                isImageSliding_ = true;
                sliderAnimTimerSec_ = CUT_OFF_TIME_SEC + 1.0f;
            }
        }
    }

    void InventoryStage::AfterChangingViewTasks()
    {
        isSliderAnimating_ = false;

        if (ViewType::Count != viewToChangeTo_)
        {
            view_ = viewToChangeTo_;
            viewToChangeTo_ = ViewType::Count;
        }

        if (isViewForcedToItems_)
        {
            view_ = ViewType::Items;
            isViewForcedToItems_ = false;
        }

        characterViewMap_[game::Game::Instance()->State().Party().GetOrderNum(creaturePtr_)]
            = view_;

        if (ViewType::Items != view_)
        {
            SetDescBoxTextFromListBoxItem(equippedListBoxUPtr_->Selected());
        }

        Setup_FirstListBoxTitle();
        Setup_DescBoxTitle();
        Setup_ButtonMouseoverText();
        ForceSelectionAndDrawOfListBox();
    }

    bool InventoryStage::HandleAchievementDisplay()
    {
        isAchievementDisplaying_ = true;
        const float HALF_SIZE(10.0f);

        detailViewSourceRect_ = sf::FloatRect(
            (SCREEN_WIDTH_ * 0.5f) - HALF_SIZE,
            (SCREEN_HEIGHT_ * 0.5f) - HALF_SIZE,
            (HALF_SIZE * 2.0f),
            (HALF_SIZE * 2.0f));

        isDetailViewFadingIn_ = true;
        detailViewTimerSec_ = 0.0f;
        detailViewSlider_.Reset(DETAILVIEW_SLIDER_SPEED_);
        return true;
    }

    bool InventoryStage::HandleViewChange(const ViewType NEW_VIEW)
    {
        if ((NEW_VIEW == ViewType::Items)
            && ((ViewType::Items == view_) || itemsButtonUPtr_->IsDisabled()))
        {
            return false;
        }

        if ((NEW_VIEW == ViewType::Titles)
            && ((ViewType::Titles == view_) || titlesButtonUPtr_->IsDisabled()))
        {
            return false;
        }

        if ((NEW_VIEW == ViewType::Conditions)
            && ((ViewType::Conditions == view_) || condsButtonUPtr_->IsDisabled()))
        {
            return false;
        }

        viewToChangeTo_ = NEW_VIEW;
        StartSlidingAnimation(true);
        return true;
    }

    void InventoryStage::UpdateTime_ViewChangeNormal(const float CUT_OFF_TIME_SEC)
    {
        if ((false == isListBoxSlidingDone_) && (false == isListBoxSliding_))
        {
            isListBoxSliding_ = true;
        }
        else if ((false == isDescBoxSlidingDone_) && (false == isDescBoxSliding_))
        {
            isDescBoxSliding_ = true;
            sliderAnimTimerSec_ = CUT_OFF_TIME_SEC + 1.0f;
        }
    }

    void InventoryStage::UpdateTime_SlideCharacterImage(const float ELAPSED_TIME_SECONDS)
    {
        if (false == hasImageChanged_)
        {
            const float RATIO(imageSlider_.Update(ELAPSED_TIME_SECONDS));

            if (isSlidingLeft_)
            {
                creatureSprite_.setPosition(
                    CREATURE_IMAGE_POS_LEFT_ + (OUT_OF_SIGHT_POS_ * RATIO),
                    creatureSprite_.getPosition().y);
            }
            else
            {
                creatureSprite_.setPosition(
                    CREATURE_IMAGE_POS_LEFT_ + (SCREEN_WIDTH_ * RATIO),
                    creatureSprite_.getPosition().y);
            }

            if (imageSlider_.IsDone())
            {
                Setup_CreatureImage();

                creatureSprite_.setPosition(OUT_OF_SIGHT_POS_, creatureSprite_.getPosition().y);

                hasImageChanged_ = true;
                imageSlider_.Reset(VIEW_CHANGE_SLIDER_SPEED_);
            }
        }
        else
        {
            auto const RATIO{ (1.0f - imageSlider_.Update(ELAPSED_TIME_SECONDS)) };

            if (isSlidingLeft_)
            {
                creatureSprite_.setPosition(
                    CREATURE_IMAGE_POS_LEFT_ + (SCREEN_WIDTH_ * RATIO),
                    creatureSprite_.getPosition().y);
            }
            else
            {
                auto const NEW_POS_LEFT{ CREATURE_IMAGE_POS_LEFT_ + (OUT_OF_SIGHT_POS_ * RATIO) };

                creatureSprite_.setPosition(NEW_POS_LEFT, creatureSprite_.getPosition().y);

                // sometimes the new creature image is wider than the last,
                // so the details text needs to move with the new image to avoid overlap
                auto const SPRITE_WIDTH{ UpdateImageDetailsPosition() };

                auto const NEW_DETAILSTEXT_POS_LEFT_EDGE{ (
                    NEW_POS_LEFT + SPRITE_WIDTH + CREATURE_IMAGE_RIGHT_PAD_) };

                if (NEW_DETAILSTEXT_POS_LEFT_EDGE > detailsTextRegionUPtr_->GetEntityPos().x)
                {
                    detailsPosLeft_ = NEW_DETAILSTEXT_POS_LEFT_EDGE;

                    detailsTextRegionUPtr_->SetEntityPos(
                        NEW_DETAILSTEXT_POS_LEFT_EDGE, detailsTextRegionUPtr_->GetEntityPos().y);
                }
            }

            if (imageSlider_.IsDone())
            {
                isImageSliding_ = false;
                isImageSlidingDone_ = true;
                hasImageChanged_ = false;
            }
        }
    }

    void InventoryStage::UpdateTime_SlideDetailText(const float ELAPSED_TIME_SECONDS)
    {
        if (false == hasDetailsChanged_)
        {
            auto const RATIO{ detailsSlider_.Update(ELAPSED_TIME_SECONDS) };

            if (isSlidingLeft_)
            {
                detailsTextRegionUPtr_->SetEntityPos(
                    detailsPosLeft_ + ((OUT_OF_SIGHT_POS_ - (SCREEN_WIDTH_ * 0.5f)) * RATIO),
                    detailsTextRegionUPtr_->GetEntityPos().y);
            }
            else
            {
                detailsTextRegionUPtr_->SetEntityPos(
                    detailsPosLeft_ + (SCREEN_WIDTH_ * RATIO),
                    detailsTextRegionUPtr_->GetEntityPos().y);
            }

            if (detailsSlider_.IsDone())
            {
                UpdateImageDetailsPosition();
                Setup_CreatureDetails(true);

                detailsTextRegionUPtr_->SetEntityPos(
                    SCREEN_WIDTH_ + 1.0f, detailsTextRegionUPtr_->GetEntityPos().y);

                hasDetailsChanged_ = true;
                detailsSlider_.Reset(VIEW_CHANGE_SLIDER_SPEED_);
            }
        }
        else
        {
            auto const RATIO{ (1.0f - detailsSlider_.Update(ELAPSED_TIME_SECONDS)) };

            if (isSlidingLeft_)
            {
                detailsTextRegionUPtr_->SetEntityPos(
                    detailsPosLeft_ + (SCREEN_WIDTH_ * RATIO),
                    detailsTextRegionUPtr_->GetEntityPos().y);
            }
            else
            {
                detailsTextRegionUPtr_->SetEntityPos(
                    detailsPosLeft_ + ((OUT_OF_SIGHT_POS_ - (SCREEN_WIDTH_ * 0.5f)) * RATIO),
                    detailsTextRegionUPtr_->GetEntityPos().y);
            }

            if (detailsSlider_.IsDone())
            {
                isDetailsSliding_ = false;
                isDetailsSlidingDone_ = true;
                hasDetailsChanged_ = false;
            }
        }
    }

    void InventoryStage::UpdateTime_SlideCenterText(const float ELAPSED_TIME_SECONDS)
    {
        if (false == hasCenterChanged_)
        {
            auto const RATIO{ centerSlider_.Update(ELAPSED_TIME_SECONDS) };

            if (centerTextRegionUPtr_->GetEntityPos().x < SCREEN_WIDTH_)
            {
                if (isSlidingLeft_)
                {
                    centerTextRegionUPtr_->SetEntityPos(
                        centerPosLeft_ + ((-1.0f * SCREEN_WIDTH_) * RATIO),
                        centerTextRegionUPtr_->GetEntityPos().y);
                }
                else
                {
                    centerTextRegionUPtr_->SetEntityPos(
                        centerPosLeft_ + (SCREEN_WIDTH_ * RATIO),
                        centerTextRegionUPtr_->GetEntityPos().y);
                }
            }

            if (centerSlider_.IsDone())
            {
                Setup_CenterText();

                centerTextRegionUPtr_->SetEntityPos(
                    SCREEN_WIDTH_ + 1.0f, centerTextRegionUPtr_->GetEntityPos().y);

                hasCenterChanged_ = true;
                centerSlider_.Reset(VIEW_CHANGE_SLIDER_SPEED_);
            }
        }
        else
        {
            if (creaturePtr_->IsBeast() == false)
            {
                auto const RATIO{ (1.0f - centerSlider_.Update(ELAPSED_TIME_SECONDS)) };

                if (isSlidingLeft_)
                {
                    centerTextRegionUPtr_->SetEntityPos(
                        centerPosLeft_ + (SCREEN_WIDTH_ * RATIO),
                        centerTextRegionUPtr_->GetEntityPos().y);
                }
                else
                {
                    centerTextRegionUPtr_->SetEntityPos(
                        centerPosLeft_ + ((-1.0f * SCREEN_WIDTH_) * RATIO),
                        centerTextRegionUPtr_->GetEntityPos().y);
                }
            }

            if (centerSlider_.IsDone() || creaturePtr_->IsBeast())
            {
                isCenterSliding_ = false;
                isCenterSlidingDone_ = true;
                hasCenterChanged_ = false;
            }
        }
    }

    void InventoryStage::UpdateTime_SlideStatusText(const float ELAPSED_TIME_SECONDS)
    {
        if (false == hasStatsChanged_)
        {
            auto const RATIO{ statsSlider_.Update(ELAPSED_TIME_SECONDS) };

            if (isSlidingLeft_)
            {
                statsTextRegionUPtr_->SetEntityPos(
                    STATS_POS_LEFT_ + ((-1.0f * SCREEN_WIDTH_) * RATIO),
                    statsTextRegionUPtr_->GetEntityPos().y);
            }
            else
            {
                statsTextRegionUPtr_->SetEntityPos(
                    STATS_POS_LEFT_ + ((SCREEN_WIDTH_ * 0.5f) * RATIO),
                    statsTextRegionUPtr_->GetEntityPos().y);
            }

            if (statsSlider_.IsDone())
            {
                Setup_CreatureStats();

                statsTextRegionUPtr_->SetEntityPos(
                    OUT_OF_SIGHT_POS_, statsTextRegionUPtr_->GetEntityPos().y);

                hasStatsChanged_ = true;
                statsSlider_.Reset(VIEW_CHANGE_SLIDER_SPEED_);
            }
        }
        else
        {
            auto const RATIO{ (1.0f - statsSlider_.Update(ELAPSED_TIME_SECONDS)) };

            if (isSlidingLeft_)
            {
                statsTextRegionUPtr_->SetEntityPos(
                    STATS_POS_LEFT_ + ((SCREEN_WIDTH_ * 0.5f) * RATIO),
                    statsTextRegionUPtr_->GetEntityPos().y);
            }
            else
            {
                statsTextRegionUPtr_->SetEntityPos(
                    STATS_POS_LEFT_ + ((-1.0f * SCREEN_WIDTH_) * RATIO),
                    statsTextRegionUPtr_->GetEntityPos().y);
            }

            if (statsSlider_.IsDone())
            {
                isStatsSliding_ = false;
                isStatsSlidingDone_ = true;
                hasStatsChanged_ = false;
            }
        }
    }

    void InventoryStage::UpdateTime_SlideListBox(const float ELAPSED_TIME_SECONDS)
    {
        if (false == hasListBoxChanged_)
        {
            auto const RATIO{ listBoxSlider_.Update(ELAPSED_TIME_SECONDS) };

            if (isSlidingLeft_)
            {
                equippedListBoxUPtr_->SetEntityPos(
                    FIRST_LISTBOX_POS_LEFT_ + ((OUT_OF_SIGHT_POS_ * 2.0f) * RATIO),
                    equippedListBoxUPtr_->GetEntityPos().y);
            }
            else
            {
                equippedListBoxUPtr_->SetEntityPos(
                    FIRST_LISTBOX_POS_LEFT_ + (SCREEN_WIDTH_ * RATIO),
                    equippedListBoxUPtr_->GetEntityPos().y);
            }

            if (listBoxSlider_.IsDone())
            {
                Setup_ListBox();

                equippedListBoxUPtr_->SetEntityPos(
                    SCREEN_WIDTH_ + 1.0f, equippedListBoxUPtr_->GetEntityPos().y);

                hasListBoxChanged_ = true;
                listBoxSlider_.Reset(VIEW_CHANGE_SLIDER_SPEED_);
            }
        }
        else
        {
            auto const RATIO{ (1.0f - listBoxSlider_.Update(ELAPSED_TIME_SECONDS)) };

            if (isSlidingLeft_)
            {
                equippedListBoxUPtr_->SetEntityPos(
                    FIRST_LISTBOX_POS_LEFT_ + (SCREEN_WIDTH_ * RATIO),
                    equippedListBoxUPtr_->GetEntityPos().y);
            }
            else
            {
                equippedListBoxUPtr_->SetEntityPos(
                    FIRST_LISTBOX_POS_LEFT_ + ((OUT_OF_SIGHT_POS_ * 2.0f) * RATIO),
                    equippedListBoxUPtr_->GetEntityPos().y);
            }

            if (listBoxSlider_.IsDone())
            {
                isListBoxSliding_ = false;
                isListBoxSlidingDone_ = true;
                hasListBoxChanged_ = false;
            }
        }
    }

    void InventoryStage::UpdateTime_SlideDescBox(const float ELAPSED_TIME_SECONDS)
    {
        if (false == hasDescBoxChanged_)
        {
            auto const RATIO{ descBoxSlider_.Update(ELAPSED_TIME_SECONDS) };

            auto const POS_LEFT_SLIDING_LEFT{
                SECOND_LISTBOX_POS_LEFT_ + (((OUT_OF_SIGHT_POS_ * 2.0f) - SCREEN_WIDTH_) * RATIO)
            };

            auto const POS_LEFT_SLIDING_RIGHT{ SECOND_LISTBOX_POS_LEFT_
                                               + ((SCREEN_WIDTH_ * 0.75f) * RATIO) };

            if (ViewType::Items == view_)
            {
                if (isSlidingLeft_)
                {
                    unEquipListBoxUPtr_->SetEntityPos(
                        POS_LEFT_SLIDING_LEFT, unEquipListBoxUPtr_->GetEntityPos().y);
                }
                else
                {
                    unEquipListBoxUPtr_->SetEntityPos(
                        POS_LEFT_SLIDING_RIGHT, unEquipListBoxUPtr_->GetEntityPos().y);
                }
            }
            else
            {
                if (isSlidingLeft_)
                {
                    descTextRegionUPtr_->SetEntityPos(
                        POS_LEFT_SLIDING_LEFT, descTextRegionUPtr_->GetEntityPos().y);

                    descBoxUPtr_->SetEntityPos(
                        POS_LEFT_SLIDING_LEFT, descBoxUPtr_->GetEntityPos().y);
                }
                else
                {
                    descTextRegionUPtr_->SetEntityPos(
                        POS_LEFT_SLIDING_RIGHT, descTextRegionUPtr_->GetEntityPos().y);

                    descBoxUPtr_->SetEntityPos(
                        POS_LEFT_SLIDING_RIGHT, descBoxUPtr_->GetEntityPos().y);
                }
            }

            if (descBoxSlider_.IsDone())
            {
                Setup_DescBox(true);

                unEquipListBoxUPtr_->SetEntityPos(
                    SCREEN_WIDTH_ + 1.0f, unEquipListBoxUPtr_->GetEntityPos().y);

                descTextRegionUPtr_->SetEntityPos(
                    SCREEN_WIDTH_ + 1.0f, descTextRegionUPtr_->GetEntityPos().y);

                descBoxUPtr_->SetEntityPos(SCREEN_WIDTH_ + 1.0f, descBoxUPtr_->GetEntityPos().y);

                hasDescBoxChanged_ = true;
                descBoxSlider_.Reset(VIEW_CHANGE_SLIDER_SPEED_);
            }
        }
        else
        {
            auto const RATIO{ (1.0f - descBoxSlider_.Update(ELAPSED_TIME_SECONDS)) };

            auto const POS_LEFT_SLIDING_LEFT{ SECOND_LISTBOX_POS_LEFT_
                                              + ((SCREEN_WIDTH_ * 0.75f) * RATIO) };

            auto const POS_LEFT_SLIDING_RIGHT{
                SECOND_LISTBOX_POS_LEFT_ + (((OUT_OF_SIGHT_POS_ * 2.0f) - SCREEN_WIDTH_) * RATIO)
            };

            if ((ViewType::Items == viewToChangeTo_) || isViewForcedToItems_
                || ((ViewType::Count == viewToChangeTo_) && (ViewType::Items == view_)))
            {
                if (isSlidingLeft_)
                {
                    unEquipListBoxUPtr_->SetEntityPos(
                        POS_LEFT_SLIDING_LEFT, unEquipListBoxUPtr_->GetEntityPos().y);
                }
                else
                {
                    unEquipListBoxUPtr_->SetEntityPos(
                        POS_LEFT_SLIDING_RIGHT, unEquipListBoxUPtr_->GetEntityPos().y);
                }
            }
            else
            {
                if (isSlidingLeft_)
                {
                    descTextRegionUPtr_->SetEntityPos(
                        POS_LEFT_SLIDING_LEFT, descTextRegionUPtr_->GetEntityPos().y);

                    descBoxUPtr_->SetEntityPos(
                        POS_LEFT_SLIDING_LEFT, descBoxUPtr_->GetEntityPos().y);
                }
                else
                {
                    descTextRegionUPtr_->SetEntityPos(
                        POS_LEFT_SLIDING_RIGHT, descTextRegionUPtr_->GetEntityPos().y);

                    descBoxUPtr_->SetEntityPos(
                        POS_LEFT_SLIDING_RIGHT, descBoxUPtr_->GetEntityPos().y);
                }
            }

            if (descBoxSlider_.IsDone())
            {
                isDescBoxSliding_ = false;
                isDescBoxSlidingDone_ = true;
                hasDescBoxChanged_ = false;
            }
        }
    }

    void InventoryStage::UpdateTime_DetailView(const float ELAPSED_TIME_SECONDS)
    {
        detailViewTimerSec_ += ELAPSED_TIME_SECONDS;

        if (detailViewTimerSec_ >= DETAILVIEW_TIMER_DURATION_SEC_)
        {
            sfml_util::SoundManager::Instance()->PlaySfx_TickOn();

            detailViewSourceRect_ = GetItemRectMouseIsOver(mousePosV_);
            if (detailViewSourceRect_ != sfml_util::gui::ListBox::ERROR_RECT_)
            {
                isAchievementDisplaying_ = false;
                isDetailViewFadingIn_ = true;
                detailViewTimerSec_ = 0.0f;
                detailViewSlider_.Reset(DETAILVIEW_SLIDER_SPEED_);
            }
        }
    }

    void InventoryStage::UpdateTime_DetailViewFade(const float ELAPSED_TIME_SECONDS)
    {
        if (isDetailViewFadingIn_)
        {
            detailViewSliderRatio_ = detailViewSlider_.Update(ELAPSED_TIME_SECONDS);
            if (detailViewSlider_.IsDone())
            {
                if (isAchievementDisplaying_)
                {
                    isDetailViewFadingIn_ = false;
                    isDetailViewDoneFading_ = true;
                    isDetailViewFadingOut_ = false;
                    SetupDetailViewCreature(creaturePtr_);
                }
                else
                {
                    auto const ITEM_PTR_OPT{ GetItemMouseIsOver(mousePosV_) };
                    if (ITEM_PTR_OPT)
                    {
                        isDetailViewFadingIn_ = false;
                        isDetailViewDoneFading_ = true;
                        isDetailViewFadingOut_ = false;
                        SetupDetailViewItem(ITEM_PTR_OPT);
                    }
                    else
                    {
                        isDetailViewFadingIn_ = false;
                        isDetailViewDoneFading_ = false;
                        isDetailViewFadingOut_ = true;
                    }
                }
            }
        }
        else if (isDetailViewFadingOut_)
        {
            detailViewSliderRatio_ = (1.0f - detailViewSlider_.Update(ELAPSED_TIME_SECONDS));

            if (detailViewSlider_.IsDone())
            {
                isDetailViewFadingIn_ = false;
                isDetailViewDoneFading_ = false;
                isDetailViewFadingOut_ = false;
            }
        }

        const sf::Color NEW_COLOR{
            0,
            0,
            0,
            static_cast<sf::Uint8>(
                DETAILVIEW_COLOR_ALPHA_START_
                + ((DETAILVIEW_COLOR_ALPHA_END_ - DETAILVIEW_COLOR_ALPHA_START_)
                   * detailViewSliderRatio_))
        };

        detailViewQuads_[0].color = NEW_COLOR;
        detailViewQuads_[1].color = NEW_COLOR;
        detailViewQuads_[2].color = NEW_COLOR;
        detailViewQuads_[3].color = NEW_COLOR;

        if ((detailViewSlider_.IsDone()) && (false == isDetailViewFadingIn_)
            && (false == isDetailViewDoneFading_) && (false == isDetailViewFadingOut_))
        {
            detailViewQuads_[0].color = sf::Color::Transparent;
            detailViewQuads_[1].color = sf::Color::Transparent;
            detailViewQuads_[2].color = sf::Color::Transparent;
            detailViewQuads_[3].color = sf::Color::Transparent;
        }

        SetDetailViewQuads();
    }

    void InventoryStage::UpdateTime_SparkleAnimation(const float ELAPSED_TIME_SECONDS)
    {
        if (sparkleAnimUPtr_)
        {
            sparkleAnimUPtr_->Update(ELAPSED_TIME_SECONDS);

            if (sparkleAnimUPtr_->IsFinished())
            {
                sparkleAnimUPtr_.reset();
                HandleCast_Step3_DisplayResults();
            }
        }
    }

    void InventoryStage::UpdateTime_SongAnimation(const float ELAPSED_TIME_SECONDS)
    {
        if (songAnimUPtr_)
        {
            songAnimUPtr_->Update(ELAPSED_TIME_SECONDS);

            if (songAnimUPtr_->IsFinished())
            {
                songAnimUPtr_.reset();
                HandleSong_Step2_DisplayResults();
            }
        }
    }

    bool InventoryStage::HandleBack()
    {
        game::LoopManager::Instance()->TransitionTo_Previous(hasTakenActionSpellOrSong_);
        return true;
    }

    bool InventoryStage::HandleEquipRequest()
    {
        if ((game::Phase::Combat == currentPhase_) && (creaturePtr_ != turnCreaturePtr_))
        {
            std::ostringstream ss;
            ss << "\nDuring combat, only the character whose turn it is may "
               << "equip items.";

            PopupRejectionWindow(ss.str());
            return false;
        }
        else
        {
            return HandleEquipActual();
        }
    }

    bool InventoryStage::HandleEquipActual()
    {
        if ((ViewType::Items == view_) && (equipButtonUPtr_->IsDisabled() == false))
        {
            auto const LISTBOX_ITEM_SPTR{ unEquipListBoxUPtr_->Selected() };

            if ((LISTBOX_ITEM_SPTR) && LISTBOX_ITEM_SPTR->ITEM_PTR_OPT)
            {
                auto const ITEM_PTR{ LISTBOX_ITEM_SPTR->ITEM_PTR_OPT.value() };
                auto const EQUIP_RESULT{ creaturePtr_->ItemEquip(ITEM_PTR) };
                if (EQUIP_RESULT.empty())
                {
                    sfml_util::SoundManager::Instance()->PlaySfx_AckMajor();

                    unEquipListBoxUPtr_->Remove(LISTBOX_ITEM_SPTR);
                    equippedListBoxUPtr_->Add(LISTBOX_ITEM_SPTR);
                    Setup_ButtonMouseoverText();
                }
                else
                {
                    std::ostringstream ss;
                    ss << "Cannot equip the " << ITEM_PTR->Name() << " because: " << EQUIP_RESULT;

                    auto const POPUP_INFO{ popup::PopupManager::Instance()->CreatePopupInfo(
                        "InventoryStage'sEquipItemFailedPopup",
                        ss.str(),
                        popup::PopupButtons::Okay,
                        popup::PopupImage::Regular) };

                    game::LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
                    isWaitingOnPopup_ = true;
                    return false;
                }
            }
        }

        return true;
    }

    bool InventoryStage::HandleUnequipRequest()
    {
        if ((game::Phase::Combat == currentPhase_) && (creaturePtr_ != turnCreaturePtr_))
        {
            std::ostringstream ss;
            ss << "\nDuring combat, only the character whose turn it is may "
               << "unequip items.";

            PopupRejectionWindow(ss.str());
            return false;
        }
        else
        {
            return HandleUnequipActual();
        }
    }

    bool InventoryStage::HandleUnequipActual()
    {
        if ((ViewType::Items == view_) && (unequipButtonUPtr_->IsDisabled() == false))
        {
            auto const LISTBOX_ITEM_SPTR{ equippedListBoxUPtr_->Selected() };
            if ((LISTBOX_ITEM_SPTR) && LISTBOX_ITEM_SPTR->ITEM_PTR_OPT)
            {
                auto const ITEM_PTR{ LISTBOX_ITEM_SPTR->ITEM_PTR_OPT.value() };

                if (ITEM_PTR->IsBodypart())
                {
                    PopupDoneWindow("Bodypart items cannot be unequipped.", true);
                    return false;
                }
                else
                {
                    sfml_util::SoundManager::Instance()->PlaySfx_AckMajor();
                    creaturePtr_->ItemUnEquip(ITEM_PTR);
                    equippedListBoxUPtr_->Remove(LISTBOX_ITEM_SPTR);
                    unEquipListBoxUPtr_->Add(LISTBOX_ITEM_SPTR);
                    Setup_ButtonMouseoverText();
                }
            }
        }

        return true;
    }

    bool InventoryStage::HandleGiveRequestInitial()
    {
        actionType_ = ActionType::Give;

        auto const POPUP_INFO{ popup::PopupManager::Instance()->CreateInventoryPromptPopupInfo(
            POPUP_NAME_GIVE_,
            std::string("\nWhat do you want to give?\n\n(I)tem\n(C)oins\n(G)ems\n(M)eteor")
                .append("Shards\n\n...or (Escape) to Cancel"),
            popup::PopupButtons::Cancel,
            popup::PopupImage::Large,
            sfml_util::Justified::Center,
            sfml_util::sound_effect::PromptGeneric,
            true,
            sfml_util::FontManager::Instance()->Size_Largeish()) };

        game::LoopManager::Instance()->PopupWaitBeginSpecific<popup::PopupStageInventoryPrompt>(
            this, POPUP_INFO);

        isWaitingOnPopup_ = true;
        return true;
    }

    bool InventoryStage::HandleGiveRequestItems()
    {
        if ((game::Phase::Combat == currentPhase_) && (creaturePtr_ != turnCreaturePtr_))
        {
            std::ostringstream ss;
            ss << "\nDuring combat, only the character whose turn it is may "
               << "interact with items.";

            PopupRejectionWindow(ss.str());
        }

        listBoxItemToGiveSPtr_ = unEquipListBoxUPtr_->Selected();

        if ((listBoxItemToGiveSPtr_) && listBoxItemToGiveSPtr_->ITEM_PTR_OPT)
        {
            auto const ITEM_PTR{ listBoxItemToGiveSPtr_->ITEM_PTR_OPT.value() };
            PopupCharacterSelectWindow("Give the " + ITEM_PTR->Name() + " to who?");
            contentType_ = ContentType::Item;
        }
        else
        {
            PopupRejectionWindow("There is no item in the unequipped list to give!");
        }

        return false;
    }

    bool InventoryStage::HandleGiveRequestCoins()
    {
        if (creaturePtr_->Inventory().Coins().IsZero())
        {
            std::ostringstream ss;
            ss << creaturePtr_->Name() << " has no coins to give!";
            PopupRejectionWindow(ss.str());
        }
        else
        {
            contentType_ = ContentType::Coins;

            if (creaturePtr_->Inventory().Coins() == 1_coin)
            {
                PopupCharacterSelectWindow("Give the coin to who?");
            }
            else
            {
                PopupCharacterSelectWindow("Give coins to who?");
            }
        }

        return false;
    }

    bool InventoryStage::HandleGiveRequestGems()
    {
        if (creaturePtr_->Inventory().Gems().IsZero())
        {
            std::ostringstream ss;
            ss << creaturePtr_->Name() << " has no gems to give!";
            PopupRejectionWindow(ss.str());
        }
        else
        {
            contentType_ = ContentType::Gems;

            if (creaturePtr_->Inventory().Gems() == 1_gem)
            {
                PopupCharacterSelectWindow("Give the gem to who?");
            }
            else
            {
                PopupCharacterSelectWindow("Give gems to who?");
            }
        }

        return false;
    }

    bool InventoryStage::HandleGiveRequestMeteorShards()
    {
        if (creaturePtr_->Inventory().MeteorShards().IsZero())
        {
            std::ostringstream ss;
            ss << creaturePtr_->Name() << " has no Meteor Shards to give!";
            PopupRejectionWindow(ss.str());
        }
        else
        {
            contentType_ = ContentType::MeteorShards;

            if (creaturePtr_->Inventory().MeteorShards() == 1_mshard)
            {
                PopupCharacterSelectWindow("Give the Meteor Shard to who?");
            }
            else
            {
                PopupCharacterSelectWindow("Give Meteor Shards to who?");
            }
        }

        return false;
    }

    bool InventoryStage::HandleGiveActualItems(const creature::CreaturePtr_t)
    {
        if ((game::Phase::Combat == currentPhase_) && (creaturePtr_ != turnCreaturePtr_))
        {
            std::ostringstream ss;
            ss << "\nDuring combat, only the character whose turn it is may "
               << "interact with items.";

            PopupRejectionWindow(ss.str());
            return false;
        }

        if ((!listBoxItemToGiveSPtr_) || (!listBoxItemToGiveSPtr_->ITEM_PTR_OPT))
        {
            return true;
        }

        auto const ITEM_PTR{ listBoxItemToGiveSPtr_->ITEM_PTR_OPT.value() };
        auto const CAN_GIVE_ITEM_STR{ creatureToGiveToPtrOpt_->Obj().ItemIsAddAllowed(ITEM_PTR) };
        if (CAN_GIVE_ITEM_STR.empty())
        {
            sfml_util::SoundManager::Instance()
                ->Getsound_effect_set(sfml_util::sound_effect_set::ItemGive)
                .PlayRandom();

            creaturePtr_->ItemRemove(ITEM_PTR);
            creatureToGiveToPtrOpt_->Obj().ItemAdd(ITEM_PTR);
            Setup_DescBox(false);
            EndOfGiveShareGatherTasks();
            return true;
        }
        else
        {
            std::ostringstream ss;
            ss << "Can't give the " << ITEM_PTR->Name() << " to "
               << creatureToGiveToPtrOpt_->Obj().Name() << " because:  " << CAN_GIVE_ITEM_STR;

            PopupRejectionWindow(ss.str());
            return false;
        }
    }

    bool
        InventoryStage::HandleGiveActualCoins(const creature::CreaturePtr_t CREATURE_TO_GIVE_TO_PTR)
    {
        if (creaturePtr_->Inventory().Coins() == 1_coin)
        {
            HandleCoinsGive(1, CREATURE_TO_GIVE_TO_PTR);
        }
        else
        {
            std::ostringstream ss;
            ss << "\nGive " << CREATURE_TO_GIVE_TO_PTR->Name() << " how many coins?";

            PopupNumberSelectWindow(ss.str(), creaturePtr_->Inventory().Coins().As<std::size_t>());
        }

        return false;
    }

    bool InventoryStage::HandleGiveActualGems(const creature::CreaturePtr_t CREATURE_TO_GIVE_TO_PTR)
    {
        if (creaturePtr_->Inventory().Gems() == 1_gem)
        {
            HandleGemsGive(1, CREATURE_TO_GIVE_TO_PTR);
        }
        else
        {
            std::ostringstream ss;
            ss << "\nGive " << CREATURE_TO_GIVE_TO_PTR->Name() << " how many gems?";

            PopupNumberSelectWindow(ss.str(), creaturePtr_->Inventory().Gems().As<std::size_t>());
        }

        return false;
    }

    bool InventoryStage::HandleGiveActualMeteorShards(
        const creature::CreaturePtr_t CREATURE_TO_GIVE_TO_PTR)
    {
        if (creaturePtr_->Inventory().MeteorShards() == 1_mshard)
        {
            HandleMeteorShardsGive(1, CREATURE_TO_GIVE_TO_PTR);
        }
        else
        {
            std::ostringstream ss;
            ss << "\nGive " << CREATURE_TO_GIVE_TO_PTR->Name() << " how many Meteor Shards?";

            PopupNumberSelectWindow(
                ss.str(), creaturePtr_->Inventory().MeteorShards().As<std::size_t>());
        }

        return false;
    }

    bool InventoryStage::HandleShare()
    {
        actionType_ = ActionType::Share;
        PopupContentSelectionWindow("What would you like to share?");
        return true;
    }

    bool InventoryStage::HandleGather()
    {
        actionType_ = ActionType::Gather;
        PopupContentSelectionWindow("What would you like to gather?");
        return true;
    }

    bool InventoryStage::HandleDropRequest()
    {
        if ((ViewType::Items == view_) && (equipButtonUPtr_->IsDisabled() == false))
        {
            auto const LISTBOX_ITEM_SPTR{ unEquipListBoxUPtr_->Selected() };
            if ((LISTBOX_ITEM_SPTR) && LISTBOX_ITEM_SPTR->ITEM_PTR_OPT)
            {
                if ((game::Phase::Combat == currentPhase_) && (creaturePtr_ != turnCreaturePtr_))
                {
                    std::ostringstream ss;
                    ss << "\nDuring combat, only the character whose turn it is may "
                       << "interact with items.";

                    PopupRejectionWindow(ss.str());
                    return false;
                }

                itemToDropSPtr_ = LISTBOX_ITEM_SPTR;

                actionType_ = ActionType::Drop;

                auto const POPUP_INFO{ popup::PopupManager::Instance()->CreatePopupInfo(
                    POPUP_NAME_DROPCONFIRM_,
                    "\nAre you sure you want to drop the "
                        + LISTBOX_ITEM_SPTR->ITEM_PTR_OPT->Obj().Name() + "?",
                    popup::PopupButtons::YesNo,
                    popup::PopupImage::Regular,
                    sfml_util::Justified::Center,
                    sfml_util::sound_effect::PromptQuestion) };

                game::LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
                isWaitingOnPopup_ = true;
                return true;
            }
        }

        return false;
    }

    bool InventoryStage::HandleDropActual()
    {
        if ((game::Phase::Combat == currentPhase_) && (creaturePtr_ != turnCreaturePtr_))
        {
            std::ostringstream ss;
            ss << "\nDuring combat, only the character whose turn it is may "
               << "interact with items.";

            PopupRejectionWindow(ss.str());
            return false;
        }
        else
        {
            sfml_util::SoundManager::Instance()
                ->Getsound_effect_set(sfml_util::sound_effect_set::ItemDrop)
                .PlayRandom();

            unEquipListBoxUPtr_->Remove(itemToDropSPtr_);
            auto const ITEM_PTR{ itemToDropSPtr_->ITEM_PTR_OPT.value() };
            itemToDropSPtr_.reset();
            creaturePtr_->ItemRemove(ITEM_PTR);
            item::ItemWarehouse::Access().Free(ITEM_PTR);
            EndOfGiveShareGatherTasks();
            Setup_DescBox(false);
            return true;
        }
    }

    bool InventoryStage::HandlePlayerChangeBeforeOrAfter(const bool IS_NEXT_CREATURE_AFTER)
    {
        if (IS_NEXT_CREATURE_AFTER)
        {
            return HandlePlayerChangeTo(
                game::Game::Instance()->State().Party().GetNextInOrderAfter(creaturePtr_), true);
        }
        else
        {
            return HandlePlayerChangeTo(
                game::Game::Instance()->State().Party().GetNextInOrderBefore(creaturePtr_), false);
        }
    }

    bool InventoryStage::HandlePlayerChangeIndex(const std::size_t CHARACTER_NUM)
    {
        auto const CURR_INDEX{ game::Game::Instance()->State().Party().GetOrderNum(creaturePtr_) };

        if (CURR_INDEX == CHARACTER_NUM)
        {
            return false;
        }
        else
        {
            return HandlePlayerChangeTo(
                game::Game::Instance()->State().Party().GetAtOrderPos(CHARACTER_NUM),
                (CURR_INDEX < CHARACTER_NUM));
        }
    }

    bool InventoryStage::HandlePlayerChangeTo(
        const creature::CreaturePtr_t CREATURE_PTR, const bool IS_SLIDING_LEFT)
    {
        if (isSliderAnimating_)
        {
            return false;
        }
        else
        {
            creaturePtr_ = CREATURE_PTR;
            StartSlidingAnimation(IS_SLIDING_LEFT);

            if (((ViewType::Titles == view_) && (CREATURE_PTR->Titles().empty()))
                || ((ViewType::Spells == view_) && (CREATURE_PTR->Spells().empty()))
                || ((ViewType::Conditions == view_)
                    && (CREATURE_PTR->HasCondition(creature::Conditions::Good))))
            {
                isViewForcedToItems_ = true;
            }

            return true;
        }
    }

    void InventoryStage::StartSlidingAnimation(const bool IS_SLIDING_LEFT)
    {
        isSliderAnimating_ = true;
        isSlidingLeft_ = IS_SLIDING_LEFT;
        sliderAnimTimerSec_ = 0.0f;

        if (ViewType::Count == viewToChangeTo_)
        {
            if (IS_SLIDING_LEFT)
            {
                isImageSliding_ = true;
            }
            else
            {
                isDescBoxSliding_ = true;
                isStatsSliding_ = true;
            }
        }
        else
        {
            isListBoxSliding_ = true;
        }

        isImageSlidingDone_ = false;
        isDetailsSlidingDone_ = false;
        isCenterSlidingDone_ = false;
        isStatsSlidingDone_ = false;
        isListBoxSlidingDone_ = false;
        isDescBoxSlidingDone_ = false;

        imageSlider_.Reset(VIEW_CHANGE_SLIDER_SPEED_);
        detailsSlider_.Reset(VIEW_CHANGE_SLIDER_SPEED_);
        centerSlider_.Reset(VIEW_CHANGE_SLIDER_SPEED_);
        statsSlider_.Reset(VIEW_CHANGE_SLIDER_SPEED_);
        listBoxSlider_.Reset(VIEW_CHANGE_SLIDER_SPEED_);
        descBoxSlider_.Reset(VIEW_CHANGE_SLIDER_SPEED_);
    }

    void InventoryStage::SetDescBoxTextFromListBoxItem(
        const sfml_util::gui::ListBoxItemSPtr_t & LISTBOX_ITEM_SPTR)
    {
        if (LISTBOX_ITEM_SPTR)
        {
            std::ostringstream ss;

            if (LISTBOX_ITEM_SPTR->COND_PTR_OPT)
            {
                ss << LISTBOX_ITEM_SPTR->COND_PTR_OPT->Obj().Name() << "\n\n"
                   << LISTBOX_ITEM_SPTR->COND_PTR_OPT->Obj().LongDesc();
            }
            else if (LISTBOX_ITEM_SPTR->TITLE_PTR_OPT)
            {
                ss << LISTBOX_ITEM_SPTR->TITLE_PTR_OPT->Obj().Name() << "\n\n"
                   << LISTBOX_ITEM_SPTR->TITLE_PTR_OPT->Obj().LongDesc();
            }

            if (ss.str().empty() == false)
            {
                SetDescBoxText(ss.str());
            }
        }
    }

    void InventoryStage::SetDescBoxText(const std::string & TEXT)
    {
        sfml_util::gui::TextInfo descTextInfo(listBoxItemTextInfo_);
        descTextInfo.color = DESCBOX_TEXT_COLOR_;
        descTextInfo.charSize = DESCBOX_TEXT_SIZE_;
        descTextInfo.text = TEXT;

        const bool IS_DTR_ALREADY_INSTANTIATED{ descTextRegionUPtr_ };

        if (IS_DTR_ALREADY_INSTANTIATED)
        {
            EntityRemove(descTextRegionUPtr_.get());
        }

        descTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "InventoryStage'sDescription",
            descTextInfo,
            DESCBOX_REGION_,
            this,
            sfml_util::gui::TextRegion::DEFAULT_NO_RESIZE_,
            sfml_util::gui::box::Info(),
            DESCBOX_MARGINS_);

        if (IS_DTR_ALREADY_INSTANTIATED)
        {
            EntityAdd(descTextRegionUPtr_.get());
        }
    }

    void InventoryStage::PopupCharacterSelectWindow(
        const std::string & PROMPT_TEXT, const bool CAN_SELECT_SELF, const bool CAN_SELECT_BEASTS)
    {
        const std::size_t CURRENT_CREATURE_ORDER_NUM(
            game::Game::Instance()->State().Party().GetOrderNum(creaturePtr_));

        auto const NUM_CHARACTERS{ game::Game::Instance()->State().Party().Characters().size() };

        std::vector<std::string> invalidTextVec;
        invalidTextVec.resize(NUM_CHARACTERS);

        for (std::size_t i(0); i < NUM_CHARACTERS; ++i)
        {
            if ((CURRENT_CREATURE_ORDER_NUM == i) && (CAN_SELECT_SELF == false))
            {
                invalidTextVec[i] = "Cannot select self";
            }
            else if (
                game::Game::Instance()->State().Party().GetAtOrderPos(i)->IsBeast()
                && (CAN_SELECT_BEASTS == false))
            {
                invalidTextVec[i] = "Cannot select Beasts";
            }
            else
            {
                invalidTextVec[i].clear();
            }
        }

        auto const POPUP_INFO{ popup::PopupManager::Instance()->CreateCharacterSelectPopupInfo(
            POPUP_NAME_CHAR_SELECT_, PROMPT_TEXT, invalidTextVec) };

        game::LoopManager::Instance()->PopupWaitBeginSpecific<popup::PopupStageCharacterSelect>(
            this, POPUP_INFO);

        isWaitingOnPopup_ = true;
    }

    void InventoryStage::PopupRejectionWindow(const std::string & REJECTION_PROMPT_TEXT)
    {
        std::ostringstream ss;
        ss << "\n" << REJECTION_PROMPT_TEXT;

        auto const POPUPINFO_NOITEM{ popup::PopupManager::Instance()->CreatePopupInfo(
            "InventoryStage'sPopupRejection",
            ss.str(),
            popup::PopupButtons::Cancel,
            popup::PopupImage::Regular,
            sfml_util::Justified::Center,
            sfml_util::sound_effect::PromptWarn,
            sfml_util::FontManager::Instance()->Size_Largeish()) };

        game::LoopManager::Instance()->PopupWaitBegin(this, POPUPINFO_NOITEM);
        isWaitingOnPopup_ = true;
        EndOfGiveShareGatherTasks();
    }

    void InventoryStage::PopupNumberSelectWindow(
        const std::string & PROMPT_TEXT, const std::size_t NUMBER_MAX)
    {
        auto const POPUP_INFO_NUMBER_SELECT{
            popup::PopupManager::Instance()->CreateNumberSelectionPopupInfo(
                POPUP_NAME_NUMBER_SELECT_,
                PROMPT_TEXT,
                1,
                NUMBER_MAX,
                sfml_util::FontManager::Instance()->Size_Largeish())
        };

        game::LoopManager::Instance()->PopupWaitBeginSpecific<popup::PopupStageNumberSelect>(
            this, POPUP_INFO_NUMBER_SELECT);

        isWaitingOnPopup_ = true;
    }

    void InventoryStage::PopupDoneWindow(
        const std::string & PROMPT_TEXT, const bool WILL_PLAY_SOUNDEFFECT)
    {
        auto const POPUP_INFO_DONE{ popup::PopupManager::Instance()->CreatePopupInfo(
            "InventoryStage'sPopupDone",
            PROMPT_TEXT,
            popup::PopupButtons::Okay,
            popup::PopupImage::Regular,
            sfml_util::Justified::Center,
            ((WILL_PLAY_SOUNDEFFECT) ? sfml_util::sound_effect::PromptGeneric
                                     : sfml_util::sound_effect::None),
            sfml_util::FontManager::Instance()->Size_Largeish()) };

        game::LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO_DONE);
        isWaitingOnPopup_ = true;
        EndOfGiveShareGatherTasks();
    }

    void InventoryStage::PopupContentSelectionWindow(const std::string & PROMPT_TEXT)
    {
        std::ostringstream ss;
        ss << PROMPT_TEXT << "\n\n(C)oins\n(G)ems\n(M)eteor Shards";

        auto const POPUP_INFO{ popup::PopupManager::Instance()->CreateInventoryPromptPopupInfo(
            POPUP_NAME_CONTENTSELECTION_,
            ss.str(),
            popup::PopupButtons::Cancel,
            popup::PopupImage::Regular,
            sfml_util::Justified::Center,
            sfml_util::sound_effect::PromptGeneric,
            false,
            sfml_util::FontManager::Instance()->Size_Normal()) };

        game::LoopManager::Instance()->PopupWaitBeginSpecific<popup::PopupStageInventoryPrompt>(
            this, POPUP_INFO);

        isWaitingOnPopup_ = true;
    }

    void InventoryStage::HandleCoinsGive(
        const std::size_t COUNT, creature::CreaturePtr_t creatureToGiveToPtr)
    {
        sfml_util::SoundManager::Instance()
            ->Getsound_effect_set(sfml_util::sound_effect_set::Coin)
            .PlayRandom();

        creaturePtr_->CoinsAdj(Coin_t(static_cast<int>(COUNT) * -1));
        creatureToGiveToPtr->CoinsAdj(Coin_t::Make(COUNT));

        std::ostringstream ss;
        ss << COUNT << " coins taken from " << creaturePtr_->Name() << " and given to "
           << creatureToGiveToPtr->Name() << ".";

        PopupDoneWindow(ss.str(), false);
    }

    void InventoryStage::HandleGemsGive(
        const std::size_t COUNT, creature::CreaturePtr_t creatureToGiveToPtr)
    {
        sfml_util::SoundManager::Instance()
            ->Getsound_effect_set(sfml_util::sound_effect_set::Gem)
            .PlayRandom();

        creaturePtr_->GemsAdj(Gem_t(static_cast<int>(COUNT) * -1));
        creatureToGiveToPtr->GemsAdj(Gem_t(static_cast<int>(COUNT)));

        std::ostringstream ss;
        ss << COUNT << " gems taken from " << creaturePtr_->Name() << " and given to "
           << creatureToGiveToPtr->Name() << ".";

        PopupDoneWindow(ss.str(), false);
    }

    void InventoryStage::HandleMeteorShardsGive(
        const std::size_t COUNT, creature::CreaturePtr_t creatureToGiveToPtr)
    {
        sfml_util::SoundManager::Instance()
            ->Getsound_effect_set(sfml_util::sound_effect_set::MeteorShard)
            .PlayRandom();

        creaturePtr_->MeteorShardsAdj(MeteorShard_t(static_cast<int>(COUNT) * -1));
        creatureToGiveToPtr->MeteorShardsAdj(MeteorShard_t(static_cast<int>(COUNT)));

        std::ostringstream ss;
        ss << COUNT << " Meteor Shards taken from " << creaturePtr_->Name() << " and given to "
           << creatureToGiveToPtr->Name() << ".";

        PopupDoneWindow(ss.str(), false);
    }

    void InventoryStage::HandleCoinsGather(const bool WILL_TRIGGER_SECONDARY_ACTIONS)
    {
        auto coinsOwnedByOtherPartyMembers{ 0_coin };
        for (auto nextCreaturePtr : game::Game::Instance()->State().Party().Characters())
        {
            if (nextCreaturePtr != creaturePtr_)
            {
                auto const NEXT_CREATURE_COINS_OWNED{ nextCreaturePtr->Inventory().Coins() };
                if (NEXT_CREATURE_COINS_OWNED > 0_coin)
                {
                    coinsOwnedByOtherPartyMembers += NEXT_CREATURE_COINS_OWNED;
                    nextCreaturePtr->CoinsAdj(NEXT_CREATURE_COINS_OWNED * Coin_t(-1));
                }
            }
        }

        if ((0 == coinsOwnedByOtherPartyMembers) && WILL_TRIGGER_SECONDARY_ACTIONS)
        {
            PopupRejectionWindow("No other party members had any coins to gather.");
            return;
        }

        creaturePtr_->CoinsAdj(coinsOwnedByOtherPartyMembers);

        if (WILL_TRIGGER_SECONDARY_ACTIONS)
        {
            sfml_util::SoundManager::Instance()
                ->Getsound_effect_set(sfml_util::sound_effect_set::Coin)
                .PlayRandom();

            std::ostringstream ss;
            ss << "\n\n"
               << creaturePtr_->Name() << " gathered " << coinsOwnedByOtherPartyMembers
               << " coins from the rest of the party, and now has "
               << creaturePtr_->Inventory().Coins() << ".";

            PopupDoneWindow(ss.str(), false);
        }
    }

    void InventoryStage::HandleGemsGather(const bool WILL_TRIGGER_SECONDARY_ACTIONS)
    {
        auto gemsOwnedByOtherPartyMembers{ 0_gem };
        for (auto nextCreaturePtr : game::Game::Instance()->State().Party().Characters())
        {
            if (nextCreaturePtr != creaturePtr_)
            {
                auto const NEXT_CREATURE_GEMS_OWNED{ nextCreaturePtr->Inventory().Gems() };
                if (NEXT_CREATURE_GEMS_OWNED > 0_gem)
                {
                    gemsOwnedByOtherPartyMembers += NEXT_CREATURE_GEMS_OWNED;
                    nextCreaturePtr->GemsAdj(NEXT_CREATURE_GEMS_OWNED * Gem_t(-1));
                }
            }
        }

        if ((0 == gemsOwnedByOtherPartyMembers) && WILL_TRIGGER_SECONDARY_ACTIONS)
        {
            PopupRejectionWindow("No other party members had any gems to gather.");
            return;
        }

        creaturePtr_->GemsAdj(gemsOwnedByOtherPartyMembers);

        if (WILL_TRIGGER_SECONDARY_ACTIONS)
        {
            sfml_util::SoundManager::Instance()
                ->Getsound_effect_set(sfml_util::sound_effect_set::Gem)
                .PlayRandom();

            std::ostringstream ss;
            ss << "\n\n"
               << creaturePtr_->Name() << " gathered " << gemsOwnedByOtherPartyMembers
               << " gems from the rest of the party, and now has "
               << creaturePtr_->Inventory().Gems() << ".";

            PopupDoneWindow(ss.str(), false);
        }
    }

    void InventoryStage::HandleMeteorShardsGather(const bool WILL_TRIGGER_SECONDARY_ACTIONS)
    {
        auto shardsOwnedByOtherPartyMembers{ 0_mshard };
        for (auto nextCreaturePtr : game::Game::Instance()->State().Party().Characters())
        {
            if (nextCreaturePtr != creaturePtr_)
            {
                auto const NEXT_CREATURE_SHARDS{ nextCreaturePtr->Inventory().MeteorShards() };
                if (NEXT_CREATURE_SHARDS > 0_mshard)
                {
                    shardsOwnedByOtherPartyMembers += NEXT_CREATURE_SHARDS;
                    nextCreaturePtr->MeteorShardsAdj(NEXT_CREATURE_SHARDS * MeteorShard_t(-1));
                }
            }
        }

        if ((0 == shardsOwnedByOtherPartyMembers) && WILL_TRIGGER_SECONDARY_ACTIONS)
        {
            PopupRejectionWindow("No other party members had any Meteor Shards to gather.");
            return;
        }

        creaturePtr_->MeteorShardsAdj(shardsOwnedByOtherPartyMembers);

        if (WILL_TRIGGER_SECONDARY_ACTIONS)
        {
            sfml_util::SoundManager::Instance()
                ->Getsound_effect_set(sfml_util::sound_effect_set::MeteorShard)
                .PlayRandom();

            std::ostringstream ss;
            ss << "\n\n"
               << creaturePtr_->Name() << " gathered " << shardsOwnedByOtherPartyMembers
               << " Meteor Shards from the rest of the party, and now has "
               << creaturePtr_->Inventory().MeteorShards() << ".";

            PopupDoneWindow(ss.str(), false);
        }
    }

    void InventoryStage::HandleCoinsShare()
    {
        // ensure there are any coins to share
        {
            Coin_t totalCoins{ 0 };
            for (auto const & CREATURE_PTR : game::Game::Instance()->State().Party().Characters())
            {
                totalCoins += CREATURE_PTR->Inventory().Coins();
            }

            if (0 == totalCoins)
            {
                PopupRejectionWindow("The party has no coins to share.");
                return;
            }
        }

        sfml_util::SoundManager::Instance()
            ->Getsound_effect_set(sfml_util::sound_effect_set::Coin)
            .PlayRandom();

        HandleCoinsGather(false);

        auto const COINS_TOTAL{ creaturePtr_->Inventory().Coins().As<int>() };

        auto const HUMANOID_COUNT{ static_cast<int>(
            game::Game::Instance()->State().Party().GetNumHumanoid()) };

        auto const COINS_TO_SHARE{ COINS_TOTAL / HUMANOID_COUNT };
        auto const COINS_LEFT_OVER{ COINS_TOTAL % HUMANOID_COUNT };

        for (auto nextCreaturePtr : game::Game::Instance()->State().Party().Characters())
        {
            if (nextCreaturePtr->Body().IsHumanoid())
            {
                nextCreaturePtr->CoinsAdj(nextCreaturePtr->Inventory().Coins() * Coin_t(-1));
                nextCreaturePtr->CoinsAdj(Coin_t(COINS_TO_SHARE));
            }
        }

        auto toHandOut{ COINS_LEFT_OVER };
        for (auto nextCreaturePtr : game::Game::Instance()->State().Party().Characters())
        {
            if (nextCreaturePtr->Body().IsHumanoid() && (toHandOut-- > 0))
            {
                nextCreaturePtr->CoinsAdj(1_coin);
            }
        }

        std::ostringstream ss;
        if (COINS_TO_SHARE > 0)
        {
            ss << "\n\nAll " << HUMANOID_COUNT << " humanoid party members share "
               << COINS_TO_SHARE;

            if (COINS_LEFT_OVER > 0)
            {
                ss << " or " << COINS_TO_SHARE + 1;
            }
        }
        else
        {
            ss << "\n\nThere were only " << COINS_TO_SHARE << ", so not every character received";
        }

        ss << " coins.";
        PopupDoneWindow(ss.str(), false);
    }

    void InventoryStage::HandleGemsShare()
    {
        // ensure there are any gems to share
        {
            Gem_t totalGems{ 0 };
            for (auto const & CREATURE_PTR : game::Game::Instance()->State().Party().Characters())
            {
                totalGems += CREATURE_PTR->Inventory().Gems();
            }

            if (0 == totalGems)
            {
                PopupRejectionWindow("The party has no gems to share.");
                return;
            }
        }

        sfml_util::SoundManager::Instance()
            ->Getsound_effect_set(sfml_util::sound_effect_set::Gem)
            .PlayRandom();

        HandleGemsGather(false);

        auto const GEMS_TOTAL{ creaturePtr_->Inventory().Gems().As<int>() };

        auto const HUMANOID_COUNT{ static_cast<int>(
            game::Game::Instance()->State().Party().GetNumHumanoid()) };

        auto const GEMS_TO_SHARE{ GEMS_TOTAL / HUMANOID_COUNT };
        auto const GEMS_LEFT_OVER{ GEMS_TOTAL % HUMANOID_COUNT };

        for (auto nextCreaturePtr : game::Game::Instance()->State().Party().Characters())
        {
            if (nextCreaturePtr->Body().IsHumanoid())
            {
                nextCreaturePtr->GemsAdj(nextCreaturePtr->Inventory().Gems() * Gem_t(-1));
                nextCreaturePtr->GemsAdj(Gem_t(GEMS_TO_SHARE));
            }
        }

        auto toHandOut{ GEMS_LEFT_OVER };
        for (auto nextCreaturePtr : game::Game::Instance()->State().Party().Characters())
        {
            if (nextCreaturePtr->Body().IsHumanoid() && (toHandOut-- > 0))
            {
                nextCreaturePtr->GemsAdj(1_gem);
            }
        }

        std::ostringstream ss;
        if (GEMS_TO_SHARE > 0)
        {
            ss << "\n\nAll " << HUMANOID_COUNT << " humanoid party members share " << GEMS_TO_SHARE;

            if (GEMS_LEFT_OVER > 0)
            {
                ss << " or " << GEMS_TO_SHARE + 1;
            }
        }
        else
        {
            ss << "\n\nThere were only " << GEMS_TO_SHARE << ", so not every character received";
        }

        ss << " gems.";
        PopupDoneWindow(ss.str(), false);
    }

    void InventoryStage::HandleMeteorShardsShare()
    {
        // ensure there are any shards to share
        {
            MeteorShard_t totalShards{ 0 };
            for (auto const & CREATURE_PTR : game::Game::Instance()->State().Party().Characters())
            {
                totalShards += CREATURE_PTR->Inventory().MeteorShards();
            }

            if (0 == totalShards)
            {
                PopupRejectionWindow("The party has no Meteor Shards to share.");
                return;
            }
        }

        sfml_util::SoundManager::Instance()
            ->Getsound_effect_set(sfml_util::sound_effect_set::MeteorShard)
            .PlayRandom();

        HandleMeteorShardsGather(false);

        const int METEORSHARDS_TOTAL{ creaturePtr_->Inventory().MeteorShards().As<int>() };

        const int HUMANOID_COUNT{ static_cast<int>(
            game::Game::Instance()->State().Party().GetNumHumanoid()) };

        auto const METEORSHARDS_TO_SHARE{ METEORSHARDS_TOTAL / HUMANOID_COUNT };
        auto const METEORSHARDS_LEFT_OVER{ METEORSHARDS_TOTAL % HUMANOID_COUNT };

        for (auto nextCreaturePtr : game::Game::Instance()->State().Party().Characters())
        {
            if (nextCreaturePtr->Body().IsHumanoid())
            {
                nextCreaturePtr->MeteorShardsAdj(
                    nextCreaturePtr->Inventory().MeteorShards() * MeteorShard_t(-1));

                nextCreaturePtr->MeteorShardsAdj(MeteorShard_t(METEORSHARDS_TO_SHARE));
            }
        }

        auto toHandOut{ METEORSHARDS_LEFT_OVER };
        for (auto nextCreaturePtr : game::Game::Instance()->State().Party().Characters())
        {
            if (nextCreaturePtr->Body().IsHumanoid() && (toHandOut-- > 0))
            {
                nextCreaturePtr->MeteorShardsAdj(1_mshard);
            }
        }

        std::ostringstream ss;
        if (METEORSHARDS_TO_SHARE > 0)
        {
            ss << "\n\nAll " << HUMANOID_COUNT << " humanoid party members share "
               << METEORSHARDS_TO_SHARE;

            if (METEORSHARDS_LEFT_OVER > 0)
            {
                ss << " or " << METEORSHARDS_TO_SHARE + 1;
            }
        }
        else
        {
            ss << "\n\nThere were only " << METEORSHARDS_TO_SHARE
               << ", so not every character received";
        }

        ss << " Meteor Shards.";
        PopupDoneWindow(ss.str(), false);
    }

    void InventoryStage::EndOfGiveShareGatherTasks()
    {
        listBoxItemToGiveSPtr_.reset();
        creatureToGiveToPtrOpt_ = boost::none;
        itemToDropSPtr_.reset();
        contentType_ = ContentType::Count;
        actionType_ = ActionType::Count;
        Setup_CreatureDetails(false);
        Setup_CreatureStats();
        Setup_CenterText();
    }

    float InventoryStage::UpdateImageDetailsPosition()
    {
        sf::Texture texture;

        sfml_util::gui::CreatureImageLoader creatureImageLoader;
        creatureImageLoader.GetImage(texture, creaturePtr_);

        sf::Sprite sprite(texture);
        sprite.setScale(CREATURE_IMAGE_SCALE_, CREATURE_IMAGE_SCALE_);

        detailsPosLeft_
            = CREATURE_IMAGE_POS_LEFT_ + sprite.getGlobalBounds().width + CREATURE_IMAGE_RIGHT_PAD_;

        return sprite.getGlobalBounds().width;
    }

    const item::ItemPtrOpt_t InventoryStage::GetItemMouseIsOver(const sf::Vector2f & MOUSE_POS_V)
    {
        if (view_ == ViewType::Items)
        {
            sfml_util::gui::ListBoxItemSPtr_t listBoxItemSPtr;

            if (unEquipListBoxUPtr_->GetEntityRegion().contains(MOUSE_POS_V))
            {
                listBoxItemSPtr = unEquipListBoxUPtr_->AtPos(MOUSE_POS_V);
            }
            else if (equippedListBoxUPtr_->GetEntityRegion().contains(MOUSE_POS_V))
            {
                listBoxItemSPtr = equippedListBoxUPtr_->AtPos(MOUSE_POS_V);
            }

            if ((listBoxItemSPtr) && listBoxItemSPtr->ITEM_PTR_OPT)
            {
                return listBoxItemSPtr->ITEM_PTR_OPT.value();
            }
        }

        return boost::none;
    }

    const sf::FloatRect InventoryStage::GetItemRectMouseIsOver(const sf::Vector2f & MOUSE_POS_V)
    {
        if (view_ == ViewType::Items)
        {
            if (unEquipListBoxUPtr_->GetEntityRegion().contains(MOUSE_POS_V))
            {
                return unEquipListBoxUPtr_->ImageRectOfItemAtPos(MOUSE_POS_V);
            }
            else if (equippedListBoxUPtr_->GetEntityRegion().contains(MOUSE_POS_V))
            {
                return equippedListBoxUPtr_->ImageRectOfItemAtPos(MOUSE_POS_V);
            }
        }

        return sfml_util::gui::ListBox::ERROR_RECT_;
    }

    void InventoryStage::SetupDetailViewItem(const item::ItemPtrOpt_t ITEM_PTR_OPT)
    {
        if (!ITEM_PTR_OPT)
        {
            detailViewTextUPtr_.reset();
            return;
        }

        auto const ITEM_PTR{ ITEM_PTR_OPT.value() };

        sfml_util::gui::ItemImageLoader itemImageMachine;
        itemImageMachine.Load(detailViewTexture_, ITEM_PTR);

        detailViewSprite_.setTexture(detailViewTexture_, true);

        auto const DETAILVIEW_IMAGE_SCALE{ sfml_util::MapByRes(0.75f, 1.25f) };

        detailViewSprite_.setScale(DETAILVIEW_IMAGE_SCALE, DETAILVIEW_IMAGE_SCALE);

        detailViewSprite_.setPosition(
            (SCREEN_WIDTH_ * 0.5f) - (detailViewSprite_.getGlobalBounds().width * 0.5f),
            DETAILVIEW_POS_TOP_ + DETAILVIEW_INNER_PAD_);

        std::ostringstream ss;
        ss << ITEM_PTR->Name() << "\n"
           << ITEM_PTR->Desc() << "\n\n"
           << item::category::ToString(ITEM_PTR->Category(), misc::Wrap::Yes) << "\n";

        if (ITEM_PTR->ExclusiveRole() != creature::role::Count)
        {
            ss << "(can only be used by " << creature::role::ToString(ITEM_PTR->ExclusiveRole())
               << "s)\n";
        }

        ss << "\n";

        ss << "weighs " << ITEM_PTR->Weight() << "\n"
           << "worth about " << ITEM_PTR->Price() << " coins\n";

        if (ITEM_PTR->IsWeapon())
        {
            ss << "Damage:  " << ITEM_PTR->DamageMin() << "-" << ITEM_PTR->DamageMax();
        }
        else if (ITEM_PTR->IsArmor())
        {
            ss << "Armor Bonus:  " << ITEM_PTR->ArmorRating();
        }

        ss << "\n\n";

        const sfml_util::gui::TextInfo TEXT_INFO(
            ss.str(),
            sfml_util::FontManager::Instance()->Font_Default1(),
            sfml_util::FontManager::Instance()->Size_Normal(),
            sf::Color::White,
            sfml_util::Justified::Center);

        auto const LEFT{ DETAILVIEW_POS_LEFT_ + DETAILVIEW_INNER_PAD_ };

        auto const TOP{ detailViewSprite_.getGlobalBounds().top
                        + detailViewSprite_.getGlobalBounds().height + DETAILVIEW_INNER_PAD_ };

        auto const DOUBLE_INNER_PAD{ 2.0f * DETAILVIEW_INNER_PAD_ };

        auto const WIDTH{ DETAILVIEW_WIDTH_ - DOUBLE_INNER_PAD };

        auto const DETAILVIEW_SPRITE_BOTTOM{ detailViewSprite_.getGlobalBounds().top
                                             + detailViewSprite_.getGlobalBounds().height };

        auto const HEIGHT{ (DETAILVIEW_HEIGHT_ - DETAILVIEW_SPRITE_BOTTOM) - DOUBLE_INNER_PAD };

        const sf::FloatRect TEXT_RECT(LEFT, TOP, WIDTH, HEIGHT);

        detailViewTextUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "InventoryStage'sDetailViewForItems", TEXT_INFO, TEXT_RECT);
    }

    void InventoryStage::SetupDetailViewCreature(const creature::CreaturePtrOpt_t CREATURE_PTR_OPT)
    {
        if (!CREATURE_PTR_OPT)
        {
            detailViewTextUPtr_.reset();
            return;
        }

        auto const CREATURE_PTR{ CREATURE_PTR_OPT.value() };

        sfml_util::gui::CreatureImageLoader creatureImageLoader;
        creatureImageLoader.GetImage(detailViewTexture_, CREATURE_PTR);

        detailViewSprite_.setTexture(detailViewTexture_);

        detailViewSprite_.setTextureRect(sf::IntRect(
            0,
            0,
            static_cast<int>(detailViewTexture_.getSize().x),
            static_cast<int>(detailViewTexture_.getSize().y)));

        detailViewSprite_.setScale(CREATURE_IMAGE_SCALE_, CREATURE_IMAGE_SCALE_);

        detailViewSprite_.setPosition(
            (SCREEN_WIDTH_ * 0.5f) - (detailViewSprite_.getGlobalBounds().width * 0.5f),
            DETAILVIEW_POS_TOP_ + DETAILVIEW_INNER_PAD_ + sfml_util::MapByRes(0.0f, 50.0f));

        std::ostringstream ss;
        ss << "\n"
           << MakeTitleString(CREATURE_PTR, creature::AchievementType::EnemiesFaced)
           << MakeTitleString(CREATURE_PTR, creature::AchievementType::MeleeHits)
           << MakeTitleString(CREATURE_PTR, creature::AchievementType::BattlesSurvived)
           << MakeTitleString(CREATURE_PTR, creature::AchievementType::ProjectileHits)
           << MakeTitleString(CREATURE_PTR, creature::AchievementType::BeastMindLinks)
           << MakeTitleString(CREATURE_PTR, creature::AchievementType::DodgedStanding)
           << MakeTitleString(CREATURE_PTR, creature::AchievementType::DodgedFlying)
           << MakeTitleString(CREATURE_PTR, creature::AchievementType::LocksPicked)
           << MakeTitleString(CREATURE_PTR, creature::AchievementType::BackstabsHits)
           << MakeTitleString(CREATURE_PTR, creature::AchievementType::SongsPlayed)
           << MakeTitleString(CREATURE_PTR, creature::AchievementType::SpiritsLifted)
           << MakeTitleString(CREATURE_PTR, creature::AchievementType::BeastRoars)
           << MakeTitleString(CREATURE_PTR, creature::AchievementType::MoonHowls)
           << MakeTitleString(CREATURE_PTR, creature::AchievementType::PackActions)
           << MakeTitleString(CREATURE_PTR, creature::AchievementType::FlyingAttackHits)
           << MakeTitleString(CREATURE_PTR, creature::AchievementType::TurnsInFlight)
           << MakeTitleString(CREATURE_PTR, creature::AchievementType::SpellsCast)
           << MakeTitleString(CREATURE_PTR, creature::AchievementType::HealthGiven)
           << MakeTitleString(CREATURE_PTR, creature::AchievementType::HealthTraded);

        const sfml_util::gui::TextInfo TEXT_INFO(
            ss.str(),
            sfml_util::FontManager::Instance()->Font_Default1(),
            sfml_util::FontManager::Instance()->Size_Smallish(),
            sf::Color::White,
            sfml_util::Justified::Left);

        auto const DETAILVIEW_BOUNDS{ detailViewSprite_.getGlobalBounds() };

        const sf::FloatRect TEXT_RECT(
            DETAILVIEW_POS_LEFT_ + DETAILVIEW_INNER_PAD_,
            DETAILVIEW_BOUNDS.top + DETAILVIEW_BOUNDS.height - 20.0f,
            DETAILVIEW_WIDTH_ - (2.0f * DETAILVIEW_INNER_PAD_),
            (DETAILVIEW_HEIGHT_ - (DETAILVIEW_BOUNDS.top + DETAILVIEW_BOUNDS.height))
                - (2.0f * DETAILVIEW_INNER_PAD_));

        detailViewTextUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "InventoryStage'sDetailViewForCreatureAchievements", TEXT_INFO, TEXT_RECT);
    }

    const std::string InventoryStage::MakeTitleString(
        const creature::CreaturePtr_t CREATURE_PTR,
        const creature::AchievementType::Enum WHICH_ACHV) const
    {
        auto const & ACHIEVEMENT{ CREATURE_PTR->GetAchievements().Get(WHICH_ACHV) };

        if (ACHIEVEMENT.IsRoleInList(CREATURE_PTR->Role()))
        {
            std::ostringstream ss;
            ss << "\n"
               << ACHIEVEMENT.Name() << MakeTitleSeparatorString(WHICH_ACHV) << ACHIEVEMENT.Count()
               << MakeTitleCountNeededString(CREATURE_PTR, WHICH_ACHV);

            return ss.str();
        }
        else
        {
            return "";
        }
    }

    const std::string InventoryStage::MakeTitleCountNeededString(
        const creature::CreaturePtr_t CREATURE_PTR,
        const creature::AchievementType::Enum WHICH_ACHV) const
    {
        auto const & ACHIEVEMENTS{ CREATURE_PTR->GetAchievements() };
        auto const TITLE_PTR_OPT{ ACHIEVEMENTS.GetNextTitle(WHICH_ACHV) };

        if (TITLE_PTR_OPT)
        {
            auto const ACHV_COUNT_REQUIRED{ TITLE_PTR_OPT->Obj().AchievementCount() };
            auto const ACHV_COUNT_CURRENT{ ACHIEVEMENTS.Get(WHICH_ACHV).Count() };
            auto const NEEDED_COUNT{ ACHV_COUNT_REQUIRED - ACHV_COUNT_CURRENT };

            std::ostringstream ss;
            ss << ", need  " << NEEDED_COUNT << " more to achieve \"" << TITLE_PTR_OPT->Obj().Name()
               << "\"";

            return ss.str();
        }
        else
        {
            return "  (There are no more titles to earn)";
        }
    }

    const std::string InventoryStage::MakeTitleSeparatorString(
        const creature::AchievementType::Enum WHICH_ACHV) const
    {
        switch (WHICH_ACHV)
        {
            case creature::AchievementType::EnemiesFaced:
            {
                return ":               ";
            }
            case creature::AchievementType::MeleeHits:
            {
                return ":                    ";
            }
            case creature::AchievementType::BattlesSurvived:
            {
                return ":             ";
            }
            case creature::AchievementType::ProjectileHits:
            {
                return ":               ";
            }
            case creature::AchievementType::BeastMindLinks:
            {
                return ":          ";
            }
            case creature::AchievementType::DodgedStanding:
            {
                return ":  ";
            }
            case creature::AchievementType::DodgedFlying:
            {
                return ":      ";
            }
            case creature::AchievementType::LocksPicked:
            {
                return ":                  ";
            }
            case creature::AchievementType::BackstabsHits:
            {
                return ":                ";
            }
            case creature::AchievementType::SongsPlayed:
            {
                return ":                 ";
            }
            case creature::AchievementType::SpiritsLifted:
            {
                return ":                 ";
            }
            case creature::AchievementType::BeastRoars:
            {
                return ":                   ";
            }
            case creature::AchievementType::MoonHowls:
            {
                return ":                  ";
            }
            case creature::AchievementType::PackActions:
            {
                return ":                  ";
            }
            case creature::AchievementType::FlyingAttackHits:
            {
                return ":         ";
            }
            case creature::AchievementType::TurnsInFlight:
            {
                return ":              ";
            }
            case creature::AchievementType::SpellsCast:
            {
                return ":                    ";
            }
            case creature::AchievementType::HealthGiven:
            {
                return ":                 ";
            }
            case creature::AchievementType::HealthTraded:
            {
                return ":                ";
            }
            case creature::AchievementType::None:
            case creature::AchievementType::Count:
            default:
            {
                return "";
            }
        }
    }

    void InventoryStage::StartDetailViewFadeOutTasks()
    {
        sfml_util::SoundManager::Instance()->PlaySfx_TickOff();

        isDetailViewFadingIn_ = false;
        isDetailViewDoneFading_ = false;
        isDetailViewFadingOut_ = true;
        detailViewSlider_.Reset(DETAILVIEW_SLIDER_SPEED_);
        SetupDetailViewItem(boost::none);
        SetupDetailViewCreature(boost::none);
    }

    void InventoryStage::HandleDetailViewMouseInterrupt(const sf::Vector2f & MOUSE_POS_V)
    {
        if (isDetailViewDoneFading_)
        {
            StartDetailViewFadeOutTasks();
        }
        else
        {
            detailViewTimerSec_ = 0.0f;
            mousePosV_ = MOUSE_POS_V;
        }
    }

    bool InventoryStage::HandleCast_Step1_TargetSelection(const spell::SpellPtr_t SPELL_PTR)
    {
        spellBeingCastPtrOpt_ = SPELL_PTR;

        auto const SPELL_NAME{ SPELL_PTR->Name() };
        auto const SPELL_TARGET{ SPELL_PTR->Target() };

        if (SPELL_TARGET == combat::TargetType::SingleCompanion)
        {
            PopupCharacterSelectWindow("Cast " + SPELL_NAME + " on who?", true, true);
        }
        else if (SPELL_TARGET == combat::TargetType::AllCompanions)
        {
            HandleCast_Step2_PerformSpell(SPELL_PTR, creature::Algorithms::Players());
        }
        else
        {
            std::ostringstream ssErr;
            ssErr << "stage::InventoryStage::HandleCast Step2 SelectTargetOrPerformOnAll"
                  << "(spell=" << SPELL_NAME << ") had a target_type of "
                  << combat::TargetType::ToString(SPELL_TARGET)
                  << " which is not yet supported while in Inventory stage.";

            SystemErrorPopup(
                "Casting this type of spell is not yet supported from the inventory.", ssErr.str());
        }

        return false;
    }

    void InventoryStage::HandleCast_Step2_PerformSpell(
        const spell::SpellPtr_t SPELL_PTR, const creature::CreaturePVec_t & TARGET_CREATURES_PVEC)
    {
        combatSoundEffectsUPtr_->PlaySpell(SPELL_PTR);

        turnActionInfo_ = combat::TurnActionInfo(SPELL_PTR, TARGET_CREATURES_PVEC);

        fightResult_ = combat::FightClub::Cast(SPELL_PTR, creaturePtr_, TARGET_CREATURES_PVEC);

        Setup_CreatureDetails(false);
        Setup_CreatureStats();
        Setup_CenterText();

        creatureEffectIndex_ = 0;
        hitInfoIndex_ = 0;

        hasTakenActionSpellOrSong_ = true;

        sparkleAnimUPtr_ = std::make_unique<sfml_util::animation::SparkleAnimation>(
            creatureSprite_.getGlobalBounds(),
            sfml_util::MapByRes(0.333f, 1.33f),
            0.5f,
            10.0f,
            0.5f,
            4.0f,
            4.0f,
            0.5f);
    }

    bool InventoryStage::HandleCast_Step3_DisplayResults()
    {
        if (!spellBeingCastPtrOpt_)
        {
            return true;
        }

        bool isFightResultCollapsed{ false };
        auto const ACTION_TEXT{ combat::Text::ActionTextIndexed(
            creaturePtr_,
            turnActionInfo_,
            fightResult_,
            false,
            creatureEffectIndex_,
            hitInfoIndex_,
            isFightResultCollapsed) };

        auto const POPUPINFO_NOITEM{ popup::PopupManager::Instance()->CreatePopupInfo(
            POPUP_NAME_SPELL_RESULT_,
            ACTION_TEXT,
            popup::PopupButtons::Okay,
            popup::PopupImage::Regular,
            sfml_util::Justified::Center,
            sfml_util::sound_effect::None,
            sfml_util::FontManager::Instance()->Size_Normal()) };

        auto const CREATURE_EFFECTS_VEC{ fightResult_.Effects() };

        auto const HIT_INFO_VEC{ CREATURE_EFFECTS_VEC[creatureEffectIndex_].GetHitInfoVec() };

        if (isFightResultCollapsed || (++hitInfoIndex_ >= HIT_INFO_VEC.size()))
        {
            hitInfoIndex_ = 0;
            if (isFightResultCollapsed || (++creatureEffectIndex_ >= CREATURE_EFFECTS_VEC.size()))
            {
                spellBeingCastPtrOpt_ = boost::none;
            }
        }

        game::LoopManager::Instance()->PopupWaitBegin(this, POPUPINFO_NOITEM);
        isWaitingOnPopup_ = true;
        return false;
    }

    void InventoryStage::ForceSelectionAndDrawOfListBox()
    {
        sf::Event::KeyEvent keyEvent;

        keyEvent.code = sf::Keyboard::Down;
        equippedListBoxUPtr_->KeyRelease(keyEvent);

        keyEvent.code = sf::Keyboard::Up;
        equippedListBoxUPtr_->KeyRelease(keyEvent);
    }

    bool InventoryStage::HandleSpellsOrSongs()
    {
        auto const IS_SPELLS{ (creaturePtr_->Role() != creature::role::Bard) };

        if (game::Phase::Combat == currentPhase_)
        {
            if (creaturePtr_ != turnCreaturePtr_)
            {
                std::ostringstream ss;
                ss << "\nDuring combat, only the character whose turn it is may ";

                if (IS_SPELLS)
                {
                    ss << "cast spells.";
                }
                else
                {
                    ss << "play songs.";
                }

                PopupRejectionWindow(ss.str());

                // return false because one popup is replacing another
                return false;
            }
            else if (hasTakenActionSpellOrSong_)
            {
                std::ostringstream ss;
                ss << "\nDuring combat, the creature whose turn it is may only ";

                if (IS_SPELLS)
                {
                    ss << "cast one spell.";
                }
                else
                {
                    ss << "play one song.";
                }

                PopupRejectionWindow(ss.str());

                // return false because one popup is replacing another
                return false;
            }
        }

        if (IS_SPELLS)
        {
            auto const CAN_CAST_STR{ creaturePtr_->CanCastSpellsStr(true) };
            if (CAN_CAST_STR.empty())
            {
                auto const POPUP_INFO{ popup::PopupManager::Instance()->CreateSpellbookPopupInfo(
                    POPUP_NAME_SPELLBOOK_, creaturePtr_, creaturePtr_->LastSpellCastNum()) };

                game::LoopManager::Instance()->PopupWaitBeginSpecific<popup::PopupStageSpellbook>(
                    this, POPUP_INFO);
            }
            else
            {
                PopupRejectionWindow(CAN_CAST_STR);
            }
        }
        else
        {
            auto const CAN_PLAY_STR{ creaturePtr_->CanPlaySongsStr(true) };
            if (CAN_PLAY_STR.empty())
            {
                auto const POPUP_INFO{ popup::PopupManager::Instance()->CreateMusicPopupInfo(
                    POPUP_NAME_MUSICSHEET_, creaturePtr_, creaturePtr_->LastSongPlayedNum()) };

                game::LoopManager::Instance()->PopupWaitBeginSpecific<popup::PopupStageMusicSheet>(
                    this, POPUP_INFO);
            }
            else
            {
                PopupRejectionWindow(CAN_PLAY_STR);
            }
        }

        // return false because one popup is replacing another
        return false;
    }

    bool InventoryStage::HandleSong_Step1_Play(const song::SongPtr_t SONG_PTR)
    {
        if (SONG_PTR->Target() == combat::TargetType::QuestSpecific)
        {
            std::ostringstream ssErr;
            ssErr << "stage::InventoryStage::HandleSong"
                  << "(song=" << SONG_PTR->Name() << ") had a target_type of "
                  << combat::TargetType::ToString(SONG_PTR->Target())
                  << " which is not yet supported while in Inventory stage.";

            SystemErrorPopup(
                "Playing this type of song is not yet supported from the inventory.", ssErr.str());

            // return false because one popup is replacing another
            return false;
        }
        else
        {
            songBeingPlayedPtrOpt_ = SONG_PTR;
            combatSoundEffectsUPtr_->PlaySong(SONG_PTR);

            auto const TARGETS_PVEC{ (
                (SONG_PTR->Target() == combat::TargetType::AllCompanions)
                    ? creature::Algorithms::Players()
                    : creature::Algorithms::NonPlayers(creature::Algorithms::Living)) };

            turnActionInfo_ = combat::TurnActionInfo(SONG_PTR, TARGETS_PVEC);
            fightResult_ = combat::FightClub::PlaySong(SONG_PTR, creaturePtr_, TARGETS_PVEC);

            Setup_CreatureDetails(false);
            Setup_CreatureStats();
            Setup_CenterText();

            creatureEffectIndex_ = 0;
            hitInfoIndex_ = 0;

            hasTakenActionSpellOrSong_ = true;

            songAnimUPtr_ = std::make_unique<sfml_util::animation::SongAnimation>(
                creatureSprite_.getGlobalBounds(),
                0.1f,
                sfml_util::MapByRes(0.1f, 0.25f),
                0.25f,
                sfml_util::MapByRes(0.35f, 0.9f),
                0.25f,
                6.0f,
                0.75f,
                3.5f,
                1.25f,
                0.5f,
                1.0f,
                0.0f);

            // return true, because NOT displaying another popup
            return true;
        }
    }

    bool InventoryStage::HandleSong_Step2_DisplayResults()
    {
        if (!songBeingPlayedPtrOpt_)
        {
            // return true, because NOT displaying another popup
            return true;
        }

        bool isFightResultCollapsed{ false };
        auto const ACTION_TEXT{ combat::Text::ActionTextIndexed(
            creaturePtr_,
            turnActionInfo_,
            fightResult_,
            false,
            creatureEffectIndex_,
            hitInfoIndex_,
            isFightResultCollapsed) };

        auto const POPUPINFO_NOITEM{ popup::PopupManager::Instance()->CreatePopupInfo(
            POPUP_NAME_SONG_RESULT_,
            ACTION_TEXT,
            popup::PopupButtons::Okay,
            popup::PopupImage::Regular,
            sfml_util::Justified::Center,
            sfml_util::sound_effect::None,
            sfml_util::FontManager::Instance()->Size_Normal()) };

        auto const CREATURE_EFFECTS_VEC{ fightResult_.Effects() };
        auto const HIT_INFO_VEC{ CREATURE_EFFECTS_VEC[creatureEffectIndex_].GetHitInfoVec() };

        if (isFightResultCollapsed || (++hitInfoIndex_ >= HIT_INFO_VEC.size()))
        {
            hitInfoIndex_ = 0;

            if (isFightResultCollapsed || (++creatureEffectIndex_ >= CREATURE_EFFECTS_VEC.size()))
            {
                songBeingPlayedPtrOpt_ = boost::none;
            }
        }

        game::LoopManager::Instance()->PopupWaitBegin(this, POPUPINFO_NOITEM);
        isWaitingOnPopup_ = true;

        // return false because one popup is replacing another
        return false;
    }

    void InventoryStage::SystemErrorPopup(
        const std::string & GENERAL_ERROR_MSG,
        const std::string & TECH_ERROR_MSG,
        const std::string & TITLE_MSG)
    {
        game::LoopManager::Instance()->PopupWaitBegin(
            this,
            popup::PopupManager::Instance()->CreateSystemErrorPopupInfo(
                "Stage'sSystemErrorPopupName", GENERAL_ERROR_MSG, TECH_ERROR_MSG, TITLE_MSG));
    }

    void InventoryStage::SetDetailViewQuads()
    {
        auto const SOURCE_LEFT{ detailViewSourceRect_.left };
        auto const SOURCE_RGT{ detailViewSourceRect_.left + detailViewSourceRect_.width };
        auto const SOURCE_TOP{ detailViewSourceRect_.top };
        auto const SOURCE_BOTTOM{ detailViewSourceRect_.top + detailViewSourceRect_.height };

        auto const TARGET_LEFT{ DETAILVIEW_POS_LEFT_ };
        auto const TARGET_RGT{ DETAILVIEW_POS_LEFT_ + DETAILVIEW_WIDTH_ };
        auto const TARGET_TOP{ DETAILVIEW_POS_TOP_ };
        auto const TARGET_BOTTOM{ DETAILVIEW_POS_TOP_ + DETAILVIEW_HEIGHT_ };

        auto const RATIO{ detailViewSliderRatio_ };

        auto const LEFT_MORE{ SOURCE_LEFT - ((SOURCE_LEFT - TARGET_LEFT) * RATIO) };
        auto const LEFT_LESS{ SOURCE_LEFT + ((TARGET_LEFT - SOURCE_LEFT) * RATIO) };
        auto const LEFT{ (SOURCE_LEFT > TARGET_LEFT) ? LEFT_MORE : LEFT_LESS };

        auto const RGT_MORE{ SOURCE_RGT - ((SOURCE_RGT - TARGET_RGT) * RATIO) };
        auto const RGT_LESS{ SOURCE_RGT + ((TARGET_RGT - SOURCE_RGT) * RATIO) };
        auto const RGT{ (SOURCE_RGT > TARGET_RGT) ? RGT_MORE : RGT_LESS };

        auto const TOP{ SOURCE_TOP - ((SOURCE_TOP - TARGET_TOP) * RATIO) };

        auto const BOTTOM_MORE{ SOURCE_BOTTOM - ((SOURCE_BOTTOM - TARGET_BOTTOM) * RATIO) };
        auto const BOTTOM_LESS{ SOURCE_BOTTOM + ((TARGET_BOTTOM - SOURCE_BOTTOM) * RATIO) };
        auto const BOTTOM{ (SOURCE_BOTTOM > TARGET_BOTTOM) ? BOTTOM_MORE : BOTTOM_LESS };

        detailViewQuads_[0].position = sf::Vector2f(LEFT, TOP);
        detailViewQuads_[1].position = sf::Vector2f(RGT, TOP);
        detailViewQuads_[2].position = sf::Vector2f(RGT, BOTTOM);
        detailViewQuads_[3].position = sf::Vector2f(LEFT, BOTTOM);
    }

    void InventoryStage::SortByName(sfml_util::gui::ListBox & listbox, bool & isSortReversed)
    {
        auto vec{ listbox.Items() };

        std::sort(std::begin(vec), std::end(vec), [isSortReversed](auto & A, auto & B) {
            if (isSortReversed)
            {
                return A->ITEM_PTR_OPT->Obj().Name() > B->ITEM_PTR_OPT->Obj().Name();
            }
            else
            {
                return A->ITEM_PTR_OPT->Obj().Name() < B->ITEM_PTR_OPT->Obj().Name();
            }
        });

        listbox.Items(vec);
        isSortReversed = !isSortReversed;
    }

    void InventoryStage::SortByPrice(sfml_util::gui::ListBox & listbox, bool & isSortReversed)
    {
        auto vec{ listbox.Items() };

        std::sort(std::begin(vec), std::end(vec), [isSortReversed](auto & A, auto & B) {
            if (isSortReversed)
            {
                return A->ITEM_PTR_OPT->Obj().Price() > B->ITEM_PTR_OPT->Obj().Price();
            }
            else
            {
                return A->ITEM_PTR_OPT->Obj().Price() < B->ITEM_PTR_OPT->Obj().Price();
            }
        });

        listbox.Items(vec);
        isSortReversed = !isSortReversed;
    }

    void InventoryStage::SortByWeight(sfml_util::gui::ListBox & listbox, bool & isSortReversed)
    {
        auto vec{ listbox.Items() };

        std::sort(std::begin(vec), std::end(vec), [isSortReversed](auto & A, auto & B) {
            if (isSortReversed)
            {
                return A->ITEM_PTR_OPT->Obj().Weight() > B->ITEM_PTR_OPT->Obj().Weight();
            }
            else
            {
                return A->ITEM_PTR_OPT->Obj().Weight() < B->ITEM_PTR_OPT->Obj().Weight();
            }
        });

        listbox.Items(vec);
        isSortReversed = !isSortReversed;
    }

    bool InventoryStage::IfMouseDownIsOnDisabledButtonPopupRejection(
        const sfml_util::gui::FourStateButtonUPtr_t & BUTTON_UPTR, const sf::Vector2f & MOUSE_POS_V)
    {
        if (BUTTON_UPTR->IsDisabled() && BUTTON_UPTR->GetEntityRegion().contains(MOUSE_POS_V))
        {
            PopupRejectionWindow(BUTTON_UPTR->GetMouseHoverText());
            return true;
        }
        else
        {
            return false;
        }
    }

} // namespace stage
} // namespace heroespath
