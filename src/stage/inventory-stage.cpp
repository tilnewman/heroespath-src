// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// inventory-stage.hpp
//
#include "inventory-stage.hpp"

#include "combat/combat-sound-effects.hpp"
#include "combat/combat-text.hpp"
#include "creature/algorithms.hpp"
#include "creature/condition.hpp"
#include "creature/creature.hpp"
#include "creature/player-party.hpp"
#include "game/game-state.hpp"
#include "game/game.hpp"
#include "gui/box-entity.hpp"
#include "gui/cached-texture.hpp"
#include "gui/display.hpp"
#include "gui/font-manager.hpp"
#include "gui/image-loaders.hpp"
#include "gui/list-box-helpers.hpp"
#include "gui/ouroboros.hpp"
#include "gui/song-animation.hpp"
#include "gui/sound-manager.hpp"
#include "gui/sparkle-animation.hpp"
#include "gui/text-info.hpp"
#include "gui/text-region.hpp"
#include "item/item-warehouse.hpp"
#include "item/item.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/config-file.hpp"
#include "misc/real.hpp"
#include "popup/popup-manager.hpp"
#include "popup/popup-response.hpp"
#include "popup/popup-stage-char-select.hpp"
#include "popup/popup-stage-inventory-prompt.hpp"
#include "popup/popup-stage-musicsheet.hpp"
#include "popup/popup-stage-num-select.hpp"
#include "popup/popup-stage-spellbook.hpp"
#include "song/song.hpp"
#include "spell/spell.hpp"
#include "stage/stage-setup-packet.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include <algorithm>

namespace heroespath
{
namespace stage
{

    const float InventoryStage::VIEW_CHANGE_SLIDER_SPEED_ { 4.0f };
    const float InventoryStage::VIEW_CHANGE_BETWEEN_TIME_SEC_ { 0.5f };
    const float InventoryStage::CREATURE_IMAGE_RIGHT_PAD_ { 10.0f };
    const float InventoryStage::DETAILVIEW_SLIDER_SPEED_ { 4.0f };
    const float InventoryStage::DETAILVIEW_TIMER_DURATION_SEC_ { 2.33f };
    const float InventoryStage::DETAILVIEW_COLOR_ALPHA_START_ { 20.0f };
    const float InventoryStage::DETAILVIEW_COLOR_ALPHA_END_ { 220.0f };
    const float InventoryStage::DETAILVIEW_INNER_PAD_ { 20.0f };

    const std::string InventoryStage::POPUP_NAME_GIVE_ { "InventoryStage'sGivePopupName" };

    const std::string InventoryStage::POPUP_NAME_CHAR_SELECT_ {
        "InventoryStage'sCharacterSelectPopupName"
    };

    const std::string InventoryStage::POPUP_NAME_NUMBER_SELECT_ {
        "InventoryStage'sPopupNumberSelectPopupName"
    };

    const std::string InventoryStage::POPUP_NAME_CONTENTSELECTION_ {
        "InventoryStage'sShareGatherSelectionPopupName"
    };

    const std::string InventoryStage::POPUP_NAME_DROPCONFIRM_ {
        "InventoryStage'sDropItemConfirmationPopupName"
    };

    const std::string InventoryStage::POPUP_NAME_SPELLBOOK_ {
        "InventoryStage'sSpellbookPopupName"
    };

    const std::string InventoryStage::POPUP_NAME_SPELL_RESULT_ {
        "InventoryStage'sSpellResultPopupName"
    };

    const std::string InventoryStage::POPUP_NAME_MUSICSHEET_ {
        "InventoryStage'sMusicSheetPopupName"
    };

    const std::string InventoryStage::POPUP_NAME_SONG_RESULT_ {
        "InventoryStage'sSongResultPopupName"
    };

    InventoryStage::InventoryStage(
        const creature::CreaturePtr_t TURN_CREATURE_PTR,
        const creature::CreaturePtr_t INVENTORY_CREATURE_PTR,
        const game::Phase::Enum PREVIOUS_PHASE)
        : StageBase(
            "Inventory",
            { gui::GuiFont::Default,
              gui::GuiFont::System,
              gui::GuiFont::SystemCondensed,
              gui::GuiFont::Number,
              gui::GuiFont::Handwriting },
            true)
        , SCREEN_WIDTH_(gui::Display::Instance()->GetWinWidth())
        , SCREEN_HEIGHT_(gui::Display::Instance()->GetWinHeight())
        , INNER_PAD_(sfutil::MapByRes(10.0f, 40.0f))
        , INNER_RECT_(
              INNER_PAD_,
              INNER_PAD_,
              SCREEN_WIDTH_ - (2.0f * INNER_PAD_),
              SCREEN_HEIGHT_ - (2.0f * INNER_PAD_))
        , CREATURE_IMAGE_POS_LEFT_(INNER_RECT_.left + sfutil::MapByRes(35.0f, 100.0f))
        , CREATURE_IMAGE_SCALE_(sfutil::MapByRes(0.75f, 3.25f))
        , CREATURE_IMAGE_HEIGHT_MAX_(gui::ContentImage::Dimmension() * CREATURE_IMAGE_SCALE_)
        , LISTBOX_HEIGHT_REDUCTION_(sfutil::MapByRes(100.0f, 400.0f))
        , LISTBOX_SCREEN_EDGE_MARGIN_(sfutil::MapByRes(35.0f, 100.0f))
        , LISTBOX_BETWEEN_SPACER_(sfutil::MapByRes(65.0f, 200.0f))
        , LISTBOX_WIDTH_(
              ((INNER_RECT_.width - (2.0f * LISTBOX_SCREEN_EDGE_MARGIN_)) - LISTBOX_BETWEEN_SPACER_)
              * 0.5f)
        , OUT_OF_SIGHT_POS_((LISTBOX_WIDTH_ + sfutil::MapByRes(100.0f, 300.0f)) * -1.0f)
        , FIRST_LISTBOX_POS_LEFT_(INNER_RECT_.left + LISTBOX_SCREEN_EDGE_MARGIN_)
        , SECOND_LISTBOX_POS_LEFT_(
              FIRST_LISTBOX_POS_LEFT_ + LISTBOX_WIDTH_ + LISTBOX_BETWEEN_SPACER_)
        , STATS_POS_LEFT_(
              (INNER_RECT_.left + INNER_RECT_.width) - sfutil::MapByRes(275.0f, 1200.0f))
        , CHARATER_IMAGE_COLOR_(sf::Color(255, 255, 255, 127))
        , LISTBOX_COLOR_IMAGE_(sf::Color(255, 255, 255, 190))
        , LISTBOX_COLOR_LINE_(sfutil::color::GrayDark)
        , LISTBOX_COLOR_FG_(LISTBOX_COLOR_LINE_)
        , LISTBOX_COLOR_BG_(sfutil::color::Orange - sf::Color(100, 100, 100, 220))
        , LISTBOX_COLOR_TITLE_(sfutil::color::Orange - sf::Color(130, 130, 130, 0))
        , DESCBOX_TEXT_COLOR_(LISTBOX_COLOR_TITLE_ - sf::Color(50, 50, 50, 0))
        , DESCBOX_TEXT_SIZE_(gui::FontManager::Instance()->Size_Largeish())
        , LISTBOX_COLORSET_(LISTBOX_COLOR_FG_, LISTBOX_COLOR_BG_)
        , stageTitle_(
              "media-images-buttons-gui-inventory-normal",
              true,
              0.0f,
              sfutil::ScreenRatioToPixelsVert(0.12f))
        , CREATURE_IMAGE_POS_TOP_(sfutil::Bottom(stageTitle_.Region()))
        , LISTBOX_POS_TOP_(
              (CREATURE_IMAGE_POS_TOP_ + CREATURE_IMAGE_HEIGHT_MAX_)
              + (LISTBOX_HEIGHT_REDUCTION_ * 0.5f))
        , LISTBOX_HEIGHT_((SCREEN_HEIGHT_ - LISTBOX_POS_TOP_) - LISTBOX_HEIGHT_REDUCTION_)
        , LISTBOX_REGION_LEFT_(
              FIRST_LISTBOX_POS_LEFT_, LISTBOX_POS_TOP_, LISTBOX_WIDTH_, LISTBOX_HEIGHT_)
        , LISTBOX_REGION_RIGHT_(
              SECOND_LISTBOX_POS_LEFT_, LISTBOX_POS_TOP_, LISTBOX_WIDTH_, LISTBOX_HEIGHT_)
        , DESCBOX_MARGIN_(15.0f)
        , DESCBOX_MARGINS_(DESCBOX_MARGIN_, DESCBOX_MARGIN_, DESCBOX_MARGIN_, DESCBOX_MARGIN_)
        , DETAILVIEW_WIDTH_(SCREEN_WIDTH_ * 0.75f)
        , DETAILVIEW_HEIGHT_(SCREEN_HEIGHT_ * 0.85f)
        , DETAILVIEW_POS_LEFT_((SCREEN_WIDTH_ * 0.5f) - (DETAILVIEW_WIDTH_ * 0.5f))
        , DETAILVIEW_POS_TOP_(sfutil::MapByRes(35.0f, 100.0f))
        , SORT_ICON_SCALE_(sfutil::MapByRes(0.1f, 0.35f))
        , SORT_ICON_SPACER_(sfutil::MapByRes(22.0f, 75.0f))
        , SORT_ICON_POS_TOP_(
              (LISTBOX_POS_TOP_ - (127.0f * SORT_ICON_SCALE_)) - sfutil::MapByRes(10.0f, 20.0f))
        , SORT_ICON_COLOR_(sf::Color(255, 255, 255, 127))
        , LIST_ELEMENT_TEXT_INFO_DEFAULT_(
              "",
              gui::GuiFont::System,
              gui::FontManager::Instance()->Size_Smallish(),
              sfutil::color::GrayDarker,
              gui::Justified::Left)
        , creaturePtr_(INVENTORY_CREATURE_PTR)
        , bottomSymbol_(0.75f, true, sf::Color::White)
        , paperBgCachedTexture_("media-images-backgrounds-paper-2")
        , paperBgSprite_(paperBgCachedTexture_.Get())
        , ouroborosUPtr_(std::make_unique<gui::Ouroboros>("InventoryStage's", true))
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
        , itemLeftListBoxUPtr_()
        , condLeftListBoxUPtr_()
        , spellLeftListBoxUPtr_()
        , titleLeftListBoxUPtr_()
        , itemRightListBoxUPtr_()
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
        , listElementToGivePtrOpt_()
        , creatureToGiveToPtrOpt_()
        , listElementToDropPtrOpt_()
        , isDetailViewFadingIn_(false)
        , isDetailViewFadingOut_(false)
        , isDetailViewDoneFading_(false)
        , isAchievementDisplaying_(false)
        , hasMouseMoved_(false)
        , isWaitingOnPopup_(false)
        , detailViewTimerSec_(0.0f)
        , detailViewSliderRatio_(0.0f)
        , mousePosV_(0.0f, 0.0f)
        , detailViewSourceRect_(0.0f, 0.0f, 0.0f, 0.0f)
        , detailViewQuads_(sf::Quads, 4)
        , detailViewSprite_()
        , detailViewCachedTextureOpt_()
        , detailViewTextUPtr_()
        , detailViewSlider_(DETAILVIEW_SLIDER_SPEED_)
        , spellBeingCastPtrOpt_()
        , songBeingPlayedPtrOpt_()
        , turnActionInfo_()
        , fightResult_()
        , creatureEffectIndex_(0)
        , hitInfoIndex_(0)
        , combatSoundEffectsUPtr_(std::make_unique<combat::CombatSoundEffects>())
        , sparkleAnimUPtr_()
        , songAnimUPtr_()
        , turnCreaturePtr_(TURN_CREATURE_PTR)
        , previousPhase_(PREVIOUS_PHASE)
        , hasTakenActionSpellOrSong_(false)
        , creatureInteraction_()
        , creatureToImageMap_()
        , creatureImageWidthScaled_(0.0f)
    {
        gui::SoundManager::Instance()->MusicStart(gui::music::Inventory);
        UpdateCreatureImage();
    }

    InventoryStage::~InventoryStage()
    {
        gui::SoundManager::Instance()->MusicStart(gui::music::Inventory);
        StageBase::ClearAllEntities();
    }

    const std::string InventoryStage::HandleCallback(
        const CondListBox_t::Callback_t::Packet_t & PACKET, const std::string & PACKET_DESCRIPTION)
    {
        return HandleConditionOrTitleCallback<creature::ConditionPtr_t>(PACKET, PACKET_DESCRIPTION);
    }

    const std::string InventoryStage::HandleCallback(
        const TitleListBox_t::Callback_t::Packet_t & PACKET, const std::string & PACKET_DESCRIPTION)
    {
        return HandleConditionOrTitleCallback<creature::TitlePtr_t>(PACKET, PACKET_DESCRIPTION);
    }

    const std::string InventoryStage::HandleCallback(
        const ItemListBox_t::Callback_t::Packet_t & PACKET, const std::string & PACKET_DESCRIPTION)
    {
        if ((PACKET.gui_event != gui::GuiEvent::DoubleClick)
            && (PACKET.keypress_event.code != sf::Keyboard::Return))
        {
            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION,
                "item listbox callback ignored because it was not a double-click or "
                "keypress-return event");
        }

        if (PACKET.listbox_ptr == itemLeftListBoxUPtr_.get())
        {
            HandleUnequipRequest();

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION,
                "left listbox (equipped item list) double-click or keypress-enter triggered "
                "unequip attempt");
        }
        else if (PACKET.listbox_ptr == itemRightListBoxUPtr_.get())
        {
            HandleEquipRequest();

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION,
                "right listbox (unequipped item list) double-click or keypress-enter triggered "
                "equip attempt");
        }

        return MakeCallbackHandlerMessage(
            PACKET_DESCRIPTION, "unknown listbox callback NOT HANDLED");
    }

    const std::string InventoryStage::HandleCallback(
        const gui::ImageTextEntity::Callback_t::Packet_t & PACKET,
        const std::string & PACKET_DESCRIPTION)
    {
        if (isSliderAnimating_)
        {
            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION,
                "image-text-entity callback ignored because animations are moving");
        }

        if (PACKET.entity_ptr == backButtonUPtr_.get())
        {
            HandleBack();
            return MakeCallbackHandlerMessage(PACKET_DESCRIPTION, "back button click");
        }

        if (PACKET.entity_ptr == itemsButtonUPtr_.get())
        {
            HandleViewChange(ViewType::Items);
            return MakeCallbackHandlerMessage(PACKET_DESCRIPTION, "items button click");
        }

        if (PACKET.entity_ptr == titlesButtonUPtr_.get())
        {
            HandleViewChange(ViewType::Titles);
            return MakeCallbackHandlerMessage(PACKET_DESCRIPTION, "titles button click");
        }

        if (PACKET.entity_ptr == condsButtonUPtr_.get())
        {
            HandleViewChange(ViewType::Conditions);
            return MakeCallbackHandlerMessage(PACKET_DESCRIPTION, "conditions button click");
        }

        if (PACKET.entity_ptr == spellsButtonUPtr_.get())
        {
            HandleSpellsOrSongs();
            return MakeCallbackHandlerMessage(PACKET_DESCRIPTION, "spells button click");
        }

        if (PACKET.entity_ptr == giveButtonUPtr_.get())
        {
            HandleGiveRequestInitial();
            return MakeCallbackHandlerMessage(PACKET_DESCRIPTION, "give button click");
        }

        if (PACKET.entity_ptr == shareButtonUPtr_.get())
        {
            HandleShare();
            return MakeCallbackHandlerMessage(PACKET_DESCRIPTION, "share button click");
        }

        if (PACKET.entity_ptr == gatherButtonUPtr_.get())
        {
            HandleGather();
            return MakeCallbackHandlerMessage(PACKET_DESCRIPTION, "gather button click");
        }

        if (PACKET.entity_ptr == equipButtonUPtr_.get())
        {
            HandleEquipActual();
            return MakeCallbackHandlerMessage(PACKET_DESCRIPTION, "equip button click");
        }

        if (PACKET.entity_ptr == unequipButtonUPtr_.get())
        {
            HandleUnequipActual();
            return MakeCallbackHandlerMessage(PACKET_DESCRIPTION, "unequip button click");
        }

        if (PACKET.entity_ptr == dropButtonUPtr_.get())
        {
            HandleDropRequest();
            return MakeCallbackHandlerMessage(PACKET_DESCRIPTION, "drop button click");
        }

        if ((PACKET.entity_ptr == eqSortButtonNameUPtr_.get()) && itemLeftListBoxUPtr_)
        {
            gui::listbox::SortByName(*itemLeftListBoxUPtr_, isSortReversedEqName_);

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION, "equipped-listbox sort-by-name button click");
        }

        if ((PACKET.entity_ptr == eqSortButtonPriceUPtr_.get()) && itemLeftListBoxUPtr_)
        {
            gui::listbox::SortByPrice(*itemLeftListBoxUPtr_, isSortReversedEqPrice_);

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION, "equipped-listbox sort-by-price button click");
        }

        if ((PACKET.entity_ptr == eqSortButtonWeightUPtr_.get()) && itemLeftListBoxUPtr_)
        {
            gui::listbox::SortByWeight(*itemLeftListBoxUPtr_, isSortReversedEqWeight_);

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION, "equipped-listbox sort-by-weight button click");
        }

        if ((PACKET.entity_ptr == unEqSortButtonNameUPtr_.get()) && itemRightListBoxUPtr_)
        {
            gui::listbox::SortByName(*itemRightListBoxUPtr_, isSortReversedUneqName_);

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION, "unequipped-listbox sort-by-name button click");
        }

        if ((PACKET.entity_ptr == unEqSortButtonPriceUPtr_.get()) && itemRightListBoxUPtr_)
        {
            gui::listbox::SortByPrice(*itemRightListBoxUPtr_, isSortReversedUneqPrice_);

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION, "unequipped-listbox sort-by-price button click");
        }

        if ((PACKET.entity_ptr == unEqSortButtonWeightUPtr_.get()) && itemRightListBoxUPtr_)
        {
            gui::listbox::SortByWeight(*itemRightListBoxUPtr_, isSortReversedUneqWeight_);

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION, "unequipped-listbox sort-by-weight button click");
        }

        return MakeCallbackHandlerMessage(
            PACKET_DESCRIPTION, "image-text-entity callback NOT HANDLED");
    }

    const std::string InventoryStage::HandleCallback(
        const misc::PopupCallback_t::Packet_t & PACKET, const std::string & PACKET_DESCRIPTION)
    {
        isWaitingOnPopup_ = false;

        if (PACKET.curently_open_popup_name == POPUP_NAME_SONG_RESULT_)
        {
            HandleSong_Step2_DisplayResults();

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION,
                "closed play bard song popup and spawned the play bard song result popup");
        }
        else if (PACKET.curently_open_popup_name == POPUP_NAME_SPELL_RESULT_)
        {
            HandleCast_Step3_DisplayResults();

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION, "closed cast spell result popup and spawned next spell popup");
        }
        else if (
            (PACKET.curently_open_popup_name == POPUP_NAME_DROPCONFIRM_)
            && (PACKET.type == popup::PopupButtons::Yes))
        {
            HandleDropActual();

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION,
                "closed drop item confirmation popup by clicking yes so the item has been dropped");
        }
        else if (
            (PACKET.curently_open_popup_name == POPUP_NAME_GIVE_)
            && (PACKET.type == popup::PopupButtons::Select) && PACKET.selection_opt)
        {
            if (PACKET.selection_opt.value() == popup::PopupInfo::ContentNum_Item())
            {
                HandleGiveRequestItems();

                return MakeCallbackHandlerMessage(
                    PACKET_DESCRIPTION,
                    "closed give popup by selecting items and the next popup spawned");
            }
            else if (PACKET.selection_opt.value() == popup::PopupInfo::ContentNum_Coins())
            {
                HandleGiveRequestCoins();

                return MakeCallbackHandlerMessage(
                    PACKET_DESCRIPTION,
                    "closed give popup by selecting coins and the next popup spawned");
            }
            else if (PACKET.selection_opt.value() == popup::PopupInfo::ContentNum_Gems())
            {
                HandleGiveRequestGems();

                return MakeCallbackHandlerMessage(
                    PACKET_DESCRIPTION,
                    "closed give popup by selecting gems and the next popup spawned");
            }
            else if (PACKET.selection_opt.value() == popup::PopupInfo::ContentNum_MeteorShards())
            {
                HandleGiveRequestMeteorShards();

                return MakeCallbackHandlerMessage(
                    PACKET_DESCRIPTION,
                    "closed give popup by selecting meteor shards and the next popup spawned");
            }
        }
        else if (
            (PACKET.curently_open_popup_name == POPUP_NAME_CHAR_SELECT_)
            && (PACKET.type == popup::PopupButtons::Select) && PACKET.selection_opt)
        {
            creatureToGiveToPtrOpt_ = game::Game::Instance()->State().Party().GetAtOrderPos(
                PACKET.selection_opt.value());

            const auto CREATURE_TO_GIVE_TO_PTR { creatureToGiveToPtrOpt_.value() };

            if (spellBeingCastPtrOpt_)
            {
                creature::CreaturePVec_t spellTargetCreaturePVec;
                spellTargetCreaturePVec.emplace_back(CREATURE_TO_GIVE_TO_PTR);

                HandleCast_Step2_PerformSpell(
                    spellBeingCastPtrOpt_.value(), spellTargetCreaturePVec);

                return MakeCallbackHandlerMessage(
                    PACKET_DESCRIPTION,
                    "closed who-will-cast popup by selecting a valid character and started cast "
                    "spell process");
            }

            switch (contentType_)
            {
                case ContentType::Item:
                {
                    HandleGiveActualItems(CREATURE_TO_GIVE_TO_PTR);

                    return MakeCallbackHandlerMessage(
                        PACKET_DESCRIPTION,
                        "closed who-to-give-item-to popup and attempted to give");
                }
                case ContentType::Coins:
                {
                    HandleGiveActualCoins(CREATURE_TO_GIVE_TO_PTR);

                    return MakeCallbackHandlerMessage(
                        PACKET_DESCRIPTION,
                        "closed who-to-give-coins-to popup and attempted to give");
                }
                case ContentType::Gems:
                {
                    HandleGiveActualGems(CREATURE_TO_GIVE_TO_PTR);

                    return MakeCallbackHandlerMessage(
                        PACKET_DESCRIPTION,
                        "closed who-to-give-gems-to popup and attempted to give");
                }
                case ContentType::MeteorShards:
                {
                    HandleGiveActualMeteorShards(CREATURE_TO_GIVE_TO_PTR);

                    return MakeCallbackHandlerMessage(
                        PACKET_DESCRIPTION,
                        "closed who-to-give-meteor-shards-to popup and attempted to give");
                }
                case ContentType::Count:
                default:
                {
                    break;
                }
            }

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION, "closed character-select popup but NOT HANDLED");
        }
        else if (
            (PACKET.curently_open_popup_name == POPUP_NAME_NUMBER_SELECT_)
            && (PACKET.type == popup::PopupButtons::Select) && creatureToGiveToPtrOpt_
            && PACKET.selection_opt)
        {
            const auto CREATURE_TO_GIVE_TO_PTR { creatureToGiveToPtrOpt_.value() };

            switch (contentType_)
            {
                case ContentType::MeteorShards:
                {
                    HandleMeteorShardsGive(PACKET.selection_opt.value(), CREATURE_TO_GIVE_TO_PTR);

                    return MakeCallbackHandlerMessage(
                        PACKET_DESCRIPTION,
                        "closed how-many-meteor-shards-select popup and attempted to give");
                }
                case ContentType::Coins:
                {
                    HandleCoinsGive(PACKET.selection_opt.value(), CREATURE_TO_GIVE_TO_PTR);

                    return MakeCallbackHandlerMessage(
                        PACKET_DESCRIPTION,
                        "closed how-many-coins-select popup and attempted to give");
                }
                case ContentType::Gems:
                {
                    HandleGemsGive(PACKET.selection_opt.value(), CREATURE_TO_GIVE_TO_PTR);

                    return MakeCallbackHandlerMessage(
                        PACKET_DESCRIPTION,
                        "closed how-many-gems-select popup and attempted to give");
                }
                case ContentType::Item:
                case ContentType::Count:
                default:
                {
                    return MakeCallbackHandlerMessage(
                        PACKET_DESCRIPTION,
                        "closed how-many-(? of something)-select popup NOT HANDLED");
                }
            }
        }
        else if (
            (PACKET.curently_open_popup_name == POPUP_NAME_CONTENTSELECTION_)
            && (PACKET.type == popup::PopupButtons::Select) && PACKET.selection_opt)
        {
            if (PACKET.selection_opt.value() == popup::PopupInfo::ContentNum_Coins())
            {
                if (ActionType::Gather == actionType_)
                {
                    HandleCoinsGather(true);

                    return MakeCallbackHandlerMessage(
                        PACKET_DESCRIPTION,
                        "closed content-selection-coins popup by selecting gather");
                }
                else if (ActionType::Share == actionType_)
                {
                    HandleCoinsShare();

                    return MakeCallbackHandlerMessage(
                        PACKET_DESCRIPTION,
                        "closed content-selection-coins popup by selecting coins share");
                }

                return MakeCallbackHandlerMessage(
                    PACKET_DESCRIPTION, "closed content-selection-coins popup NOT HANDLED");
            }
            else if (PACKET.selection_opt.value() == popup::PopupInfo::ContentNum_Gems())
            {
                if (ActionType::Gather == actionType_)
                {
                    HandleGemsGather(true);

                    return MakeCallbackHandlerMessage(
                        PACKET_DESCRIPTION,
                        "closed content-selection-gems popup by selecting gather");
                }
                else if (ActionType::Share == actionType_)
                {
                    HandleGemsShare();
                    return MakeCallbackHandlerMessage(
                        PACKET_DESCRIPTION,
                        "closed content-selection-gems popup by selecting share");
                }

                return MakeCallbackHandlerMessage(
                    PACKET_DESCRIPTION, "closed content-selection-gems popup NOT HANDLED");
            }
            else if (PACKET.selection_opt.value() == popup::PopupInfo::ContentNum_MeteorShards())
            {
                if (ActionType::Gather == actionType_)
                {
                    HandleMeteorShardsGather(true);

                    return MakeCallbackHandlerMessage(
                        PACKET_DESCRIPTION,
                        "closed content-selection-shards popup by selecting gather");
                }
                else if (ActionType::Share == actionType_)
                {
                    HandleMeteorShardsShare();

                    return MakeCallbackHandlerMessage(
                        PACKET_DESCRIPTION,
                        "closed content-selection-shards popup by selecting share");
                }

                return MakeCallbackHandlerMessage(
                    PACKET_DESCRIPTION, "closed content-selection-gems popup NOT HANDLED");
            }
        }
        else if (
            (PACKET.curently_open_popup_name == POPUP_NAME_SPELLBOOK_)
            && (PACKET.type == popup::PopupButtons::Select) && PACKET.selection_opt)
        {
            const spell::SpellPVec_t SPELLS_PVEC { creaturePtr_->SpellsPVec() };

            const auto RESPONSE_SELECTION_INDEX { PACKET.selection_opt.value() };

            if (RESPONSE_SELECTION_INDEX >= SPELLS_PVEC.size())
            {
                M_HP_LOG_ERR(
                    "SPELL, selection=" << PACKET.selection_opt.value()
                                        << ") Selection was greater than SpellPVec.size="
                                        << SPELLS_PVEC.size());

                return MakeCallbackHandlerMessage(
                    PACKET_DESCRIPTION,
                    "closed spellbook popup by selecting but NOT HANDLED DUE TO ERROR where the "
                    "selection was invalid somehow");
            }

            const auto SPELL_PTR { SPELLS_PVEC.at(RESPONSE_SELECTION_INDEX) };

            creaturePtr_->LastSpellCastNum(RESPONSE_SELECTION_INDEX);
            HandleCast_Step1_TargetSelection(SPELL_PTR);

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION,
                "closed spellbook popup by selecting and target selection process started");
        }
        else if (
            (PACKET.curently_open_popup_name == POPUP_NAME_MUSICSHEET_)
            && (PACKET.type == popup::PopupButtons::Select) && PACKET.selection_opt)
        {
            const auto SONGS_PVEC { creaturePtr_->SongsPVec() };
            const auto RESPONSE_SELECTION_INDEX { PACKET.selection_opt.value() };

            if (RESPONSE_SELECTION_INDEX >= SONGS_PVEC.size())
            {
                M_HP_LOG_ERR(
                    "SONG, selection=" << PACKET.selection_opt.value()
                                       << ") Selection was greater than SpellPVec.size="
                                       << SONGS_PVEC.size());

                return MakeCallbackHandlerMessage(
                    PACKET_DESCRIPTION,
                    "closed songbook popup by selecting but NOT HANDLED DUE TO ERROR where the "
                    "selection was invalid somehow");
            }

            creaturePtr_->LastSongPlayedNum(RESPONSE_SELECTION_INDEX);
            HandleSong_Step1_Play(SONGS_PVEC.at(RESPONSE_SELECTION_INDEX));

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION,
                "closed songbook popup by selecting and play song process started");
        }

        return MakeCallbackHandlerMessage(
            PACKET_DESCRIPTION, "image-text-entity callback NOT HANDLED");
    }

    void InventoryStage::Setup()
    {
        EntityAdd(ouroborosUPtr_);

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
        ForceSelectionAndDrawOfLeftListBox();
    }

    void InventoryStage::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(paperBgSprite_, states);
        target.draw(stageTitle_, states);
        target.draw(bottomSymbol_, states);
        target.draw(creatureSprite_, states);
        StageBase::draw(target, states);

        // Always draw because it is a fast operation and will
        // be fully transparent when should not be drawn.
        target.draw(detailViewQuads_, states);

        if (detailViewTextUPtr_)
        {
            sf::RenderStates statesBlendAdd { states };
            statesBlendAdd.blendMode = sf::BlendAdd;
            target.draw(detailViewSprite_, statesBlendAdd);

            detailViewTextUPtr_->draw(target, states);
        }

        auto newBlendModeStates { states };
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
            && (IsFading() == false))
        {
            gui::SoundManager::Instance()->PlaySfx_Keypress();

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

        return StageBase::KeyRelease(KEY_EVENT);
    }

    void InventoryStage::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        if (isSliderAnimating_)
        {
            sliderAnimTimerSec_ += ELAPSED_TIME_SECONDS;

            const auto CUT_OFF_TIME_SEC { 100.0f };

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
            if (hasMouseMoved_ && (IsDetailViewFadingOrVisible() == false) && (IsFading() == false)
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

        StageBase::UpdateTime(ELAPSED_TIME_SECONDS);
    }

    void InventoryStage::UpdateMousePos(const sf::Vector2i & NEW_MOUSE_POS)
    {
        const sf::Vector2f NEW_MOUSE_POS_F { NEW_MOUSE_POS };
        hasMouseMoved_ = true;
        HandleDetailViewMouseInterrupt(NEW_MOUSE_POS_F);
        StageBase::UpdateMousePos(NEW_MOUSE_POS);
    }

    void InventoryStage::UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V)
    {
        HandleDetailViewMouseInterrupt(MOUSE_POS_V);
        StageBase::UpdateMouseDown(MOUSE_POS_V);

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
        paperBgSprite_.setScale(
            SCREEN_WIDTH_ / static_cast<float>(paperBgSprite_.getLocalBounds().width),
            SCREEN_HEIGHT_ / static_cast<float>(paperBgSprite_.getLocalBounds().height));
    }

    void InventoryStage::Setup_InstructionText()
    {
        const gui::TextInfo INSTR_TEXT_INFO(
            "(use arrows or numbers to change characters, press 'a' to see achievements)",
            gui::GuiFont::Default,
            gui::FontManager::Instance()->Size_Small(),
            sfutil::color::GrayDark,
            gui::Justified::Left,
            sf::Text::Italic);

        insTextRegionUPtr_ = std::make_unique<gui::TextRegion>(
            "InventoryStage'sInstruction",
            INSTR_TEXT_INFO,
            sf::FloatRect(0.0f, sfutil::Bottom(stageTitle_.Region()) - 10.0f, 0.0f, 0.0f));

        insTextRegionUPtr_->SetEntityPos(
            (SCREEN_WIDTH_ * 0.5f) - (insTextRegionUPtr_->GetEntityRegion().width * 0.5f) + 93.0f,
            insTextRegionUPtr_->GetEntityPos().y);

        EntityAdd(insTextRegionUPtr_);
    }

    void InventoryStage::Setup_CreatureImage() {}

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
            const gui::TextInfo DETAILS_TEXT_INFO(
                ss.str(),
                gui::GuiFont::Default,
                gui::FontManager::Instance()->Size_Smallish(),
                sfutil::color::GrayDarker,
                gui::Justified::Left);

            detailsTextRegionUPtr_ = std::make_unique<gui::TextRegion>(
                "InventoryStage'sDetails", DETAILS_TEXT_INFO, detailsTextRect);

            EntityAdd(detailsTextRegionUPtr_);
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
        const auto STR_MOD_STR { creaturePtr_->TraitModifiedString(
            creature::Traits::Strength, true) };

        const auto ACC_MOD_STR { creaturePtr_->TraitModifiedString(
            creature::Traits::Accuracy, true) };

        const auto CHA_MOD_STR { creaturePtr_->TraitModifiedString(creature::Traits::Charm, true) };

        const auto LCK_MOD_STR { creaturePtr_->TraitModifiedString(creature::Traits::Luck, true) };

        const auto SPD_MOD_STR { creaturePtr_->TraitModifiedString(creature::Traits::Speed, true) };

        const auto INT_MOD_STR { creaturePtr_->TraitModifiedString(
            creature::Traits::Intelligence, true) };

        std::ostringstream ss;
        ss << "Strength:       " << creaturePtr_->Strength() << " " << STR_MOD_STR << "\n"
           << "Accuracy:      " << creaturePtr_->Accuracy() << " " << ACC_MOD_STR << "\n"
           << "Charm:          " << creaturePtr_->Charm() << " " << CHA_MOD_STR << "\n"
           << "Luck:             " << creaturePtr_->Luck() << " " << LCK_MOD_STR << "\n"
           << "Speed:            " << creaturePtr_->Speed() << " " << SPD_MOD_STR << "\n"
           << "Intelligence:   " << creaturePtr_->Intelligence() << " " << INT_MOD_STR << "\n"
           << "\n \n ";

        if (!statsTextRegionUPtr_)
        {
            const sf::FloatRect STATS_TEXT_RECT(
                STATS_POS_LEFT_, sfutil::Bottom(stageTitle_.Region()) + 20.0f, 0.0f, 0.0f);

            const gui::TextInfo STATS_TEXT_INFO(
                ss.str(),
                gui::GuiFont::SystemCondensed,
                gui::FontManager::Instance()->Size_Normal(),
                sfutil::color::GrayDarker,
                gui::Justified::Left);

            statsTextRegionUPtr_ = std::make_unique<gui::TextRegion>(
                "InventoryStage'sStats", STATS_TEXT_INFO, STATS_TEXT_RECT);

            EntityAdd(statsTextRegionUPtr_);
        }
        else
        {
            statsTextRegionUPtr_->SetText(ss.str());
        }
    }

    void InventoryStage::Setup_CenterText()
    {
        const auto & INVENTORY { creaturePtr_->Inventory() };

        std::ostringstream ss;
        ss << "Coins:  " << INVENTORY.Coins() << "\n"
           << "Gems:  " << INVENTORY.Gems() << "\n"
           << "Meteor Shards:  " << INVENTORY.MeteorShards() << "\n"
           << "Mana:  " << creaturePtr_->Mana() << "/" << creaturePtr_->ManaNormal() << "\n"
           << "Weight: " << INVENTORY.Weight() << "/" << creaturePtr_->WeightCanCarry() << "\n"
           << "\n \n ";

        const bool WAS_ALREADY_INSTANTIATED { centerTextRegionUPtr_ };

        if (WAS_ALREADY_INSTANTIATED)
        {
            centerTextRegionUPtr_->SetText(ss.str());
        }
        else
        {
            const sf::FloatRect CENTER_TEXT_RECT(
                0.0f,
                sfutil::Bottom(stageTitle_.Region()) + stageTitle_.DefaultBottomPad()
                    + sfutil::MapByRes(5.0f, 30.0f),
                0.0f,
                0.0f);

            const gui::TextInfo CENTER_TEXT_INFO(
                ss.str(),
                gui::GuiFont::SystemCondensed,
                gui::FontManager::Instance()->Size_Normal(),
                sfutil::color::GrayDarker,
                gui::Justified::Left);

            centerTextRegionUPtr_ = std::make_unique<gui::TextRegion>(
                "InventoryStage'sCenter", CENTER_TEXT_INFO, CENTER_TEXT_RECT);

            EntityAdd(centerTextRegionUPtr_);
        }

        const float POS_LEFT { ((WAS_ALREADY_INSTANTIATED == false) && creaturePtr_->IsBeast())
                                   ? (SCREEN_WIDTH_ + 1.0f)
                                   : (SCREEN_WIDTH_ * 0.5f) - sfutil::MapByRes(100.0f, 300.0f) };

        centerTextRegionUPtr_->SetEntityPos(POS_LEFT, centerTextRegionUPtr_->GetEntityPos().y);
        centerPosLeft_ = POS_LEFT;
    }

    void InventoryStage::Setup_ListBox()
    {
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

        FreeAllLeftListBoxes();

        gui::BoxEntityInfo listBoxInfoLeft;
        listBoxInfoLeft.SetupBorder(true, 1);
        listBoxInfoLeft.SetupColor(LISTBOX_COLOR_BG_);
        listBoxInfoLeft.focus_colors = LISTBOX_COLORSET_;

        gui::ListBoxPacket listBoxPacketLeft(
            LISTBOX_REGION_LEFT_, listBoxInfoLeft, LISTBOX_COLOR_LINE_, LISTBOX_COLOR_IMAGE_);

        switch (viewToUse)
        {
            case ViewType::Conditions:
            {
                condLeftListBoxUPtr_ = std::make_unique<CondListBox_t>(
                    "InventoryStage'sLeftCondition",
                    misc::MakeNotNull(this),
                    misc::MakeNotNull(this),
                    listBoxPacketLeft);

                for (const auto & CONDITION_PTR : creaturePtr_->ConditionsPVec())
                {
                    condLeftListBoxUPtr_->Append(
                        std::make_unique<gui::ListElement<creature::ConditionPtr_t>>(
                            CONDITION_PTR,
                            gui::TextInfo(LIST_ELEMENT_TEXT_INFO_DEFAULT_, CONDITION_PTR->Name())));
                }

                EntityAdd(condLeftListBoxUPtr_);
                break;
            }
            case ViewType::Items:
            {
                itemLeftListBoxUPtr_ = std::make_unique<ItemListBox_t>(
                    "InventoryStage'sEquippedItem",
                    misc::MakeNotNull(this),
                    misc::MakeNotNull(this),
                    listBoxPacketLeft);

                for (const auto & ITEM_PTR : creaturePtr_->Inventory().ItemsEquipped())
                {
                    itemLeftListBoxUPtr_->Append(
                        std::make_unique<gui::ListElement<item::ItemPtr_t>>(
                            ITEM_PTR,
                            gui::TextInfo(LIST_ELEMENT_TEXT_INFO_DEFAULT_, ITEM_PTR->Name())));
                }

                EntityAdd(itemLeftListBoxUPtr_);
                break;
            }
            case ViewType::Spells:
            {
                spellLeftListBoxUPtr_ = std::make_unique<SpellListBox_t>(
                    "InventoryStage'sSpell",
                    misc::MakeNotNull(this),
                    misc::MakeNotNull(this),
                    listBoxPacketLeft);

                for (const auto & SPELL_PTR : creaturePtr_->SpellsPVec())
                {
                    spellLeftListBoxUPtr_->Append(
                        std::make_unique<gui::ListElement<spell::SpellPtr_t>>(
                            SPELL_PTR,
                            gui::TextInfo(LIST_ELEMENT_TEXT_INFO_DEFAULT_, SPELL_PTR->Name())));
                }

                EntityAdd(spellLeftListBoxUPtr_);
                break;
            }
            case ViewType::Titles:
            case ViewType::Count:
            default:
            {
                titleLeftListBoxUPtr_ = std::make_unique<TitleListBox_t>(
                    "InventoryStage'sTitle",
                    misc::MakeNotNull(this),
                    misc::MakeNotNull(this),
                    listBoxPacketLeft);

                for (const auto & TITLE_PTR : creaturePtr_->TitlesPVec())
                {
                    titleLeftListBoxUPtr_->Append(
                        std::make_unique<gui::ListElement<creature::TitlePtr_t>>(
                            TITLE_PTR,
                            gui::TextInfo(LIST_ELEMENT_TEXT_INFO_DEFAULT_, TITLE_PTR->Name())));
                }

                EntityAdd(titleLeftListBoxUPtr_);
                break;
            }
        }
    }

    void InventoryStage::Setup_DescBox(const bool WILL_MOVE_OFFSCREEN)
    {
        const bool IS_UNEQ_ALREADY_INSTANTIATED { itemRightListBoxUPtr_ };

        sf::Vector2f origPosV(OUT_OF_SIGHT_POS_, OUT_OF_SIGHT_POS_);
        if (IS_UNEQ_ALREADY_INSTANTIATED)
        {
            origPosV = itemRightListBoxUPtr_->GetEntityPos();
            EntityRemove(itemRightListBoxUPtr_);
        }

        gui::BoxEntityInfo listBoxInfoRight;
        listBoxInfoRight_.SetupColor(LISTBOX_COLOR_BG_);
        listBoxInfoRight_.SetupBorder(true, 1.0f);
        listBoxInfoRight_.focus_colors = LISTBOX_COLORSET_;

        gui::ListBoxPacket listBoxPacketRight(
            LISTBOX_REGION_RIGHT_, listBoxInfoRight_, LISTBOX_COLOR_LINE_, LISTBOX_COLOR_IMAGE_);

        itemRightListBoxUPtr_ = std::make_unique<ItemListBox_t>(
            "InventoryStage'sUnEquippedItem",
            misc::MakeNotNull(this),
            misc::MakeNotNull(this),
            listBoxPacketRight);

        for (const auto & ITEM_PTR : creaturePtr_->Inventory().Items())
        {
            itemRightListBoxUPtr_->Append(std::make_unique<gui::ListElement<item::ItemPtr_t>>(
                ITEM_PTR, gui::TextInfo(LIST_ELEMENT_TEXT_INFO_DEFAULT_, ITEM_PTR->Name())));
        }

        EntityAdd(itemRightListBoxUPtr_);

        if (IS_UNEQ_ALREADY_INSTANTIATED)
        {
            if (WILL_MOVE_OFFSCREEN)
            {
                itemRightListBoxUPtr_->SetEntityPos(
                    SCREEN_WIDTH_ + 1.0f, itemRightListBoxUPtr_->GetEntityPos().y);
            }
            else
            {
                itemRightListBoxUPtr_->SetEntityPos(origPosV.x, origPosV.y);
            }
        }

        if (!descBoxUPtr_)
        {
            descBoxUPtr_ = std::make_unique<gui::BoxEntity>(
                "InventoryStage'sDesc", LISTBOX_REGION_RIGHT_, listBoxInfoRight);

            EntityAdd(descBoxUPtr_);
        }

        descBoxUPtr_->SetEntityPos(SCREEN_WIDTH_ + 1.0f, descBoxUPtr_->GetEntityPos().y);

        if (descTextRegionUPtr_)
        {
            EntityRemove(descTextRegionUPtr_);
        }

        descTextRegionUPtr_ = std::make_unique<gui::TextRegion>(
            "InventoryStage'sDescriptionBox",
            gui::TextInfo(LIST_ELEMENT_TEXT_INFO_DEFAULT_, " ", DESCBOX_TEXT_COLOR_),
            LISTBOX_REGION_RIGHT_,
            gui::TextRegion::DEFAULT_NO_RESIZE_,
            gui::BoxEntityInfo(),
            DESCBOX_MARGINS_);

        EntityAdd(descTextRegionUPtr_);

        descTextRegionUPtr_->SetEntityPos(
            SCREEN_WIDTH_ + 1.0f, descTextRegionUPtr_->GetEntityPos().y);
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
        Setup_SortButton(
            "InventoryStage's_EquippedListbox_SortByName_",
            "media-images-misc-abc",
            eqSortButtonNameUPtr_);

        Setup_SortButton(
            "InventoryStage's_EquippedListbox_SortByPrice_",
            "media-images-misc-money-bag",
            eqSortButtonPriceUPtr_);

        Setup_SortButton(
            "InventoryStage's_EquippedListbox_SortByWeight_",
            "media-images-misc-weight",
            eqSortButtonWeightUPtr_);

        Setup_SortButton(
            "InventoryStage's_UnequippedListbox_SortByName_",
            "media-images-misc-abc",
            unEqSortButtonNameUPtr_);

        Setup_SortButton(
            "InventoryStage's_UnequippedListbox_SortByPrice_",
            "media-images-misc-money-bag",
            unEqSortButtonPriceUPtr_);

        Setup_SortButton(
            "InventoryStage's_UnequippedListbox_SortByWeight_",
            "media-images-misc-weight",
            unEqSortButtonWeightUPtr_);

        const auto THREE_ICONS_WIDTH { eqSortButtonNameUPtr_->GetEntityRegion().width
                                       + eqSortButtonPriceUPtr_->GetEntityRegion().width
                                       + eqSortButtonWeightUPtr_->GetEntityRegion().width };

        const auto POS_HORIZ_NAME_EQ { ((FIRST_LISTBOX_POS_LEFT_ + LISTBOX_WIDTH_)
                                        - THREE_ICONS_WIDTH)
                                       - (SORT_ICON_SPACER_ * 2.25f) };

        const auto POS_HORIZ_NAME_UNEQ { ((SECOND_LISTBOX_POS_LEFT_ + LISTBOX_WIDTH_)
                                          - THREE_ICONS_WIDTH)
                                         - (SORT_ICON_SPACER_ * 2.25f) };

        const auto POS_VERT_NAME { SORT_ICON_POS_TOP_
                                   - (eqSortButtonNameUPtr_->GetEntityRegion().height * 0.5f) };

        const auto POS_HORIZ_PRICE_EQ {
            POS_HORIZ_NAME_EQ + eqSortButtonNameUPtr_->GetEntityRegion().width + SORT_ICON_SPACER_
        };

        const auto POS_HORIZ_PRICE_UNEQ { POS_HORIZ_NAME_UNEQ
                                          + unEqSortButtonNameUPtr_->GetEntityRegion().width
                                          + SORT_ICON_SPACER_ };

        const auto POS_VERT_PRICE { SORT_ICON_POS_TOP_
                                    - (eqSortButtonPriceUPtr_->GetEntityRegion().height * 0.5f) };

        const auto POS_HORIZ_WEIGHT_EQ {
            POS_HORIZ_PRICE_EQ + eqSortButtonPriceUPtr_->GetEntityRegion().width + SORT_ICON_SPACER_
        };

        const auto POS_HORIZ_WEIGHT_UNEQ { POS_HORIZ_PRICE_UNEQ
                                           + unEqSortButtonPriceUPtr_->GetEntityRegion().width
                                           + SORT_ICON_SPACER_ };

        const auto POS_VERT_WEIGHT { SORT_ICON_POS_TOP_
                                     - (eqSortButtonWeightUPtr_->GetEntityRegion().height * 0.5f) };

        eqSortButtonNameUPtr_->SetEntityPos(POS_HORIZ_NAME_EQ, POS_VERT_NAME);
        eqSortButtonPriceUPtr_->SetEntityPos(POS_HORIZ_PRICE_EQ, POS_VERT_PRICE);
        eqSortButtonWeightUPtr_->SetEntityPos(POS_HORIZ_WEIGHT_EQ, POS_VERT_WEIGHT);
        unEqSortButtonNameUPtr_->SetEntityPos(POS_HORIZ_NAME_UNEQ, POS_VERT_NAME);
        unEqSortButtonPriceUPtr_->SetEntityPos(POS_HORIZ_PRICE_UNEQ, POS_VERT_PRICE);
        unEqSortButtonWeightUPtr_->SetEntityPos(POS_HORIZ_WEIGHT_UNEQ, POS_VERT_WEIGHT);
    }

    void InventoryStage::Setup_SortButton(
        const std::string & NAME,
        const std::string & IMAGE_PATH_KEY,
        gui::ImageTextEntityUPtr_t & sortButtonUPtr)
    {
        const auto SORT_IMAGE_WIDTH { sfutil::ScreenRatioToPixelsHoriz(0.02f) };

        const sf::FloatRect SORT_IMAGE_REGION(0.0f, 0.0f, SORT_IMAGE_WIDTH, 0.0f);

        const gui::EntityImageInfo SORT_ENTITY_IMAGE(
            gui::CachedTexture(IMAGE_PATH_KEY), SORT_IMAGE_REGION, boost::none, SORT_ICON_COLOR_);

        sortButtonUPtr = std::make_unique<gui::ImageTextEntity>(
            NAME,
            gui::MouseImageInfo(true, SORT_ENTITY_IMAGE),
            gui::MouseTextInfo(),
            gui::ImageTextEntity::Callback_t::IHandlerPtr_t(this),
            gui::ImageTextEntity::MouseStateSync::Image);

        EntityAdd(sortButtonUPtr);
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

        const gui::TextInfo LISTBOX_TEXT_INFO(
            titleText,
            gui::GuiFont::System,
            gui::FontManager::Instance()->Size_Normal(),
            LISTBOX_COLOR_TITLE_,
            gui::Justified::Center);

        const sf::FloatRect LISTBOX_TEXT_RECT(FIRST_LISTBOX_POS_LEFT_, 0.0f, LISTBOX_WIDTH_, 0.0f);

        if (!eqTitleTextRegionUPtr_)
        {
            eqTitleTextRegionUPtr_ = std::make_unique<gui::TextRegion>(
                "InventoryStage'sEquippedListBoxTitle", LISTBOX_TEXT_INFO, LISTBOX_TEXT_RECT);

            EntityAdd(eqTitleTextRegionUPtr_);
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
        const auto TITLETEXT { (ViewType::Items == view_) ? "Unequipped Items" : "Description" };

        const gui::TextInfo DESC_TEXT_INFO(
            TITLETEXT,
            gui::GuiFont::System,
            gui::FontManager::Instance()->Size_Normal(),
            LISTBOX_COLOR_TITLE_,
            gui::Justified::Center);

        const sf::FloatRect DESC_TEXT_RECT(SECOND_LISTBOX_POS_LEFT_, 0.0f, LISTBOX_WIDTH_, 0.0f);

        if (!unEqTitleTextRegionUPtr_)
        {
            unEqTitleTextRegionUPtr_ = std::make_unique<gui::TextRegion>(
                "InventoryStage'sUnequippedListBoxTitle", DESC_TEXT_INFO, DESC_TEXT_RECT);

            EntityAdd(unEqTitleTextRegionUPtr_);
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
        gui::ImageTextEntityUPtr_t & buttonUPtr,
        const std::string & TEXT,
        const float HORIZ_OFFSET_MULT)
    {
        gui::TextInfo textInfo(
            TEXT,
            gui::GuiFont::System,
            gui::FontManager::Instance()->Size_Largeish(),
            sf::Color::Black,
            gui::Justified::Left);

        const auto COLOR_DISABLED { sf::Color(0, 0, 0, 180) };
        const auto COLOR_OVER { sfutil::color::GrayDarker };
        const auto COLOR_DOWN { COLOR_DISABLED };

        const gui::MouseTextInfo MOUSE_TEXT_INFO(textInfo, COLOR_DOWN, COLOR_OVER, COLOR_DISABLED);

        buttonUPtr = std::make_unique<gui::ImageTextEntity>(
            "InventoryStage's" + TEXT,
            gui::MouseImageInfo(),
            MOUSE_TEXT_INFO,
            gui::ImageTextEntity::Callback_t::IHandlerPtr_t(this),
            gui::ImageTextEntity::MouseStateSync::Image);

        buttonUPtr->SetEntityPos(
            (INNER_RECT_.left + (HORIZ_OFFSET_MULT * INNER_RECT_.width))
                - buttonUPtr->GetEntityRegion().width,
            sfutil::CenterOfVert(bottomSymbol_.Region())
                - (buttonUPtr->GetEntityRegion().height * 0.5f));

        EntityAdd(buttonUPtr);
    }

    void InventoryStage::Setup_ButtonMouseoverText()
    {
        SetButtonDisabledIf(backButtonUPtr_, false);

        backButtonUPtr_->SetMouseHoverText("Click here or press 'b' to return to previous screen.");

        if (ViewType::Items == view_)
        {
            SetButtonDisabledIf(itemsButtonUPtr_, true);
            itemsButtonUPtr_->SetMouseHoverText("Already viewing items.");
        }
        else
        {
            SetButtonDisabledIf(itemsButtonUPtr_, false);
            itemsButtonUPtr_->SetMouseHoverText("Click here or press 'i' to view items.");
        }

        if (ViewType::Spells == view_)
        {
            const auto ROLE_ENUM { creaturePtr_->Role() };
            if (ROLE_ENUM == creature::role::Bard)
            {
                spellsButtonUPtr_->TextEntity()->SetText("(S)ongs");
                spellsButtonUPtr_->Sync();
                SetButtonDisabledIf(spellsButtonUPtr_, true);

                spellsButtonUPtr_->SetMouseHoverText("Already viewing songs.");
            }
            else if (
                (ROLE_ENUM == creature::role::Cleric) || (ROLE_ENUM == creature::role::Sorcerer))
            {
                spellsButtonUPtr_->TextEntity()->SetText("(S)pells");
                spellsButtonUPtr_->Sync();
                SetButtonDisabledIf(spellsButtonUPtr_, true);

                spellsButtonUPtr_->SetMouseHoverText("Already viewing spells.");
            }
        }
        else
        {
            const auto ROLE_ENUM { creaturePtr_->Role() };
            if (ROLE_ENUM == creature::role::Bard)
            {
                spellsButtonUPtr_->TextEntity()->SetText("(S)ongs");
                spellsButtonUPtr_->Sync();
                SetButtonDisabledIf(spellsButtonUPtr_, false);

                spellsButtonUPtr_->SetMouseHoverText(
                    "Click here or press 's' to view and play magical songs.");
            }
            else if (
                (ROLE_ENUM == creature::role::Cleric) || (ROLE_ENUM == creature::role::Sorcerer))
            {
                spellsButtonUPtr_->TextEntity()->SetText("(S)pells");
                spellsButtonUPtr_->Sync();
                SetButtonDisabledIf(spellsButtonUPtr_, false);

                spellsButtonUPtr_->SetMouseHoverText(
                    "Click here or press 's' to view and cast spells.");
            }
        }

        if (creaturePtr_->Titles().empty())
        {
            SetButtonDisabledIf(titlesButtonUPtr_, true);

            titlesButtonUPtr_->SetMouseHoverText(
                creaturePtr_->Name() + " does not have any titles.");
        }
        else if (ViewType::Titles == view_)
        {
            SetButtonDisabledIf(titlesButtonUPtr_, true);
            titlesButtonUPtr_->SetMouseHoverText("Already viewing titles.");
        }
        else
        {
            SetButtonDisabledIf(titlesButtonUPtr_, false);
            titlesButtonUPtr_->SetMouseHoverText("Click here or press 't' to view titles.");
        }

        if (ViewType::Conditions == view_)
        {
            SetButtonDisabledIf(condsButtonUPtr_, true);
            condsButtonUPtr_->SetMouseHoverText("Already viewing conditions.");
        }
        else
        {
            SetButtonDisabledIf(condsButtonUPtr_, false);
            condsButtonUPtr_->SetMouseHoverText("Click here or press 'c' to view conditions.");
        }

        if (creaturePtr_->IsBeast())
        {
            SetButtonDisabledIf(giveButtonUPtr_, true);
            SetButtonDisabledIf(shareButtonUPtr_, true);
            SetButtonDisabledIf(gatherButtonUPtr_, true);

            const std::string DENY_MSG {
                "Beasts cannot carry items, coins, gems, or Meteor Shards."
            };

            giveButtonUPtr_->SetMouseHoverText(DENY_MSG);
            shareButtonUPtr_->SetMouseHoverText(DENY_MSG);
            gatherButtonUPtr_->SetMouseHoverText(DENY_MSG);
        }
        else
        {
            SetButtonDisabledIf(giveButtonUPtr_, false);
            SetButtonDisabledIf(shareButtonUPtr_, false);
            SetButtonDisabledIf(gatherButtonUPtr_, false);

            giveButtonUPtr_->SetMouseHoverText(
                "Click here or press 'g' to give something to another character.");

            shareButtonUPtr_->SetMouseHoverText(
                "Click here or press 'h' to share something between all characters.");

            gatherButtonUPtr_->SetMouseHoverText(
                "Click here or press 'r' to gather all of something from all other characters.");
        }

        if (ViewType::Items == view_)
        {
            if (itemRightListBoxUPtr_ && (itemRightListBoxUPtr_->Empty() == false))
            {
                const auto ITEM_PTR { itemRightListBoxUPtr_->Selection()->Element() };

                SetButtonDisabledIf(equipButtonUPtr_, false);

                equipButtonUPtr_->SetMouseHoverText(
                    "Click here or press 'e' to equip the " + ITEM_PTR->Name() + ".");

                SetButtonDisabledIf(dropButtonUPtr_, false);

                dropButtonUPtr_->SetMouseHoverText(
                    "(Click here or press 'd' to drop the " + ITEM_PTR->Name() + ")");
            }
            else
            {
                SetButtonDisabledIf(equipButtonUPtr_, true);

                equipButtonUPtr_->SetMouseHoverText(
                    "There is no unequipped item selected to equip.");

                SetButtonDisabledIf(dropButtonUPtr_, true);

                dropButtonUPtr_->SetMouseHoverText("There is no unequipped item selected to drop.");
            }
        }
        else if (creaturePtr_->IsBeast())
        {
            SetButtonDisabledIf(equipButtonUPtr_, true);
            equipButtonUPtr_->SetMouseHoverText("Beasts cannot carry, equip, or drop items.");
            SetButtonDisabledIf(dropButtonUPtr_, true);
            dropButtonUPtr_->SetMouseHoverText("Beasts cannot carry, equip, or drop items.");
        }
        else
        {
            SetButtonDisabledIf(equipButtonUPtr_, true);

            equipButtonUPtr_->SetMouseHoverText("Click 'Item' or press 'i' to switch to the items "
                                                "view if you want to equip an item.");

            SetButtonDisabledIf(dropButtonUPtr_, true);

            dropButtonUPtr_->SetMouseHoverText("Click 'Item' or press 'i' to switch to the items "
                                               "view if you want to drop an item.");
        }

        if (ViewType::Items == view_)
        {
            if (itemLeftListBoxUPtr_ && (itemLeftListBoxUPtr_->Empty() == false))
            {
                const auto ITEM_PTR { itemLeftListBoxUPtr_->Selection()->Element() };

                SetButtonDisabledIf(unequipButtonUPtr_, false);

                unequipButtonUPtr_->SetMouseHoverText(
                    "Click here or press 'u' to unequip the " + ITEM_PTR->Name() + ".");
            }
            else
            {
                SetButtonDisabledIf(unequipButtonUPtr_, true);
                unequipButtonUPtr_->SetMouseHoverText("There is no item to unequip.");
            }
        }
        else if (creaturePtr_->IsBeast())
        {
            SetButtonDisabledIf(unequipButtonUPtr_, true);
            unequipButtonUPtr_->SetMouseHoverText("Beasts cannot carry, equip, or drop items.");
        }
        else
        {
            SetButtonDisabledIf(unequipButtonUPtr_, true);

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

        if ((ViewType::Items != view_) && !itemLeftListBoxUPtr_)
        {
            if (condLeftListBoxUPtr_)
            {
                SetDescBoxTextToSelectedForConditionOrTitle(*condLeftListBoxUPtr_);
            }
            else if (titleLeftListBoxUPtr_)
            {
                SetDescBoxTextToSelectedForConditionOrTitle(*titleLeftListBoxUPtr_);
            }
        }

        Setup_FirstListBoxTitle();
        Setup_DescBoxTitle();
        Setup_ButtonMouseoverText();
        ForceSelectionAndDrawOfLeftListBox();
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
        detailViewSlider_ = gui::SliderZeroToOne(DETAILVIEW_SLIDER_SPEED_);
        return true;
    }

    bool InventoryStage::HandleViewChange(const ViewType NEW_VIEW)
    {
        if ((NEW_VIEW == ViewType::Items)
            && ((ViewType::Items == view_)
                || (itemsButtonUPtr_->GetMouseState() == gui::MouseState::Disabled)))
        {
            return false;
        }

        if ((NEW_VIEW == ViewType::Titles)
            && ((ViewType::Titles == view_)
                || (titlesButtonUPtr_->GetMouseState() == gui::MouseState::Disabled)))
        {
            return false;
        }

        if ((NEW_VIEW == ViewType::Conditions)
            && ((ViewType::Conditions == view_)
                || (condsButtonUPtr_->GetMouseState() == gui::MouseState::Disabled)))
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

            if (imageSlider_.IsStopped())
            {
                Setup_CreatureImage();

                creatureSprite_.setPosition(OUT_OF_SIGHT_POS_, creatureSprite_.getPosition().y);

                hasImageChanged_ = true;
                imageSlider_ = gui::SliderZeroToOne(VIEW_CHANGE_SLIDER_SPEED_);
            }
        }
        else
        {
            const auto RATIO { (1.0f - imageSlider_.Update(ELAPSED_TIME_SECONDS)) };

            if (isSlidingLeft_)
            {
                creatureSprite_.setPosition(
                    CREATURE_IMAGE_POS_LEFT_ + (SCREEN_WIDTH_ * RATIO),
                    creatureSprite_.getPosition().y);
            }
            else
            {
                const auto NEW_POS_LEFT { CREATURE_IMAGE_POS_LEFT_ + (OUT_OF_SIGHT_POS_ * RATIO) };

                creatureSprite_.setPosition(NEW_POS_LEFT, creatureSprite_.getPosition().y);

                // sometimes the new creature image is wider than the last,
                // so the details text needs to move with the new image to avoid overlap
                UpdateImageDetailsPosition();

                const auto NEW_DETAILSTEXT_POS_LEFT_EDGE { (
                    NEW_POS_LEFT + creatureImageWidthScaled_ + CREATURE_IMAGE_RIGHT_PAD_) };

                if (NEW_DETAILSTEXT_POS_LEFT_EDGE > detailsTextRegionUPtr_->GetEntityPos().x)
                {
                    detailsPosLeft_ = NEW_DETAILSTEXT_POS_LEFT_EDGE;

                    detailsTextRegionUPtr_->SetEntityPos(
                        NEW_DETAILSTEXT_POS_LEFT_EDGE, detailsTextRegionUPtr_->GetEntityPos().y);
                }
            }

            if (imageSlider_.IsStopped())
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
            const auto RATIO { detailsSlider_.Update(ELAPSED_TIME_SECONDS) };

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

            if (detailsSlider_.IsStopped())
            {
                UpdateImageDetailsPosition();
                Setup_CreatureDetails(true);

                detailsTextRegionUPtr_->SetEntityPos(
                    SCREEN_WIDTH_ + 1.0f, detailsTextRegionUPtr_->GetEntityPos().y);

                hasDetailsChanged_ = true;
                detailsSlider_ = gui::SliderZeroToOne(VIEW_CHANGE_SLIDER_SPEED_);
            }
        }
        else
        {
            const auto RATIO { (1.0f - detailsSlider_.Update(ELAPSED_TIME_SECONDS)) };

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

            if (detailsSlider_.IsStopped())
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
            const auto RATIO { centerSlider_.Update(ELAPSED_TIME_SECONDS) };

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

            if (centerSlider_.IsStopped())
            {
                Setup_CenterText();

                centerTextRegionUPtr_->SetEntityPos(
                    SCREEN_WIDTH_ + 1.0f, centerTextRegionUPtr_->GetEntityPos().y);

                hasCenterChanged_ = true;
                centerSlider_ = gui::SliderZeroToOne(VIEW_CHANGE_SLIDER_SPEED_);
            }
        }
        else
        {
            if (creaturePtr_->IsBeast() == false)
            {
                const auto RATIO { (1.0f - centerSlider_.Update(ELAPSED_TIME_SECONDS)) };

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

            if (centerSlider_.IsStopped() || creaturePtr_->IsBeast())
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
            const auto RATIO { statsSlider_.Update(ELAPSED_TIME_SECONDS) };

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

            if (statsSlider_.IsStopped())
            {
                Setup_CreatureStats();

                statsTextRegionUPtr_->SetEntityPos(
                    OUT_OF_SIGHT_POS_, statsTextRegionUPtr_->GetEntityPos().y);

                hasStatsChanged_ = true;
                statsSlider_ = gui::SliderZeroToOne(VIEW_CHANGE_SLIDER_SPEED_);
            }
        }
        else
        {
            const auto RATIO { (1.0f - statsSlider_.Update(ELAPSED_TIME_SECONDS)) };

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

            if (statsSlider_.IsStopped())
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
            const auto RATIO { listBoxSlider_.Update(ELAPSED_TIME_SECONDS) };

            float newPosHoriz { 0.0f };

            if (isSlidingLeft_)
            {
                newPosHoriz = FIRST_LISTBOX_POS_LEFT_ + ((OUT_OF_SIGHT_POS_ * 2.0f) * RATIO);
            }
            else
            {
                newPosHoriz = FIRST_LISTBOX_POS_LEFT_ + (SCREEN_WIDTH_ * RATIO);
            }

            LeftListBoxSetHorizPosition(newPosHoriz);

            if (listBoxSlider_.IsStopped())
            {
                Setup_ListBox();
                LeftListBoxSetHorizPosition(SCREEN_WIDTH_ + 1.0f);

                hasListBoxChanged_ = true;
                listBoxSlider_ = gui::SliderZeroToOne(VIEW_CHANGE_SLIDER_SPEED_);
            }
        }
        else
        {
            const auto RATIO { (1.0f - listBoxSlider_.Update(ELAPSED_TIME_SECONDS)) };

            float newPosHoriz { 0.0f };

            if (isSlidingLeft_)
            {
                newPosHoriz = FIRST_LISTBOX_POS_LEFT_ + (SCREEN_WIDTH_ * RATIO);
            }
            else
            {
                newPosHoriz = FIRST_LISTBOX_POS_LEFT_ + ((OUT_OF_SIGHT_POS_ * 2.0f) * RATIO);
            }

            LeftListBoxSetHorizPosition(newPosHoriz);

            if (listBoxSlider_.IsStopped())
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
            const auto RATIO { descBoxSlider_.Update(ELAPSED_TIME_SECONDS) };

            const auto POS_LEFT_SLIDING_LEFT {
                SECOND_LISTBOX_POS_LEFT_ + (((OUT_OF_SIGHT_POS_ * 2.0f) - SCREEN_WIDTH_) * RATIO)
            };

            const auto POS_LEFT_SLIDING_RIGHT { SECOND_LISTBOX_POS_LEFT_
                                                + ((SCREEN_WIDTH_ * 0.75f) * RATIO) };

            if (ViewType::Items == view_)
            {
                if (isSlidingLeft_)
                {
                    itemRightListBoxUPtr_->SetEntityPos(
                        POS_LEFT_SLIDING_LEFT, itemRightListBoxUPtr_->GetEntityPos().y);
                }
                else
                {
                    itemRightListBoxUPtr_->SetEntityPos(
                        POS_LEFT_SLIDING_RIGHT, itemRightListBoxUPtr_->GetEntityPos().y);
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

            if (descBoxSlider_.IsStopped())
            {
                Setup_DescBox(true);

                itemRightListBoxUPtr_->SetEntityPos(
                    SCREEN_WIDTH_ + 1.0f, itemRightListBoxUPtr_->GetEntityPos().y);

                descTextRegionUPtr_->SetEntityPos(
                    SCREEN_WIDTH_ + 1.0f, descTextRegionUPtr_->GetEntityPos().y);

                descBoxUPtr_->SetEntityPos(SCREEN_WIDTH_ + 1.0f, descBoxUPtr_->GetEntityPos().y);

                hasDescBoxChanged_ = true;
                descBoxSlider_ = gui::SliderZeroToOne(VIEW_CHANGE_SLIDER_SPEED_);
            }
        }
        else
        {
            const auto RATIO { (1.0f - descBoxSlider_.Update(ELAPSED_TIME_SECONDS)) };

            const auto POS_LEFT_SLIDING_LEFT { SECOND_LISTBOX_POS_LEFT_
                                               + ((SCREEN_WIDTH_ * 0.75f) * RATIO) };

            const auto POS_LEFT_SLIDING_RIGHT {
                SECOND_LISTBOX_POS_LEFT_ + (((OUT_OF_SIGHT_POS_ * 2.0f) - SCREEN_WIDTH_) * RATIO)
            };

            if ((ViewType::Items == viewToChangeTo_) || isViewForcedToItems_
                || ((ViewType::Count == viewToChangeTo_) && (ViewType::Items == view_)))
            {
                if (isSlidingLeft_)
                {
                    itemRightListBoxUPtr_->SetEntityPos(
                        POS_LEFT_SLIDING_LEFT, itemRightListBoxUPtr_->GetEntityPos().y);
                }
                else
                {
                    itemRightListBoxUPtr_->SetEntityPos(
                        POS_LEFT_SLIDING_RIGHT, itemRightListBoxUPtr_->GetEntityPos().y);
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

            if (descBoxSlider_.IsStopped())
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
            gui::SoundManager::Instance()->PlaySfx_TickOn();

            const auto ITEM_RECT_OPT { GetItemRectMouseIsOver(mousePosV_) };

            if (ITEM_RECT_OPT)
            {
                detailViewSourceRect_ = ITEM_RECT_OPT.value();
                isAchievementDisplaying_ = false;
                isDetailViewFadingIn_ = true;
                detailViewTimerSec_ = 0.0f;
                detailViewSlider_ = gui::SliderZeroToOne(DETAILVIEW_SLIDER_SPEED_);
            }
        }
    }

    void InventoryStage::UpdateTime_DetailViewFade(const float ELAPSED_TIME_SECONDS)
    {
        if (isDetailViewFadingIn_)
        {
            detailViewSliderRatio_ = detailViewSlider_.Update(ELAPSED_TIME_SECONDS);
            if (detailViewSlider_.IsStopped())
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
                    const auto ITEM_PTR_OPT { GetItemMouseIsOver(mousePosV_) };
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

            if (detailViewSlider_.IsStopped())
            {
                isDetailViewFadingIn_ = false;
                isDetailViewDoneFading_ = false;
                isDetailViewFadingOut_ = false;
            }
        }

        const sf::Color NEW_COLOR {
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

        if ((detailViewSlider_.IsStopped()) && (false == isDetailViewFadingIn_)
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
        TransitionTo(stage::SetupPacket(stage::Stage::Previous, hasTakenActionSpellOrSong_));

        return true;
    }

    bool InventoryStage::HandleEquipRequest()
    {
        if ((game::Phase::Combat == previousPhase_) && (creaturePtr_ != turnCreaturePtr_))
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
        if ((ViewType::Items == view_)
            && (equipButtonUPtr_->GetMouseState() != gui::MouseState::Disabled)
            && itemLeftListBoxUPtr_ && itemRightListBoxUPtr_
            && (itemRightListBoxUPtr_->Empty() == false))
        {
            const auto ITEM_PTR { itemRightListBoxUPtr_->Selection()->Element() };

            const auto EQUIP_RESULT { creaturePtr_->ItemEquip(ITEM_PTR) };
            if (EQUIP_RESULT.empty())
            {
                gui::SoundManager::Instance()->PlaySfx_AckMajor();
                itemRightListBoxUPtr_->Move(ITEM_PTR, *itemLeftListBoxUPtr_);
                Setup_ButtonMouseoverText();
                return true;
            }
            else
            {
                std::ostringstream ss;
                ss << "Cannot equip the " << ITEM_PTR->Name() << " because: " << EQUIP_RESULT;

                const auto POPUP_INFO { popup::PopupManager::Instance()->CreatePopupInfo(
                    "InventoryStage'sEquipItemFailedPopup",
                    ss.str(),
                    popup::PopupButtons::Okay,
                    popup::PopupImage::Regular) };

                SpawnPopup(misc::MakeNotNull(this), POPUP_INFO);
                isWaitingOnPopup_ = true;
            }
        }

        return false;
    }

    bool InventoryStage::HandleUnequipRequest()
    {
        if ((game::Phase::Combat == previousPhase_) && (creaturePtr_ != turnCreaturePtr_))
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
        if ((ViewType::Items == view_)
            && (unequipButtonUPtr_->GetMouseState() != gui::MouseState::Disabled)
            && itemLeftListBoxUPtr_ && (itemLeftListBoxUPtr_->Empty() == false)
            && itemRightListBoxUPtr_)
        {
            const auto ITEM_PTR { itemLeftListBoxUPtr_->Selection()->Element() };

            if (ITEM_PTR->IsBodypart())
            {
                PopupDoneWindow("Bodypart items cannot be unequipped.", true);
            }
            else
            {
                gui::SoundManager::Instance()->PlaySfx_AckMajor();
                creaturePtr_->ItemUnEquip(ITEM_PTR);
                itemLeftListBoxUPtr_->Move(ITEM_PTR, *itemRightListBoxUPtr_);
                Setup_ButtonMouseoverText();
                return true;
            }
        }

        return false;
    }

    bool InventoryStage::HandleGiveRequestInitial()
    {
        actionType_ = ActionType::Give;

        const auto POPUP_INFO { popup::PopupManager::Instance()->CreateInventoryPromptPopupInfo(
            POPUP_NAME_GIVE_,
            std::string("\nWhat do you want to give?\n\n(I)tem\n(C)oins\n(G)ems\n(M)eteor")
                .append("Shards\n\n...or (Escape) to Cancel"),
            popup::PopupButtons::Cancel,
            popup::PopupImage::Large,
            gui::Justified::Center,
            gui::sound_effect::PromptGeneric,
            true,
            gui::FontManager::Instance()->Size_Largeish()) };

        SpawnPopup(misc::MakeNotNull(this), POPUP_INFO);

        isWaitingOnPopup_ = true;
        return true;
    }

    bool InventoryStage::HandleGiveRequestItems()
    {
        if ((game::Phase::Combat == previousPhase_) && (creaturePtr_ != turnCreaturePtr_))
        {
            std::ostringstream ss;
            ss << "\nDuring combat, only the character whose turn it is may "
               << "interact with items.";

            PopupRejectionWindow(ss.str());
        }

        if (itemRightListBoxUPtr_ && (itemRightListBoxUPtr_->Empty() == false))
        {
            listElementToGivePtrOpt_ = itemRightListBoxUPtr_->Selection();

            PopupCharacterSelectWindow(
                "Give the " + listElementToGivePtrOpt_.value()->Element()->Name() + " to who?");

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
        if (!listElementToGivePtrOpt_ || !creatureToGiveToPtrOpt_)
        {
            return true;
        }

        if ((game::Phase::Combat == previousPhase_) && (creaturePtr_ != turnCreaturePtr_))
        {
            std::ostringstream ss;
            ss << "\nDuring combat, only the character whose turn it is may "
               << "interact with items.";

            PopupRejectionWindow(ss.str());
            return false;
        }

        const auto ITEM_PTR { listElementToGivePtrOpt_.value()->Element() };

        const auto CAN_GIVE_ITEM_STR { creatureToGiveToPtrOpt_.value()->ItemIsAddAllowed(
            ITEM_PTR) };

        if (CAN_GIVE_ITEM_STR.empty())
        {
            gui::SoundManager::Instance()
                ->GetSoundEffectSet(gui::sound_effect_set::ItemGive)
                .PlayRandom();

            creaturePtr_->ItemRemove(ITEM_PTR);
            creatureToGiveToPtrOpt_.value()->ItemAdd(ITEM_PTR);

            // NOTE this call destroys the unEquippedListBox which destroys all of its items, which
            // leaves listElementToGivePtrOpt_ dangling
            Setup_DescBox(false);
            listElementToGivePtrOpt_ = boost::none;

            EndOfGiveShareGatherTasks();
            return true;
        }
        else
        {
            std::ostringstream ss;
            ss << "Can't give the " << ITEM_PTR->Name() << " to "
               << creatureToGiveToPtrOpt_.value()->Name() << " because:  " << CAN_GIVE_ITEM_STR;

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
        if ((ViewType::Items == view_)
            && (equipButtonUPtr_->GetMouseState() != gui::MouseState::Disabled)
            && itemRightListBoxUPtr_ && (itemRightListBoxUPtr_->Empty() == false))
        {
            if ((game::Phase::Combat == previousPhase_) && (creaturePtr_ != turnCreaturePtr_))
            {
                std::ostringstream ss;
                ss << "\nDuring combat, only the character whose turn it is may "
                   << "interact with items.";

                PopupRejectionWindow(ss.str());
                return false;
            }

            listElementToDropPtrOpt_ = itemRightListBoxUPtr_->Selection();

            actionType_ = ActionType::Drop;

            const auto POPUP_INFO { popup::PopupManager::Instance()->CreatePopupInfo(
                POPUP_NAME_DROPCONFIRM_,
                "\nAre you sure you want to drop the "
                    + listElementToDropPtrOpt_.value()->Element()->Name() + "?",
                popup::PopupButtons::YesNo,
                popup::PopupImage::Regular,
                gui::Justified::Center,
                gui::sound_effect::PromptQuestion) };

            SpawnPopup(misc::MakeNotNull(this), POPUP_INFO);
            isWaitingOnPopup_ = true;
            return true;
        }

        return false;
    }

    bool InventoryStage::HandleDropActual()
    {
        if (!listElementToDropPtrOpt_)
        {
            return false;
        }

        if ((game::Phase::Combat == previousPhase_) && (creaturePtr_ != turnCreaturePtr_))
        {
            std::ostringstream ss;
            ss << "\nDuring combat, only the character whose turn it is may "
               << "interact with items.";

            PopupRejectionWindow(ss.str());
            return false;
        }

        gui::SoundManager::Instance()
            ->GetSoundEffectSet(gui::sound_effect_set::ItemDrop)
            .PlayRandom();

        const auto ITEM_PTR { listElementToDropPtrOpt_.value()->Element() };
        itemRightListBoxUPtr_->RemoveFree(listElementToDropPtrOpt_);
        creaturePtr_->ItemRemove(ITEM_PTR);

        // NOTE after this call ITEM_PTR is left dangling
        item::ItemWarehouse::Access().Free(ITEM_PTR);

        EndOfGiveShareGatherTasks();
        Setup_DescBox(false);
        return true;
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
        const auto CURR_INDEX { game::Game::Instance()->State().Party().GetOrderNum(creaturePtr_) };

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
            UpdateCreatureImage();

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

        imageSlider_ = gui::SliderZeroToOne(VIEW_CHANGE_SLIDER_SPEED_);
        detailsSlider_ = gui::SliderZeroToOne(VIEW_CHANGE_SLIDER_SPEED_);
        centerSlider_ = gui::SliderZeroToOne(VIEW_CHANGE_SLIDER_SPEED_);
        statsSlider_ = gui::SliderZeroToOne(VIEW_CHANGE_SLIDER_SPEED_);
        listBoxSlider_ = gui::SliderZeroToOne(VIEW_CHANGE_SLIDER_SPEED_);
        descBoxSlider_ = gui::SliderZeroToOne(VIEW_CHANGE_SLIDER_SPEED_);
    }

    void InventoryStage::SetDescBoxText(const std::string & TEXT)
    {
        if (descTextRegionUPtr_)
        {
            EntityRemove(descTextRegionUPtr_);
        }

        descTextRegionUPtr_ = std::make_unique<gui::TextRegion>(
            "InventoryStage'sDescription",
            gui::TextInfo(
                LIST_ELEMENT_TEXT_INFO_DEFAULT_, TEXT, DESCBOX_TEXT_COLOR_, DESCBOX_TEXT_SIZE_),
            LISTBOX_REGION_RIGHT_,
            misc::MakeNotNull(this),
            gui::TextRegion::DEFAULT_NO_RESIZE_,
            gui::BoxEntityInfo(),
            DESCBOX_MARGINS_);

        EntityAdd(descTextRegionUPtr_);
    }

    void InventoryStage::PopupCharacterSelectWindow(
        const std::string & PROMPT_TEXT, const bool CAN_SELECT_SELF, const bool CAN_SELECT_BEASTS)
    {
        const std::size_t CURRENT_CREATURE_ORDER_NUM(
            game::Game::Instance()->State().Party().GetOrderNum(creaturePtr_));

        const auto NUM_CHARACTERS { game::Game::Instance()->State().Party().Characters().size() };

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

        const auto POPUP_INFO { popup::PopupManager::Instance()->CreateCharacterSelectPopupInfo(
            POPUP_NAME_CHAR_SELECT_, PROMPT_TEXT, invalidTextVec) };

        SpawnPopup(misc::MakeNotNull(this), POPUP_INFO);

        isWaitingOnPopup_ = true;
    }

    void InventoryStage::PopupRejectionWindow(const std::string & REJECTION_PROMPT_TEXT)
    {
        std::ostringstream ss;
        ss << "\n" << REJECTION_PROMPT_TEXT;

        const auto POPUPINFO_NOITEM { popup::PopupManager::Instance()->CreatePopupInfo(
            "InventoryStage'sPopupRejection",
            ss.str(),
            popup::PopupButtons::Cancel,
            popup::PopupImage::Regular,
            gui::Justified::Center,
            gui::sound_effect::PromptWarn,
            gui::FontManager::Instance()->Size_Largeish()) };

        SpawnPopup(misc::MakeNotNull(this), POPUPINFO_NOITEM);
        isWaitingOnPopup_ = true;
        EndOfGiveShareGatherTasks();
    }

    void InventoryStage::PopupNumberSelectWindow(
        const std::string & PROMPT_TEXT, const std::size_t NUMBER_MAX)
    {
        const auto POPUP_INFO_NUMBER_SELECT {
            popup::PopupManager::Instance()->CreateNumberSelectionPopupInfo(
                POPUP_NAME_NUMBER_SELECT_,
                PROMPT_TEXT,
                1,
                NUMBER_MAX,
                gui::FontManager::Instance()->Size_Largeish())
        };

        SpawnPopup(misc::MakeNotNull(this), POPUP_INFO_NUMBER_SELECT);

        isWaitingOnPopup_ = true;
    }

    void InventoryStage::PopupDoneWindow(
        const std::string & PROMPT_TEXT, const bool WILL_PLAY_SOUNDEFFECT)
    {
        const auto POPUP_INFO_DONE { popup::PopupManager::Instance()->CreatePopupInfo(
            "InventoryStage'sPopupDone",
            PROMPT_TEXT,
            popup::PopupButtons::Okay,
            popup::PopupImage::Regular,
            gui::Justified::Center,
            ((WILL_PLAY_SOUNDEFFECT) ? gui::sound_effect::PromptGeneric : gui::sound_effect::None),
            gui::FontManager::Instance()->Size_Largeish()) };

        SpawnPopup(misc::MakeNotNull(this), POPUP_INFO_DONE);
        isWaitingOnPopup_ = true;
        EndOfGiveShareGatherTasks();
    }

    void InventoryStage::PopupContentSelectionWindow(const std::string & PROMPT_TEXT)
    {
        std::ostringstream ss;
        ss << PROMPT_TEXT << "\n\n(C)oins\n(G)ems\n(M)eteor Shards";

        const auto POPUP_INFO { popup::PopupManager::Instance()->CreateInventoryPromptPopupInfo(
            POPUP_NAME_CONTENTSELECTION_,
            ss.str(),
            popup::PopupButtons::Cancel,
            popup::PopupImage::Regular,
            gui::Justified::Center,
            gui::sound_effect::PromptGeneric,
            false,
            gui::FontManager::Instance()->Size_Normal()) };

        SpawnPopup(misc::MakeNotNull(this), POPUP_INFO);

        isWaitingOnPopup_ = true;
    }

    void InventoryStage::HandleCoinsGive(
        const std::size_t COUNT, creature::CreaturePtr_t creatureToGiveToPtr)
    {
        gui::SoundManager::Instance()->GetSoundEffectSet(gui::sound_effect_set::Coin).PlayRandom();

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
        gui::SoundManager::Instance()->GetSoundEffectSet(gui::sound_effect_set::Gem).PlayRandom();

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
        gui::SoundManager::Instance()
            ->GetSoundEffectSet(gui::sound_effect_set::MeteorShard)
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
        auto coinsOwnedByOtherPartyMembers { 0_coin };
        for (auto nextCreaturePtr : game::Game::Instance()->State().Party().Characters())
        {
            if (nextCreaturePtr != creaturePtr_)
            {
                const auto NEXT_CREATURE_COINS_OWNED { nextCreaturePtr->Inventory().Coins() };
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
            gui::SoundManager::Instance()
                ->GetSoundEffectSet(gui::sound_effect_set::Coin)
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
        auto gemsOwnedByOtherPartyMembers { 0_gem };
        for (auto nextCreaturePtr : game::Game::Instance()->State().Party().Characters())
        {
            if (nextCreaturePtr != creaturePtr_)
            {
                const auto NEXT_CREATURE_GEMS_OWNED { nextCreaturePtr->Inventory().Gems() };
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
            gui::SoundManager::Instance()
                ->GetSoundEffectSet(gui::sound_effect_set::Gem)
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
        auto shardsOwnedByOtherPartyMembers { 0_mshard };
        for (auto nextCreaturePtr : game::Game::Instance()->State().Party().Characters())
        {
            if (nextCreaturePtr != creaturePtr_)
            {
                const auto NEXT_CREATURE_SHARDS { nextCreaturePtr->Inventory().MeteorShards() };
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
            gui::SoundManager::Instance()
                ->GetSoundEffectSet(gui::sound_effect_set::MeteorShard)
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
            Coin_t totalCoins { 0 };
            for (const auto & CREATURE_PTR : game::Game::Instance()->State().Party().Characters())
            {
                totalCoins += CREATURE_PTR->Inventory().Coins();
            }

            if (0 == totalCoins)
            {
                PopupRejectionWindow("The party has no coins to share.");
                return;
            }
        }

        gui::SoundManager::Instance()->GetSoundEffectSet(gui::sound_effect_set::Coin).PlayRandom();

        HandleCoinsGather(false);

        const auto COINS_TOTAL { creaturePtr_->Inventory().Coins().As<int>() };

        const auto HUMANOID_COUNT { static_cast<int>(
            game::Game::Instance()->State().Party().GetNumHumanoid()) };

        const auto COINS_TO_SHARE { COINS_TOTAL / HUMANOID_COUNT };
        const auto COINS_LEFT_OVER { COINS_TOTAL % HUMANOID_COUNT };

        for (auto nextCreaturePtr : game::Game::Instance()->State().Party().Characters())
        {
            if (nextCreaturePtr->Body().IsHumanoid())
            {
                nextCreaturePtr->CoinsAdj(nextCreaturePtr->Inventory().Coins() * Coin_t(-1));
                nextCreaturePtr->CoinsAdj(Coin_t(COINS_TO_SHARE));
            }
        }

        auto toHandOut { COINS_LEFT_OVER };
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
            Gem_t totalGems { 0 };
            for (const auto & CREATURE_PTR : game::Game::Instance()->State().Party().Characters())
            {
                totalGems += CREATURE_PTR->Inventory().Gems();
            }

            if (0 == totalGems)
            {
                PopupRejectionWindow("The party has no gems to share.");
                return;
            }
        }

        gui::SoundManager::Instance()->GetSoundEffectSet(gui::sound_effect_set::Gem).PlayRandom();

        HandleGemsGather(false);

        const auto GEMS_TOTAL { creaturePtr_->Inventory().Gems().As<int>() };

        const auto HUMANOID_COUNT { static_cast<int>(
            game::Game::Instance()->State().Party().GetNumHumanoid()) };

        const auto GEMS_TO_SHARE { GEMS_TOTAL / HUMANOID_COUNT };
        const auto GEMS_LEFT_OVER { GEMS_TOTAL % HUMANOID_COUNT };

        for (auto nextCreaturePtr : game::Game::Instance()->State().Party().Characters())
        {
            if (nextCreaturePtr->Body().IsHumanoid())
            {
                nextCreaturePtr->GemsAdj(nextCreaturePtr->Inventory().Gems() * Gem_t(-1));
                nextCreaturePtr->GemsAdj(Gem_t(GEMS_TO_SHARE));
            }
        }

        auto toHandOut { GEMS_LEFT_OVER };
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
            MeteorShard_t totalShards { 0 };
            for (const auto & CREATURE_PTR : game::Game::Instance()->State().Party().Characters())
            {
                totalShards += CREATURE_PTR->Inventory().MeteorShards();
            }

            if (0 == totalShards)
            {
                PopupRejectionWindow("The party has no Meteor Shards to share.");
                return;
            }
        }

        gui::SoundManager::Instance()
            ->GetSoundEffectSet(gui::sound_effect_set::MeteorShard)
            .PlayRandom();

        HandleMeteorShardsGather(false);

        const int METEORSHARDS_TOTAL { creaturePtr_->Inventory().MeteorShards().As<int>() };

        const int HUMANOID_COUNT { static_cast<int>(
            game::Game::Instance()->State().Party().GetNumHumanoid()) };

        const auto METEORSHARDS_TO_SHARE { METEORSHARDS_TOTAL / HUMANOID_COUNT };
        const auto METEORSHARDS_LEFT_OVER { METEORSHARDS_TOTAL % HUMANOID_COUNT };

        for (auto nextCreaturePtr : game::Game::Instance()->State().Party().Characters())
        {
            if (nextCreaturePtr->Body().IsHumanoid())
            {
                nextCreaturePtr->MeteorShardsAdj(
                    nextCreaturePtr->Inventory().MeteorShards() * MeteorShard_t(-1));

                nextCreaturePtr->MeteorShardsAdj(MeteorShard_t(METEORSHARDS_TO_SHARE));
            }
        }

        auto toHandOut { METEORSHARDS_LEFT_OVER };
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
        listElementToGivePtrOpt_ = boost::none;
        creatureToGiveToPtrOpt_ = boost::none;
        listElementToDropPtrOpt_ = boost::none;
        contentType_ = ContentType::Count;
        actionType_ = ActionType::Count;
        Setup_CreatureDetails(false);
        Setup_CreatureStats();
        Setup_CenterText();
    }

    void InventoryStage::UpdateImageDetailsPosition()
    {
        detailsPosLeft_
            = CREATURE_IMAGE_POS_LEFT_ + creatureImageWidthScaled_ + CREATURE_IMAGE_RIGHT_PAD_;
    }

    const item::ItemPtrOpt_t InventoryStage::GetItemMouseIsOver(const sf::Vector2f & MOUSE_POS_V)
    {
        if ((view_ == ViewType::Items) && itemLeftListBoxUPtr_ && itemRightListBoxUPtr_)
        {
            {
                const auto UNEQUIP_MOUSEOVER_ELEMENT_PTR_OPT { itemRightListBoxUPtr_->AtPos(
                    MOUSE_POS_V) };

                if (UNEQUIP_MOUSEOVER_ELEMENT_PTR_OPT)
                {
                    return UNEQUIP_MOUSEOVER_ELEMENT_PTR_OPT.value()->ElementOpt();
                }
            }

            {
                const auto EQUIP_MOUSEOVER_ELEMENT_PTR_OPT { itemRightListBoxUPtr_->AtPos(
                    MOUSE_POS_V) };

                if (EQUIP_MOUSEOVER_ELEMENT_PTR_OPT)
                {
                    return EQUIP_MOUSEOVER_ELEMENT_PTR_OPT.value()->ElementOpt();
                }
            }
        }

        return boost::none;
    }

    const FloatRectOpt_t InventoryStage::GetItemRectMouseIsOver(const sf::Vector2f & MOUSE_POS_V)
    {
        if (view_ == ViewType::Items)
        {
            if (itemRightListBoxUPtr_)
            {
                const auto UNEQUIP_MOUSEOVER_ELEMENT_PTR_OPT { itemRightListBoxUPtr_->AtPos(
                    MOUSE_POS_V) };

                if (UNEQUIP_MOUSEOVER_ELEMENT_PTR_OPT)
                {
                    return itemRightListBoxUPtr_->ElementRegion(
                        UNEQUIP_MOUSEOVER_ELEMENT_PTR_OPT.value(), true);
                }
            }

            if (itemLeftListBoxUPtr_)
            {
                const auto EQUIP_MOUSEOVER_ELEMENT_PTR_OPT { itemLeftListBoxUPtr_->AtPos(
                    MOUSE_POS_V) };

                if (EQUIP_MOUSEOVER_ELEMENT_PTR_OPT)
                {
                    return itemLeftListBoxUPtr_->ElementRegion(
                        EQUIP_MOUSEOVER_ELEMENT_PTR_OPT.value(), true);
                }
            }
        }

        return boost::none;
    }

    void InventoryStage::SetupDetailViewItem(const item::ItemPtrOpt_t ITEM_PTR_OPT)
    {
        if (!ITEM_PTR_OPT)
        {
            detailViewTextUPtr_.reset();
            return;
        }

        const auto ITEM_PTR { ITEM_PTR_OPT.value() };

        detailViewCachedTextureOpt_ = gui::LoadAndCacheImage(ITEM_PTR);
        detailViewSprite_.setTexture(detailViewCachedTextureOpt_->Get(), true);

        const auto DETAILVIEW_IMAGE_SCALE { sfutil::MapByRes(0.75f, 1.25f) };
        detailViewSprite_.setScale(DETAILVIEW_IMAGE_SCALE, DETAILVIEW_IMAGE_SCALE);

        detailViewSprite_.setPosition(
            (SCREEN_WIDTH_ * 0.5f) - (detailViewSprite_.getGlobalBounds().width * 0.5f),
            DETAILVIEW_POS_TOP_ + DETAILVIEW_INNER_PAD_);

        std::ostringstream ss;
        ss << ITEM_PTR->Name() << "\n"
           << ITEM_PTR->Desc() << "\n\n"
           << item::category::ToString(ITEM_PTR->Category(), EnumStringHow(Wrap::Yes)) << "\n";

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

        const gui::TextInfo TEXT_INFO(
            ss.str(),
            gui::GuiFont::Default,
            gui::FontManager::Instance()->Size_Normal(),
            sf::Color::White,
            gui::Justified::Center);

        const auto DETAILVIEW_SPRITE_BOTTOM { sfutil::Bottom(detailViewSprite_) };

        const auto LEFT { DETAILVIEW_POS_LEFT_ + DETAILVIEW_INNER_PAD_ };

        const auto TOP { DETAILVIEW_SPRITE_BOTTOM + DETAILVIEW_INNER_PAD_ };

        const auto DOUBLE_INNER_PAD { 2.0f * DETAILVIEW_INNER_PAD_ };

        const auto WIDTH { DETAILVIEW_WIDTH_ - DOUBLE_INNER_PAD };

        const auto HEIGHT { (DETAILVIEW_HEIGHT_ - DETAILVIEW_SPRITE_BOTTOM) - DOUBLE_INNER_PAD };

        const sf::FloatRect TEXT_RECT(LEFT, TOP, WIDTH, HEIGHT);

        detailViewTextUPtr_ = std::make_unique<gui::TextRegion>(
            "InventoryStage'sDetailViewForItems", TEXT_INFO, TEXT_RECT);
    }

    void InventoryStage::SetupDetailViewCreature(const creature::CreaturePtrOpt_t CREATURE_PTR_OPT)
    {
        if (!CREATURE_PTR_OPT)
        {
            detailViewTextUPtr_.reset();
            return;
        }

        const auto CREATURE_PTR { CREATURE_PTR_OPT.value() };

        detailViewCachedTextureOpt_ = gui::LoadAndCacheImage(CREATURE_PTR);
        detailViewSprite_.setTexture(detailViewCachedTextureOpt_->Get(), true);
        detailViewSprite_.setScale(CREATURE_IMAGE_SCALE_, CREATURE_IMAGE_SCALE_);

        detailViewSprite_.setPosition(
            (SCREEN_WIDTH_ * 0.5f) - (detailViewSprite_.getGlobalBounds().width * 0.5f),
            DETAILVIEW_POS_TOP_ + DETAILVIEW_INNER_PAD_ + sfutil::SpacerOld(50.0f));

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

        const gui::TextInfo TEXT_INFO(
            ss.str(),
            gui::GuiFont::Default,
            gui::FontManager::Instance()->Size_Smallish(),
            sf::Color::White,
            gui::Justified::Left);

        const auto DETAILVIEW_BOTTOM { sfutil::Bottom(detailViewSprite_) };

        const sf::FloatRect TEXT_RECT(
            DETAILVIEW_POS_LEFT_ + DETAILVIEW_INNER_PAD_,
            DETAILVIEW_BOTTOM - 20.0f,
            DETAILVIEW_WIDTH_ - (2.0f * DETAILVIEW_INNER_PAD_),
            (DETAILVIEW_HEIGHT_ - DETAILVIEW_BOTTOM) - (2.0f * DETAILVIEW_INNER_PAD_));

        detailViewTextUPtr_ = std::make_unique<gui::TextRegion>(
            "InventoryStage'sDetailViewForCreatureAchievements", TEXT_INFO, TEXT_RECT);
    }

    const std::string InventoryStage::MakeTitleString(
        const creature::CreaturePtr_t CREATURE_PTR,
        const creature::AchievementType::Enum WHICH_ACHV) const
    {
        const auto & ACHIEVEMENT { CREATURE_PTR->GetAchievements().Get(WHICH_ACHV) };

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
        const auto & ACHIEVEMENTS { CREATURE_PTR->GetAchievements() };
        const auto TITLE_PTR_OPT { ACHIEVEMENTS.GetNextTitle(WHICH_ACHV) };

        if (TITLE_PTR_OPT)
        {
            const auto ACHV_COUNT_REQUIRED { TITLE_PTR_OPT.value()->AchievementCount() };
            const auto ACHV_COUNT_CURRENT { ACHIEVEMENTS.Get(WHICH_ACHV).Count() };
            const auto NEEDED_COUNT { ACHV_COUNT_REQUIRED - ACHV_COUNT_CURRENT };

            std::ostringstream ss;
            ss << ", need  " << NEEDED_COUNT << " more to achieve \""
               << TITLE_PTR_OPT.value()->Name() << "\"";

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
        gui::SoundManager::Instance()->PlaySfx_TickOff();

        isDetailViewFadingIn_ = false;
        isDetailViewDoneFading_ = false;
        isDetailViewFadingOut_ = true;
        detailViewSlider_ = gui::SliderZeroToOne(DETAILVIEW_SLIDER_SPEED_);
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

        const auto SPELL_NAME { SPELL_PTR->Name() };
        const auto SPELL_TARGET { SPELL_PTR->Target() };

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

        fightResult_ = creatureInteraction_.Cast(SPELL_PTR, creaturePtr_, TARGET_CREATURES_PVEC);

        Setup_CreatureDetails(false);
        Setup_CreatureStats();
        Setup_CenterText();

        creatureEffectIndex_ = 0;
        hitInfoIndex_ = 0;

        hasTakenActionSpellOrSong_ = true;

        sparkleAnimUPtr_ = std::make_unique<gui::animation::SparkleAnimation>(
            creatureSprite_.getGlobalBounds(),
            sfutil::MapByRes(0.333f, 1.33f),
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

        combat::Text combatText;
        bool isFightResultCollapsed { false };

        const auto ACTION_TEXT { combatText.ActionTextIndexed(
            creaturePtr_,
            turnActionInfo_,
            fightResult_,
            false,
            creatureEffectIndex_,
            hitInfoIndex_,
            isFightResultCollapsed) };

        const auto POPUPINFO_NOITEM { popup::PopupManager::Instance()->CreatePopupInfo(
            POPUP_NAME_SPELL_RESULT_,
            ACTION_TEXT,
            popup::PopupButtons::Okay,
            popup::PopupImage::Regular,
            gui::Justified::Center,
            gui::sound_effect::None,
            gui::FontManager::Instance()->Size_Normal()) };

        const auto CREATURE_EFFECTS_VEC { fightResult_.Effects() };

        const auto HIT_INFO_VEC { CREATURE_EFFECTS_VEC[creatureEffectIndex_].GetHitInfoVec() };

        if (isFightResultCollapsed || (++hitInfoIndex_ >= HIT_INFO_VEC.size()))
        {
            hitInfoIndex_ = 0;
            if (isFightResultCollapsed || (++creatureEffectIndex_ >= CREATURE_EFFECTS_VEC.size()))
            {
                spellBeingCastPtrOpt_ = boost::none;
            }
        }

        SpawnPopup(misc::MakeNotNull(this), POPUPINFO_NOITEM);
        isWaitingOnPopup_ = true;
        return false;
    }

    void InventoryStage::ForceSelectionAndDrawOfLeftListBox()
    {
        if (itemLeftListBoxUPtr_)
        {
            SetFocus(itemLeftListBoxUPtr_);
        }
        else if (condLeftListBoxUPtr_)
        {
            SetFocus(condLeftListBoxUPtr_);
        }
        else if (spellLeftListBoxUPtr_)
        {
            SetFocus(spellLeftListBoxUPtr_);
        }
        else if (titleLeftListBoxUPtr_)
        {
            SetFocus(titleLeftListBoxUPtr_);
        }
    }

    bool InventoryStage::HandleSpellsOrSongs()
    {
        const auto IS_SPELLS { (creaturePtr_->Role() != creature::role::Bard) };

        if (game::Phase::Combat == previousPhase_)
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
            const auto CAN_CAST_STR { creaturePtr_->CanCastSpellsStr(true) };
            if (CAN_CAST_STR.empty())
            {
                const auto POPUP_INFO { popup::PopupManager::Instance()->CreateSpellbookPopupInfo(
                    POPUP_NAME_SPELLBOOK_, creaturePtr_, creaturePtr_->LastSpellCastNum()) };

                SpawnPopup(misc::MakeNotNull(this), POPUP_INFO);
            }
            else
            {
                PopupRejectionWindow(CAN_CAST_STR);
            }
        }
        else
        {
            const auto CAN_PLAY_STR { creaturePtr_->CanPlaySongsStr(true) };
            if (CAN_PLAY_STR.empty())
            {
                const auto POPUP_INFO { popup::PopupManager::Instance()->CreateMusicPopupInfo(
                    POPUP_NAME_MUSICSHEET_, creaturePtr_, creaturePtr_->LastSongPlayedNum()) };

                SpawnPopup(misc::MakeNotNull(this), POPUP_INFO);
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

            const auto TARGETS_PVEC { (
                (SONG_PTR->Target() == combat::TargetType::AllCompanions)
                    ? creature::Algorithms::Players()
                    : creature::Algorithms::NonPlayers(creature::Algorithms::Living)) };

            turnActionInfo_ = combat::TurnActionInfo(SONG_PTR, TARGETS_PVEC);
            fightResult_ = creatureInteraction_.PlaySong(SONG_PTR, creaturePtr_, TARGETS_PVEC);

            Setup_CreatureDetails(false);
            Setup_CreatureStats();
            Setup_CenterText();

            creatureEffectIndex_ = 0;
            hitInfoIndex_ = 0;

            hasTakenActionSpellOrSong_ = true;

            songAnimUPtr_ = std::make_unique<gui::animation::SongAnimation>(
                creatureSprite_.getGlobalBounds(),
                0.1f,
                sfutil::MapByRes(0.1f, 0.25f),
                0.25f,
                sfutil::MapByRes(0.35f, 0.9f),
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

        combat::Text combatText;

        bool isFightResultCollapsed { false };
        const auto ACTION_TEXT { combatText.ActionTextIndexed(
            creaturePtr_,
            turnActionInfo_,
            fightResult_,
            false,
            creatureEffectIndex_,
            hitInfoIndex_,
            isFightResultCollapsed) };

        const auto POPUPINFO_NOITEM { popup::PopupManager::Instance()->CreatePopupInfo(
            POPUP_NAME_SONG_RESULT_,
            ACTION_TEXT,
            popup::PopupButtons::Okay,
            popup::PopupImage::Regular,
            gui::Justified::Center,
            gui::sound_effect::None,
            gui::FontManager::Instance()->Size_Normal()) };

        const auto CREATURE_EFFECTS_VEC { fightResult_.Effects() };
        const auto HIT_INFO_VEC { CREATURE_EFFECTS_VEC[creatureEffectIndex_].GetHitInfoVec() };

        if (isFightResultCollapsed || (++hitInfoIndex_ >= HIT_INFO_VEC.size()))
        {
            hitInfoIndex_ = 0;

            if (isFightResultCollapsed || (++creatureEffectIndex_ >= CREATURE_EFFECTS_VEC.size()))
            {
                songBeingPlayedPtrOpt_ = boost::none;
            }
        }

        SpawnPopup(misc::MakeNotNull(this), POPUPINFO_NOITEM);
        isWaitingOnPopup_ = true;

        // return false because one popup is replacing another
        return false;
    }

    void InventoryStage::SystemErrorPopup(
        const std::string & GENERAL_ERROR_MSG,
        const std::string & TECH_ERROR_MSG,
        const std::string & TITLE_MSG)
    {
        SpawnPopup(
            misc::MakeNotNull(this),
            popup::PopupManager::Instance()->CreateSystemErrorPopupInfo(
                "Stage'sSystemErrorPopupName", GENERAL_ERROR_MSG, TECH_ERROR_MSG, TITLE_MSG));
    }

    void InventoryStage::SetDetailViewQuads()
    {
        const auto SOURCE_LEFT { detailViewSourceRect_.left };
        const auto SOURCE_RGT { detailViewSourceRect_.left + detailViewSourceRect_.width };
        const auto SOURCE_TOP { detailViewSourceRect_.top };
        const auto SOURCE_BOTTOM { detailViewSourceRect_.top + detailViewSourceRect_.height };

        const auto TARGET_LEFT { DETAILVIEW_POS_LEFT_ };
        const auto TARGET_RGT { DETAILVIEW_POS_LEFT_ + DETAILVIEW_WIDTH_ };
        const auto TARGET_TOP { DETAILVIEW_POS_TOP_ };
        const auto TARGET_BOTTOM { DETAILVIEW_POS_TOP_ + DETAILVIEW_HEIGHT_ };

        const auto RATIO { detailViewSliderRatio_ };

        const auto LEFT_MORE { SOURCE_LEFT - ((SOURCE_LEFT - TARGET_LEFT) * RATIO) };
        const auto LEFT_LESS { SOURCE_LEFT + ((TARGET_LEFT - SOURCE_LEFT) * RATIO) };
        const auto LEFT { (SOURCE_LEFT > TARGET_LEFT) ? LEFT_MORE : LEFT_LESS };

        const auto RGT_MORE { SOURCE_RGT - ((SOURCE_RGT - TARGET_RGT) * RATIO) };
        const auto RGT_LESS { SOURCE_RGT + ((TARGET_RGT - SOURCE_RGT) * RATIO) };
        const auto RGT { (SOURCE_RGT > TARGET_RGT) ? RGT_MORE : RGT_LESS };

        const auto TOP { SOURCE_TOP - ((SOURCE_TOP - TARGET_TOP) * RATIO) };

        const auto BOTTOM_MORE { SOURCE_BOTTOM - ((SOURCE_BOTTOM - TARGET_BOTTOM) * RATIO) };
        const auto BOTTOM_LESS { SOURCE_BOTTOM + ((TARGET_BOTTOM - SOURCE_BOTTOM) * RATIO) };
        const auto BOTTOM { (SOURCE_BOTTOM > TARGET_BOTTOM) ? BOTTOM_MORE : BOTTOM_LESS };

        detailViewQuads_[0].position = sf::Vector2f(LEFT, TOP);
        detailViewQuads_[1].position = sf::Vector2f(RGT, TOP);
        detailViewQuads_[2].position = sf::Vector2f(RGT, BOTTOM);
        detailViewQuads_[3].position = sf::Vector2f(LEFT, BOTTOM);
    }

    bool InventoryStage::IfMouseDownIsOnDisabledButtonPopupRejection(
        const gui::ImageTextEntityUPtr_t & BUTTON_UPTR, const sf::Vector2f & MOUSE_POS_V)
    {
        if ((BUTTON_UPTR->GetMouseState() == gui::MouseState::Disabled)
            && BUTTON_UPTR->GetEntityRegion().contains(MOUSE_POS_V))
        {
            PopupRejectionWindow(BUTTON_UPTR->GetMouseHoverText());
            return true;
        }
        else
        {
            return false;
        }
    }

    void InventoryStage::FreeAllLeftListBoxes()
    {
        if (itemLeftListBoxUPtr_)
        {
            EntityRemove(itemLeftListBoxUPtr_);
            itemLeftListBoxUPtr_.reset();
        }

        if (condLeftListBoxUPtr_)
        {
            EntityRemove(condLeftListBoxUPtr_);
            condLeftListBoxUPtr_.reset();
        }

        if (spellLeftListBoxUPtr_)
        {
            EntityRemove(spellLeftListBoxUPtr_);
            spellLeftListBoxUPtr_.reset();
        }

        if (titleLeftListBoxUPtr_)
        {
            EntityRemove(titleLeftListBoxUPtr_);
            titleLeftListBoxUPtr_.reset();
        }
    }

    void InventoryStage::LeftListBoxSetHorizPosition(const float NEW_POS_HORIZ)
    {
        if (itemLeftListBoxUPtr_)
        {
            itemLeftListBoxUPtr_->SetEntityPos(
                NEW_POS_HORIZ, itemLeftListBoxUPtr_->GetEntityPos().y);
        }
        else if (condLeftListBoxUPtr_)
        {
            condLeftListBoxUPtr_->SetEntityPos(
                NEW_POS_HORIZ, condLeftListBoxUPtr_->GetEntityPos().y);
        }
        else if (spellLeftListBoxUPtr_)
        {
            spellLeftListBoxUPtr_->SetEntityPos(
                NEW_POS_HORIZ, spellLeftListBoxUPtr_->GetEntityPos().y);
        }
        else if (titleLeftListBoxUPtr_)
        {
            titleLeftListBoxUPtr_->SetEntityPos(
                NEW_POS_HORIZ, titleLeftListBoxUPtr_->GetEntityPos().y);
        }
    }

    void InventoryStage::UpdateCreatureImage()
    {
        const auto FOUND_ITER { creatureToImageMap_.Find(creaturePtr_) };

        if (FOUND_ITER == std::end(creatureToImageMap_))
        {
            auto cachedTexture { gui::LoadAndCacheImage(
                creaturePtr_, gui::ImageOptions::InvertedCharacterOptions()) };

            creatureSprite_.setTexture(cachedTexture.Get(), true);

            creatureToImageMap_.Append(creaturePtr_, cachedTexture);
        }
        else
        {
            creatureSprite_.setTexture(FOUND_ITER->second.Get(), true);
        }

        creatureSprite_.setPosition(CREATURE_IMAGE_POS_LEFT_, CREATURE_IMAGE_POS_TOP_);
        creatureSprite_.setColor(CHARATER_IMAGE_COLOR_);
        creatureSprite_.setScale(CREATURE_IMAGE_SCALE_, CREATURE_IMAGE_SCALE_);
        creatureImageWidthScaled_ = creatureSprite_.getGlobalBounds().width;
    }

    void InventoryStage::SetButtonDisabledIf(
        gui::ImageTextEntityUPtr_t & buttonUPtr, const bool WILL_DISABLE)
    {
        if (WILL_DISABLE)
        {
            buttonUPtr->SetMouseState(gui::MouseState::Disabled);
        }
        else
        {
            buttonUPtr->SetMouseState(gui::MouseState::Up);
        }
    }

} // namespace stage
} // namespace heroespath
