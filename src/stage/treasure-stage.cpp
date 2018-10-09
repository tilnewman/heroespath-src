// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// treasure-stage.cpp
//
#include "treasure-stage.hpp"

#include "combat/combat-text.hpp"
#include "combat/encounter.hpp"
#include "combat/trap-holder.hpp"
#include "creature/algorithms.hpp"
#include "creature/creature.hpp"
#include "creature/player-party.hpp"
#include "creature/stats.hpp"
#include "creature/title.hpp"
#include "game/game-state.hpp"
#include "game/game.hpp"
#include "game/loop-manager.hpp"
#include "item/item-profile-warehouse.hpp"
#include "item/item-warehouse.hpp"
#include "item/item.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/config-file.hpp"
#include "misc/random.hpp"
#include "misc/real.hpp"
#include "popup/popup-manager.hpp"
#include "popup/popup-stage-char-select.hpp"
#include "popup/popup-stage-combat-over.hpp"
#include "popup/popup-stage-image-fade.hpp"
#include "popup/popup-stage-treasure-trap.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/list-element.hpp"
#include "sfml-util/text-region.hpp"
#include "sfml-util/title-image-loader.hpp"
#include "stage/treasure-display-stage.hpp"

#include <algorithm>
#include <exception>
#include <set>
#include <sstream>
#include <vector>

namespace heroespath
{
namespace stage
{

    // all of these popup names must be unique
    const std::string TreasureStage::POPUP_NAME_NO_TREASURE_ { "PopupName_AllEnemiesRan" };

    const std::string TreasureStage::POPUP_NAME_WORN_ONLY_ { "PopupName_HeldOnly" };

    const std::string TreasureStage::POPUP_NAME_LOCKBOX_ONLY_ { "PopupName_LockboxOnly" };

    const std::string TreasureStage::POPUP_NAME_LOCKBOX_AND_HELD_ { "PopupName_LockboxAndWorn" };

    const std::string TreasureStage::POPUP_NAME_LOCK_PICK_FAILURE_ { "PopupName_LockPickFailure" };

    const std::string TreasureStage::POPUP_NAME_DAMAGE_REPORT_ { "PopupName_DamageReport" };

    const std::string TreasureStage::POPUP_NAME_ALL_CHARACTERS_DIED_ {
        "PopupName_AllCharactersDied"
    };

    const std::string TreasureStage::POPUP_NAME_COIN_SHARE_ { "PopupName_CoinShare" };

    const std::string TreasureStage::POPUP_NAME_GEM_SHARE_ { "PopupName_GemShare" };

    const std::string TreasureStage::POPUP_NAME_ITEM_TAKE_REJECTION_ {
        "PopupName_ItemTakeRejection"
    };

    const std::string TreasureStage::POPUP_NAME_ALL_ITEMS_TAKEN_ { "PopupName_AllItemsTaken" };

    const std::string TreasureStage::POPUP_NAME_NOT_ALL_ITEMS_TAKEN_ {
        "PopupName_NotAllItemsTaken"
    };

    TreasureStage::TreasureStage()
        : Stage(
              "Treasure",
              { sfml_util::GuiFont::Default,
                sfml_util::GuiFont::System,
                sfml_util::GuiFont::SystemCondensed,
                sfml_util::GuiFont::Number,
                sfml_util::GuiFont::DefaultBoldFlavor,
                sfml_util::GuiFont::Handwriting },
              true)
        , displayStagePtr_(new TreasureDisplayStage(this))
        , treasureImageType_(item::TreasureImage::Count)
        , itemCacheHeld_()
        , itemCacheLockbox_()
        , treasureAvailable_(item::TreasureAvailable::Count)
        , trap_()
        , fightResult_()
        , creatureEffectIndex_(0)
        , updateItemDisplayNeeded_(false)
        , willProcessLockpickTitle_(false)
        , lockPicking_()
        , creatureInteraction_()
    {}

    TreasureStage::~TreasureStage()
    {
        item::ItemWarehouse::Access().Free(itemCacheHeld_.items_pvec);
        item::ItemWarehouse::Access().Free(itemCacheLockbox_.items_pvec);
        Stage::ClearAllEntities();
    }

    bool TreasureStage::HandleCallback(const sfml_util::PopupCallback_t::PacketPtr_t & PACKET_PTR)
    {
        if (PACKET_PTR->Name() == POPUP_NAME_NO_TREASURE_)
        {
            TransitionToAdventureStage();
            return false;
        }

        if (PACKET_PTR->Name() == POPUP_NAME_WORN_ONLY_)
        {
            SetupForCollection();
            return true;
        }

        if ((PACKET_PTR->Name() == POPUP_NAME_LOCKBOX_ONLY_)
            || (PACKET_PTR->Name() == POPUP_NAME_LOCKBOX_AND_HELD_))
        {
            if (PACKET_PTR->Response() == popup::ResponseTypes::Yes)
            {
                lockPicking_.PopupCharacterSelection(this);
                return false;
            }
            else
            {
                SetupForCollectionWithoutLockbox();
                return true;
            }
        }

        if (PACKET_PTR->Name() == lockPicking_.POPUP_NAME_CHARACTER_SELECTION_)
        {
            if (PACKET_PTR->Response() == popup::ResponseTypes::Select)
            {
                if (lockPicking_.HandleCharacterSelectionPopupResponse(this, PACKET_PTR))
                {
                    return false;
                }
                else
                {
                    SetupForCollectionWithoutLockbox();
                    return true;
                }
            }
            else
            {
                if (item::TreasureAvailable::LockboxOnly == treasureAvailable_)
                {
                    // Since the player is choosing to skip the lockbox and there is no
                    // held treasure, then simply return to the Adventure Stage.
                    game::LoopManager::Instance()->TransitionTo_Adventure();
                    return false;
                }
                else if (item::TreasureAvailable::HeldAndLockbox == treasureAvailable_)
                {
                    // Since the player is choosing to skip the lockbox,
                    // then prevent from searching the lockbox.
                    SetupForCollectionWithoutLockbox();
                    return true;
                }
            }
        }

        if (PACKET_PTR->Name() == lockPicking_.POPUP_NAME_NO_CHARACTER_CAN_PICK_)
        {
            SetupForCollectionWithoutLockbox();
            return true;
        }

        if (PACKET_PTR->Name() == lockPicking_.POPUP_NAME_ATTEMPTING_)
        {
            if (lockPicking_.Attempt())
            {
                LockPickSuccess();
            }
            else
            {
                LockPickFailure();
            }

            return false;
        }

        if ((PACKET_PTR->Name() == POPUP_NAME_LOCK_PICK_FAILURE_)
            || (PACKET_PTR->Name() == POPUP_NAME_DAMAGE_REPORT_))
        {
            if (PromptPlayerWithDamagePopups() == DamagePopup::Displayed)
            {
                return false;
            }
            else
            {
                if (CheckAndHandleAllKilledByTrap() == false)
                {
                    LockboxOpen();
                }

                return false;
            }
        }

        if (PACKET_PTR->Name() == lockPicking_.POPUP_NAME_SUCCESS_)
        {
            displayStagePtr_->UpdateTreasureImage(treasureImageType_);

            if (ShareAndShowPopupIfNeeded(ShareType::Coins))
            {
                return false;
            }
            else
            {
                if (ShareAndShowPopupIfNeeded(ShareType::Gems))
                {
                    return false;
                }
                else
                {
                    if (ProcessLockpickTitleAndPopupIfNeeded())
                    {
                        return false;
                    }
                    else
                    {
                        SetupForCollection();
                        return true;
                    }
                }
            }
        }

        if (PACKET_PTR->Name() == POPUP_NAME_COIN_SHARE_)
        {
            if (ShareAndShowPopupIfNeeded(ShareType::Gems))
            {
                return false;
            }
            else
            {
                if (ProcessLockpickTitleAndPopupIfNeeded())
                {
                    return false;
                }
                else
                {
                    SetupForCollection();
                    return true;
                }
            }
        }

        if (PACKET_PTR->Name() == POPUP_NAME_GEM_SHARE_)
        {
            if (ProcessLockpickTitleAndPopupIfNeeded())
            {
                return false;
            }
            else
            {
                SetupForCollection();
                return true;
            }
        }

        if (PACKET_PTR->Name() == lockPicking_.POPUP_NAME_TITLE_ARCHIEVED_)
        {
            SetupForCollection();
            return true;
        }

        if (PACKET_PTR->Name() == POPUP_NAME_ITEM_TAKE_REJECTION_)
        {
            displayStagePtr_->CanDisplayItemDetail(true);
            return true;
        }

        return true;
    }

    void TreasureStage::Setup()
    {
        displayStagePtr_->Setup();

        // give control of the TreasureDispayStage object lifetime to the Loop class
        game::LoopManager::Instance()->AddStage(displayStagePtr_);

        // TODO TEMP REMOVE -once finished testing
        // create a fake collection of dead creatures, using the predetermined initial encounter
        combat::Encounter::Instance()->BeginCombatTasks();
        //
        const auto NONPLAYER_CREATURE_PVEC { creature::Algorithms::NonPlayers() };
        for (const auto & NEXT_CREATURE_PTR : NONPLAYER_CREATURE_PVEC)
        {
            combat::Encounter::Instance()->HandleKilledCreature(NEXT_CREATURE_PTR);
        }
        //
        combat::Encounter::Instance()->EndCombatTasks();

        treasureImageType_ = combat::Encounter::Instance()->BeginTreasureStageTasks();
        itemCacheHeld_ = combat::Encounter::Instance()->TakeDeadNonPlayerItemsHeldCache();
        itemCacheLockbox_ = combat::Encounter::Instance()->TakeDeadNonPlayerItemsLockboxCache();

        displayStagePtr_->SetupAfterPleaseWait(treasureImageType_);
        treasureAvailable_ = DetermineTreasureAvailableState(itemCacheHeld_, itemCacheLockbox_);
        PromptUserBasedOnTreasureAvailability(treasureAvailable_, treasureImageType_);
    }

    void TreasureStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        Stage::Draw(target, STATES);

        if (updateItemDisplayNeeded_)
        {
            updateItemDisplayNeeded_ = false;
            UpdateItemDisplay();
        }
    }

    bool TreasureStage::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if (displayStagePtr_->IsItemDetailMovingOrShowing())
        {
            return false;
        }

        if (KEY_EVENT.code == sf::Keyboard::Space)
        {
            return HandleKeypress_Space();
        }

        if ((KEY_EVENT.code >= sf::Keyboard::Num1) && (KEY_EVENT.code <= sf::Keyboard::Num6))
        {
            return HandleKeypress_Number(KEY_EVENT.code);
        }

        if ((KEY_EVENT.code == sf::Keyboard::Left) || (KEY_EVENT.code == sf::Keyboard::Right))
        {
            return HandleKeypress_LeftRight(KEY_EVENT.code);
        }

        if (KEY_EVENT.code == sf::Keyboard::T)
        {
            TakeAllItems();
        }

        if (KEY_EVENT.code == sf::Keyboard::E)
        {
            Exit();
        }

        return Stage::KeyRelease(KEY_EVENT);
    }

    bool TreasureStage::HandleListboxCallback(
        const ItemListBoxPtr_t & TREASURE_LISTBOX_PTR,
        const ItemListBoxPtr_t & INVENTORY_LISTBOX_PTR,
        const ItemListBox_t::Callback_t::PacketPtr_t & PACKET_PTR)
    {
        if ((PACKET_PTR->gui_event == sfml_util::GuiEvent::DoubleClick)
            || (PACKET_PTR->keypress_event.code == sf::Keyboard::Return))
        {
            const auto ITEM_PTR { PACKET_PTR->selected_element_ptr->Element() };

            if (PACKET_PTR->listbox_ptr == TREASURE_LISTBOX_PTR.Ptr())
            {
                TakeItem(ITEM_PTR);
            }
            else if (PACKET_PTR->listbox_ptr == INVENTORY_LISTBOX_PTR.Ptr())
            {
                PutItemBack(ITEM_PTR);
            }

            return true;
        }

        return true;
    }

    void TreasureStage::TakeAllItems()
    {
        item::ItemPVec_t & itemsPVec { (
            (displayStagePtr_->IsShowingHeldItems()) ? itemCacheHeld_.items_pvec
                                                     : itemCacheLockbox_.items_pvec) };

        if (itemsPVec.empty())
        {
            sfml_util::SoundManager::Instance()->PlaySfx_Reject();
            return;
        }

        std::sort(itemsPVec.begin(), itemsPVec.end(), [](auto A_PTR, auto B_PTR) {
            return A_PTR->Weight() > B_PTR->Weight();
        });

        creature::CreaturePVec_t whoWillTakeItems
            = { game::Game::Instance()->State().Party().Characters().at(
                displayStagePtr_->CharacterIndexShowingInventory()) };

        for (const auto & CHARACTER_PTR : game::Game::Instance()->State().Party().Characters())
        {
            if ((CHARACTER_PTR->IsBeast() == false) && (CHARACTER_PTR != whoWillTakeItems.front()))
            {
                whoWillTakeItems.emplace_back(CHARACTER_PTR);
            }
        }

        // for each item to be taken, attempt to give it to each possible character
        std::set<std::string> creatureNamesWhoTookItems;
        item::ItemPVec_t itemsToRemovePVec;
        for (const auto & ITEM_PTR : itemsPVec)
        {
            for (auto creaturePtr : whoWillTakeItems)
            {
                if (creaturePtr->ItemIsAddAllowed(ITEM_PTR).empty())
                {
                    creaturePtr->ItemAdd(ITEM_PTR);
                    itemsToRemovePVec.emplace_back(ITEM_PTR);
                    break;
                }
            }
        }

        // remove the taken items from the cache
        for (const auto & ITEM_PTR : itemsToRemovePVec)
        {
            itemsPVec.erase(
                std::remove(itemsPVec.begin(), itemsPVec.end(), ITEM_PTR), itemsPVec.end());
        }

        UpdateItemDisplay();

        // display a popup that reports how many items were taken
        const auto NUM_ITEMS_REMAINING { itemsPVec.size() };
        if (NUM_ITEMS_REMAINING == 0)
        {
            sfml_util::SoundManager::Instance()->PlaySfx_AckMajor();

            std::ostringstream ss;
            ss << "\n"
               << "All " << itemsToRemovePVec.size() << " items were taken.";

            const auto POPUP_INFO { popup::PopupManager::Instance()->CreatePopupInfo(
                POPUP_NAME_ALL_ITEMS_TAKEN_,
                ss.str(),
                popup::PopupButtons::Okay,
                popup::PopupImage::Regular) };

            game::LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
        }
        else if (itemsToRemovePVec.empty())
        {
            sfml_util::SoundManager::Instance()->PlaySfx_Reject();

            const auto POPUP_INFO { popup::PopupManager::Instance()->CreatePopupInfo(
                POPUP_NAME_NOT_ALL_ITEMS_TAKEN_,
                "None of the items could be taken by any of your characters.",
                popup::PopupButtons::Okay,
                popup::PopupImage::Regular) };

            game::LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
        }
        else
        {
            sfml_util::SoundManager::Instance()->PlaySfx_Reject();

            std::ostringstream ss;
            ss << "\n"
               << NUM_ITEMS_REMAINING << " items could not be taken by any of your characters.";

            const auto POPUP_INFO { popup::PopupManager::Instance()->CreatePopupInfo(
                POPUP_NAME_NOT_ALL_ITEMS_TAKEN_,
                ss.str(),
                popup::PopupButtons::Okay,
                popup::PopupImage::Regular) };

            game::LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
        }
    }

    void TreasureStage::Exit() { TransitionToAdventureStage(); }

    item::TreasureAvailable::Enum TreasureStage::DetermineTreasureAvailableState(
        const item::ItemCache & CACHE_HELD, const item::ItemCache & CACHE_LOCKBOX)
    {
        if (CACHE_HELD.Empty() && CACHE_LOCKBOX.Empty())
        {
            return item::TreasureAvailable::NoTreasure;
        }
        else if ((CACHE_HELD.Empty() == false) && CACHE_LOCKBOX.Empty())
        {
            return item::TreasureAvailable::HeldOnly;
        }
        else if (CACHE_HELD.Empty() && (CACHE_LOCKBOX.Empty() == false))
        {
            return item::TreasureAvailable::LockboxOnly;
        }
        else
        {
            return item::TreasureAvailable::HeldAndLockbox;
        }
    }

    void TreasureStage::PromptUserBasedOnTreasureAvailability(
        const item::TreasureAvailable::Enum TREASURE_AVAILABLE,
        const item::TreasureImage::Enum TREASURE_IMAGE)
    {
        switch (TREASURE_AVAILABLE)
        {
            case item::TreasureAvailable::NoTreasure:
            {
                std::ostringstream ss;
                ss << "\nYour enemies had no possessions on them and they carried no lockbox.  "
                   << "Click Continue to return to the Adventure screen.";

                const auto POPUP_INFO { popup::PopupManager::Instance()->CreatePopupInfo(
                    POPUP_NAME_NO_TREASURE_,
                    ss.str(),
                    popup::PopupButtons::Continue,
                    popup::PopupImage::Regular) };

                game::LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
                break;
            }
            case item::TreasureAvailable::HeldOnly:
            {
                std::ostringstream ss;
                ss << "\nYour enemies were wearing possessions but they carried no lockbox.  "
                   << "Click Continue to search what they left behind.";

                const auto POPUP_INFO { popup::PopupManager::Instance()->CreatePopupInfo(
                    POPUP_NAME_WORN_ONLY_,
                    ss.str(),
                    popup::PopupButtons::Continue,
                    popup::PopupImage::Regular) };

                game::LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
                break;
            }
            case item::TreasureAvailable::LockboxOnly:
            case item::TreasureAvailable::HeldAndLockbox:
            {
                std::ostringstream ss;
                ss << "\nYour enemies carried a "
                   << item::TreasureImage::ToContainerName(TREASURE_IMAGE)
                   << ".  Attempt to pick the lock?";

                const auto POPUP_INFO { popup::PopupManager::Instance()->CreatePopupInfo(
                    ((TREASURE_AVAILABLE == item::TreasureAvailable::LockboxOnly)
                         ? POPUP_NAME_LOCKBOX_ONLY_
                         : POPUP_NAME_LOCKBOX_AND_HELD_),
                    ss.str(),
                    popup::PopupButtons::YesNo,
                    popup::PopupImage::Regular) };

                game::LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
                break;
            }
            case item::TreasureAvailable::Count:
            default:
            {
                std::ostringstream ss;
                ss << "stage::TreasureStage::PromptUserBasedOnTreasureAvailability("
                   << treasureAvailable_ << ")_InvalidValueError.";

                throw std::range_error(ss.str());
            }
        }
    }

    void TreasureStage::SetupForCollectionWithoutLockbox()
    {
        treasureAvailable_ = item::TreasureAvailable::HeldOnly;
        SetupForCollection();
    }

    bool TreasureStage::DetermineIfLockPickingSucceeded(
        const creature::CreaturePtr_t CHAR_WHO_IS_PICKING_THE_LOCK_PTR) const
    {
        return creature::Stats::Test(
            CHAR_WHO_IS_PICKING_THE_LOCK_PTR,
            creature::Traits::Luck,
            static_cast<creature::Stats::With>(
                creature::Stats::With::RaceRoleBonus | creature::Stats::With::StandardBonus));
    }

    sfml_util::sound_effect::Enum TreasureStage::SelectRandomTreasureOpeningSfx() const
    {
        std::vector<sfml_util::sound_effect::Enum> treasureOpeningSfx
            = { sfml_util::sound_effect::TreasureOpen1,
                sfml_util::sound_effect::TreasureOpen2,
                sfml_util::sound_effect::TreasureOpen3,
                sfml_util::sound_effect::TreasureOpen4,
                sfml_util::sound_effect::TreasureOpen5 };

        return misc::Vector::SelectRandom(treasureOpeningSfx);
    }

    void TreasureStage::LockPickSuccess()
    {
        willProcessLockpickTitle_ = true;
        LockboxOpen();
    }

    void TreasureStage::LockPickFailure()
    {
        trap_ = combat::trap::Holder::SelectRandomWithSeverityRatioNear(
            combat::Encounter::Instance()->DefeatedPartyTreasureRatioPer());

        const auto LOCK_PICKING_CREATURE_PTR_OPT {
            combat::Encounter::Instance()->LockPickCreaturePtrOpt()
        };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (!!LOCK_PICKING_CREATURE_PTR_OPT),
            "stage::TreasureStage::LockPickFailure() called when "
            "combat::Encounter::Instance()->LockPickCreaturePtrOpt() returned uninitialized.");

        fightResult_
            = creatureInteraction_.TreasureTrap(trap_, LOCK_PICKING_CREATURE_PTR_OPT.value());

        const auto POPUP_INFO { popup::PopupManager::Instance()->CreateTrapPopupInfo(
            POPUP_NAME_LOCK_PICK_FAILURE_,
            trap_.Description(item::TreasureImage::ToContainerName(treasureImageType_)),
            trap_.SoundEffect()) };

        game::LoopManager::Instance()->PopupWaitBeginSpecific<popup::PopupStageTreasureTrap>(
            this, POPUP_INFO);
    }

    TreasureStage::DamagePopup TreasureStage::PromptPlayerWithDamagePopups()
    {
        if (creatureEffectIndex_ < fightResult_.Effects().size())
        {
            bool ignored { false };

            const auto LOCK_PICKING_CREATURE_PTR_OPT {
                combat::Encounter::Instance()->LockPickCreaturePtrOpt()
            };

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (!!LOCK_PICKING_CREATURE_PTR_OPT),
                "stage::TreasureStage::PromptPlayerWithDamagePopups() called when "
                "combat::Encounter::Instance()->LockPickCreaturePtrOpt() returned uninitialized.");

            combat::Text combatText;

            const auto DAMAGE_TEXT { combatText.ActionTextIndexed(
                LOCK_PICKING_CREATURE_PTR_OPT.value(),
                combat::TurnActionInfo(combat::TurnAction::TreasureUnlock),
                fightResult_,
                false,
                creatureEffectIndex_,
                0,
                ignored) };

            const auto POPUP_INFO { popup::PopupManager::Instance()->CreatePopupInfo(
                POPUP_NAME_DAMAGE_REPORT_,
                "\n" + DAMAGE_TEXT,
                popup::PopupButtons::Continue,
                popup::PopupImage::Regular) };

            game::LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);

            ++creatureEffectIndex_;
            return DamagePopup::Displayed;
        }
        else
        {
            return DamagePopup::AllFinished;
        }
    }

    bool TreasureStage::CheckAndHandleAllKilledByTrap()
    {
        const auto ALL_LIVING_PVEC { creature::Algorithms::PlayersByType(
            creature::Algorithms::TypeOpt::Player, creature::Algorithms::PlayerOpt::Living) };

        if (ALL_LIVING_PVEC.empty())
        {
            game::LoopManager::Instance()->PopupWaitBeginSpecific<popup::PopupStageCombatOver>(
                this,
                popup::PopupManager::Instance()->CreateCombatOverPopupInfo(
                    POPUP_NAME_ALL_CHARACTERS_DIED_, combat::CombatEnd::Lose));

            return true;
        }
        else
        {
            return false;
        }
    }

    void TreasureStage::LockboxOpen()
    {
        if (item::TreasureImage::ChestClosed == treasureImageType_)
        {
            treasureImageType_ = item::TreasureImage::ChestOpen;
        }
        else if (item::TreasureImage::LockboxClosed == treasureImageType_)
        {
            treasureImageType_ = item::TreasureImage::LockboxOpen;
        }

        sfml_util::SoundManager::Instance()->SoundEffectPlay(
            SelectRandomTreasureOpeningSfx(), 0.5f);

        lockPicking_.PopupSuccess(this, item::TreasureImage::ToContainerName(treasureImageType_));
    }

    bool TreasureStage::ShareAndShowPopupIfNeeded(const ShareType WHAT_IS_SHARED)
    {
        const auto TOTAL_SHARED { Share(WHAT_IS_SHARED) };
        if (TOTAL_SHARED == 0)
        {
            return false;
        }
        else
        {
            std::ostringstream ss;
            ss << "\nThe party finds and shares " << TOTAL_SHARED << " "
               << ((WHAT_IS_SHARED == ShareType::Coins) ? "coins" : "gems") << ".";

            const auto POPUP_NAME { (
                (WHAT_IS_SHARED == ShareType::Coins) ? POPUP_NAME_COIN_SHARE_
                                                     : POPUP_NAME_GEM_SHARE_) };

            const auto SOUND_EFFECT_SET { (
                (WHAT_IS_SHARED == ShareType::Coins) ? sfml_util::sound_effect_set::Coin
                                                     : sfml_util::sound_effect_set::Gem) };

            const auto SOUND_EFFECT { sfml_util::SoundManager::Instance()
                                          ->GetSoundEffectSet(SOUND_EFFECT_SET)
                                          .SelectRandom() };

            const auto POPUP_INFO { popup::PopupManager::Instance()->CreatePopupInfo(
                POPUP_NAME,
                ss.str(),
                popup::PopupButtons::Continue,
                popup::PopupImage::Regular,
                sfml_util::Justified::Center,
                SOUND_EFFECT,
                sfml_util::FontManager::Instance()->Size_Normal()) };

            game::LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
            return true;
        }
    }

    int TreasureStage::Share(const ShareType WHAT_IS_SHARED)
    {
        const int TOTAL { (
            (WHAT_IS_SHARED == ShareType::Coins)
                ? (itemCacheHeld_.coins.As<int>() + itemCacheLockbox_.coins.As<int>())
                : (itemCacheHeld_.gems.As<int>() + itemCacheLockbox_.gems.As<int>())) };

        if (TOTAL <= 0)
        {
            return 0;
        }

        const auto HUMANOID_COUNT { static_cast<int>(
            game::Game::Instance()->State().Party().GetNumHumanoid()) };

        const auto SHARED_AMOUNT { TOTAL / HUMANOID_COUNT };
        const auto LEFTOVER_AMOUNT { TOTAL % HUMANOID_COUNT };

        for (auto nextCreaturePtr : game::Game::Instance()->State().Party().Characters())
        {
            if (nextCreaturePtr->Body().IsHumanoid())
            {
                if (WHAT_IS_SHARED == ShareType::Coins)
                {
                    nextCreaturePtr->CoinsAdj(nextCreaturePtr->Inventory().Coins() * Coin_t(-1));
                    nextCreaturePtr->CoinsAdj(Coin_t(SHARED_AMOUNT));
                }
                else
                {
                    nextCreaturePtr->GemsAdj(nextCreaturePtr->Inventory().Gems() * Gem_t(-1));
                    nextCreaturePtr->GemsAdj(Gem_t(SHARED_AMOUNT));
                }
            }
        }

        auto toHandOut { LEFTOVER_AMOUNT };
        for (auto nextCreaturePtr : game::Game::Instance()->State().Party().Characters())
        {
            if (nextCreaturePtr->Body().IsHumanoid() && (toHandOut-- > 0))
            {
                if (WHAT_IS_SHARED == ShareType::Coins)
                {
                    nextCreaturePtr->CoinsAdj(1_coin);
                }
                else
                {
                    nextCreaturePtr->GemsAdj(1_gem);
                }
            }
        }

        return TOTAL;
    }

    void TreasureStage::SetupForCollection()
    {
        displayStagePtr_->UpdateItemCaches(itemCacheHeld_, itemCacheLockbox_);
        displayStagePtr_->SetupForCollection(treasureAvailable_, treasureImageType_);
    }

    bool TreasureStage::HandleKeypress_Space()
    {
        if (displayStagePtr_->CanTreasureChange())
        {
            displayStagePtr_->TreasureChange();
            PlaySoundEffect_KeypressValid();
            return true;
        }
        else
        {
            PlaySoundEffect_KeypressInvalid();
            return false;
        }
    }

    bool TreasureStage::HandleKeypress_Number(const sf::Keyboard::Key KEY)
    {
        PlaySoundEffect_KeypressValid();

        auto characterIndex { [KEY]() -> std::size_t {
            if (KEY == sf::Keyboard::Num1)
                return 0;
            else if (KEY == sf::Keyboard::Num2)
                return 1;
            else if (KEY == sf::Keyboard::Num3)
                return 2;
            else if (KEY == sf::Keyboard::Num4)
                return 3;
            else if (KEY == sf::Keyboard::Num5)
                return 4;
            else
                return 5;
        }() };

        const auto CHARACTER_INDEX_MAX { displayStagePtr_->CharacterIndexMax() };
        if (characterIndex > CHARACTER_INDEX_MAX)
        {
            characterIndex = CHARACTER_INDEX_MAX;
        }

        displayStagePtr_->InventoryChange(characterIndex);
        return true;
    }

    bool TreasureStage::HandleKeypress_LeftRight(const sf::Keyboard::Key KEY)
    {
        if (KEY == sf::Keyboard::Left)
        {
            PlaySoundEffect_KeypressValid();

            auto characterIndex { displayStagePtr_->CharacterIndex() };

            if (0 == characterIndex)
            {
                characterIndex = displayStagePtr_->CharacterIndexMax();
            }
            else
            {
                --characterIndex;
            }

            displayStagePtr_->InventoryChange(characterIndex);

            return true;
        }

        if (KEY == sf::Keyboard::Right)
        {
            PlaySoundEffect_KeypressValid();

            auto characterIndex { displayStagePtr_->CharacterIndex() };

            if (displayStagePtr_->CharacterIndexMax() == characterIndex)
            {
                characterIndex = 0;
            }
            else
            {
                ++characterIndex;
            }

            displayStagePtr_->InventoryChange(characterIndex);

            return true;
        }

        return false;
    }

    void TreasureStage::PlaySoundEffect_KeypressValid() const
    {
        sfml_util::SoundManager::Instance()->PlaySfx_Keypress();
    }

    void TreasureStage::PlaySoundEffect_KeypressInvalid() const
    {
        sfml_util::SoundManager::Instance()->PlaySfx_Reject();
    }

    void TreasureStage::TakeItem(const item::ItemPtr_t ITEM_PTR)
    {
        auto creaturePtr { displayStagePtr_->WhichCharacterInventoryIsDisplayed() };
        const auto IS_ITEM_ADD_ALLOWED_STR { creaturePtr->ItemIsAddAllowed(ITEM_PTR) };

        if (IS_ITEM_ADD_ALLOWED_STR.empty())
        {
            sfml_util::SoundManager::Instance()->PlaySfx_AckMajor();

            creaturePtr->ItemAdd(ITEM_PTR);

            if (displayStagePtr_->IsShowingHeldItems())
            {
                itemCacheHeld_.items_pvec.erase(
                    std::remove(
                        itemCacheHeld_.items_pvec.begin(),
                        itemCacheHeld_.items_pvec.end(),
                        ITEM_PTR),
                    itemCacheHeld_.items_pvec.end());
            }
            else
            {
                itemCacheLockbox_.items_pvec.erase(
                    std::remove(
                        itemCacheLockbox_.items_pvec.begin(),
                        itemCacheLockbox_.items_pvec.end(),
                        ITEM_PTR),
                    itemCacheLockbox_.items_pvec.end());
            }

            // Can't update display now because it would invalidate the this pointer.
            updateItemDisplayNeeded_ = true;
        }
        else
        {
            displayStagePtr_->CanDisplayItemDetail(false);

            std::ostringstream ss;
            ss << creaturePtr->Name() << " can't take the " << ITEM_PTR->Name()
               << " because:  " << IS_ITEM_ADD_ALLOWED_STR;

            const auto POPUP_INFO { popup::PopupManager::Instance()->CreatePopupInfo(
                POPUP_NAME_ITEM_TAKE_REJECTION_,
                ss.str(),
                popup::PopupButtons::Cancel,
                popup::PopupImage::Regular,
                sfml_util::Justified::Center,
                sfml_util::sound_effect::PromptWarn,
                sfml_util::FontManager::Instance()->Size_Largeish()) };

            game::LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
        }
    }

    void TreasureStage::PutItemBack(const item::ItemPtr_t ITEM_PTR)
    {
        sfml_util::SoundManager::Instance()
            ->GetSoundEffectSet(sfml_util::sound_effect_set::ItemDrop)
            .PlayRandom();

        displayStagePtr_->WhichCharacterInventoryIsDisplayed()->ItemRemove(ITEM_PTR);

        if (displayStagePtr_->IsShowingHeldItems())
        {
            itemCacheHeld_.items_pvec.emplace_back(ITEM_PTR);
        }
        else
        {
            itemCacheLockbox_.items_pvec.emplace_back(ITEM_PTR);
        }

        // Can't update display now because it would invalidate the this pointer.
        updateItemDisplayNeeded_ = true;
    }

    void TreasureStage::UpdateItemDisplay()
    {
        displayStagePtr_->UpdateItemCaches(itemCacheHeld_, itemCacheLockbox_);
        displayStagePtr_->RefreshAfterCacheUpdate();
    }

    bool TreasureStage::ProcessLockpickTitleAndPopupIfNeeded()
    {
        if (willProcessLockpickTitle_)
        {
            willProcessLockpickTitle_ = false;
            return lockPicking_.HandleAchievementIncrementAndReturnTrueOnNewTitleWithPopup(this);
        }
        else
        {
            return false;
        }
    }

    void TreasureStage::TransitionToAdventureStage()
    {
        combat::Encounter::Instance()->EndTreasureStageTasks(itemCacheHeld_, itemCacheLockbox_);
        itemCacheHeld_.Reset();
        itemCacheLockbox_.Reset();
        game::LoopManager::Instance()->TransitionTo_Adventure();
    }

} // namespace stage
} // namespace heroespath
