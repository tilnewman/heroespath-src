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

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/gui/text-region.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/gui/creature-image-manager.hpp"
#include "sfml-util/gui/item-image-manager.hpp"
#include "sfml-util/gui/title-image-manager.hpp"
#include "sfml-util/gui/list-box-item.hpp"
#include "sfml-util/gui/box.hpp"

#include "game/ouroboros.hpp"
#include "game/game.hpp"
#include "game/game-data-file.hpp"
#include "game/loop-manager.hpp"
#include "game/state/game-state.hpp"
#include "game/player/party.hpp"
#include "game/player/character.hpp"
#include "game/non-player/party.hpp"
#include "game/non-player/character.hpp"
#include "game/creature/condition.hpp"
#include "game/creature/creature.hpp"
#include "game/spell/spell-base.hpp"
#include "game/item/item.hpp"
#include "game/item/item-warehouse.hpp"

#include "misc/real.hpp"
#include "misc/assertlogandthrow.hpp"

#include <vector>


namespace game
{
namespace stage
{

    const float InventoryStage::VIEW_CHANGE_SLIDER_SPEED_     (4.0f);
    const float InventoryStage::VIEW_CHANGE_BETWEEN_TIME_SEC_ (0.5f);
    const float InventoryStage::CREATURE_IMAGE_RIGHT_PAD_     (10.0f);
    const float InventoryStage::DETAILVIEW_SLIDER_SPEED_      (4.0f);
    const float InventoryStage::DETAILVIEW_TIMER_DURATION_SEC_(2.33f);
    const float InventoryStage::DETAILVIEW_COLOR_ALPHA_START_ (20.0f);
    const float InventoryStage::DETAILVIEW_COLOR_ALPHA_END_   (220.0f);
    const float InventoryStage::DETAILVIEW_INNER_PAD_         (20.0f);
    //
    const std::string InventoryStage::POPUP_NAME_GIVE_            ("InventoryStage'sGivePopupName");
    const std::string InventoryStage::POPUP_NAME_CHAR_SELECT_     ("InventoryStage'sCharacterSelectPopupName");
    const std::string InventoryStage::POPUP_NAME_NUMBER_SELECT_   ("InventoryStage'sPopupNumberSelectPopupName");
    const std::string InventoryStage::POPUP_NAME_CONTENTSELECTION_("InventoryStage'sShareGatherSelectionPopupName");
    const std::string InventoryStage::POPUP_NAME_DROPCONFIRM_     ("InventoryStage'sDropItemConfirmationPopupName");


    InventoryStage::InventoryStage(creature::CreaturePtr_t creaturePtr)
    :
        Stage                      ("Inventory", 0.0f, 0.0f, sfml_util::Display::Instance()->GetWinWidth(), sfml_util::Display::Instance()->GetWinHeight()),
        SCREEN_WIDTH_              (sfml_util::Display::Instance()->GetWinWidth()),
        SCREEN_HEIGHT_             (sfml_util::Display::Instance()->GetWinHeight()),
        INNER_PAD_                 (sfml_util::MapByRes(10.0f, 40.0f)),
        INNER_RECT_                (INNER_PAD_, INNER_PAD_, SCREEN_WIDTH_ - (2.0f * INNER_PAD_), SCREEN_HEIGHT_ - (2.0f * INNER_PAD_)),
        CREATURE_IMAGE_POS_LEFT_   (INNER_RECT_.left + sfml_util::MapByRes(35.0f, 100.0f)),
        CREATURE_IMAGE_SCALE_      (sfml_util::MapByRes(0.75f, 3.25f)),
        CREATURE_IMAGE_HEIGHT_MAX_ (sfml_util::gui::CreatureImageManager::DimmensionMax() * CREATURE_IMAGE_SCALE_),
        LISTBOX_HEIGHT_REDUCTION_  (sfml_util::MapByRes(100.0f, 400.0f)),
        LISTBOX_SCREEN_EDGE_MARGIN_(sfml_util::MapByRes(35.0f, 100.0f)),
        LISTBOX_BETWEEN_SPACER_    (sfml_util::MapByRes(65.0f, 200.0f)),
        LISTBOX_WIDTH_             (((INNER_RECT_.width - (2.0f * LISTBOX_SCREEN_EDGE_MARGIN_)) - LISTBOX_BETWEEN_SPACER_) * 0.5f),
        OUT_OF_SIGHT_POS_          ((LISTBOX_WIDTH_ + sfml_util::MapByRes(100.0f, 300.0f)) * -1.0f),
        FIRST_LISTBOX_POS_LEFT_    (INNER_RECT_.left + LISTBOX_SCREEN_EDGE_MARGIN_),
        SECOND_LISTBOX_POS_LEFT_   (FIRST_LISTBOX_POS_LEFT_ + LISTBOX_WIDTH_ + LISTBOX_BETWEEN_SPACER_),
        STATS_POS_LEFT_            ((INNER_RECT_.left + INNER_RECT_.width) - sfml_util::MapByRes(275.0f, 1200.0f)),
        LISTBOX_COLOR_IMAGE_       (sf::Color(255, 255, 255, 190)),
        LISTBOX_COLOR_LINE_        (sfml_util::FontManager::Color_GrayDark()),
        LISTBOX_COLOR_FG_          (LISTBOX_COLOR_LINE_),
        LISTBOX_COLOR_BG_          (sfml_util::FontManager::Color_Orange() - sf::Color(100, 100, 100, 220)),
        LISTBOX_COLOR_TITLE_       (sfml_util::FontManager::Color_Orange() - sf::Color(130, 130, 130, 0)),
        DESCBOX_TEXT_COLOR_        (LISTBOX_COLOR_TITLE_ - sf::Color(50, 50, 50, 0)),
        DESCBOX_TEXT_SIZE_         (sfml_util::FontManager::Instance()->Size_Largeish()),
        LISTBOX_COLORSET_          (LISTBOX_COLOR_FG_, LISTBOX_COLOR_BG_),
        LISTBOX_BG_INFO_           (LISTBOX_COLOR_BG_),
        mainMenuTitle_             ("inventory_button.png", true, 1.0f, 0.75f),
        CREATURE_IMAGE_POS_TOP_    (mainMenuTitle_.LowerPosition(false)),
        LISTBOX_POS_TOP_           (CREATURE_IMAGE_POS_TOP_ + CREATURE_IMAGE_HEIGHT_MAX_ + (LISTBOX_HEIGHT_REDUCTION_ * 0.5f)),
        LISTBOX_HEIGHT_            ((SCREEN_HEIGHT_ - LISTBOX_POS_TOP_) - LISTBOX_HEIGHT_REDUCTION_),
        LISTBOX_REGION_            (FIRST_LISTBOX_POS_LEFT_, LISTBOX_POS_TOP_, LISTBOX_WIDTH_, LISTBOX_HEIGHT_),
        DESCBOX_REGION_            (SECOND_LISTBOX_POS_LEFT_, LISTBOX_POS_TOP_, LISTBOX_WIDTH_, LISTBOX_HEIGHT_),
        DESCBOX_MARGIN_            (15.0f),
        DESCBOX_MARGINS_           (DESCBOX_MARGIN_, DESCBOX_MARGIN_, DESCBOX_MARGIN_, DESCBOX_MARGIN_),
        DETAILVIEW_WIDTH_          (SCREEN_WIDTH_ * 0.75f),
        DETAILVIEW_HEIGHT_         (SCREEN_HEIGHT_ * 0.85f),
        DETAILVIEW_POS_LEFT_       ((SCREEN_WIDTH_ * 0.5f) - (DETAILVIEW_WIDTH_ * 0.5f)),
        DETAILVIEW_POS_TOP_        (sfml_util::MapByRes(35.0f, 100.0f)),
        listBoxItemTextInfo_    (" ", sfml_util::FontManager::Instance()->Font_Default2(), sfml_util::FontManager::Instance()->Size_Smallish(), sfml_util::FontManager::Color_GrayDarker(), sfml_util::Justified::Left),
        creaturePtr_            (creaturePtr),
        bottomSymbol_           (0.75f, true, BottomSymbol::DEFAULT_INVALID_DIMM_, BottomSymbol::DEFAULT_INVALID_DIMM_, BottomSymbol::DEFAULT_HORIZ_POS_, 0.0f, sf::Color::White),
        paperBgTexture_         (),
        paperBgSprite_          (),
        ouroborosUPtr_          (),
        creatureTexture_        (),
        creatureSprite_         (),
        view_                   (ViewType::Items),
        characterViewMap_       (),
        isSliderAnimating_      (false),
        isSlidingLeft_          (false),
        isViewForcedToItems_    (false),
        viewToChangeTo_         (ViewType::Count),
        sliderAnimTimerSec_     (VIEW_CHANGE_BETWEEN_TIME_SEC_ + 1.0f),//anything greater than VIEW_CHANGE_BETWEEN_TIME_SEC_ will work here
        detailsPosLeft_         (0.0f),
        centerPosLeft_          (0.0f),
        isImageSliding_         (false),
        isDetailsSliding_       (false),
        isCenterSliding_        (false),
        isStatsSliding_         (false),
        isListBoxSliding_       (false),
        isDescBoxSliding_       (false),
        isImageSlidingDone_     (true),
        isDetailsSlidingDone_   (true),
        isCenterSlidingDone_    (true),
        isStatsSlidingDone_     (true),
        isListBoxSlidingDone_   (true),
        isDescBoxSlidingDone_   (true),
        hasImageChanged_        (false),
        hasDetailsChanged_      (false),
        hasCenterChanged_       (false),
        hasStatsChanged_        (false),
        hasListBoxChanged_      (false),
        hasDescBoxChanged_      (false),
        imageSlider_            (VIEW_CHANGE_SLIDER_SPEED_),
        detailsSlider_          (VIEW_CHANGE_SLIDER_SPEED_),
        centerSlider_           (VIEW_CHANGE_SLIDER_SPEED_),
        statsSlider_            (VIEW_CHANGE_SLIDER_SPEED_),
        listBoxSlider_          (VIEW_CHANGE_SLIDER_SPEED_),
        descBoxSlider_          (VIEW_CHANGE_SLIDER_SPEED_),
        detailsTextRegionUPtr_  (),
        statsTextRegionUPtr_    (),
        eqTitleTextRegionUPtr_  (),
        unEqTitleTextRegionUPtr_(),
        equippedListBoxSPtr_    (),
        unEquipListBoxSPtr_     (),
        insTextRegionUPtr_      (),
        descTextRegionUPtr_     (),
        descBoxUPtr_            (),
        centerTextRegionUPtr_   (),
        backButtonUPtr_         (),
        itemsButtonUPtr_        (),
        titlesButtonUPtr_       (),
        condsButtonUPtr_        (),
        spellsButtonUPtr_       (),
        giveButtonUPtr_         (),
        shareButtonUPtr_        (),
        gatherButtonUPtr_       (),
        equipButtonUPtr_        (),
        unequipButtonUPtr_      (),
        buttonPVec_             (),
        actionType_             (ActionType::Count),
        contentType_            (ContentType::Count),
        listBoxItemToGiveSPtr_  (),
        creatureToGiveToPtr_    (),
        iItemToDropPtr_         (),
        isDetailViewFadingIn_   (false),
        isDetailViewFadingOut_  (false),
        isDetailViewDoneFading_ (false),
        isAchievementDisplaying_(false),
        hasMouseMoved_          (false),
        isWaitingOnPopup_       (false),
        detailViewTimerSec_     (0.0f),
        detailViewSliderRatio_  (0.0f),
        mousePosV_              (0.0f, 0.0f),
        detailViewSourceRect_   (sfml_util::gui::ListBox::ERROR_RECT_),
        detailViewQuads_        (sf::Quads, 4),
        detailViewSprite_       (),
        detailViewTexture_      (),
        detailViewTextUPtr_     (),
        detailViewSlider_       (DETAILVIEW_SLIDER_SPEED_)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((creaturePtr != nullptr),
            "game::stage::InventoryStage::InventoryStage() was given a null "
            << "creature shared pointer.");

        const std::size_t NUM_CHARACTERS(characterViewMap_.size());
        for (std::size_t i(0); i < NUM_CHARACTERS; ++i)
        {
            characterViewMap_[i] = ViewType::Count;
        }
    }


    InventoryStage::~InventoryStage()
    {
        ClearAllEntities();
    }


    bool InventoryStage::HandleCallback(const sfml_util::gui::callback::ListBoxEventPackage & PACKAGE)
    {
        if (PACKAGE.package.PTR_ != nullptr)
        {
            if ((PACKAGE.gui_event == sfml_util::GuiEvent::Click) ||
                (PACKAGE.gui_event == sfml_util::GuiEvent::SelectionChange) ||
                (PACKAGE.keypress_event.code == sf::Keyboard::Up) ||
                (PACKAGE.keypress_event.code == sf::Keyboard::Down))
            {
                SetDescBoxTextFromListBoxItem(PACKAGE.package.PTR_->GetSelected());
            }
            else if ((PACKAGE.gui_event == sfml_util::GuiEvent::DoubleClick) || (PACKAGE.keypress_event.code == sf::Keyboard::Return))
            {
                if (PACKAGE.package.PTR_ == equippedListBoxSPtr_.get())
                {
                    return HandleUnequip();
                }
                else if (PACKAGE.package.PTR_ == unEquipListBoxSPtr_.get())
                {
                    return HandleEquip();
                }
            }
        }

        return false;
    }


    bool InventoryStage::HandleCallback(const sfml_util::gui::callback::FourStateButtonCallbackPackage_t & PACKAGE)
    {
        if (isSliderAnimating_)
            return false;

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
            return HandleViewChange(ViewType::Spells);
        }

        if (PACKAGE.PTR_ == giveButtonUPtr_.get())
        {
            return HandleGive();
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
            return HandleEquip();
        }

        if (PACKAGE.PTR_ == unequipButtonUPtr_.get())
        {
            return HandleUnequip();
        }

        if (PACKAGE.PTR_ == dropButtonUPtr_.get())
        {
            return HandleDrop();
        }

        return false;
    }


    bool InventoryStage::HandleCallback(const game::callback::PopupResponse & POPUP_RESPONSE)
    {
        isWaitingOnPopup_ = false;

        if ((POPUP_RESPONSE.Info().Name() == POPUP_NAME_DROPCONFIRM_) && (POPUP_RESPONSE.Response() == sfml_util::Response::Yes))
        {
            sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::ItemDrop).PlayRandom();
            creaturePtr_->ItemRemove(iItemToDropPtr_);
            item::ItemWarehouse::Instance()->Free(iItemToDropPtr_);
            EndOfGiveShareGatherTasks();
            SetupDescBox(false);
        }
        else if ((POPUP_RESPONSE.Info().Name() == POPUP_NAME_GIVE_) && (POPUP_RESPONSE.Response() == sfml_util::Response::Select))
        {
            if (POPUP_RESPONSE.Selection() == PopupInfo::ContentNum_Item())
            {
                listBoxItemToGiveSPtr_ = unEquipListBoxSPtr_->GetSelected();

                if (listBoxItemToGiveSPtr_.get() != nullptr)
                {
                    auto const IITEM_PTR(listBoxItemToGiveSPtr_->ITEM_CPTR);
                    if (IITEM_PTR != nullptr)
                    {
                        PopupCharacterSelectWindow("Give the " + IITEM_PTR->Name() + " to whom?");
                        contentType_ = ContentType::Item;
                        return false;
                    }
                }
                else
                {
                    PopupRejectionWindow("There is no item in the unequipped list to give!");
                    return false;
                }
            }
            else if (POPUP_RESPONSE.Selection() == PopupInfo::ContentNum_Coins())
            {
                if (creaturePtr_->Inventory().Coins() == 0)
                {
                    std::ostringstream ss;
                    ss << creaturePtr_->Name() << " has no coins to give!";

                    PopupRejectionWindow(ss.str());
                    return false;
                }
                else if (creaturePtr_->Inventory().Coins() == 1)
                {
                    PopupCharacterSelectWindow("Give the coin to whom?");
                }
                else
                {
                    PopupCharacterSelectWindow("Give coins to whom?");
                }

                contentType_ = ContentType::Coins;
                return false;
            }
            else if (POPUP_RESPONSE.Selection() == PopupInfo::ContentNum_Gems())
            {
                if (creaturePtr_->Inventory().Gems() == 0)
                {
                    std::ostringstream ss;
                    ss << creaturePtr_->Name() << " has no gems to give!";
                    PopupRejectionWindow(ss.str());
                    return false;
                }
                else if (creaturePtr_->Inventory().Gems() == 1)
                {
                    PopupCharacterSelectWindow("Give the gem to whom?");
                }
                else
                {
                    PopupCharacterSelectWindow("Give gems to whom?");
                }

                contentType_ = ContentType::Gems;
                return false;
            }
            else if (POPUP_RESPONSE.Selection() == PopupInfo::ContentNum_MeteorShards())
            {
                if (creaturePtr_->Inventory().MeteorShards() == 0)
                {
                    std::ostringstream ss;
                    ss << creaturePtr_->Name() << " has no Meteor Shards to give!";
                    PopupRejectionWindow(ss.str());
                    return false;
                }
                else if (creaturePtr_->Inventory().MeteorShards() == 1)
                {
                    PopupCharacterSelectWindow("Give the Meteor Shard to whom?");
                }
                else
                {
                    PopupCharacterSelectWindow("Give Meteor Shards to whom?");
                }

                contentType_ = ContentType::MeteorShards;
                return false;
            }
        }
        else if ((POPUP_RESPONSE.Info().Name() == POPUP_NAME_CHAR_SELECT_) && (POPUP_RESPONSE.Response() == sfml_util::Response::Select))
        {
            creatureToGiveToPtr_ = Game::Instance()->State().Party().GetAtOrderPos(POPUP_RESPONSE.Selection());

            if (creatureToGiveToPtr_ == nullptr)
            {
                return false;
            }

            switch (contentType_)
            {
                case ContentType::Item:
                {
                    if (listBoxItemToGiveSPtr_.get() == nullptr)
                    {
                        return false;
                    }

                    const item::ItemPtr_t IITEM_PTR(listBoxItemToGiveSPtr_->ITEM_CPTR);
                    const std::string CAN_GIVE_ITEM_STR(creatureToGiveToPtr_->ItemIsAddAllowed(IITEM_PTR));

                    if (CAN_GIVE_ITEM_STR.empty() == false)
                    {
                        std::ostringstream ss;
                        ss << "Can't give the " << IITEM_PTR->Name() << " to " << creatureToGiveToPtr_->Name() << " because:  " << CAN_GIVE_ITEM_STR;
                        PopupRejectionWindow(ss.str(), true);
                        return false;
                    }
                    else
                    {
                        sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::ItemGive).PlayRandom();
                        creaturePtr_->ItemRemove(IITEM_PTR);
                        creatureToGiveToPtr_->ItemAdd(IITEM_PTR);
                        SetupDescBox(false);
                        EndOfGiveShareGatherTasks();
                        return false;
                    }
                }
                case ContentType::Coins:
                {
                    if (creaturePtr_->Inventory().Coins() == 1)
                    {
                        HandleCoinsGive(1, creatureToGiveToPtr_);
                        return false;
                    }
                    else
                    {
                        std::ostringstream ss;
                        ss << "\nGive " << creatureToGiveToPtr_->Name() << " how many coins?";
                        PopupNumberSelectWindow(ss.str(), static_cast<std::size_t>(creaturePtr_->Inventory().Coins()));
                        return false;
                    }
                }
                case ContentType::Gems:
                {
                    if (creaturePtr_->Inventory().Gems() == 1)
                    {
                        HandleGemsGive(1, creatureToGiveToPtr_);
                        return false;
                    }
                    else
                    {
                        std::ostringstream ss;
                        ss << "\nGive " << creatureToGiveToPtr_->Name() << " how many gems?";
                        PopupNumberSelectWindow(ss.str(), static_cast<std::size_t>(creaturePtr_->Inventory().Gems()));
                        return false;
                    }
                }
                case ContentType::MeteorShards:
                case ContentType::Count:
                default:
                {
                    if (creaturePtr_->Inventory().MeteorShards() == 1)
                    {
                        HandleMeteorShardsGive(1, creatureToGiveToPtr_);
                        return false;
                    }
                    else
                    {
                        std::ostringstream ss;
                        ss << "\nGive " << creatureToGiveToPtr_->Name() << " how many Meteor Shards?";
                        PopupNumberSelectWindow(ss.str(), static_cast<std::size_t>(creaturePtr_->Inventory().MeteorShards()));
                        return false;
                    }
                }
            }
        }
        else if ((POPUP_RESPONSE.Info().Name() == POPUP_NAME_NUMBER_SELECT_) &&
                 (POPUP_RESPONSE.Response() == sfml_util::Response::Select) &&
                 (creatureToGiveToPtr_ != nullptr))
        {
            switch (contentType_)
            {
                case ContentType::MeteorShards:
                {
                    HandleMeteorShardsGive(POPUP_RESPONSE.Selection(), creatureToGiveToPtr_);
                    return false;
                }
                case ContentType::Coins:
                {
                    HandleCoinsGive(POPUP_RESPONSE.Selection(), creatureToGiveToPtr_);
                    return false;
                }
                case ContentType::Gems:
                {
                    HandleGemsGive(POPUP_RESPONSE.Selection(), creatureToGiveToPtr_);
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
        else if ((POPUP_RESPONSE.Info().Name() == POPUP_NAME_CONTENTSELECTION_) && (POPUP_RESPONSE.Response() == sfml_util::Response::Select))
        {
            if (POPUP_RESPONSE.Selection() == PopupInfo::ContentNum_Coins())
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
            else if (POPUP_RESPONSE.Selection() == PopupInfo::ContentNum_Gems())
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
            else if (POPUP_RESPONSE.Selection() == PopupInfo::ContentNum_MeteorShards())
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

        return false;
    }


    void InventoryStage::Setup()
    {
        //paper background
        sfml_util::LoadImageOrTexture<sf::Texture>(paperBgTexture_, GameDataFile::Instance()->GetMediaPath("media-images-backgrounds-paper-2"));
        paperBgTexture_.setSmooth(true);
        paperBgSprite_.setTexture(paperBgTexture_ );
        paperBgSprite_.setScale(SCREEN_WIDTH_ / static_cast<float>(paperBgTexture_.getSize().x), SCREEN_HEIGHT_ / static_cast<float>(paperBgTexture_.getSize().y));
        paperBgSprite_.setPosition(0.0f, 0.0f);

        //ouroboros
        ouroborosUPtr_ = std::make_unique<Ouroboros>("InventoryStage's", true);
        EntityAdd(ouroborosUPtr_.get());

        //instruction text
        const sfml_util::gui::TextInfo INSTR_TEXT_INFO("(use arrows or numbers to change characters, press 'a' to see achievements)",
                                                       sfml_util::FontManager::Instance()->Font_Default1(),
                                                       sfml_util::FontManager::Instance()->Size_Small(),
                                                       sfml_util::FontManager::Color_GrayDark(),
                                                       sf::BlendAlpha,
                                                       sf::Text::Italic,
                                                       sfml_util::Justified::Left);

        insTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "InventoryStage'sInstruction",
            INSTR_TEXT_INFO,
            sf::FloatRect(0.0f, mainMenuTitle_.LowerPosition(false) - 10.0f, 0.0f, 0.0f));
        
            insTextRegionUPtr_->SetEntityPos((SCREEN_WIDTH_ * 0.5f) - (insTextRegionUPtr_->GetEntityRegion().width * 0.5f) + 93.0f, insTextRegionUPtr_->GetEntityPos().y);
        EntityAdd(insTextRegionUPtr_.get());

        //player character image in the top left corner
        SetupCreatureImage();

        //details text
        SetupCreatureDetails(true);
        EntityAdd(detailsTextRegionUPtr_.get());

        //stats text
        SetupCreatureStats();
        EntityAdd(statsTextRegionUPtr_.get());

        //coins, gems, meteor shards, total weight carried
        SetupCenterText();
        EntityAdd(centerTextRegionUPtr_.get());

        //equipped item list box
        SetupListBox();
        EntityAdd(equippedListBoxSPtr_.get());

        //UnEquipped item list box
        SetupDescBox(false);
        EntityAdd(unEquipListBoxSPtr_.get());

        //first listbox title text
        SetupFirstListBoxTitle();
        EntityAdd(eqTitleTextRegionUPtr_.get());

        //descbox title text
        SetupDescBoxTitle();
        EntityAdd(unEqTitleTextRegionUPtr_.get());
        EntityAdd(descTextRegionUPtr_.get());
        EntityAdd(descBoxUPtr_.get());

        //buttons
        sfml_util::gui::TextInfo buttonTextInfo(" ",
                                                sfml_util::FontManager::Instance()->Font_Default2(),
                                                sfml_util::FontManager::Instance()->Size_Largeish(),
                                                sf::Color::Black,
                                                sfml_util::Justified::Left);

        const sf::Color BUTTON_COLOR_DISABLED(sf::Color(0, 0, 0, 180));
        const sf::Color BUTTON_COLOR_OVER(sfml_util::FontManager::Color_GrayDarker());
        const sf::Color BUTTON_COLOR_DOWN(BUTTON_COLOR_DISABLED);
        sfml_util::gui::TextInfo buttonTextInfoDisabled(buttonTextInfo);
        buttonTextInfoDisabled.color = BUTTON_COLOR_DISABLED;

        buttonTextInfo.text = "(B)ack";
        buttonTextInfoDisabled.text = "(B)ack";
        const sfml_util::gui::MouseTextInfo BUTTON_MOUSE_TEXT_BACK(buttonTextInfo, BUTTON_COLOR_DOWN, BUTTON_COLOR_OVER);
        backButtonUPtr_ = std::make_unique<sfml_util::gui::FourStateButton>("InventoryStage'sBack", 0.0f, 0.0f, "", "", "", "", BUTTON_MOUSE_TEXT_BACK, buttonTextInfoDisabled);
        backButtonUPtr_->SetEntityPos((INNER_RECT_.left + (0.085f * INNER_RECT_.width)) - backButtonUPtr_->GetEntityRegion().width, bottomSymbol_.VertPosMiddle() - (backButtonUPtr_->GetEntityRegion().height * 0.5f));
        backButtonUPtr_->SetCallbackHandler(this);
        EntityAdd(backButtonUPtr_.get());
        buttonPVec_.push_back(backButtonUPtr_.get());

        buttonTextInfo.text = "(I)tems";
        buttonTextInfoDisabled.text = "(I)tems";
        const sfml_util::gui::MouseTextInfo BUTTON_MOUSE_TEXT_ITEMS(buttonTextInfo, BUTTON_COLOR_DOWN, BUTTON_COLOR_OVER);
        itemsButtonUPtr_ = std::make_unique<sfml_util::gui::FourStateButton>("InventoryStage'sItems", 0.0f, 0.0f, "", "", "", "", BUTTON_MOUSE_TEXT_ITEMS, buttonTextInfoDisabled);
        itemsButtonUPtr_->SetEntityPos((INNER_RECT_.left + (0.155f * INNER_RECT_.width)) - itemsButtonUPtr_->GetEntityRegion().width, bottomSymbol_.VertPosMiddle() - (itemsButtonUPtr_->GetEntityRegion().height * 0.5f));
        itemsButtonUPtr_->SetCallbackHandler(this);
        EntityAdd(itemsButtonUPtr_.get());
        buttonPVec_.push_back(itemsButtonUPtr_.get());

        buttonTextInfo.text = "(T)itles";
        buttonTextInfoDisabled.text = "(T)itles";
        const sfml_util::gui::MouseTextInfo BUTTON_MOUSE_TEXT_TITLES(buttonTextInfo, BUTTON_COLOR_DOWN, BUTTON_COLOR_OVER);
        titlesButtonUPtr_ = std::make_unique<sfml_util::gui::FourStateButton>("InventoryStage'sTitles", 0.0f, 0.0f, "", "", "", "", BUTTON_MOUSE_TEXT_TITLES, buttonTextInfoDisabled);
        titlesButtonUPtr_->SetEntityPos((INNER_RECT_.left + (0.225f * INNER_RECT_.width)) - titlesButtonUPtr_->GetEntityRegion().width, bottomSymbol_.VertPosMiddle() - (titlesButtonUPtr_->GetEntityRegion().height * 0.5f));
        titlesButtonUPtr_->SetCallbackHandler(this);
        EntityAdd(titlesButtonUPtr_.get());
        buttonPVec_.push_back(titlesButtonUPtr_.get());

        buttonTextInfo.text = "(C)onditions";
        buttonTextInfoDisabled.text = "(C)onditions";
        const sfml_util::gui::MouseTextInfo BUTTON_MOUSE_TEXT_CONDITIONS(buttonTextInfo, BUTTON_COLOR_DOWN, BUTTON_COLOR_OVER);
        condsButtonUPtr_ = std::make_unique<sfml_util::gui::FourStateButton>("InventoryStage'sConditions", 0.0f, 0.0f, "", "", "", "", BUTTON_MOUSE_TEXT_CONDITIONS, buttonTextInfoDisabled);
        condsButtonUPtr_->SetEntityPos((INNER_RECT_.left + (0.34f * INNER_RECT_.width)) - condsButtonUPtr_->GetEntityRegion().width, bottomSymbol_.VertPosMiddle() - (condsButtonUPtr_->GetEntityRegion().height * 0.5f));
        condsButtonUPtr_->SetCallbackHandler(this);
        EntityAdd(condsButtonUPtr_.get());
        buttonPVec_.push_back(condsButtonUPtr_.get());

        buttonTextInfo.text = "(S)pells";
        buttonTextInfoDisabled.text = "(S)pells";
        const sfml_util::gui::MouseTextInfo BUTTON_MOUSE_TEXT_SPELLS(buttonTextInfo, BUTTON_COLOR_DOWN, BUTTON_COLOR_OVER);
        spellsButtonUPtr_ = std::make_unique<sfml_util::gui::FourStateButton>("InventoryStage'sSpells", 0.0f, 0.0f, "", "", "", "", BUTTON_MOUSE_TEXT_SPELLS, buttonTextInfoDisabled);
        spellsButtonUPtr_->SetEntityPos((INNER_RECT_.left + (0.42f * INNER_RECT_.width)) - spellsButtonUPtr_->GetEntityRegion().width, bottomSymbol_.VertPosMiddle() - (spellsButtonUPtr_->GetEntityRegion().height * 0.5f));
        spellsButtonUPtr_->SetCallbackHandler(this);
        EntityAdd(spellsButtonUPtr_.get());
        buttonPVec_.push_back(spellsButtonUPtr_.get());

        buttonTextInfo.text = "(G)ive";
        buttonTextInfoDisabled.text = "(G)ive";
        const sfml_util::gui::MouseTextInfo BUTTON_MOUSE_TEXT_GIVE(buttonTextInfo, BUTTON_COLOR_DOWN, BUTTON_COLOR_OVER);
        giveButtonUPtr_ = std::make_unique<sfml_util::gui::FourStateButton>("InventoryStage'sGive", 0.0f, 0.0f, "", "", "", "", BUTTON_MOUSE_TEXT_GIVE, buttonTextInfoDisabled);
        giveButtonUPtr_->SetEntityPos((INNER_RECT_.left + (0.49f * INNER_RECT_.width)) - giveButtonUPtr_->GetEntityRegion().width, bottomSymbol_.VertPosMiddle() - (giveButtonUPtr_->GetEntityRegion().height * 0.5f));
        giveButtonUPtr_->SetCallbackHandler(this);
        EntityAdd(giveButtonUPtr_.get());
        buttonPVec_.push_back(giveButtonUPtr_.get());

        buttonTextInfo.text = "S(h)are";
        buttonTextInfoDisabled.text = "S(h)are";
        const sfml_util::gui::MouseTextInfo BUTTON_MOUSE_TEXT_SHARE(buttonTextInfo, BUTTON_COLOR_DOWN, BUTTON_COLOR_OVER);
        shareButtonUPtr_ = std::make_unique<sfml_util::gui::FourStateButton>("InventoryStage'sShare", 0.0f, 0.0f, "", "", "", "", BUTTON_MOUSE_TEXT_SHARE, buttonTextInfoDisabled);
        shareButtonUPtr_->SetEntityPos((INNER_RECT_.left + (0.57f * INNER_RECT_.width)) - shareButtonUPtr_->GetEntityRegion().width, bottomSymbol_.VertPosMiddle() - (shareButtonUPtr_->GetEntityRegion().height * 0.5f));
        shareButtonUPtr_->SetCallbackHandler(this);
        EntityAdd(shareButtonUPtr_.get());
        buttonPVec_.push_back(shareButtonUPtr_.get());

        buttonTextInfo.text = "Gathe(r)";
        buttonTextInfoDisabled.text = "Gathe(r)";
        const sfml_util::gui::MouseTextInfo BUTTON_MOUSE_TEXT_GATHER(buttonTextInfo, BUTTON_COLOR_DOWN, BUTTON_COLOR_OVER);
        gatherButtonUPtr_ = std::make_unique<sfml_util::gui::FourStateButton>("InventoryStage'sGather", 0.0f, 0.0f, "", "", "", "", BUTTON_MOUSE_TEXT_GATHER, buttonTextInfoDisabled);
        gatherButtonUPtr_->SetEntityPos((INNER_RECT_.left + (0.667f * INNER_RECT_.width)) - gatherButtonUPtr_->GetEntityRegion().width, bottomSymbol_.VertPosMiddle() - (gatherButtonUPtr_->GetEntityRegion().height * 0.5f));
        gatherButtonUPtr_->SetCallbackHandler(this);
        EntityAdd(gatherButtonUPtr_.get());
        buttonPVec_.push_back(gatherButtonUPtr_.get());

        buttonTextInfo.text = "(E)quip";
        buttonTextInfoDisabled.text = "(E)quip";
        const sfml_util::gui::MouseTextInfo BUTTON_MOUSE_TEXT_EQUIP(buttonTextInfo, BUTTON_COLOR_DOWN, BUTTON_COLOR_OVER);
        equipButtonUPtr_ = std::make_unique<sfml_util::gui::FourStateButton>("InventoryStage'sEquip", 0.0f, 0.0f, "", "", "", "", BUTTON_MOUSE_TEXT_EQUIP, buttonTextInfoDisabled);
        equipButtonUPtr_->SetEntityPos((INNER_RECT_.left + (0.76f * INNER_RECT_.width)) - equipButtonUPtr_->GetEntityRegion().width, bottomSymbol_.VertPosMiddle() - (equipButtonUPtr_->GetEntityRegion().height * 0.5f));
        equipButtonUPtr_->SetCallbackHandler(this);
        EntityAdd(equipButtonUPtr_.get());
        buttonPVec_.push_back(equipButtonUPtr_.get());

        buttonTextInfo.text = "(U)nequip";
        buttonTextInfoDisabled.text = "(U)nequip";
        const sfml_util::gui::MouseTextInfo BUTTON_MOUSE_TEXT_UNEQUIP(buttonTextInfo, BUTTON_COLOR_DOWN, BUTTON_COLOR_OVER);
        unequipButtonUPtr_ = std::make_unique<sfml_util::gui::FourStateButton>("InventoryStage'sUnequip", 0.0f, 0.0f, "", "", "", "", BUTTON_MOUSE_TEXT_UNEQUIP, buttonTextInfoDisabled);
        unequipButtonUPtr_->SetEntityPos((INNER_RECT_.left + (0.865f * INNER_RECT_.width)) - unequipButtonUPtr_->GetEntityRegion().width, bottomSymbol_.VertPosMiddle() - (unequipButtonUPtr_->GetEntityRegion().height * 0.5f));
        unequipButtonUPtr_->SetCallbackHandler(this);
        EntityAdd(unequipButtonUPtr_.get());
        buttonPVec_.push_back(unequipButtonUPtr_.get());

        buttonTextInfo.text = "(D)rop";
        buttonTextInfoDisabled.text = "(D)rop";
        const sfml_util::gui::MouseTextInfo BUTTON_MOUSE_TEXT_DROP(buttonTextInfo, BUTTON_COLOR_DOWN, BUTTON_COLOR_OVER);
        dropButtonUPtr_ = std::make_unique<sfml_util::gui::FourStateButton>("InventoryStage'sDrop", 0.0f, 0.0f, "", "", "", "", BUTTON_MOUSE_TEXT_DROP, buttonTextInfoDisabled);
        dropButtonUPtr_->SetEntityPos((INNER_RECT_.left + (0.94f * INNER_RECT_.width)) - dropButtonUPtr_->GetEntityRegion().width, bottomSymbol_.VertPosMiddle() - (dropButtonUPtr_->GetEntityRegion().height * 0.5f));
        dropButtonUPtr_->SetCallbackHandler(this);
        EntityAdd(dropButtonUPtr_.get());
        buttonPVec_.push_back(dropButtonUPtr_.get());

        SetupButtons();

        //force action on the equipped item listbox to force focus and selection
        sf::Event::KeyEvent keyEvent;
        keyEvent.code = sf::Keyboard::Down;
        equippedListBoxSPtr_->KeyRelease(keyEvent);
        keyEvent.code = sf::Keyboard::Up;
        equippedListBoxSPtr_->KeyRelease(keyEvent);
    }


    void InventoryStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        target.draw(paperBgSprite_, STATES);
        target.draw(mainMenuTitle_, STATES);
        bottomSymbol_.Draw(target, STATES);
        target.draw(creatureSprite_, STATES);
        Stage::Draw(target, STATES);

        //always draw because it is a fast operation and will be fully transparent when should not be drawn
        target.draw(detailViewQuads_, STATES);

        if (detailViewTextUPtr_.get() != nullptr)
        {
            sf::RenderStates statesBlendAdd{ STATES };
            statesBlendAdd.blendMode = sf::BlendAdd;
            target.draw(detailViewSprite_, statesBlendAdd);

            detailViewTextUPtr_->draw(target, STATES);
        }
    }


    bool InventoryStage::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if ((isSliderAnimating_ == false) &&
            (IsDetailViewFadingOrVisible() == false) &&
            (game::LoopManager::Instance()->IsFading() == false))
        {
            sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::Switch).PlayRandom();

            if (KEY_EVENT.code == sf::Keyboard::A)
            {
                return HandleAchievementDisplay();
            }

            if (KEY_EVENT.code == sf::Keyboard::B)
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
                return HandleViewChange(ViewType::Spells);
            }

            if (KEY_EVENT.code == sf::Keyboard::G)
            {
                return HandleGive();
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
                return HandleEquip();
            }

            if (KEY_EVENT.code == sf::Keyboard::U)
            {
                return HandleUnequip();
            }

            if (KEY_EVENT.code == sf::Keyboard::D)
            {
                return HandleDrop();
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
            const float CUT_OFF_TIME_SEC(100.0f);
            if ((sliderAnimTimerSec_ > VIEW_CHANGE_BETWEEN_TIME_SEC_) && (sliderAnimTimerSec_ < CUT_OFF_TIME_SEC))
            {
                sliderAnimTimerSec_ = 0.0f;

                if (ViewType::Count == viewToChangeTo_)
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
                            sliderAnimTimerSec_ = CUT_OFF_TIME_SEC + 1.0f;//anything greater than CUT_OFF_TIME_SEC will work here
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
                            sliderAnimTimerSec_ = CUT_OFF_TIME_SEC + 1.0f;//anything greater than CUT_OFF_TIME_SEC will work here
                        }
                    }
                }
                else
                {
                    if ((false == isListBoxSlidingDone_) && (false == isListBoxSliding_))
                    {
                        isListBoxSliding_ = true;
                    }
                    else if ((false == isDescBoxSlidingDone_) && (false == isDescBoxSliding_))
                    {
                        isDescBoxSliding_ = true;
                        sliderAnimTimerSec_ = CUT_OFF_TIME_SEC + 1.0f;//anything greater than CUT_OFF_TIME_SEC will work here
                    }
                }
            }

            if (isImageSliding_)
            {
                if (false == hasImageChanged_)
                {
                    const float RATIO(imageSlider_.Update(ELAPSED_TIME_SECONDS));

                    if (isSlidingLeft_)
                    {
                        creatureSprite_.setPosition(CREATURE_IMAGE_POS_LEFT_ + (OUT_OF_SIGHT_POS_ * RATIO), creatureSprite_.getPosition().y);
                    }
                    else
                    {
                        creatureSprite_.setPosition(CREATURE_IMAGE_POS_LEFT_ + (SCREEN_WIDTH_ * RATIO), creatureSprite_.getPosition().y);
                    }

                    if (imageSlider_.GetIsDone())
                    {
                        SetupCreatureImage();
                        creatureSprite_.setPosition(OUT_OF_SIGHT_POS_, creatureSprite_.getPosition().y);
                        hasImageChanged_ = true;
                        imageSlider_.Reset(VIEW_CHANGE_SLIDER_SPEED_);
                    }
                }
                else
                {
                    const float RATIO((1.0f - imageSlider_.Update(ELAPSED_TIME_SECONDS)));

                    if (isSlidingLeft_)
                    {
                        creatureSprite_.setPosition(CREATURE_IMAGE_POS_LEFT_ + (SCREEN_WIDTH_ * RATIO), creatureSprite_.getPosition().y);
                    }
                    else
                    {
                        const float NEW_POS_LEFT(CREATURE_IMAGE_POS_LEFT_ + (OUT_OF_SIGHT_POS_ * RATIO));
                        creatureSprite_.setPosition(NEW_POS_LEFT, creatureSprite_.getPosition().y);

                        //sometimes the new creature image is wider than the last, so the details text needs to move with the new image to avoid overlap
                        const float SPRITE_WIDTH(UpdateImageDetailsPosition());
                        const float NEW_DETAILSTEXT_POS_LEFT_EDGE((NEW_POS_LEFT + SPRITE_WIDTH + CREATURE_IMAGE_RIGHT_PAD_));
                        if (NEW_DETAILSTEXT_POS_LEFT_EDGE > detailsTextRegionUPtr_->GetEntityPos().x)
                        {
                            detailsPosLeft_ = NEW_DETAILSTEXT_POS_LEFT_EDGE;
                            detailsTextRegionUPtr_->SetEntityPos(NEW_DETAILSTEXT_POS_LEFT_EDGE, detailsTextRegionUPtr_->GetEntityPos().y);
                        }
                    }

                    if (imageSlider_.GetIsDone())
                    {
                        isImageSliding_ = false;
                        isImageSlidingDone_ = true;
                        hasImageChanged_ = false;
                    }
                }
            }

            if (isDetailsSliding_)
            {
                if (false == hasDetailsChanged_)
                {
                    const float RATIO(detailsSlider_.Update(ELAPSED_TIME_SECONDS));

                    if (isSlidingLeft_)
                    {
                        detailsTextRegionUPtr_->SetEntityPos(detailsPosLeft_ + ((OUT_OF_SIGHT_POS_ - (SCREEN_WIDTH_ * 0.5f)) * RATIO), detailsTextRegionUPtr_->GetEntityPos().y);
                    }
                    else
                    {
                        detailsTextRegionUPtr_->SetEntityPos(detailsPosLeft_ + (SCREEN_WIDTH_ * RATIO), detailsTextRegionUPtr_->GetEntityPos().y);
                    }

                    if (detailsSlider_.GetIsDone())
                    {
                        UpdateImageDetailsPosition();
                        SetupCreatureDetails(true);
                        detailsTextRegionUPtr_->SetEntityPos(SCREEN_WIDTH_ + 1.0f, detailsTextRegionUPtr_->GetEntityPos().y);
                        hasDetailsChanged_ = true;
                        detailsSlider_.Reset(VIEW_CHANGE_SLIDER_SPEED_);
                    }
                }
                else
                {
                    const float RATIO((1.0f - detailsSlider_.Update(ELAPSED_TIME_SECONDS)));

                    if (isSlidingLeft_)
                    {
                        detailsTextRegionUPtr_->SetEntityPos(detailsPosLeft_ + (SCREEN_WIDTH_ * RATIO), detailsTextRegionUPtr_->GetEntityPos().y);
                    }
                    else
                    {
                        detailsTextRegionUPtr_->SetEntityPos(detailsPosLeft_ + ((OUT_OF_SIGHT_POS_ - (SCREEN_WIDTH_ * 0.5f)) * RATIO), detailsTextRegionUPtr_->GetEntityPos().y);
                    }

                    if (detailsSlider_.GetIsDone())
                    {
                        isDetailsSliding_ = false;
                        isDetailsSlidingDone_ = true;
                        hasDetailsChanged_ = false;
                    }
                }
            }

            if (isCenterSliding_)
            {
                if (false == hasCenterChanged_)
                {
                    const float RATIO(centerSlider_.Update(ELAPSED_TIME_SECONDS));

                    if (centerTextRegionUPtr_->GetEntityPos().x < SCREEN_WIDTH_)
                    {
                        if (isSlidingLeft_)
                        {
                            centerTextRegionUPtr_->SetEntityPos(centerPosLeft_ + ((-1.0f * SCREEN_WIDTH_) * RATIO), centerTextRegionUPtr_->GetEntityPos().y);
                        }
                        else
                        {
                            centerTextRegionUPtr_->SetEntityPos(centerPosLeft_ + (SCREEN_WIDTH_ * RATIO), centerTextRegionUPtr_->GetEntityPos().y);
                        }
                    }

                    if (centerSlider_.GetIsDone())
                    {
                        SetupCenterText();
                        centerTextRegionUPtr_->SetEntityPos(SCREEN_WIDTH_ + 1.0f, centerTextRegionUPtr_->GetEntityPos().y);
                        hasCenterChanged_ = true;
                        centerSlider_.Reset(VIEW_CHANGE_SLIDER_SPEED_);
                    }
                }
                else
                {
                    if (creaturePtr_->IsBeast() == false)
                    {
                        const float RATIO((1.0f - centerSlider_.Update(ELAPSED_TIME_SECONDS)));

                        if (isSlidingLeft_)
                        {
                            centerTextRegionUPtr_->SetEntityPos(centerPosLeft_ + (SCREEN_WIDTH_ * RATIO), centerTextRegionUPtr_->GetEntityPos().y);
                        }
                        else
                        {
                            centerTextRegionUPtr_->SetEntityPos(centerPosLeft_ + ((-1.0f * SCREEN_WIDTH_) * RATIO), centerTextRegionUPtr_->GetEntityPos().y);
                        }
                    }

                    if (centerSlider_.GetIsDone() || creaturePtr_->IsBeast())
                    {
                        isCenterSliding_ = false;
                        isCenterSlidingDone_ = true;
                        hasCenterChanged_ = false;
                    }
                }
            }

            if (isStatsSliding_)
            {
                if (false == hasStatsChanged_)
                {
                    const float RATIO(statsSlider_.Update(ELAPSED_TIME_SECONDS));

                    if (isSlidingLeft_)
                    {
                        statsTextRegionUPtr_->SetEntityPos(STATS_POS_LEFT_ + ((-1.0f * SCREEN_WIDTH_) * RATIO), statsTextRegionUPtr_->GetEntityPos().y);
                    }
                    else
                    {
                        statsTextRegionUPtr_->SetEntityPos(STATS_POS_LEFT_ + ((SCREEN_WIDTH_ * 0.5f) * RATIO), statsTextRegionUPtr_->GetEntityPos().y);
                    }

                    if (statsSlider_.GetIsDone())
                    {
                        SetupCreatureStats();
                        statsTextRegionUPtr_->SetEntityPos(OUT_OF_SIGHT_POS_, statsTextRegionUPtr_->GetEntityPos().y);
                        hasStatsChanged_ = true;
                        statsSlider_.Reset(VIEW_CHANGE_SLIDER_SPEED_);
                    }
                }
                else
                {
                    const float RATIO((1.0f - statsSlider_.Update(ELAPSED_TIME_SECONDS)));

                    if (isSlidingLeft_)
                    {
                        statsTextRegionUPtr_->SetEntityPos(STATS_POS_LEFT_ + ((SCREEN_WIDTH_ * 0.5f) * RATIO), statsTextRegionUPtr_->GetEntityPos().y);
                    }
                    else
                    {
                        statsTextRegionUPtr_->SetEntityPos(STATS_POS_LEFT_ + ((-1.0f * SCREEN_WIDTH_) * RATIO), statsTextRegionUPtr_->GetEntityPos().y);
                    }

                    if (statsSlider_.GetIsDone())
                    {
                        isStatsSliding_ = false;
                        isStatsSlidingDone_ = true;
                        hasStatsChanged_ = false;
                    }
                }
            }

            if (isListBoxSliding_)
            {
                if (false == hasListBoxChanged_)
                {
                    const float RATIO(listBoxSlider_.Update(ELAPSED_TIME_SECONDS));

                    if (isSlidingLeft_)
                    {
                        equippedListBoxSPtr_->SetEntityPos(FIRST_LISTBOX_POS_LEFT_ + ((OUT_OF_SIGHT_POS_ * 2.0f) * RATIO), equippedListBoxSPtr_->GetEntityPos().y);
                    }
                    else
                    {
                        equippedListBoxSPtr_->SetEntityPos(FIRST_LISTBOX_POS_LEFT_ + (SCREEN_WIDTH_ * RATIO), equippedListBoxSPtr_->GetEntityPos().y);
                    }

                    if (listBoxSlider_.GetIsDone())
                    {
                        SetupListBox();
                        equippedListBoxSPtr_->SetEntityPos(SCREEN_WIDTH_ + 1.0f, equippedListBoxSPtr_->GetEntityPos().y);
                        hasListBoxChanged_ = true;
                        listBoxSlider_.Reset(VIEW_CHANGE_SLIDER_SPEED_);
                    }
                }
                else
                {
                    const float RATIO((1.0f - listBoxSlider_.Update(ELAPSED_TIME_SECONDS)));

                    if (isSlidingLeft_)
                    {
                        equippedListBoxSPtr_->SetEntityPos(FIRST_LISTBOX_POS_LEFT_ + (SCREEN_WIDTH_ * RATIO), equippedListBoxSPtr_->GetEntityPos().y);
                    }
                    else
                    {
                        equippedListBoxSPtr_->SetEntityPos(FIRST_LISTBOX_POS_LEFT_ + ((OUT_OF_SIGHT_POS_ * 2.0f) * RATIO), equippedListBoxSPtr_->GetEntityPos().y);
                    }

                    if (listBoxSlider_.GetIsDone())
                    {
                        isListBoxSliding_ = false;
                        isListBoxSlidingDone_ = true;
                        hasListBoxChanged_ = false;
                    }
                }
            }

            if (isDescBoxSliding_)
            {
                if (false == hasDescBoxChanged_)
                {
                    const float RATIO(descBoxSlider_.Update(ELAPSED_TIME_SECONDS));
                    const float POS_LEFT_SLIDING_LEFT(SECOND_LISTBOX_POS_LEFT_ + (((OUT_OF_SIGHT_POS_ * 2.0f) - SCREEN_WIDTH_) * RATIO));
                    const float POS_LEFT_SLIDING_RIGHT(SECOND_LISTBOX_POS_LEFT_ + ((SCREEN_WIDTH_ * 0.75f) * RATIO));

                    if (ViewType::Items == view_)
                    {
                        if (isSlidingLeft_)
                        {
                            unEquipListBoxSPtr_->SetEntityPos(POS_LEFT_SLIDING_LEFT, unEquipListBoxSPtr_->GetEntityPos().y);
                        }
                        else
                        {
                            unEquipListBoxSPtr_->SetEntityPos(POS_LEFT_SLIDING_RIGHT, unEquipListBoxSPtr_->GetEntityPos().y);
                        }
                    }
                    else
                    {
                        if (isSlidingLeft_)
                        {
                            descTextRegionUPtr_->SetEntityPos(POS_LEFT_SLIDING_LEFT, descTextRegionUPtr_->GetEntityPos().y);
                            descBoxUPtr_->SetEntityPos(POS_LEFT_SLIDING_LEFT, descBoxUPtr_->GetEntityPos().y);
                        }
                        else
                        {
                            descTextRegionUPtr_->SetEntityPos(POS_LEFT_SLIDING_RIGHT, descTextRegionUPtr_->GetEntityPos().y);
                            descBoxUPtr_->SetEntityPos(POS_LEFT_SLIDING_RIGHT, descBoxUPtr_->GetEntityPos().y);
                        }
                    }

                    if (descBoxSlider_.GetIsDone())
                    {
                        SetupDescBox(true);
                        unEquipListBoxSPtr_->SetEntityPos(SCREEN_WIDTH_ + 1.0f, unEquipListBoxSPtr_->GetEntityPos().y);
                        descTextRegionUPtr_->SetEntityPos(SCREEN_WIDTH_ + 1.0f, descTextRegionUPtr_->GetEntityPos().y);
                        descBoxUPtr_->SetEntityPos(SCREEN_WIDTH_ + 1.0f, descBoxUPtr_->GetEntityPos().y);
                        hasDescBoxChanged_ = true;
                        descBoxSlider_.Reset(VIEW_CHANGE_SLIDER_SPEED_);
                    }
                }
                else
                {
                    const float RATIO((1.0f - descBoxSlider_.Update(ELAPSED_TIME_SECONDS)));
                    const float POS_LEFT_SLIDING_LEFT(SECOND_LISTBOX_POS_LEFT_ + ((SCREEN_WIDTH_ * 0.75f) * RATIO));
                    const float POS_LEFT_SLIDING_RIGHT(SECOND_LISTBOX_POS_LEFT_ + (((OUT_OF_SIGHT_POS_ * 2.0f) - SCREEN_WIDTH_) * RATIO));

                    if ((ViewType::Items == viewToChangeTo_) || isViewForcedToItems_ || ((ViewType::Count == viewToChangeTo_) && (ViewType::Items == view_)))
                    {
                        if (isSlidingLeft_)
                        {
                            unEquipListBoxSPtr_->SetEntityPos(POS_LEFT_SLIDING_LEFT, unEquipListBoxSPtr_->GetEntityPos().y);
                        }
                        else
                        {
                            unEquipListBoxSPtr_->SetEntityPos(POS_LEFT_SLIDING_RIGHT, unEquipListBoxSPtr_->GetEntityPos().y);
                        }
                    }
                    else
                    {
                        if (isSlidingLeft_)
                        {
                            descTextRegionUPtr_->SetEntityPos(POS_LEFT_SLIDING_LEFT, descTextRegionUPtr_->GetEntityPos().y);
                            descBoxUPtr_->SetEntityPos(POS_LEFT_SLIDING_LEFT, descBoxUPtr_->GetEntityPos().y);
                        }
                        else
                        {
                            descTextRegionUPtr_->SetEntityPos(POS_LEFT_SLIDING_RIGHT, descTextRegionUPtr_->GetEntityPos().y);
                            descBoxUPtr_->SetEntityPos(POS_LEFT_SLIDING_RIGHT, descBoxUPtr_->GetEntityPos().y);
                        }
                    }

                    if (descBoxSlider_.GetIsDone())
                    {
                        isDescBoxSliding_ = false;
                        isDescBoxSlidingDone_ = true;
                        hasDescBoxChanged_ = false;
                    }
                }
            }

            if (ViewType::Count == viewToChangeTo_)
            {
                if (isImageSlidingDone_ &&
                    isDetailsSlidingDone_ &&
                    isCenterSlidingDone_ &&
                    isStatsSlidingDone_ &&
                    isListBoxSlidingDone_ &&
                    isDescBoxSlidingDone_)
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
            if (hasMouseMoved_ &&
                (IsDetailViewFadingOrVisible() == false) &&
                (game::LoopManager::Instance()->IsFading() == false) &&
                (GetItemMouseIsOver(mousePosV_) != nullptr) &&
                (false == isWaitingOnPopup_))
            {
                detailViewTimerSec_ += ELAPSED_TIME_SECONDS;

                if (detailViewTimerSec_ >= DETAILVIEW_TIMER_DURATION_SEC_)
                {
                    sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::TickOn).PlayRandom();
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

            if (isDetailViewFadingIn_ || isDetailViewFadingOut_)
            {
                if (isDetailViewFadingIn_)
                {
                    detailViewSliderRatio_ = detailViewSlider_.Update(ELAPSED_TIME_SECONDS);
                    if (detailViewSlider_.GetIsDone())
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
                            const item::ItemPtr_t IITEM_PTR(GetItemMouseIsOver(mousePosV_));
                            if (IITEM_PTR == nullptr)
                            {
                                isDetailViewFadingIn_ = false;
                                isDetailViewDoneFading_ = false;
                                isDetailViewFadingOut_ = true;
                            }
                            else
                            {
                                isDetailViewFadingIn_ = false;
                                isDetailViewDoneFading_ = true;
                                isDetailViewFadingOut_ = false;
                                SetupDetailViewItem(IITEM_PTR);
                            }
                        }
                    }
                }
                else if (isDetailViewFadingOut_)
                {
                    detailViewSliderRatio_ = (1.0f - detailViewSlider_.Update(ELAPSED_TIME_SECONDS));
                    if (detailViewSlider_.GetIsDone())
                    {
                        isDetailViewFadingIn_ = false;
                        isDetailViewDoneFading_ = false;
                        isDetailViewFadingOut_ = false;
                    }
                }

                const sf::Color NEW_COLOR(0, 0, 0, static_cast<sf::Uint8>(DETAILVIEW_COLOR_ALPHA_START_ + ((DETAILVIEW_COLOR_ALPHA_END_ - DETAILVIEW_COLOR_ALPHA_START_) * detailViewSliderRatio_)));
                detailViewQuads_[0].color = NEW_COLOR;
                detailViewQuads_[1].color = NEW_COLOR;
                detailViewQuads_[2].color = NEW_COLOR;
                detailViewQuads_[3].color = NEW_COLOR;

                if ((detailViewSlider_.GetIsDone()) &&
                    (false == isDetailViewFadingIn_) &&
                    (false == isDetailViewDoneFading_) &&
                    (false == isDetailViewFadingOut_))
                {
                    detailViewQuads_[0].color = sf::Color::Transparent;
                    detailViewQuads_[1].color = sf::Color::Transparent;
                    detailViewQuads_[2].color = sf::Color::Transparent;
                    detailViewQuads_[3].color = sf::Color::Transparent;
                }

                const float SOURCE_RECT_LEFT(detailViewSourceRect_.left);
                const float SOURCE_RECT_RIGHT(detailViewSourceRect_.left + detailViewSourceRect_.width);
                const float SOURCE_RECT_TOP(detailViewSourceRect_.top);
                const float SOURCE_RECT_BOTTOM(detailViewSourceRect_.top + detailViewSourceRect_.height);

                const float POS_LEFT_MOUSEX_MORE(SOURCE_RECT_LEFT - ((SOURCE_RECT_LEFT - DETAILVIEW_POS_LEFT_) * detailViewSliderRatio_));
                const float POS_LEFT_MOUSEX_LESS(SOURCE_RECT_LEFT + ((DETAILVIEW_POS_LEFT_ - SOURCE_RECT_LEFT) * detailViewSliderRatio_));
                const float POS_LEFT((SOURCE_RECT_LEFT > DETAILVIEW_POS_LEFT_) ? POS_LEFT_MOUSEX_MORE : POS_LEFT_MOUSEX_LESS);

                const float POS_RIGHT_MOUSEX_MORE(SOURCE_RECT_RIGHT - ((SOURCE_RECT_RIGHT - (DETAILVIEW_POS_LEFT_ + DETAILVIEW_WIDTH_)) * detailViewSliderRatio_));
                const float POS_RIGHT_MOUSEX_LESS(SOURCE_RECT_RIGHT + (((DETAILVIEW_POS_LEFT_ + DETAILVIEW_WIDTH_) - SOURCE_RECT_RIGHT) * detailViewSliderRatio_));
                const float POS_RIGHT((SOURCE_RECT_RIGHT > (DETAILVIEW_POS_LEFT_ + DETAILVIEW_WIDTH_)) ? POS_RIGHT_MOUSEX_MORE : POS_RIGHT_MOUSEX_LESS);

                const float POS_TOP(SOURCE_RECT_TOP - ((SOURCE_RECT_TOP - DETAILVIEW_POS_TOP_) * detailViewSliderRatio_));

                const float POS_BOTTOM_MOUSEY_MORE(SOURCE_RECT_BOTTOM - ((SOURCE_RECT_BOTTOM - (DETAILVIEW_POS_TOP_ + DETAILVIEW_HEIGHT_)) * detailViewSliderRatio_));
                const float POS_BOTTOM_MOUSEY_LESS(SOURCE_RECT_BOTTOM + (((DETAILVIEW_POS_TOP_ + DETAILVIEW_HEIGHT_) - SOURCE_RECT_BOTTOM) * detailViewSliderRatio_));
                const float POS_BOTTOM((SOURCE_RECT_BOTTOM > (DETAILVIEW_POS_TOP_ + DETAILVIEW_HEIGHT_)) ? POS_BOTTOM_MOUSEY_MORE : POS_BOTTOM_MOUSEY_LESS);

                detailViewQuads_[0].position = sf::Vector2f(POS_LEFT, POS_TOP);
                detailViewQuads_[1].position = sf::Vector2f(POS_RIGHT, POS_TOP);
                detailViewQuads_[2].position = sf::Vector2f(POS_RIGHT, POS_BOTTOM);
                detailViewQuads_[3].position = sf::Vector2f(POS_LEFT, POS_BOTTOM);
            }
        }

        Stage::UpdateTime(ELAPSED_TIME_SECONDS);
    }


    void InventoryStage::UpdateMousePos(const sf::Vector2f & MOUSE_POS_V)
    {
        hasMouseMoved_ = true;
        HandleDetailViewMouseInterrupt(MOUSE_POS_V);
        Stage::UpdateMousePos(MOUSE_POS_V);
    }


    void InventoryStage::UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V)
    {
        HandleDetailViewMouseInterrupt(MOUSE_POS_V);
        Stage::UpdateMouseDown(MOUSE_POS_V);

        for (auto const NEXT_BUTTON_PTR : buttonPVec_)
        {
            if ((NEXT_BUTTON_PTR->IsDisabled()) &&
                (NEXT_BUTTON_PTR->GetEntityRegion().contains(MOUSE_POS_V)))
            {
                PopupRejectionWindow(NEXT_BUTTON_PTR->GetMouseHoverText(), false);
                break;
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

        characterViewMap_[Game::Instance()->State().Party().GetOrderNum(creaturePtr_)] = view_;

        if (ViewType::Items != view_)
            SetDescBoxTextFromListBoxItem(equippedListBoxSPtr_->GetSelected());

        SetupFirstListBoxTitle();
        SetupDescBoxTitle();
        SetupButtons();
    }


    void InventoryStage::SetupButtons()
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

        if (creaturePtr_->Spells().empty())
        {
            spellsButtonUPtr_->SetIsDisabled(true);
            spellsButtonUPtr_->SetMouseHoverText(creaturePtr_->Name() + " does not know any spells.");
        }
        else if (ViewType::Spells == view_)
        {
            spellsButtonUPtr_->SetIsDisabled(true);
            spellsButtonUPtr_->SetMouseHoverText("Already viewing spells.");
        }
        else
        {
            spellsButtonUPtr_->SetIsDisabled(false);
            spellsButtonUPtr_->SetMouseHoverText("Click here or press 's' to view and cast spells.");
        }

        if (creaturePtr_->Titles().empty())
        {
            titlesButtonUPtr_->SetIsDisabled(true);
            titlesButtonUPtr_->SetMouseHoverText(creaturePtr_->Name() + " does not have any titles.");
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

            const std::string DENY_MSG("Beasts cannot carry items, coins, gems, or Meteor Shards.");

            giveButtonUPtr_->SetMouseHoverText(DENY_MSG);
            shareButtonUPtr_->SetMouseHoverText(DENY_MSG);
            gatherButtonUPtr_->SetMouseHoverText(DENY_MSG);
        }
        else
        {
            giveButtonUPtr_->SetIsDisabled(false);
            shareButtonUPtr_->SetIsDisabled(false);
            gatherButtonUPtr_->SetIsDisabled(false);

            giveButtonUPtr_->SetMouseHoverText("Click here or press 'g' to give something to another character.");
            shareButtonUPtr_->SetMouseHoverText("Click here or press 'h' to share something between all characters.");
            gatherButtonUPtr_->SetMouseHoverText("Click here or press 'r' to gather all of something from all other characters.");
        }

        if (ViewType::Items == view_)
        {
            auto selectedItemPtr{ unEquipListBoxSPtr_->GetSelected() };

            if ((selectedItemPtr != nullptr) &&
                (selectedItemPtr->ITEM_CPTR != nullptr) &&
                (selectedItemPtr->ITEM_CPTR->Name().empty() == false))
            {
                equipButtonUPtr_->SetIsDisabled(false);
                equipButtonUPtr_->SetMouseHoverText("Click here or press 'e' to equip the " + unEquipListBoxSPtr_->GetSelected()->ITEM_CPTR->Name() + ".");
                dropButtonUPtr_->SetIsDisabled(false);
                dropButtonUPtr_->SetMouseHoverText("(Click here or press 'd' to drop the " + unEquipListBoxSPtr_->GetSelected()->ITEM_CPTR->Name() + ")");
            }
            else
            {
                equipButtonUPtr_->SetIsDisabled(true);
                equipButtonUPtr_->SetMouseHoverText("There is no unequipped item selected to equip.");
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
            equipButtonUPtr_->SetMouseHoverText("Click 'Item' or press 'i' to switch to the items view if you want to equip an item.");
            dropButtonUPtr_->SetIsDisabled(true);
            dropButtonUPtr_->SetMouseHoverText("Click 'Item' or press 'i' to switch to the items view if you want to drop an item.");
        }

        if (ViewType::Items == view_)
        {
            if ((equippedListBoxSPtr_->GetSelected().get() != nullptr) &&
                (equippedListBoxSPtr_->GetSelected()->ITEM_CPTR != nullptr) &&
                (equippedListBoxSPtr_->GetSelected()->ITEM_CPTR->Name().empty() == false))
            {
                unequipButtonUPtr_->SetIsDisabled(false);
                unequipButtonUPtr_->SetMouseHoverText("Click here or press 'u' to unequip the " + equippedListBoxSPtr_->GetSelected()->ITEM_CPTR->Name() + ".");
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
            unequipButtonUPtr_->SetMouseHoverText("Click 'Item' or press 'i' to switch to the items view if you want to unequip an item.");
        }
    }


    bool InventoryStage::HandleAchievementDisplay()
    {
        isAchievementDisplaying_ = true;
        const float HALF_SIZE(10.0f);
        detailViewSourceRect_ = sf::FloatRect((SCREEN_WIDTH_ * 0.5f) - HALF_SIZE, (SCREEN_HEIGHT_ * 0.5f) - HALF_SIZE, (HALF_SIZE * 2.0f), (HALF_SIZE * 2.0f));
        isDetailViewFadingIn_ = true;
        detailViewTimerSec_ = 0.0f;
        detailViewSlider_.Reset(DETAILVIEW_SLIDER_SPEED_);
        return true;
    }


    bool InventoryStage::HandleViewChange(const ViewType NEW_VIEW)
    {
        if ((NEW_VIEW == ViewType::Items) && ((ViewType::Items == view_) || itemsButtonUPtr_->IsDisabled()))
            return false;

        if ((NEW_VIEW == ViewType::Titles) && ((ViewType::Titles == view_) || titlesButtonUPtr_->IsDisabled()))
            return false;

        if ((NEW_VIEW == ViewType::Conditions) && ((ViewType::Conditions == view_) || condsButtonUPtr_->IsDisabled()))
            return false;

        if ((NEW_VIEW == ViewType::Spells) && ((ViewType::Spells == view_) || spellsButtonUPtr_->IsDisabled()))
            return false;

        viewToChangeTo_ = NEW_VIEW;
        StartSlidingAnimation(true);
        return true;
    }


    bool InventoryStage::HandleBack()
    {
        game::LoopManager::Instance()->Goto_Previous();
        return true;
    }


    bool InventoryStage::HandleEquip()
    {
        if ((ViewType::Items == view_) && (equipButtonUPtr_->IsDisabled() == false))
        {
            const sfml_util::gui::ListBoxItemSPtr_t LISTBOX_ITEM_SPTR(unEquipListBoxSPtr_->GetSelected());
            if ((LISTBOX_ITEM_SPTR.get() != nullptr) && (LISTBOX_ITEM_SPTR->ITEM_CPTR != nullptr))
            {
                const item::ItemPtr_t IITEM_PTR(LISTBOX_ITEM_SPTR->ITEM_CPTR);
                const std::string EQUIP_RESULT(creaturePtr_->ItemEquip(IITEM_PTR));
                if (EQUIP_RESULT.empty())
                {
                    unEquipListBoxSPtr_->Remove(LISTBOX_ITEM_SPTR);
                    equippedListBoxSPtr_->Add(LISTBOX_ITEM_SPTR);
                    SetupButtons();
                    return true;
                }
                else
                {
                    std::ostringstream ss;
                    ss << "Cannot equip the " << IITEM_PTR->Name() << " because: " << EQUIP_RESULT;

                    const PopupInfo POPUP_INFO(sfml_util::gui::PopupManager::Instance()->CreatePopupInfo("InventoryStage'sEquipItemFailedPopup",
                                                                                                         ss.str(),
                                                                                                         sfml_util::PopupButtons::Okay,
                                                                                                         sfml_util::PopupImage::Regular));
                    LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
                    isWaitingOnPopup_ = true;
                }
            }
        }

        return false;
    }


    bool InventoryStage::HandleUnequip()
    {
        if ((ViewType::Items == view_) && (unequipButtonUPtr_->IsDisabled() == false))
        {
            const sfml_util::gui::ListBoxItemSPtr_t LISTBOX_ITEM_SPTR(equippedListBoxSPtr_->GetSelected());
            if ((LISTBOX_ITEM_SPTR.get() != nullptr) && (LISTBOX_ITEM_SPTR->ITEM_CPTR != nullptr))
            {
                if (LISTBOX_ITEM_SPTR->ITEM_CPTR->IsBodypart())
                {
                    PopupDoneWindow("Bodypart items cannot be unequipped.", true);
                    return false;
                }
                else
                {
                    const item::ItemPtr_t IITEM_PTR(LISTBOX_ITEM_SPTR->ITEM_CPTR);
                    creaturePtr_->ItemUnEquip(IITEM_PTR);
                    equippedListBoxSPtr_->Remove(LISTBOX_ITEM_SPTR);
                    unEquipListBoxSPtr_->Add(LISTBOX_ITEM_SPTR);
                    SetupButtons();
                    return true;
                }
            }
        }

        return false;
    }


    bool InventoryStage::HandleGive()
    {
        actionType_ = ActionType::Give;
        const PopupInfo POPUP_INFO(sfml_util::gui::PopupManager::Instance()->CreatePopupInfo(POPUP_NAME_GIVE_,
                                                                                             "\nWhat do you want to give?\n\n(I)tem\n(C)oins\n(G)ems\n(M)eteor Shards\n\n...or (Escape) to Cancel",
                                                                                             sfml_util::PopupButtons::Cancel,
                                                                                             sfml_util::PopupImage::Large,
                                                                                             sfml_util::Justified::Center,
                                                                                             sfml_util::sound_effect::PromptGeneric,
                                                                                             Popup::ContentSelectionWithItem,
                                                                                             sfml_util::FontManager::Instance()->Size_Largeish()));
        LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
        isWaitingOnPopup_ = true;
        return true;
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


    bool InventoryStage::HandleDrop()
    {
        if ((ViewType::Items == view_) && (equipButtonUPtr_->IsDisabled() == false))
        {
            const sfml_util::gui::ListBoxItemSPtr_t LISTBOX_ITEM_SPTR(unEquipListBoxSPtr_->GetSelected());
            if ((LISTBOX_ITEM_SPTR.get() != nullptr) && (LISTBOX_ITEM_SPTR->ITEM_CPTR != nullptr))
            {
                const item::ItemPtr_t IITEM_PTR(LISTBOX_ITEM_SPTR->ITEM_CPTR);

                iItemToDropPtr_ = IITEM_PTR;

                actionType_ = ActionType::Drop;

                const PopupInfo POPUP_INFO(sfml_util::gui::PopupManager::Instance()->CreatePopupInfo(POPUP_NAME_DROPCONFIRM_,
                                                                                                     "\nAre you sure you want to drop the " + IITEM_PTR->Name() + "?",
                                                                                                     sfml_util::PopupButtons::YesNo,
                                                                                                     sfml_util::PopupImage::Regular,
                                                                                                     sfml_util::Justified::Center,
                                                                                                     sfml_util::sound_effect::PromptQuestion));
                LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
                isWaitingOnPopup_ = true;
                return true;
            }
        }

        return false;
    }


    bool InventoryStage::HandlePlayerChangeBeforeOrAfter(const bool IS_NEXT_CREATURE_AFTER)
    {
        if (IS_NEXT_CREATURE_AFTER)
        {
            return HandlePlayerChangeTo(Game::Instance()->State().Party().
                GetNextInOrderAfter(creaturePtr_), true);
        }
        else
        {
            return HandlePlayerChangeTo(Game::Instance()->State().Party().
                GetNextInOrderBefore(creaturePtr_), false);
        }
    }


    bool InventoryStage::HandlePlayerChangeIndex(const std::size_t CHARACTER_NUM)
    {
        const std::size_t CURR_INDEX(Game::Instance()->State().Party().
            GetOrderNum(creaturePtr_));

        if (CURR_INDEX == CHARACTER_NUM)
        {
            return false;
        }
        else
        {
            return HandlePlayerChangeTo(Game::Instance()->State().Party().
                GetAtOrderPos(CHARACTER_NUM), (CURR_INDEX < CHARACTER_NUM));
        }
    }


    bool InventoryStage::HandlePlayerChangeTo(const creature::CreaturePtrC_t CREATURE_CPTRC, const bool IS_SLIDING_LEFT)
    {
        if (isSliderAnimating_)
        {
            return false;
        }
        else
        {
            creaturePtr_ = CREATURE_CPTRC;
            StartSlidingAnimation(IS_SLIDING_LEFT);

            if (((ViewType::Titles == view_) && (CREATURE_CPTRC->Titles().empty())) ||
                ((ViewType::Spells == view_) && (CREATURE_CPTRC->Spells().empty())) ||
                ((ViewType::Conditions == view_) && (CREATURE_CPTRC->HasCondition(creature::Conditions::Good))))
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


    void InventoryStage::SetupCreatureImage()
    {
        sfml_util::gui::CreatureImageManager::Instance()->GetImage(creatureTexture_, creaturePtr_->ImageFilename(), true);
        sfml_util::Invert(creatureTexture_);
        sfml_util::Mask(creatureTexture_, sf::Color::White);
        creatureTexture_.setSmooth(true);
        creatureSprite_.setTexture(creatureTexture_);
        creatureSprite_.setTextureRect( sf::IntRect(0, 0, static_cast<int>(creatureTexture_.getSize().x), static_cast<int>(creatureTexture_.getSize().y)) );
        creatureSprite_.setPosition(CREATURE_IMAGE_POS_LEFT_, CREATURE_IMAGE_POS_TOP_);
        creatureSprite_.setColor(sf::Color(255, 255, 255, 127));
        creatureSprite_.setScale(CREATURE_IMAGE_SCALE_, CREATURE_IMAGE_SCALE_);
    }


    void InventoryStage::SetupCreatureDetails(const bool WILL_UPDATE_POSITION)
    {
        std::ostringstream ss;
        ss << "Character # " << game::Game::Instance()->State().Party().GetOrderNum(creaturePtr_) + 1 << "\n"
            << creaturePtr_->Name() << "\n"
            << creaturePtr_->SexName() << "\n"
            << creaturePtr_->Race().Name();

        if (creaturePtr_->IsBeast())
        {
            if (creaturePtr_->Race().Which() != creature::race::Wolfen)
                ss << ", " << creaturePtr_->Role().Name();

            ss << " " << creaturePtr_->RankClassName() << "\n";
        }
        else
        {
            ss << " " << creaturePtr_->RankClassName() << " " << creaturePtr_->Role().Name() << "\n";
        }

        ss << "Rank:  " << creaturePtr_->Rank() << "\n"
           << "Experience: " << creaturePtr_->Exp() << "\n"
           << "Health:  " << creaturePtr_->HealthCurrent() << "/" << creaturePtr_->HealthNormal() << " " << creaturePtr_->HealthPercentStr() << "\n"
           << "Condition:  " << creaturePtr_->ConditionNames(4) << "\n"
           << "\n";

        const sfml_util::gui::TextInfo DETAILS_TEXT_INFO(ss.str(),
                                                         sfml_util::FontManager::Instance()->Font_Default1(),
                                                         sfml_util::FontManager::Instance()->Size_Smallish(),
                                                         sfml_util::FontManager::Color_GrayDarker(),
                                                         sfml_util::Justified::Left);

        sf::FloatRect detailsTextRect(CREATURE_IMAGE_POS_LEFT_ + creatureSprite_.getGlobalBounds().width + CREATURE_IMAGE_RIGHT_PAD_,
                                      creatureSprite_.getPosition().y + CREATURE_IMAGE_RIGHT_PAD_,
                                      0.0f,
                                      0.0f);

        if ((WILL_UPDATE_POSITION == false) && (detailsTextRegionUPtr_.get() != nullptr))
        {
            detailsTextRect.left = detailsTextRegionUPtr_->GetEntityPos().x;
        }

        detailsPosLeft_ = detailsTextRect.left;

        if (detailsTextRegionUPtr_.get() == nullptr)
        {
            detailsTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "InventoryStage'sDetails", DETAILS_TEXT_INFO, detailsTextRect);
        }
        else
        {
            detailsTextRegionUPtr_->SetText(ss.str());
        }

        detailsTextRegionUPtr_->SetEntityPos(detailsPosLeft_, detailsTextRegionUPtr_->GetEntityPos().y);
    }


    void InventoryStage::SetupCreatureStats()
    {
        const stats::StatSet STATS(creaturePtr_->Stats());

        const std::string STR_MOD_STR(STATS.Str().ModifiedCurrentStr(true));
        const std::string ACC_MOD_STR(STATS.Acc().ModifiedCurrentStr(true));
        const std::string CHA_MOD_STR(STATS.Cha().ModifiedCurrentStr(true));
        const std::string LCK_MOD_STR(STATS.Lck().ModifiedCurrentStr(true));
        const std::string SPD_MOD_STR(STATS.Spd().ModifiedCurrentStr(true));
        const std::string INT_MOD_STR(STATS.Int().ModifiedCurrentStr(true));

        std::ostringstream ss;
        ss << "Strength:       "   << STATS.Str().Current() << " " << STR_MOD_STR << "\n"
           << "Accuracy:      "    << STATS.Acc().Current() << " " << ACC_MOD_STR << "\n"
           << "Charm:          "   << STATS.Cha().Current() << " " << CHA_MOD_STR << "\n"
           << "Luck:             " << STATS.Lck().Current() << " " << LCK_MOD_STR << "\n"
           << "Speed:            " << STATS.Spd().Current() << " " << SPD_MOD_STR << "\n"
           << "Intelligence:   "   << STATS.Int().Current() << " " << INT_MOD_STR << "\n"
           << "\n \n ";

        const sfml_util::gui::TextInfo STATS_TEXT_INFO(ss.str(),
                                                       sfml_util::FontManager::Instance()->Font_Typical(),
                                                       sfml_util::FontManager::Instance()->Size_Normal(),
                                                       sfml_util::FontManager::Color_GrayDarker(),
                                                       sfml_util::Justified::Left);

        const sf::FloatRect STATS_TEXT_RECT(STATS_POS_LEFT_,
                                            mainMenuTitle_.LowerPosition(false) + 20.0f,
                                            0.0f,
                                            0.0f);

        if (statsTextRegionUPtr_.get() == nullptr)
        {
            statsTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "InventoryStage'sStats", STATS_TEXT_INFO, STATS_TEXT_RECT);
        }
        else
        {
            statsTextRegionUPtr_->SetText(ss.str());
        }
    }

    void InventoryStage::SetupCenterText()
    {
        std::ostringstream ss;
        auto const & INVENTORY{ creaturePtr_->Inventory() };
        ss << "Coins:  " << INVENTORY.Coins() << "\n"
           << "Gems:  " << INVENTORY.Gems() << "\n"
           << "Meteor Shards:  " << INVENTORY.MeteorShards() << "\n"
           << "Mana:  " << creaturePtr_->ManaCurrent() << "/" << creaturePtr_->ManaNormal() << "\n"
           << "Weight: " << INVENTORY.Weight() << "/" << creaturePtr_->WeightCanCarry() << "\n"
           << "\n \n ";

        const sfml_util::gui::TextInfo CENTER_TEXT_INFO(ss.str(),
                                                        sfml_util::FontManager::Instance()->Font_Typical(),
                                                        sfml_util::FontManager::Instance()->Size_Normal(),
                                                        sfml_util::FontManager::Color_GrayDarker(),
                                                        sfml_util::Justified::Left);

        const sf::FloatRect CENTER_TEXT_RECT(0.0f,
                                             mainMenuTitle_.LowerPosition(true) + sfml_util::MapByRes(5.0f, 30.0f),
                                             0.0f,
                                             0.0f);

        const bool WAS_ALREADY_INSTANTIATED(centerTextRegionUPtr_.get() != nullptr);

        if (WAS_ALREADY_INSTANTIATED == false)
        {
            centerTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "InventoryStage'sCenter", CENTER_TEXT_INFO, CENTER_TEXT_RECT);
        }
        else
        {
            centerTextRegionUPtr_->SetText(ss.str());
        }

        const float POS_LEFT(((WAS_ALREADY_INSTANTIATED == false) && (creaturePtr_->IsBeast())) ? (SCREEN_WIDTH_ + 1.0f) : (SCREEN_WIDTH_ * 0.5f) - sfml_util::MapByRes(100.0f, 300.0f));
        centerTextRegionUPtr_->SetEntityPos(POS_LEFT, centerTextRegionUPtr_->GetEntityPos().y);
        centerPosLeft_ = POS_LEFT;
    }


    void InventoryStage::SetupListBox()
    {
        const sfml_util::gui::box::Info LISTBOX_BOX_INFO(1,
                                                         true,
                                                         LISTBOX_REGION_,
                                                         LISTBOX_COLORSET_,
                                                         LISTBOX_BG_INFO_);

        sfml_util::gui::ListBoxItemSLst_t listBoxItemsSList;

        //establish which view to use
        ViewType viewToUse(viewToChangeTo_);

        if (ViewType::Count == viewToUse)
            viewToUse = view_;

        if (isViewForcedToItems_)
            viewToUse = ViewType::Items;

        switch(viewToUse)
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

                    listBoxItemsSList.push_back(LISTBOXITEM_SPTR);
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

                    listBoxItemsSList.push_back(LISTBOXITEM_SPTR);
                }
                break;
            }

            case ViewType::Spells:
            {
                for (auto const NEXT_SPELL_PTR : creaturePtr_->SpellsPVec())
                {
                    listBoxItemTextInfo_.text = NEXT_SPELL_PTR->Name();

                    auto const LISTBOXITEM_SPTR = std::make_shared<sfml_util::gui::ListBoxItem>(
                        NEXT_SPELL_PTR->Name() + "_SpellsListBoxEntry",
                        listBoxItemTextInfo_,
                        NEXT_SPELL_PTR);

                    listBoxItemsSList.push_back(LISTBOXITEM_SPTR);
                }
                break;
            }

            case ViewType::Titles:
            case ViewType::Count:
            default:
            {
                for (auto const NEXT_TITLE_PTR : creaturePtr_->TitlesPVec())
                {
                    listBoxItemTextInfo_.text = NEXT_TITLE_PTR->Name();

                    auto const LISTBOXITEM_SPTR = std::make_shared<sfml_util::gui::ListBoxItem>(
                        NEXT_TITLE_PTR->Name() + "_TitlesListBoxEntry",
                        listBoxItemTextInfo_,
                        NEXT_TITLE_PTR);

                    listBoxItemsSList.push_back(LISTBOXITEM_SPTR);
                }
                break;
            }
        }

        const bool IS_ALREADY_INSTANTIATED(equippedListBoxSPtr_.get() != nullptr);

        if (IS_ALREADY_INSTANTIATED)
        {
            EntityRemove(equippedListBoxSPtr_.get());
        }

        equippedListBoxSPtr_ = std::make_shared<sfml_util::gui::ListBox>(
            "InventoryStage'sLeftListBox",
            LISTBOX_REGION_,
            listBoxItemsSList,
            this,
            10.0f,
            6.0f,
            LISTBOX_BOX_INFO,
            LISTBOX_COLOR_LINE_,
            sfml_util::gui::ListBox::NO_LIMIT_,
            this);

        if (IS_ALREADY_INSTANTIATED)
        {
            EntityAdd(equippedListBoxSPtr_.get());
        }

        equippedListBoxSPtr_->SetSelectedIndex(0);
        equippedListBoxSPtr_->SetImageColor(LISTBOX_COLOR_IMAGE_);
    }


    void InventoryStage::SetupDescBox(const bool WILL_MOVE_OFFSCREEN)
    {
        sfml_util::gui::ListBoxItemSLst_t unEquipItemsSList;
        for (auto const & NEXT_ITEM_SPTR : creaturePtr_->Inventory().Items())
        {
            std::ostringstream itemEntryNameSS;
            itemEntryNameSS << NEXT_ITEM_SPTR->Name() << " " << "UnEquippedItemsListBoxEntry";

            listBoxItemTextInfo_.text = NEXT_ITEM_SPTR->Name();

            auto const EQUIPPED_LISTBOXITEM_SPTR = std::make_shared<sfml_util::gui::ListBoxItem>(
                itemEntryNameSS.str(),
                listBoxItemTextInfo_,
                NEXT_ITEM_SPTR);

            unEquipItemsSList.push_back(EQUIPPED_LISTBOXITEM_SPTR);
        }

        const sfml_util::gui::box::Info LISTBOX_BOX_INFO(1,
                                                         true,
                                                         DESCBOX_REGION_,
                                                         LISTBOX_COLORSET_,
                                                         LISTBOX_BG_INFO_);

        const bool IS_EQ_ALREADY_INSTANTIATED(unEquipListBoxSPtr_.get() != nullptr);

        sf::Vector2f origPosV(OUT_OF_SIGHT_POS_, OUT_OF_SIGHT_POS_);
        if (IS_EQ_ALREADY_INSTANTIATED)
        {
            origPosV = unEquipListBoxSPtr_->GetEntityPos();
            EntityRemove(unEquipListBoxSPtr_.get());
        }

        unEquipListBoxSPtr_ = std::make_shared<sfml_util::gui::ListBox>(
            "InventoryStage'sUnEquipped",
            DESCBOX_REGION_,
            unEquipItemsSList,
            this,
            10.0f,
            6.0f,
            LISTBOX_BOX_INFO,
            LISTBOX_COLOR_LINE_,
            sfml_util::gui::ListBox::NO_LIMIT_,
            this);

        if (IS_EQ_ALREADY_INSTANTIATED)
        {
            EntityAdd(unEquipListBoxSPtr_.get());
        }

        unEquipListBoxSPtr_->SetImageColor(LISTBOX_COLOR_IMAGE_);
        unEquipListBoxSPtr_->SetSelectedIndex(0);

        if (IS_EQ_ALREADY_INSTANTIATED)
        {
            if (WILL_MOVE_OFFSCREEN)
            {
                unEquipListBoxSPtr_->SetEntityPos(SCREEN_WIDTH_ + 1.0f, unEquipListBoxSPtr_->GetEntityPos().y);
            }
            else
            {
                unEquipListBoxSPtr_->SetEntityPos(origPosV.x, origPosV.y);
            }
        }

        if (descBoxUPtr_.get() == nullptr)
        {
            descBoxUPtr_ = std::make_unique<sfml_util::gui::box::Box>("InventoryStage'sDesc", LISTBOX_BOX_INFO);
        }

        sfml_util::gui::TextInfo descTextInfo(listBoxItemTextInfo_);
        descTextInfo.color = DESCBOX_TEXT_COLOR_;
        descTextInfo.text = " ";

        const bool IS_DTR_ALREADY_INSTANTIATED(descTextRegionUPtr_.get() != nullptr);

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

        if (IS_DTR_ALREADY_INSTANTIATED)
        {
            EntityAdd(descTextRegionUPtr_.get());
        }

        descTextRegionUPtr_->SetEntityPos(SCREEN_WIDTH_ + 1.0f, descTextRegionUPtr_->GetEntityPos().y);
        descBoxUPtr_->SetEntityPos(SCREEN_WIDTH_ + 1.0f, descBoxUPtr_->GetEntityPos().y);
    }


    void InventoryStage::SetupFirstListBoxTitle()
    {
        std::string titleText("");
        switch (view_)
        {
            case ViewType::Conditions:  { titleText = "Conditions"; break; }
            case ViewType::Items:       { titleText = "Equipped Items"; break; }
            case ViewType::Spells:      { titleText = "Spells"; break; }
            case ViewType::Titles:
            case ViewType::Count:
            default:                { titleText = "Titles"; break; }
        }

        const sfml_util::gui::TextInfo LISTBOX_TEXT_INFO(titleText,
                                                         sfml_util::FontManager::Instance()->Font_Default2(),
                                                         sfml_util::FontManager::Instance()->Size_Normal(),
                                                         LISTBOX_COLOR_TITLE_,
                                                         sfml_util::Justified::Center);

        const sf::FloatRect LISTBOX_TEXT_RECT(FIRST_LISTBOX_POS_LEFT_, 0.0f, LISTBOX_WIDTH_, 0.0f);

        if (eqTitleTextRegionUPtr_.get() == nullptr)
        {
            eqTitleTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "InventoryStage'sEquippedListBoxTitle",
                LISTBOX_TEXT_INFO,
                LISTBOX_TEXT_RECT);
        }
        else
        {
            eqTitleTextRegionUPtr_->Setup(LISTBOX_TEXT_INFO, LISTBOX_TEXT_RECT);
        }

        eqTitleTextRegionUPtr_->SetEntityPos(eqTitleTextRegionUPtr_->GetEntityPos().x, LISTBOX_POS_TOP_ - eqTitleTextRegionUPtr_->GetEntityRegion().height);
    }


    void InventoryStage::SetupDescBoxTitle()
    {
        const std::string TITLETEXT((ViewType::Items == view_) ? "Unequipped Items" : "Description");

        const sfml_util::gui::TextInfo DESC_TEXT_INFO(TITLETEXT,
                                                      sfml_util::FontManager::Instance()->Font_Default2(),
                                                      sfml_util::FontManager::Instance()->Size_Normal(),
                                                      LISTBOX_COLOR_TITLE_,
                                                      sfml_util::Justified::Center);

        const sf::FloatRect DESC_TEXT_RECT(SECOND_LISTBOX_POS_LEFT_, 0.0f, LISTBOX_WIDTH_, 0.0f);

        if (unEqTitleTextRegionUPtr_.get() == nullptr)
        {
            unEqTitleTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "InventoryStage'sUnequippedListBoxTitle",
                DESC_TEXT_INFO,
                DESC_TEXT_RECT);
        }
        else
        {
            unEqTitleTextRegionUPtr_->Setup(DESC_TEXT_INFO, DESC_TEXT_RECT);
        }

        unEqTitleTextRegionUPtr_->SetEntityPos(unEqTitleTextRegionUPtr_->GetEntityPos().x, LISTBOX_POS_TOP_ - unEqTitleTextRegionUPtr_->GetEntityRegion().height);
    }


    void InventoryStage::SetDescBoxTextFromListBoxItem(const sfml_util::gui::ListBoxItemSPtr_t & LISTBOX_ITEM_SPTR)
    {
        if (LISTBOX_ITEM_SPTR.get() != nullptr)
        {
            std::ostringstream ss;

            if (LISTBOX_ITEM_SPTR->COND_CPTRC != nullptr)
            {
                ss << LISTBOX_ITEM_SPTR->COND_CPTRC->Name() << "\n\n" << LISTBOX_ITEM_SPTR->COND_CPTRC->LongDesc();
            }
            else if (LISTBOX_ITEM_SPTR->TITLE_CPTRC != nullptr)
            {
                ss << LISTBOX_ITEM_SPTR->TITLE_CPTRC->Name() << "\n\n" << LISTBOX_ITEM_SPTR->TITLE_CPTRC->LongDesc();
            }
            else if (LISTBOX_ITEM_SPTR->SPELL_CPTRC != nullptr)
            {
                ss << LISTBOX_ITEM_SPTR->SPELL_CPTRC->Name() << "\n\n" << LISTBOX_ITEM_SPTR->SPELL_CPTRC->DescComplete();
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

        const bool IS_DTR_ALREADY_INSTANTIATED(descTextRegionUPtr_.get() != nullptr);

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


    void InventoryStage::PopupCharacterSelectWindow(const std::string & PROMPT_TEXT)
    {
        std::ostringstream ss;
        ss << PROMPT_TEXT << "  Select a character by pressing the number key:\n\n";

        const std::size_t CURRENT_CREATURE_ORDER_NUM(Game::Instance()->State().Party().GetOrderNum(creaturePtr_));

        std::vector<std::size_t> invalidCharacterNumVec;
        const std::size_t NUM_CHARACTERS(6);
        for (std::size_t i(0); i < NUM_CHARACTERS; ++i)
        {
            if ((CURRENT_CREATURE_ORDER_NUM != i) && (Game::Instance()->State().Party().GetAtOrderPos(i)->Body().IsHumanoid()))
            {
                ss << "(" << i + 1 << ") " << Game::Instance()->State().Party().GetAtOrderPos(i)->Name() << "\n";
            }
            else
            {
                invalidCharacterNumVec.push_back(i);
            }
        }

        const PopupInfo POPUP_INFO(sfml_util::gui::PopupManager::Instance()->CreatePopupInfo(POPUP_NAME_CHAR_SELECT_,
                                                                                             ss.str(),
                                                                                             invalidCharacterNumVec,
                                                                                             sfml_util::FontManager::Instance()->Size_Smallish()));
        LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
        isWaitingOnPopup_ = true;
    }


    void InventoryStage::PopupRejectionWindow(const std::string & REJECTION_PROMPT_TEXT, const bool WILL_USE_REGULAR_SIZE_POPUP)
    {
        const PopupInfo POPUP_INFO_NOITEM(sfml_util::gui::PopupManager::Instance()->CreatePopupInfo("InventoryStage'sPopupRejection",
                                                                                                    REJECTION_PROMPT_TEXT,
                                                                                                    sfml_util::PopupButtons::Cancel,
                                                                                                    ((WILL_USE_REGULAR_SIZE_POPUP) ? sfml_util::PopupImage::Regular : sfml_util::PopupImage::Banner),
                                                                                                    sfml_util::Justified::Center,
                                                                                                    sfml_util::sound_effect::PromptWarn,
                                                                                                    game::Popup::Generic,
                                                                                                    sfml_util::FontManager::Instance()->Size_Largeish()));
        LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO_NOITEM);
        isWaitingOnPopup_ = true;
        EndOfGiveShareGatherTasks();
    }


    void InventoryStage::PopupNumberSelectWindow(const std::string & PROMPT_TEXT, const std::size_t NUMBER_MAX)
    {
        const PopupInfo POPUP_INFO_NUMBER_SELECT(sfml_util::gui::PopupManager::Instance()->CreatePopupInfo(POPUP_NAME_NUMBER_SELECT_,
                                                                                                           PROMPT_TEXT,
                                                                                                           1,
                                                                                                           NUMBER_MAX,
                                                                                                           sfml_util::FontManager::Instance()->Size_Largeish()));
        LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO_NUMBER_SELECT);
        isWaitingOnPopup_ = true;
    }


    void InventoryStage::PopupDoneWindow(const std::string & PROMPT_TEXT, const bool WILL_PLAY_SOUNDEFFECT)
    {
        const PopupInfo POPUP_INFO_DONE(sfml_util::gui::PopupManager::Instance()->CreatePopupInfo("InventoryStage'sPopupDone",
                                                                                                  PROMPT_TEXT,
                                                                                                  sfml_util::PopupButtons::Okay,
                                                                                                  sfml_util::PopupImage::Regular,
                                                                                                  sfml_util::Justified::Center,
                                                                                                  ((WILL_PLAY_SOUNDEFFECT) ? sfml_util::sound_effect::PromptGeneric : sfml_util::sound_effect::None),
                                                                                                  Popup::Generic,
                                                                                                  sfml_util::FontManager::Instance()->Size_Largeish()));
        LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO_DONE);
        isWaitingOnPopup_ = true;
        EndOfGiveShareGatherTasks();
    }


    void InventoryStage::PopupContentSelectionWindow(const std::string & PROMPT_TEXT)
    {
        std::ostringstream ss;
        ss << PROMPT_TEXT << "\n\n(C)oins\n(G)ems\n(M)eteor Shards";
        const PopupInfo POPUP_INFO_DONE(sfml_util::gui::PopupManager::Instance()->CreatePopupInfo(POPUP_NAME_CONTENTSELECTION_,
                                                                                                  ss.str(),
                                                                                                  sfml_util::PopupButtons::Cancel,
                                                                                                  sfml_util::PopupImage::Regular,
                                                                                                  sfml_util::Justified::Center,
                                                                                                  sfml_util::sound_effect::PromptGeneric,
                                                                                                  game::Popup::ContentSelectionWithoutItem,
                                                                                                  sfml_util::FontManager::Instance()->Size_Normal()));
        LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO_DONE);
        isWaitingOnPopup_ = true;
    }


    void InventoryStage::HandleCoinsGive(const std::size_t COUNT, creature::CreaturePtr_t creatureToGiveToPtr)
    {
        sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::Coin).PlayRandom();

        creaturePtr_->CoinsAdj(static_cast<item::Coin_t>(COUNT) * -1);
        creatureToGiveToPtr->CoinsAdj(static_cast<item::Coin_t>(COUNT));

        std::ostringstream ss;
        ss << COUNT << " coins taken from " << creaturePtr_->Name() << " and given to " << creatureToGiveToPtr->Name() << ".";
        PopupDoneWindow(ss.str(), false);
    }


    void InventoryStage::HandleGemsGive(const std::size_t COUNT, creature::CreaturePtr_t creatureToGiveToPtr)
    {
        sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::Gem).PlayRandom();

        creaturePtr_->GemsAdj(static_cast<item::Gem_t>(COUNT) * -1);
        creatureToGiveToPtr->GemsAdj(static_cast<item::Gem_t>(COUNT));

        std::ostringstream ss;
        ss << COUNT << " gems taken from " << creaturePtr_->Name() << " and given to " << creatureToGiveToPtr->Name() << ".";
        PopupDoneWindow(ss.str(), false);
    }


    void InventoryStage::HandleMeteorShardsGive(const std::size_t COUNT, creature::CreaturePtr_t creatureToGiveToPtr)
    {
        sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::MeteorShard).PlayRandom();

        creaturePtr_->MeteorShardsAdj(static_cast<item::Meteor_t>(COUNT) * -1);
        creatureToGiveToPtr->MeteorShardsAdj(static_cast<item::Meteor_t>(COUNT));

        std::ostringstream ss;
        ss << COUNT << " Meteor Shards taken from " << creaturePtr_->Name() << " and given to " << creatureToGiveToPtr->Name() << ".";
        PopupDoneWindow(ss.str(), false);
    }


    void InventoryStage::HandleCoinsGather(const bool WILL_POPUP)
    {
        sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::Coin).PlayRandom();

        std::size_t coinsOwnedByOtherPartyMembers(0);
        for (auto nextCreaturePtr : Game::Instance()->State().Party().Characters())
        {
            if (nextCreaturePtr != creaturePtr_)
            {
                const item::Coin_t NEXT_CREATURE_COINS_OWNED(nextCreaturePtr->Inventory().Coins());
                if (NEXT_CREATURE_COINS_OWNED > 0)
                {
                    coinsOwnedByOtherPartyMembers += static_cast<std::size_t>(NEXT_CREATURE_COINS_OWNED);
                    nextCreaturePtr->CoinsAdj(NEXT_CREATURE_COINS_OWNED * -1);
                }
            }
        }

        creaturePtr_->CoinsAdj(static_cast<item::Coin_t>(coinsOwnedByOtherPartyMembers));

        if (WILL_POPUP)
        {
            std::ostringstream ss;
            ss << "\n\n" << creaturePtr_->Name() << " gathered " << coinsOwnedByOtherPartyMembers
                << " coins from the rest of the party, and now has "
                << creaturePtr_->Inventory().Coins() << ".";

            PopupDoneWindow(ss.str(), false);
        }
    }


    void InventoryStage::HandleGemsGather(const bool WILL_POPUP)
    {
        sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::Gem).PlayRandom();

        std::size_t gemsOwnedByOtherPartyMembers(0);
        for (auto nextCreaturePtr : Game::Instance()->State().Party().Characters())
        {
            if (nextCreaturePtr != creaturePtr_)
            {
                const item::Gem_t NEXT_CREATURE_GEMS_OWNED(nextCreaturePtr->Inventory().Gems());
                if (NEXT_CREATURE_GEMS_OWNED > 0)
                {
                    gemsOwnedByOtherPartyMembers += static_cast<std::size_t>(NEXT_CREATURE_GEMS_OWNED);
                    nextCreaturePtr->GemsAdj(NEXT_CREATURE_GEMS_OWNED * -1);
                }
            }
        }

        creaturePtr_->GemsAdj(static_cast<item::Gem_t>(gemsOwnedByOtherPartyMembers));

        if (WILL_POPUP)
        {
            std::ostringstream ss;
            ss << "\n\n" << creaturePtr_->Name() << " gathered " << gemsOwnedByOtherPartyMembers
                << " gems from the rest of the party, and now has "
                << creaturePtr_->Inventory().Gems() << ".";

            PopupDoneWindow(ss.str(), false);
        }
    }


    void InventoryStage::HandleMeteorShardsGather(const bool WILL_POPUP)
    {
        sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::MeteorShard).PlayRandom();

        std::size_t shardsOwnedByOtherPartyMembers(0);
        for (auto nextCreaturePtr : Game::Instance()->State().Party().Characters())
        {
            if (nextCreaturePtr != creaturePtr_)
            {
                const item::Meteor_t NEXT_CREATURE_SHARDS_OWNED(nextCreaturePtr->Inventory().MeteorShards());
                if (NEXT_CREATURE_SHARDS_OWNED > 0)
                {
                    shardsOwnedByOtherPartyMembers += static_cast<std::size_t>(NEXT_CREATURE_SHARDS_OWNED);
                    nextCreaturePtr->MeteorShardsAdj(NEXT_CREATURE_SHARDS_OWNED * -1);
                }
            }
        }

        creaturePtr_->MeteorShardsAdj(static_cast<item::Meteor_t>(shardsOwnedByOtherPartyMembers));

        if (WILL_POPUP)
        {
            std::ostringstream ss;
            ss << "\n\n" << creaturePtr_->Name() << " gathered " << shardsOwnedByOtherPartyMembers
                << " Meteor Shards from the rest of the party, and now has "
                << creaturePtr_->Inventory().MeteorShards() << ".";

            PopupDoneWindow(ss.str(), false);
        }
    }


    void InventoryStage::HandleCoinsShare()
    {
        sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::Coin).PlayRandom();

        HandleCoinsGather(false);

        const item::Coin_t COINS_TOTAL(creaturePtr_->Inventory().Coins());

        const item::Coin_t HUMANOID_COUNT(static_cast<item::Coin_t>(
            Game::Instance()->State().Party().GetNumHumanoid()));

        const item::Coin_t COINS_TO_SHARE(COINS_TOTAL / HUMANOID_COUNT);
        const item::Coin_t COINS_LEFT_OVER(COINS_TOTAL % HUMANOID_COUNT);

        for (auto nextCreaturePtr : Game::Instance()->State().Party().Characters())
        {
            if (nextCreaturePtr->Body().IsHumanoid())
            {
                nextCreaturePtr->CoinsAdj(nextCreaturePtr->Inventory().Coins() * -1);
                nextCreaturePtr->CoinsAdj(COINS_TO_SHARE);
            }
        }

        item::Coin_t toHandOut(COINS_LEFT_OVER);
        for (auto nextCreaturePtr : Game::Instance()->State().Party().Characters())
        {
            if (nextCreaturePtr->Body().IsHumanoid() && (toHandOut-- > 0))
            {
                nextCreaturePtr->CoinsAdj(1);
            }
        }

        std::ostringstream ss;
        if (COINS_TO_SHARE > 0)
        {
            ss << "\n\nAll " << HUMANOID_COUNT << " humanoid party members share " << COINS_TO_SHARE;

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
        sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::Gem).PlayRandom();

        HandleGemsGather(false);

        const item::Gem_t GEMS_TOTAL(creaturePtr_->Inventory().Gems());
        const item::Gem_t HUMANOID_COUNT(static_cast<item::Gem_t>(Game::Instance()->State().Party().GetNumHumanoid()));
        const item::Gem_t GEMS_TO_SHARE(GEMS_TOTAL / HUMANOID_COUNT);
        const item::Gem_t GEMS_LEFT_OVER(GEMS_TOTAL % HUMANOID_COUNT);

        for (auto nextCreaturePtr : Game::Instance()->State().Party().Characters())
        {
            if (nextCreaturePtr->Body().IsHumanoid())
            {
                nextCreaturePtr->GemsAdj(nextCreaturePtr->Inventory().Gems() * -1);
                nextCreaturePtr->GemsAdj(GEMS_TO_SHARE);
            }
        }

        item::Gem_t toHandOut(GEMS_LEFT_OVER);
        for (auto nextCreaturePtr : Game::Instance()->State().Party().Characters())
        {
            if (nextCreaturePtr->Body().IsHumanoid() && (toHandOut-- > 0))
            {
                nextCreaturePtr->GemsAdj(1);
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
        sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::MeteorShard).PlayRandom();

        HandleMeteorShardsGather(false);

        const item::Meteor_t METEORSHARDS_TOTAL(creaturePtr_->Inventory().MeteorShards());
        const item::Meteor_t HUMANOID_COUNT(static_cast<item::Meteor_t>(Game::Instance()->State().Party().GetNumHumanoid()));
        const item::Meteor_t METEORSHARDS_TO_SHARE(METEORSHARDS_TOTAL / HUMANOID_COUNT);
        const item::Meteor_t METEORSHARDS_LEFT_OVER(METEORSHARDS_TOTAL % HUMANOID_COUNT);

        for (auto nextCreaturePtr : Game::Instance()->State().Party().Characters())
        {
            if (nextCreaturePtr->Body().IsHumanoid())
            {
                nextCreaturePtr->MeteorShardsAdj(nextCreaturePtr->Inventory().MeteorShards() * -1);
                nextCreaturePtr->MeteorShardsAdj(METEORSHARDS_TO_SHARE);
            }
        }

        item::Gem_t toHandOut(METEORSHARDS_LEFT_OVER);
        for (auto nextCreaturePtr : Game::Instance()->State().Party().Characters())
        {
            if (nextCreaturePtr->Body().IsHumanoid() && (toHandOut-- > 0))
            {
                nextCreaturePtr->MeteorShardsAdj(1);
            }
        }

        std::ostringstream ss;
        if (METEORSHARDS_TO_SHARE > 0)
        {
            ss << "\n\nAll " << HUMANOID_COUNT << " humanoid party members share " << METEORSHARDS_TO_SHARE;

            if (METEORSHARDS_LEFT_OVER > 0)
            {
                ss << " or " << METEORSHARDS_TO_SHARE + 1;
            }
        }
        else
        {
            ss << "\n\nThere were only " << METEORSHARDS_TO_SHARE << ", so not every character received";
        }

        ss << " Meteor Shards.";
        PopupDoneWindow(ss.str(), false);
    }


    void InventoryStage::EndOfGiveShareGatherTasks()
    {
        listBoxItemToGiveSPtr_.reset();
        creatureToGiveToPtr_ = nullptr;
        iItemToDropPtr_ = nullptr;
        contentType_ = ContentType::Count;
        actionType_ = ActionType::Count;
        SetupCreatureDetails(false);
        SetupCreatureStats();
        SetupCenterText();
    }


    float InventoryStage::UpdateImageDetailsPosition()
    {
        sf::Texture texture;
        sfml_util::gui::CreatureImageManager::Instance()->GetImage(texture, creaturePtr_->ImageFilename());
        sf::Sprite sprite(texture);
        sprite.setScale(CREATURE_IMAGE_SCALE_, CREATURE_IMAGE_SCALE_);
        detailsPosLeft_ = CREATURE_IMAGE_POS_LEFT_ + sprite.getGlobalBounds().width + CREATURE_IMAGE_RIGHT_PAD_;
        return sprite.getGlobalBounds().width;
    }


    item::ItemPtr_t InventoryStage::GetItemMouseIsOver(const sf::Vector2f & MOUSE_POS_V)
    {
        if (view_ == ViewType::Items)
        {
            sfml_util::gui::ListBoxItemSPtr_t listBoxItemPtr;

            if (unEquipListBoxSPtr_->GetEntityRegion().contains(MOUSE_POS_V))
            {
                listBoxItemPtr = unEquipListBoxSPtr_->GetAtPosition(MOUSE_POS_V);
            }
            else if (equippedListBoxSPtr_->GetEntityRegion().contains(MOUSE_POS_V))
            {
                listBoxItemPtr = equippedListBoxSPtr_->GetAtPosition(MOUSE_POS_V);
            }

            if (listBoxItemPtr.get() != nullptr)
            {
                return listBoxItemPtr->ITEM_CPTR;
            }
        }

        return item::ItemPtr_t();
    }


    const sf::FloatRect InventoryStage::GetItemRectMouseIsOver(const sf::Vector2f & MOUSE_POS_V)
    {
        if (view_ == ViewType::Items)
        {
            sfml_util::gui::ListBoxItemSPtr_t listBoxItemPtr;

            if (unEquipListBoxSPtr_->GetEntityRegion().contains(MOUSE_POS_V))
            {
                return unEquipListBoxSPtr_->GetRectAtLocation(MOUSE_POS_V);
            }
            else if (equippedListBoxSPtr_->GetEntityRegion().contains(MOUSE_POS_V))
            {
                return equippedListBoxSPtr_->GetRectAtLocation(MOUSE_POS_V);
            }
        }

        return sfml_util::gui::ListBox::ERROR_RECT_;
    }


    void InventoryStage::SetupDetailViewItem(const item::ItemPtr_t IITEM_PTR)
    {
        if (IITEM_PTR == nullptr)
        {
            detailViewTextUPtr_.reset();
            return;
        }

        sfml_util::gui::ItemImageManager::Instance()->Load(detailViewTexture_, IITEM_PTR);
        detailViewTexture_.setSmooth(true);

        detailViewSprite_.setTexture(detailViewTexture_);
        detailViewSprite_.setTextureRect(sf::IntRect(0, 0, static_cast<int>(detailViewTexture_.getSize().x), static_cast<int>(detailViewTexture_.getSize().y)));
        const float DETAILVIEW_IMAGE_SCALE(sfml_util::MapByRes(0.75f, 1.25f));
        detailViewSprite_.setScale(DETAILVIEW_IMAGE_SCALE, DETAILVIEW_IMAGE_SCALE);
        detailViewSprite_.setPosition((SCREEN_WIDTH_ * 0.5f) - (detailViewSprite_.getGlobalBounds().width * 0.5f), DETAILVIEW_POS_TOP_ + DETAILVIEW_INNER_PAD_);

        std::ostringstream ss;
        ss << IITEM_PTR->Name() << "\n"
            << IITEM_PTR->Desc() << "\n\n"
            << item::category::ToString(IITEM_PTR->Category(), true) << "\n";

        if (IITEM_PTR->ExclusiveRole() != creature::role::Count)
        {
            ss << "(can only be used by " << creature::role::ToString(IITEM_PTR->ExclusiveRole()) << "s)\n";
        }

        ss << "\n";

        ss  << "weighs " << IITEM_PTR->Weight() << "\n"
            << "worth about " << IITEM_PTR->Price() << " coins\n";

        if (IITEM_PTR->IsWeapon())
        {
            ss << "Damage:  " << IITEM_PTR->DamageMin() << "-" << IITEM_PTR->DamageMax();
        }
        else if (IITEM_PTR->IsArmor())
        {
            ss << "Armor Bonus:  " << IITEM_PTR->ArmorRating();
        }

        ss << "\n\n";

        const sfml_util::gui::TextInfo TEXT_INFO(ss.str(),
                                                 sfml_util::FontManager::Instance()->Font_Default1(),
                                                 sfml_util::FontManager::Instance()->Size_Normal(),
                                                 sf::Color::White,
                                                 sfml_util::Justified::Center);

        const sf::FloatRect TEXT_RECT(DETAILVIEW_POS_LEFT_ + DETAILVIEW_INNER_PAD_,
                                      detailViewSprite_.getGlobalBounds().top + detailViewSprite_.getGlobalBounds().height + DETAILVIEW_INNER_PAD_,
                                      DETAILVIEW_WIDTH_ - (2.0f * DETAILVIEW_INNER_PAD_),
                                      (DETAILVIEW_HEIGHT_ - (detailViewSprite_.getGlobalBounds().top + detailViewSprite_.getGlobalBounds().height)) - (2.0f * DETAILVIEW_INNER_PAD_));

        detailViewTextUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "InventoryStage'sDetailViewForItems", TEXT_INFO, TEXT_RECT);
    }


    void InventoryStage::SetupDetailViewCreature(creature::CreatureCPtrC_t CREATURE_CPTRC)
    {
        if (CREATURE_CPTRC == nullptr)
        {
            detailViewTextUPtr_.reset();
            return;
        }

        sfml_util::gui::CreatureImageManager::Instance()->GetImage(detailViewTexture_, CREATURE_CPTRC->ImageFilename(), true);
        detailViewTexture_.setSmooth(true);

        detailViewSprite_.setTexture(detailViewTexture_);
        detailViewSprite_.setTextureRect(sf::IntRect(0, 0, static_cast<int>(detailViewTexture_.getSize().x), static_cast<int>(detailViewTexture_.getSize().y)));
        detailViewSprite_.setScale(CREATURE_IMAGE_SCALE_, CREATURE_IMAGE_SCALE_);
        detailViewSprite_.setPosition((SCREEN_WIDTH_ * 0.5f) - (detailViewSprite_.getGlobalBounds().width * 0.5f), DETAILVIEW_POS_TOP_ + DETAILVIEW_INNER_PAD_ + sfml_util::MapByRes(0.0f, 50.0f));

        std::ostringstream ss;
        creature::Achievements a(CREATURE_CPTRC->AchievementsCopy());

        const std::string NOMORE_TITLE_MSG_STR("  (There are no more titles to earn)");

        creature::TitleCPtrC_t ENEMIESFACED_NEXT_TITLE_CPTR    (a.GetNextTitle(creature::AchievementType::EnemiesFaced));
        creature::TitleCPtrC_t MELEEHITS_NEXT_TITLE_CPTR       (a.GetNextTitle(creature::AchievementType::MeleeHits));
        creature::TitleCPtrC_t BATTLESSURVIVED_NEXT_TITLE_CPTR (a.GetNextTitle(creature::AchievementType::BattlesSurvived));
        creature::TitleCPtrC_t PROJECTILEHITS_NEXT_TITLE_CPTR  (a.GetNextTitle(creature::AchievementType::ProjectileHits));
        creature::TitleCPtrC_t BEASTMINDLINKS_NEXT_TITLE_CPTR  (a.GetNextTitle(creature::AchievementType::BeastMindLinks));
        creature::TitleCPtrC_t DODGEDSTANDING_NEXT_TITLE_CPTR  (a.GetNextTitle(creature::AchievementType::DodgedStanding));
        creature::TitleCPtrC_t DODGEDFLYING_NEXT_TITLE_CPTR    (a.GetNextTitle(creature::AchievementType::DodgedFlying));
        creature::TitleCPtrC_t LOCKSPICKED_NEXT_TITLE_CPTR     (a.GetNextTitle(creature::AchievementType::LocksPicked));
        creature::TitleCPtrC_t BACKSTABSHITS_NEXT_TITLE_CPTR   (a.GetNextTitle(creature::AchievementType::BackstabsHits));
        creature::TitleCPtrC_t SONGSPLAYED_NEXT_TITLE_CPTR     (a.GetNextTitle(creature::AchievementType::SongsPlayed));
        creature::TitleCPtrC_t SPIRITSLIFTED_NEXT_TITLE_CPTR   (a.GetNextTitle(creature::AchievementType::SpiritsLifted));
        creature::TitleCPtrC_t BEASTROARS_NEXT_TITLE_CPTR      (a.GetNextTitle(creature::AchievementType::BeastRoars));
        creature::TitleCPtrC_t MOONHOWLS_NEXT_TITLE_CPTR       (a.GetNextTitle(creature::AchievementType::MoonHowls));
        creature::TitleCPtrC_t PACKACTIONS_NEXT_TITLE_CPTR     (a.GetNextTitle(creature::AchievementType::PackActions));
        creature::TitleCPtrC_t FLYINGATTACKHITS_NEXT_TITLE_CPTR(a.GetNextTitle(creature::AchievementType::FlyingAttackHits));
        creature::TitleCPtrC_t TURNSINFLIGHT_NEXT_TITLE_CPTR   (a.GetNextTitle(creature::AchievementType::TurnsInFlight));
        creature::TitleCPtrC_t SPELLSCAST_NEXT_TITLE_CPTR      (a.GetNextTitle(creature::AchievementType::SpellsCast));
        creature::TitleCPtrC_t HEALTHGIVEN_NEXT_TITLE_CPTR     (a.GetNextTitle(creature::AchievementType::HealthGiven));
        creature::TitleCPtrC_t HEALTHTRADED_NEXT_TITLE_CPTR    (a.GetNextTitle(creature::AchievementType::HealthTraded));

        std::string enemiesFacedNextTitleStr        (NOMORE_TITLE_MSG_STR);
        std::string meleeHitsNextTitleStr           (NOMORE_TITLE_MSG_STR);
        std::string battlesSurvivedNextTitleStr     (NOMORE_TITLE_MSG_STR);
        std::string projectileHitsNextTitleStr      (NOMORE_TITLE_MSG_STR);
        std::string beastMindLinksNextTitleStr      (NOMORE_TITLE_MSG_STR);
        std::string dodgedStandingNextTitleStr      (NOMORE_TITLE_MSG_STR);
        std::string dodgedFlyingNextTitleStr        (NOMORE_TITLE_MSG_STR);
        std::string locksPickedNextTitleStr         (NOMORE_TITLE_MSG_STR);
        std::string backstabsHitsNextTitleStr       (NOMORE_TITLE_MSG_STR);
        std::string songsPlayedNextTitleStr         (NOMORE_TITLE_MSG_STR);
        std::string spiritsLiftedNextTitleStr       (NOMORE_TITLE_MSG_STR);
        std::string beastRoarsNextTitleStr          (NOMORE_TITLE_MSG_STR);
        std::string moonHowlsNextTitleStr           (NOMORE_TITLE_MSG_STR);
        std::string packActionsNextTitleStr         (NOMORE_TITLE_MSG_STR);
        std::string flyingAttackHitsNextTitleStr    (NOMORE_TITLE_MSG_STR);
        std::string turnsInFlightNextTitleStr       (NOMORE_TITLE_MSG_STR);
        std::string spellsCastNextTitleStr          (NOMORE_TITLE_MSG_STR);
        std::string healthGivenNextTitleStr         (NOMORE_TITLE_MSG_STR);
        std::string healthTradedNextTitleStr        (NOMORE_TITLE_MSG_STR);

        if (ENEMIESFACED_NEXT_TITLE_CPTR != nullptr)
        {
            ss.str("");
            ss << ", need  " << (ENEMIESFACED_NEXT_TITLE_CPTR->AchievementCount() - a.AchievementCopy(creature::AchievementType::EnemiesFaced).Count())
               << " more to achieve \"" << ENEMIESFACED_NEXT_TITLE_CPTR->Name() << "\"";
            enemiesFacedNextTitleStr = ss.str();
        }

        if (MELEEHITS_NEXT_TITLE_CPTR != nullptr)
        {
            ss.str("");
            ss << ", need  " << (MELEEHITS_NEXT_TITLE_CPTR->AchievementCount() - a.AchievementCopy(creature::AchievementType::MeleeHits).Count())
               << " more to achieve \"" << MELEEHITS_NEXT_TITLE_CPTR->Name() << "\"";
            meleeHitsNextTitleStr = ss.str();
        }

        if (BATTLESSURVIVED_NEXT_TITLE_CPTR != nullptr)
        {
            ss.str("");
            ss  << ", need  " << (BATTLESSURVIVED_NEXT_TITLE_CPTR->AchievementCount() - a.AchievementCopy(creature::AchievementType::BattlesSurvived).Count())
                << " more to achieve \"" << BATTLESSURVIVED_NEXT_TITLE_CPTR->Name() << "\"";
            battlesSurvivedNextTitleStr = ss.str();
        }

        if (PROJECTILEHITS_NEXT_TITLE_CPTR != nullptr)
        {
            ss.str("");
            ss << ", need  " << (PROJECTILEHITS_NEXT_TITLE_CPTR->AchievementCount() - a.AchievementCopy(creature::AchievementType::ProjectileHits).Count())
               << " more to achieve \"" << PROJECTILEHITS_NEXT_TITLE_CPTR->Name() << "\"";
            projectileHitsNextTitleStr = ss.str();
        }

        if (BEASTMINDLINKS_NEXT_TITLE_CPTR != nullptr)
        {
            ss.str("");
            ss << ", need  " << (BEASTMINDLINKS_NEXT_TITLE_CPTR->AchievementCount() - a.AchievementCopy(creature::AchievementType::BeastMindLinks).Count())
               << " more to achieve \"" << BEASTMINDLINKS_NEXT_TITLE_CPTR->Name() << "\"";
            beastMindLinksNextTitleStr = ss.str();
        }

        if (DODGEDSTANDING_NEXT_TITLE_CPTR != nullptr)
        {
            ss.str("");
            ss << ", need  " << (DODGEDSTANDING_NEXT_TITLE_CPTR->AchievementCount() - a.AchievementCopy(creature::AchievementType::DodgedStanding).Count())
               << " more to achieve \"" << DODGEDSTANDING_NEXT_TITLE_CPTR->Name() << "\"";
            dodgedStandingNextTitleStr = ss.str();
        }

        if (DODGEDFLYING_NEXT_TITLE_CPTR != nullptr)
        {
            ss.str("");
            ss << ", need  " << (DODGEDFLYING_NEXT_TITLE_CPTR->AchievementCount() - a.AchievementCopy(creature::AchievementType::DodgedFlying).Count())
               << " more to achieve \"" << DODGEDFLYING_NEXT_TITLE_CPTR->Name() << "\"";
            dodgedFlyingNextTitleStr = ss.str();
        }

        if (LOCKSPICKED_NEXT_TITLE_CPTR != nullptr)
        {
            ss.str("");
            ss << ", need  " << (LOCKSPICKED_NEXT_TITLE_CPTR->AchievementCount() - a.AchievementCopy(creature::AchievementType::LocksPicked).Count())
               << " more to achieve \"" << LOCKSPICKED_NEXT_TITLE_CPTR->Name() << "\"";
            locksPickedNextTitleStr = ss.str();
        }

        if (BACKSTABSHITS_NEXT_TITLE_CPTR != nullptr)
        {
            ss.str("");
            ss << ", need  " << (BACKSTABSHITS_NEXT_TITLE_CPTR->AchievementCount() - a.AchievementCopy(creature::AchievementType::BackstabsHits).Count())
               << " more to achieve \"" << BACKSTABSHITS_NEXT_TITLE_CPTR->Name() << "\"";
            backstabsHitsNextTitleStr = ss.str();
        }

        if (SONGSPLAYED_NEXT_TITLE_CPTR != nullptr)
        {
            ss.str("");
            ss << ", need  " << (SONGSPLAYED_NEXT_TITLE_CPTR->AchievementCount() - a.AchievementCopy(creature::AchievementType::SongsPlayed).Count())
               << " more to achieve \"" << SONGSPLAYED_NEXT_TITLE_CPTR->Name() << "\"";
            songsPlayedNextTitleStr = ss.str();
        }

        if (SPIRITSLIFTED_NEXT_TITLE_CPTR != nullptr)
        {
            ss.str("");
            ss << ", need  " << (SPIRITSLIFTED_NEXT_TITLE_CPTR->AchievementCount() - a.AchievementCopy(creature::AchievementType::SpiritsLifted).Count())
               << " more to achieve \"" << SPIRITSLIFTED_NEXT_TITLE_CPTR->Name() << "\"";
            spiritsLiftedNextTitleStr = ss.str();
        }

        if (BEASTROARS_NEXT_TITLE_CPTR != nullptr)
        {
            ss.str("");
            ss << ", need  " << (BEASTROARS_NEXT_TITLE_CPTR->AchievementCount() - a.AchievementCopy(creature::AchievementType::BeastRoars).Count())
               << " more to achieve \"" << BEASTROARS_NEXT_TITLE_CPTR->Name() << "\"";
            beastRoarsNextTitleStr = ss.str();
        }

        if (MOONHOWLS_NEXT_TITLE_CPTR != nullptr)
        {
            ss.str("");
            ss << ", need  " << (MOONHOWLS_NEXT_TITLE_CPTR->AchievementCount() - a.AchievementCopy(creature::AchievementType::MoonHowls).Count())
               << " more to achieve \"" << MOONHOWLS_NEXT_TITLE_CPTR->Name() << "\"";
            moonHowlsNextTitleStr = ss.str();
        }

        if (PACKACTIONS_NEXT_TITLE_CPTR != nullptr)
        {
            ss.str("");
            ss << ", need  " << (PACKACTIONS_NEXT_TITLE_CPTR->AchievementCount() - a.AchievementCopy(creature::AchievementType::PackActions).Count())
               << " more to achieve \"" << PACKACTIONS_NEXT_TITLE_CPTR->Name() << "\"";
            packActionsNextTitleStr = ss.str();
        }

        if (FLYINGATTACKHITS_NEXT_TITLE_CPTR != nullptr)
        {
            ss.str("");
            ss << ", need  " << (FLYINGATTACKHITS_NEXT_TITLE_CPTR->AchievementCount() - a.AchievementCopy(creature::AchievementType::FlyingAttackHits).Count())
               << " more to achieve \"" << FLYINGATTACKHITS_NEXT_TITLE_CPTR->Name() << "\"";
            flyingAttackHitsNextTitleStr = ss.str();
        }

        if (TURNSINFLIGHT_NEXT_TITLE_CPTR != nullptr)
        {
            ss.str("");
            ss << ", need  " << (TURNSINFLIGHT_NEXT_TITLE_CPTR->AchievementCount() - a.AchievementCopy(creature::AchievementType::TurnsInFlight).Count())
               << " more to achieve \"" << TURNSINFLIGHT_NEXT_TITLE_CPTR->Name() << "\"";
            turnsInFlightNextTitleStr = ss.str();
        }

        if (SPELLSCAST_NEXT_TITLE_CPTR != nullptr)
        {
            ss.str("");
            ss << ", need  " << (SPELLSCAST_NEXT_TITLE_CPTR->AchievementCount() - a.AchievementCopy(creature::AchievementType::SpellsCast).Count())
               << " more to achieve \"" << SPELLSCAST_NEXT_TITLE_CPTR->Name() << "\"";
            spellsCastNextTitleStr = ss.str();
        }

        if (HEALTHGIVEN_NEXT_TITLE_CPTR != nullptr)
        {
            ss.str("");
            ss << ", need  " << (HEALTHGIVEN_NEXT_TITLE_CPTR->AchievementCount() - a.AchievementCopy(creature::AchievementType::HealthGiven).Count())
               << " more to achieve \"" << HEALTHGIVEN_NEXT_TITLE_CPTR->Name() << "\"";
            healthGivenNextTitleStr = ss.str();
        }

        if (HEALTHTRADED_NEXT_TITLE_CPTR != nullptr)
        {
            ss.str("");
            ss << ", need  " << (HEALTHTRADED_NEXT_TITLE_CPTR->AchievementCount() - a.AchievementCopy(creature::AchievementType::HealthTraded).Count())
               << " more to achieve \"" << HEALTHTRADED_NEXT_TITLE_CPTR->Name() << "\"";
            healthTradedNextTitleStr = ss.str();
        }

        auto const ROLE{ CREATURE_CPTRC->Role().Which() };

        ss.str("");
        ss << "\n" << a.AchievementCopy(creature::AchievementType::BackstabsHits).Name()     << ":                "      << a.AchievementCopy(creature::AchievementType::BackstabsHits).Count()      << backstabsHitsNextTitleStr;
        const std::string BACKSTABHITS_FINAL_STR((a.AchievementCopy(creature::AchievementType::BackstabsHits).IsRoleInList(ROLE)) ? ss.str() : "");

        ss.str("");
        ss << "\n" << a.AchievementCopy(creature::AchievementType::BattlesSurvived).Name()   << ":             "         << a.AchievementCopy(creature::AchievementType::BattlesSurvived).Count()    << battlesSurvivedNextTitleStr;
        const std::string BATTLESSURVIVED_FINAL_STR((a.AchievementCopy(creature::AchievementType::BattlesSurvived).IsRoleInList(ROLE)) ? ss.str() : "");

        ss.str("");
        ss << "\n" << a.AchievementCopy(creature::AchievementType::BeastMindLinks).Name()    << ":          "            << a.AchievementCopy(creature::AchievementType::BeastMindLinks).Count()     << beastMindLinksNextTitleStr;
        const std::string BEASTMINDLINKS_FINAL_STR((a.AchievementCopy(creature::AchievementType::BeastMindLinks).IsRoleInList(ROLE)) ? ss.str() : "");

        ss.str("");
        ss << "\n" << a.AchievementCopy(creature::AchievementType::BeastRoars).Name()        << ":                   "   << a.AchievementCopy(creature::AchievementType::BeastRoars).Count()         << beastRoarsNextTitleStr;
        const std::string BEASTROARS_FINAL_STR((a.AchievementCopy(creature::AchievementType::BeastRoars).IsRoleInList(ROLE)) ? ss.str() : "");

        ss.str("");
        ss << "\n" << a.AchievementCopy(creature::AchievementType::FlyingAttackHits).Name()  << ":         "             << a.AchievementCopy(creature::AchievementType::FlyingAttackHits).Count()   << flyingAttackHitsNextTitleStr;
        const std::string FLYINGATTACKHITS_FINAL_STR((a.AchievementCopy(creature::AchievementType::FlyingAttackHits).IsRoleInList(ROLE)) ? ss.str() : "");

        ss.str("");
        ss << "\n" << a.AchievementCopy(creature::AchievementType::DodgedFlying).Name()      << ":      "                << a.AchievementCopy(creature::AchievementType::DodgedFlying).Count()       << dodgedFlyingNextTitleStr;
        const std::string DODGEDFLYING_FINAL_STR((a.AchievementCopy(creature::AchievementType::DodgedFlying).IsRoleInList(ROLE)) ? ss.str() : "");

        ss.str("");
        ss << "\n" << a.AchievementCopy(creature::AchievementType::DodgedStanding).Name()    << ":  "                    << a.AchievementCopy(creature::AchievementType::DodgedStanding).Count()     << dodgedStandingNextTitleStr;
        const std::string DODGEDSTANDING_FINAL_STR((a.AchievementCopy(creature::AchievementType::DodgedStanding).IsRoleInList(ROLE)) ? ss.str() : "");

        ss.str("");
        ss << "\n" << a.AchievementCopy(creature::AchievementType::EnemiesFaced).Name()      << ":               "       << a.AchievementCopy(creature::AchievementType::EnemiesFaced).Count()       << enemiesFacedNextTitleStr;
        const std::string ENEMIESFACED_FINAL_STR((a.AchievementCopy(creature::AchievementType::EnemiesFaced).IsRoleInList(ROLE)) ? ss.str() : "");

        ss.str("");
        ss << "\n" << a.AchievementCopy(creature::AchievementType::HealthGiven).Name()       << ":                 "     << a.AchievementCopy(creature::AchievementType::HealthGiven).Count()        << healthGivenNextTitleStr;
        const std::string HEALTHGIVEN_FINAL_STR((a.AchievementCopy(creature::AchievementType::HealthGiven).IsRoleInList(ROLE)) ? ss.str() : "");

        ss.str("");
        ss << "\n" << a.AchievementCopy(creature::AchievementType::HealthTraded).Name()      << ":                "      << a.AchievementCopy(creature::AchievementType::HealthTraded).Count()       << healthTradedNextTitleStr;
        const std::string HEALTHTRADED_FINAL_STR((a.AchievementCopy(creature::AchievementType::HealthTraded).IsRoleInList(ROLE)) ? ss.str() : "");

        ss.str("");
        ss << "\n" << a.AchievementCopy(creature::AchievementType::LocksPicked).Name()       << ":                  "    << a.AchievementCopy(creature::AchievementType::LocksPicked).Count()        << locksPickedNextTitleStr;
        const std::string LOCKSPICKED_FINAL_STR((a.AchievementCopy(creature::AchievementType::LocksPicked).IsRoleInList(ROLE)) ? ss.str() : "");

        ss.str("");
        ss << "\n" << a.AchievementCopy(creature::AchievementType::MeleeHits).Name()         << ":                    "  << a.AchievementCopy(creature::AchievementType::MeleeHits).Count()          << meleeHitsNextTitleStr;
        const std::string MELEEHITS_FINAL_STR((a.AchievementCopy(creature::AchievementType::MeleeHits).IsRoleInList(ROLE)) ? ss.str() : "");

        ss.str("");
        ss << "\n" << a.AchievementCopy(creature::AchievementType::MoonHowls).Name()         << ":                  "    << a.AchievementCopy(creature::AchievementType::MoonHowls).Count()          << moonHowlsNextTitleStr;
        const std::string MOONHOWLS_FINAL_STR((a.AchievementCopy(creature::AchievementType::MoonHowls).IsRoleInList(ROLE)) ? ss.str() : "");

        ss.str("");
        ss << "\n" << a.AchievementCopy(creature::AchievementType::PackActions).Name()       << ":                  "    << a.AchievementCopy(creature::AchievementType::PackActions).Count()        << packActionsNextTitleStr;
        const std::string PACKACTIONS_FINAL_STR((a.AchievementCopy(creature::AchievementType::PackActions).IsRoleInList(ROLE)) ? ss.str() : "");

        ss.str("");
        ss << "\n" << a.AchievementCopy(creature::AchievementType::ProjectileHits).Name()    << ":               "       << a.AchievementCopy(creature::AchievementType::ProjectileHits).Count()     << projectileHitsNextTitleStr;
        const std::string PROJECTILEHITS_FINAL_STR((a.AchievementCopy(creature::AchievementType::ProjectileHits).IsRoleInList(ROLE)) ? ss.str() : "");

        ss.str("");
        ss << "\n" << a.AchievementCopy(creature::AchievementType::SongsPlayed).Name()       << ":                 "     << a.AchievementCopy(creature::AchievementType::SongsPlayed).Count()        << songsPlayedNextTitleStr;
        const std::string SONGSPLAYED_FINAL_STR((a.AchievementCopy(creature::AchievementType::SongsPlayed).IsRoleInList(ROLE)) ? ss.str() : "");

        ss.str("");
        ss << "\n" << a.AchievementCopy(creature::AchievementType::SpellsCast).Name()        << ":                    "  << a.AchievementCopy(creature::AchievementType::SpellsCast).Count()         << spellsCastNextTitleStr;
        const std::string SPELLSCAST_FINAL_STR((a.AchievementCopy(creature::AchievementType::SpellsCast).IsRoleInList(ROLE)) ? ss.str() : "");

        ss.str("");
        ss << "\n" << a.AchievementCopy(creature::AchievementType::SpiritsLifted).Name()     << ":                 "     << a.AchievementCopy(creature::AchievementType::SpiritsLifted).Count()      << spiritsLiftedNextTitleStr;
        const std::string SPIRITSLIFTED_FINAL_STR((a.AchievementCopy(creature::AchievementType::SpiritsLifted).IsRoleInList(ROLE)) ? ss.str() : "");

        ss.str("");
        ss << "\n" << a.AchievementCopy(creature::AchievementType::TurnsInFlight).Name()     << ":              "        << a.AchievementCopy(creature::AchievementType::TurnsInFlight).Count()      << turnsInFlightNextTitleStr;
        const std::string TURNSINFLIGHT_FINAL_STR((a.AchievementCopy(creature::AchievementType::TurnsInFlight).IsRoleInList(ROLE)) ? ss.str() : "");

        ss.str("");
        ss << "\n"
           << ENEMIESFACED_FINAL_STR
           << MELEEHITS_FINAL_STR
           << BATTLESSURVIVED_FINAL_STR
           << PROJECTILEHITS_FINAL_STR
           << BEASTMINDLINKS_FINAL_STR
           << DODGEDSTANDING_FINAL_STR
           << DODGEDFLYING_FINAL_STR
           << LOCKSPICKED_FINAL_STR
           << BACKSTABHITS_FINAL_STR
           << SONGSPLAYED_FINAL_STR
           << SPIRITSLIFTED_FINAL_STR
           << BEASTROARS_FINAL_STR
           << MOONHOWLS_FINAL_STR
           << PACKACTIONS_FINAL_STR
           << FLYINGATTACKHITS_FINAL_STR
           << TURNSINFLIGHT_FINAL_STR
           << SPELLSCAST_FINAL_STR
           << HEALTHGIVEN_FINAL_STR
           << HEALTHTRADED_FINAL_STR;

        const sfml_util::gui::TextInfo TEXT_INFO(ss.str(),
                                                 sfml_util::FontManager::Instance()->Font_Default1(),
                                                 sfml_util::FontManager::Instance()->Size_Smallish(),
                                                 sf::Color::White,
                                                 sfml_util::Justified::Left);

        auto const DETAILVIEW_BOUNDS{ detailViewSprite_.getGlobalBounds() };

        const sf::FloatRect TEXT_RECT(DETAILVIEW_POS_LEFT_ + DETAILVIEW_INNER_PAD_,
                                      DETAILVIEW_BOUNDS.top + DETAILVIEW_BOUNDS.height - 20.0f,
                                      DETAILVIEW_WIDTH_ - (2.0f * DETAILVIEW_INNER_PAD_),
                                      (DETAILVIEW_HEIGHT_ - (DETAILVIEW_BOUNDS.top + DETAILVIEW_BOUNDS.height)) - (2.0f * DETAILVIEW_INNER_PAD_));

        detailViewTextUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "InventoryStage'sDetailViewForCreatureAchievements", TEXT_INFO, TEXT_RECT);
    }


    void InventoryStage::StartDetailViewFadeOutTasks()
    {
        sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::TickOff).PlayRandom();
        isDetailViewFadingIn_ = false;
        isDetailViewDoneFading_ = false;
        isDetailViewFadingOut_ = true;
        detailViewSlider_.Reset(DETAILVIEW_SLIDER_SPEED_);
        SetupDetailViewItem(nullptr);
        SetupDetailViewCreature(nullptr);
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

}
}
