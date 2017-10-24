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
// treasure-stage.cpp
//
#include "treasure-stage.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/gui/text-region.hpp"
#include "sfml-util/gui/list-box-item.hpp"
#include "sfml-util/gui/title-image-manager.hpp"

#include "popup/popup-manager.hpp"
#include "popup/popup-stage-char-select.hpp"
#include "popup/popup-stage-treasure-trap.hpp"
#include "popup/popup-stage-item-profile-wait.hpp"
#include "popup/popup-stage-combat-over.hpp"
#include "popup/popup-stage-image-fade.hpp"

#include "game/game.hpp"
#include "game/loop-manager.hpp"
#include "game/game-data-file.hpp"
#include "game/combat/encounter.hpp"
#include "game/combat/fight.hpp"
#include "game/combat/combat-text.hpp"
#include "game/non-player/party.hpp"
#include "game/non-player/character.hpp"
#include "game/player/character.hpp"
#include "game/creature/algorithms.hpp"
#include "game/creature/stats.hpp"
#include "game/creature/title.hpp"
#include "game/item/item.hpp"
#include "game/item/item-profile-warehouse.hpp"
#include "game/trap-warehouse.hpp"
#include "game/stage/treasure-display-stage.hpp"

//TODO TEMP REMOVE -once done testing
#include "game/player/party.hpp"
#include "game/player/fake-party.hpp"
#include "game/state/game-state.hpp"
#include "game/state/game-state-factory.hpp"

#include "misc/real.hpp"
#include "misc/random.hpp"
#include "misc/assertlogandthrow.hpp"

#include <set>
#include <string>
#include <vector>
#include <sstream>
#include <exception>
#include <algorithm>


namespace game
{
namespace stage
{

    //all of these popup names must be unique
    const std::string TreasureStage::POPUP_NAME_ITEMPROFILE_PLEASEWAIT_{
        "PopupName_ItemProfilePleaseWait" };

    const std::string TreasureStage::POPUP_NAME_NO_TREASURE_{
        "PopupName_AllEnemiesRan" };

    const std::string TreasureStage::POPUP_NAME_WORN_ONLY_{
        "PopupName_HeldOnly" };

    const std::string TreasureStage::POPUP_NAME_LOCKBOX_ONLY_{
        "PopupName_LockboxOnly" };

    const std::string TreasureStage::POPUP_NAME_LOCKBOX_AND_HELD_{
        "PopupName_LockboxAndWorn" };

    const std::string TreasureStage::POPUP_NAME_CHAR_SELECT_{
        "PopupName_CharacterSelect" };

    const std::string TreasureStage::POPUP_NAME_NO_CHARS_CAN_PICK_THE_LOCK_{
        "PopupName_NoCharactersCanAttemptToPickTheLock" };

    const std::string TreasureStage::POPUP_NAME_LOCK_PICK_ATTEMPT_{
        "PopupName_LockPickAttempt" };

    const std::string TreasureStage::POPUP_NAME_LOCK_PICK_FAILURE_{
        "PopupName_LockPickFailure" };

    const std::string TreasureStage::POPUP_NAME_DAMAGE_REPORT_{
        "PopupName_DamageReport" };

    const std::string TreasureStage::POPUP_NAME_LOCKBOX_OPEN_{
        "PopupName_LockboxOpen" };

    const std::string TreasureStage::POPUP_NAME_ALL_CHARACTERS_DIED_{
        "PopupName_AllCharactersDied" };

    const std::string TreasureStage::POPUP_NAME_COIN_SHARE_{
        "PopupName_CoinShare" };

    const std::string TreasureStage::POPUP_NAME_GEM_SHARE_{
        "PopupName_GemShare" };

    const std::string TreasureStage::POPUP_NAME_ITEM_TAKE_REJECTION_{
        "PopupName_ItemTakeRejection" };

    const std::string TreasureStage::POPUP_NAME_TITLE_ACHIEVEMENT_{
        "PopupName_TitleAchievement" };

    const std::string TreasureStage::POPUP_NAME_ALL_ITEMS_TAKEN_{
        "PopupName_AllItemsTaken" };

    const std::string TreasureStage::POPUP_NAME_NOT_ALL_ITEMS_TAKEN_{
        "PopupName_NotAllItemsTaken" };


    TreasureStage::TreasureStage()
    :
        Stage               ("Treasure"),
        displayStagePtr_    (nullptr),
        setupCountdown_     (0),
        treasureImageType_  (item::TreasureImage::Count),
        itemCacheHeld_      (),
        itemCacheLockbox_   (),
        treasureAvailable_  (item::TreasureAvailable::Count),
        trap_               (),
        fightResult_        (),
        creatureEffectIndex_(0),
        updateItemDisplayNeeded_(false),
        willProcessLockpickTitle_(false),
        creatureWhoPickedTheLockPtr_(nullptr)
    {}


    TreasureStage::~TreasureStage()
    {
        combat::Encounter::Instance()->EndTreasureStageTasks(
            itemCacheHeld_, itemCacheLockbox_);

        ClearAllEntities();
    }


    bool TreasureStage::HandleCallback(const popup::PopupResponse & POPUP_RESPONSE)
    {
        if (POPUP_RESPONSE.Info().Name() == POPUP_NAME_ITEMPROFILE_PLEASEWAIT_)
        {
            SetupAfterDelay();
            return false;
        }

        if (POPUP_RESPONSE.Info().Name() == POPUP_NAME_NO_TREASURE_)
        {
            TransitionToAdventureStage();
            return false;
        }

        if (POPUP_RESPONSE.Info().Name() == POPUP_NAME_WORN_ONLY_)
        {
            SetupForCollection();
            return true;
        }

        if ((POPUP_RESPONSE.Info().Name() == POPUP_NAME_LOCKBOX_ONLY_) ||
            (POPUP_RESPONSE.Info().Name() == POPUP_NAME_LOCKBOX_AND_HELD_))
        {
            if (POPUP_RESPONSE.Response() == popup::ResponseTypes::Yes)
            {
                PromptPlayerWhichCharacterWillPickLock();
                return false;
            }
            else
            {
                SetupForCollectionWithoutLockbox();
                return true;
            }
        }

        if (POPUP_RESPONSE.Info().Name() == POPUP_NAME_CHAR_SELECT_)
        {
            if (POPUP_RESPONSE.Response() == popup::ResponseTypes::Select)
            {
                auto const SELECTION{ POPUP_RESPONSE.Selection() };

                if (SELECTION < Game::Instance()->State().Party().Characters().size())
                {
                    creatureWhoPickedTheLockPtr_ =
                        Game::Instance()->State().Party().GetAtOrderPos(SELECTION);

                    combat::Encounter::Instance()->LockPickCreaturePtr(creatureWhoPickedTheLockPtr_);

                    PromptPlayerWithLockPickPopup(creatureWhoPickedTheLockPtr_->Name());
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
                    //Since the player is choosing to skip the lockbox and there is no
                    //held treasure, then simply return to the Adventure Stage.
                    LoopManager::Instance()->TransitionTo_Adventure();
                    return false;
                }
                else if (item::TreasureAvailable::HeldAndLockbox == treasureAvailable_)
                {
                    //Since the player is choosing to skip the lockbox,
                    //then prevent from searching the lockbox.
                    SetupForCollectionWithoutLockbox();
                    return true;
                }
            }
        }

        if (POPUP_RESPONSE.Info().Name() == POPUP_NAME_NO_CHARS_CAN_PICK_THE_LOCK_)
        {
            SetupForCollectionWithoutLockbox();
            return true;
        }

        if (POPUP_RESPONSE.Info().Name() == POPUP_NAME_LOCK_PICK_ATTEMPT_)
        {
            if (DetermineIfLockPickingSucceeded(
                combat::Encounter::Instance()->LockPickCreaturePtr()))
            {
                LockPickSuccess();
            }
            else
            {
                LockPickFailure();
            }

            return false;
        }

        if ((POPUP_RESPONSE.Info().Name() == POPUP_NAME_LOCK_PICK_FAILURE_) ||
            (POPUP_RESPONSE.Info().Name() == POPUP_NAME_DAMAGE_REPORT_))
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

        if (POPUP_RESPONSE.Info().Name() == POPUP_NAME_LOCKBOX_OPEN_)
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

        if (POPUP_RESPONSE.Info().Name() == POPUP_NAME_COIN_SHARE_)
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

        if (POPUP_RESPONSE.Info().Name() == POPUP_NAME_GEM_SHARE_)
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

        if (POPUP_RESPONSE.Info().Name() == POPUP_NAME_TITLE_ACHIEVEMENT_)
        {
            SetupForCollection();
            return true;
        }

        if (POPUP_RESPONSE.Info().Name() == POPUP_NAME_ITEM_TAKE_REJECTION_)
        {
            displayStagePtr_->CanDisplayItemDetail(true);
            return true;
        }

        return true;
    }


    void TreasureStage::Setup()
    {
        displayStagePtr_ = new TreasureDisplayStage(this);
        displayStagePtr_->Setup();

        //give control of the TreasureDispayStage object lifetime to the Loop class
        LoopManager::Instance()->AddStage(displayStagePtr_);

        if (item::ItemProfileWarehouse::Instance()->Count() == 0)
        {
            //This number was found by experiment to be a good number of draw frames to allow the
            //background image to fade in a little bit before displaying the 'Please Wait' popup.
            setupCountdown_ = 60;
        }
        else
        {
            setupCountdown_ = 0;
            SetupAfterDelay();
        }
    }


    void TreasureStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        HandleCountdownAndPleaseWaitPopup();
        Stage::Draw(target, STATES);

        if (updateItemDisplayNeeded_)
        {
            updateItemDisplayNeeded_ = false;
            UpdateItemDisplay();
        }
    }


    bool TreasureStage::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if ((displayStagePtr_ != nullptr) &&
            (displayStagePtr_->IsItemDetailMovingOrShowing()))
        {
            return false;
        }

        if (KEY_EVENT.code == sf::Keyboard::Space)
        {
            return HandleKeypress_Space();
        }

        if ((KEY_EVENT.code >= sf::Keyboard::Num1) &&
            (KEY_EVENT.code <= sf::Keyboard::Num6))
        {
            return HandleKeypress_Number(KEY_EVENT.code);
        }

        if ((KEY_EVENT.code == sf::Keyboard::Left) ||
            (KEY_EVENT.code == sf::Keyboard::Right))
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
        const sfml_util::gui::ListBox * const TREASURE_LISTBOX_PTR,
        const sfml_util::gui::ListBox * const INVENTORY_LISTBOX_PTR,
        const sfml_util::gui::callback::ListBoxEventPackage & PACKAGE)
    {
        auto const LISTBOX_ITEM_SPTR{ PACKAGE.package.PTR_->GetSelected() };

        if ((LISTBOX_ITEM_SPTR.get() != nullptr) &&
            (LISTBOX_ITEM_SPTR->ITEM_CPTR != nullptr))
        {
            if ((PACKAGE.gui_event == sfml_util::GuiEvent::DoubleClick) ||
                (PACKAGE.keypress_event.code == sf::Keyboard::Return))
            {
                if (PACKAGE.package.PTR_ == TREASURE_LISTBOX_PTR)
                {
                    TakeItem(LISTBOX_ITEM_SPTR->ITEM_CPTR);
                }
                else if (PACKAGE.package.PTR_ == INVENTORY_LISTBOX_PTR)
                {
                    PutItemBack(LISTBOX_ITEM_SPTR->ITEM_CPTR);
                }
            }

            return true;
        }

        return true;
    }


    void TreasureStage::TakeAllItems()
    {
        if (nullptr == displayStagePtr_)
        {
            sfml_util::SoundManager::Instance()->
                SoundEffectPlay(sfml_util::sound_effect::PromptWarn);

            return;
        }

        item::ItemPVec_t & itemsPVec{
            ((displayStagePtr_->IsShowingHeldItems()) ?
                itemCacheHeld_.items_pvec :
                itemCacheLockbox_.items_pvec) };

        if (itemsPVec.empty())
        {
            sfml_util::SoundManager::Instance()->
                SoundEffectPlay(sfml_util::sound_effect::PromptWarn);

            return;
        }

        std::sort(itemsPVec.begin(), itemsPVec.end(), [](auto A_PTR, auto B_PTR)
        {
            return A_PTR->Weight() > B_PTR->Weight();
        });

        //create a vector of creature ptrs in the order of who should take items
        auto const CHARACTER_INDEX{ displayStagePtr_->CharacterIndexShowingInventory() };

        creature::CreaturePVec_t whoWillTakeItems(
            1,
            Game::Instance()->State().Party().Characters()[CHARACTER_INDEX]);

        for (auto const CHARACTER_PTR : Game::Instance()->State().Party().Characters())
        {
            if ((CHARACTER_PTR->IsBeast() == false) &&
                (CHARACTER_PTR != whoWillTakeItems[0]))
            {
                whoWillTakeItems.push_back(CHARACTER_PTR);
            }
        }

        //for each item to be taken, attempt to give it to each possible character
        std::set<std::string> creatureNamesWhoTookItems;
        item::ItemPVec_t itemsToRemovePVec;
        for (auto const ITEM_PTR : itemsPVec)
        {
            for (auto creaturePtr : whoWillTakeItems)
            {
                if (creaturePtr->ItemIsAddAllowed(ITEM_PTR).empty())
                {
                    creaturePtr->ItemAdd(ITEM_PTR);
                    itemsToRemovePVec.push_back(ITEM_PTR);
                    break;
                }
            }
        }

        //remove the taken items from the cache
        for (auto const ITEM_PTR : itemsToRemovePVec)
        {
            itemsPVec.erase(
                std::remove(
                    itemsPVec.begin(),
                    itemsPVec.end(),
                    ITEM_PTR),
                itemsPVec.end() );
        }

        UpdateItemDisplay();

        //display a popup that reports how many items were taken
        auto const NUM_ITEMS_REMAINING{ itemsPVec.size() };
        if (NUM_ITEMS_REMAINING == 0)
        {
            sfml_util::SoundManager::Instance()->
                Getsound_effect_set(sfml_util::sound_effect_set::Switch).PlayRandom();

            std::ostringstream ss;
            ss << "\n" << "All " << itemsToRemovePVec.size()
                << " items were taken.";

            auto const POPUP_INFO{ popup::PopupManager::Instance()->CreatePopupInfo(
                POPUP_NAME_ALL_ITEMS_TAKEN_,
                ss.str(),
                popup::PopupButtons::Okay,
                popup::PopupImage::Regular) };

            LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
        }
        else if (itemsToRemovePVec.empty())
        {
            sfml_util::SoundManager::Instance()->
                SoundEffectPlay(sfml_util::sound_effect::PromptWarn);

            auto const POPUP_INFO{ popup::PopupManager::Instance()->CreatePopupInfo(
                POPUP_NAME_NOT_ALL_ITEMS_TAKEN_,
                "None of the items could be taken by any of your characters.",
                popup::PopupButtons::Okay,
                popup::PopupImage::Regular) };

            LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
        }
        else if (NUM_ITEMS_REMAINING > 0)
        {
            sfml_util::SoundManager::Instance()->
                SoundEffectPlay(sfml_util::sound_effect::PromptWarn);

            std::ostringstream ss;
            ss << "\n" << NUM_ITEMS_REMAINING
                << " items could not be taken by any of your characters.";

            auto const POPUP_INFO{ popup::PopupManager::Instance()->CreatePopupInfo(
                POPUP_NAME_NOT_ALL_ITEMS_TAKEN_,
                ss.str(),
                popup::PopupButtons::Okay,
                popup::PopupImage::Regular) };

            LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
        }
    }


    void TreasureStage::Exit()
    {
        if (nullptr == displayStagePtr_)
        {
            return;
        }

        TransitionToAdventureStage();
    }


    void TreasureStage::HandleCountdownAndPleaseWaitPopup()
    {
        //If the ItemProfileWarehouse needs to be setup, then wait for the background
        //to fade in a little before displaying the 'Please Wait' popup.
        if (setupCountdown_ > 0)
        {
            if (0 == --setupCountdown_)
            {
                LoopManager::Instance()->PopupWaitBeginSpecific<popup::PopupStageItemProfileWait>(
                    this,
                    popup::PopupManager::Instance()->CreateItemProfilePleaseWaitPopupInfo(
                        POPUP_NAME_ITEMPROFILE_PLEASEWAIT_));
            }
        }
    }


    void TreasureStage::SetupAfterDelay()
    {
        //TEMP TODO REMOVE -once done testing
        //create a party of characters to work with during testing
        state::GameStateFactory::Instance()->NewGame(player::FakeParty::Make());

        //TODO TEMP REMOVE -once finished testing
        //create a fake collection of dead creatures, using the predetermined initial encounter
        combat::Encounter::Instance()->BeginCombatTasks();
        //
        auto const NONPLAYER_CREATURE_PVEC{ creature::Algorithms::NonPlayers() };
        for (auto const NEXT_CREATURE_PTR : NONPLAYER_CREATURE_PVEC)
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
        PromptUserBasedonTreasureAvailability(treasureAvailable_, treasureImageType_);
    }


    item::TreasureAvailable::Enum TreasureStage::DetermineTreasureAvailableState(
        const item::ItemCache & CACHE_HELD,
        const item::ItemCache & CACHE_LOCKBOX)
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


    void TreasureStage::PromptUserBasedonTreasureAvailability(
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

                auto const POPUP_INFO{ popup::PopupManager::Instance()->CreatePopupInfo(
                    POPUP_NAME_NO_TREASURE_,
                    ss.str(),
                    popup::PopupButtons::Continue,
                    popup::PopupImage::Regular) };

                LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
                break;
            }
            case item::TreasureAvailable::HeldOnly:
            {
                std::ostringstream ss;
                ss << "\nYour enemies were wearing possessions but they carried no lockbox.  "
                    << "Click Continue to search what they left behind.";

                auto const POPUP_INFO{ popup::PopupManager::Instance()->CreatePopupInfo(
                    POPUP_NAME_WORN_ONLY_,
                    ss.str(),
                    popup::PopupButtons::Continue,
                    popup::PopupImage::Regular) };

                LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
                break;
            }
            case item::TreasureAvailable::LockboxOnly:
            case item::TreasureAvailable::HeldAndLockbox:
            {
                std::ostringstream ss;
                ss << "\nYour enemies carried a "
                    << item::TreasureImage::ToContainerName(TREASURE_IMAGE)
                    << ".  Attempt to pick the lock?";

                auto const POPUP_INFO{ popup::PopupManager::Instance()->CreatePopupInfo(
                    ((TREASURE_AVAILABLE == item::TreasureAvailable::LockboxOnly) ?
                        POPUP_NAME_LOCKBOX_ONLY_ :
                        POPUP_NAME_LOCKBOX_AND_HELD_),
                    ss.str(),
                    popup::PopupButtons::YesNo,
                    popup::PopupImage::Regular) };

                LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
                break;
            }
            case item::TreasureAvailable::Count:
            default:
            {
                std::ostringstream ss;
                ss << "stage::TreasureStage::PromptUserBasedonTreasureAvailability("
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


    void TreasureStage::PromptPlayerWhichCharacterWillPickLock()
    {
        auto const INVALID_MSGS{ MakeInvalidLockPickCharacterMessages() };

        auto isThereAValidCharacterWhoCanAttemptToPickTheLock{ AreAnyStringsEmpty(INVALID_MSGS) };

        if (isThereAValidCharacterWhoCanAttemptToPickTheLock)
        {
            auto const POPUP_INFO{ popup::PopupManager::Instance()->CreateCharacterSelectPopupInfo(
                POPUP_NAME_CHAR_SELECT_,
                "Who will attempt to pick the lock?",
                INVALID_MSGS,
                FindCharacterIndexWhoPrevAttemptedLockPicking()) };

            LoopManager::Instance()->
                PopupWaitBeginSpecific<popup::PopupStageCharacterSelect>(this, POPUP_INFO);
        }
        else
        {
            auto const MSG{
                std::string("There are no characters who can attempt to pick the lock!  ") +
                    "They are all incapable or incapacitated" };

            auto const POPUP_INFO{ popup::PopupManager::Instance()->CreatePopupInfo(
                POPUP_NAME_NO_CHARS_CAN_PICK_THE_LOCK_,
                MSG,
                popup::PopupButtons::Continue,
                popup::PopupImage::Regular) };

            LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
        }
    }


    std::size_t TreasureStage::FindCharacterIndexWhoPrevAttemptedLockPicking() const
    {
        auto const NUM_CHARACTERS{ Game::Instance()->State().Party().Characters().size() };

        auto const PREV_LOCKPICK_CREATURE_PTR{
            combat::Encounter::Instance()->LockPickCreaturePtr() };

        if (PREV_LOCKPICK_CREATURE_PTR != nullptr)
        {
            for (std::size_t i(0); i < NUM_CHARACTERS; ++i)
            {
                auto const CREATURE_PTR{ Game::Instance()->State().Party().GetAtOrderPos(i) };
                if (CREATURE_PTR == PREV_LOCKPICK_CREATURE_PTR)
                {
                    return i;
                }
            }

            combat::Encounter::Instance()->LockPickCreaturePtr(nullptr);
        }

        return 0;
    }


    const misc::StrVec_t TreasureStage::MakeInvalidLockPickCharacterMessages() const
    {
        auto const NUM_CHARACTERS{ Game::Instance()->State().Party().Characters().size() };

        misc::StrVec_t invalidMsgsVec;
        invalidMsgsVec.resize(NUM_CHARACTERS);

        for (std::size_t i(0); i < NUM_CHARACTERS; ++i)
        {
            auto const CREATURE_PTR{ Game::Instance()->State().Party().GetAtOrderPos(i) };
            if (CREATURE_PTR->IsBeast())
            {
                invalidMsgsVec[i] = "Beasts cannot pick locks";
            }
            else if (CREATURE_PTR->CanTakeAction() == false)
            {
                std::ostringstream ss;

                ss << creature::sex::HeSheIt(CREATURE_PTR->Sex(), true)
                    << " is " << CREATURE_PTR->CanTakeActionStr(false) << ".";

                invalidMsgsVec[i] = ss.str();
            }
            else
            {
                invalidMsgsVec[i].clear();
            }
        }

        return invalidMsgsVec;
    }


    bool TreasureStage::AreAnyStringsEmpty(const misc::StrVec_t & INVALID_MSGS) const
    {
        for (auto const & INVALID_MSG : INVALID_MSGS)
        {
            if (INVALID_MSG.empty())
            {
                return true;
            }
        }

        return false;
    }


    void TreasureStage::PromptPlayerWithLockPickPopup(
        const std::string & CHAR_PICKING_NAME)
    {
        auto const POPUP_INFO{ popup::PopupManager::Instance()->CreateKeepAlivePopupInfo(
            POPUP_NAME_LOCK_PICK_ATTEMPT_,
            CHAR_PICKING_NAME + " is attempting to pick the lock...",
            4.0f,//the duration of the longest lockpick sfx
            sfml_util::FontManager::Instance()->Size_Normal(),
            popup::PopupButtons::None,
            popup::PopupImage::Regular,
            SelectRandomLockPickingSfx()) };

        LoopManager::Instance()->PopupWaitBeginSpecific<popup::PopupStageGeneric>(
            this, POPUP_INFO);
    }


    bool TreasureStage::DetermineIfLockPickingSucceeded(
        const creature::CreaturePtr_t CHAR_WHO_IS_PICKING_THE_LOCK_PTR) const
    {
        return creature::Stats::Test(
            CHAR_WHO_IS_PICKING_THE_LOCK_PTR,
            stats::Traits::Luck,
            static_cast<creature::Stats::With>(
                creature::Stats::With::RaceRoleBonus |
                creature::Stats::With::StandardBonus));
    }


    sfml_util::sound_effect::Enum TreasureStage::SelectRandomLockPickingSfx() const
    {
        std::vector<sfml_util::sound_effect::Enum> lockPickingSfx = {
            sfml_util::sound_effect::TreasurePicking1,
            sfml_util::sound_effect::TreasurePicking2,
            sfml_util::sound_effect::TreasurePicking3,
            sfml_util::sound_effect::TreasurePicking4,
            sfml_util::sound_effect::TreasurePicking5,
            sfml_util::sound_effect::TreasurePicking6,
            sfml_util::sound_effect::TreasurePicking7 };

        return misc::Vector::SelectRandom(lockPickingSfx);
    }


    sfml_util::sound_effect::Enum TreasureStage::SelectRandomTreasureOpeningSfx() const
    {
        std::vector<sfml_util::sound_effect::Enum> treasureOpeningSfx = {
            sfml_util::sound_effect::TreasureOpen1,
            sfml_util::sound_effect::TreasureOpen2,
            sfml_util::sound_effect::TreasureOpen3,
            sfml_util::sound_effect::TreasureOpen4,
            sfml_util::sound_effect::TreasureOpen5,
            sfml_util::sound_effect::TreasureOpen6,
            sfml_util::sound_effect::TreasureOpen7,
            sfml_util::sound_effect::TreasureOpen8,
            sfml_util::sound_effect::TreasureOpen9 };

        return misc::Vector::SelectRandom(treasureOpeningSfx);
    }


    void TreasureStage::LockPickSuccess()
    {
        sfml_util::SoundManager::Instance()->SoundEffectPlay(
            sfml_util::sound_effect::TreasureUnlock);

        willProcessLockpickTitle_ = true;

        LockboxOpen();
    }


    void TreasureStage::LockPickFailure()
    {
        trap_ = trap::Warehouse::SelectRandomWithSeverityRatioNear(
            combat::Encounter::Instance()->DefeatedPartyTreasureRatioPer());

        fightResult_ = combat::FightClub::TreasureTrap(
            trap_,
            combat::Encounter::Instance()->LockPickCreaturePtr());

        auto const POPUP_INFO{ popup::PopupManager::Instance()->CreateTrapPopupInfo(
            POPUP_NAME_LOCK_PICK_FAILURE_,
            trap_.Description(item::TreasureImage::ToContainerName(treasureImageType_)),
            trap_.SoundEffect()) };

        LoopManager::Instance()->PopupWaitBeginSpecific<popup::PopupStageTreasureTrap>(
            this, POPUP_INFO);
    }


    TreasureStage::DamagePopup TreasureStage::PromptPlayerWithDamagePopups()
    {
        if (creatureEffectIndex_ < fightResult_.Effects().size())
        {
            bool ignored{ false };

            auto const DAMAGE_TEXT{ combat::Text::ActionTextIndexed(
                combat::Encounter::Instance()->LockPickCreaturePtr(),
                combat::TurnActionInfo(combat::TurnAction::TreasureUnlock),
                fightResult_,
                false,
                creatureEffectIndex_,
                0,
                ignored) };

            auto const POPUP_INFO{ popup::PopupManager::Instance()->CreatePopupInfo(
                POPUP_NAME_DAMAGE_REPORT_,
                "\n" + DAMAGE_TEXT,
                popup::PopupButtons::Continue,
                popup::PopupImage::Regular) };

            LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);

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
        auto const ALL_LIVING_PVEC{ creature::Algorithms::PlayersByType(
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
            SelectRandomTreasureOpeningSfx(),
            0.5f);

        auto const POPUP_INFO{ popup::PopupManager::Instance()->CreateKeepAlivePopupInfo(
            POPUP_NAME_LOCKBOX_OPEN_,
            "\nThe " + item::TreasureImage::ToContainerName(treasureImageType_) + " Opens!",
            4.0f,
            sfml_util::FontManager::Instance()->Size_Large(),
            popup::PopupButtons::Continue,
            popup::PopupImage::Regular,
            sfml_util::sound_effect::None) };

        LoopManager::Instance()->PopupWaitBeginSpecific<popup::PopupStageGeneric>(
            this, POPUP_INFO);
    }


    bool TreasureStage::ShareAndShowPopupIfNeeded(const ShareType WHAT_IS_SHARED)
    {
        auto const TOTAL_SHARED{ Share(WHAT_IS_SHARED) };
        if (TOTAL_SHARED == 0)
        {
            return false;
        }
        else
        {
            std::ostringstream ss;
            ss << "\nThe party finds and shares " << TOTAL_SHARED << " "
                << ((WHAT_IS_SHARED == ShareType::Coins) ? "coins" : "gems" ) << ".";

            auto const POPUP_NAME{
                ((WHAT_IS_SHARED == ShareType::Coins) ?
                    POPUP_NAME_COIN_SHARE_ :
                    POPUP_NAME_GEM_SHARE_) };

            auto const SOUND_EFFECT_SET{
                ((WHAT_IS_SHARED == ShareType::Coins) ?
                    sfml_util::sound_effect_set::Coin :
                    sfml_util::sound_effect_set::Gem) };

            auto const SOUND_EFFECT{ sfml_util::SoundManager::Instance()->
                Getsound_effect_set(SOUND_EFFECT_SET).SelectRandom() };

            auto const POPUP_INFO{ popup::PopupManager::Instance()->CreatePopupInfo(
                POPUP_NAME,
                ss.str(),
                popup::PopupButtons::Continue,
                popup::PopupImage::Regular,
                sfml_util::Justified::Center,
                SOUND_EFFECT,
                sfml_util::FontManager::Instance()->Size_Normal()) };

            LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
            return true;
        }
    }


    int TreasureStage::Share(const ShareType WHAT_IS_SHARED)
    {
        const int TOTAL{
            ((WHAT_IS_SHARED == ShareType::Coins) ?
                (itemCacheHeld_.coins.AsInt() + itemCacheLockbox_.coins.AsInt()) :
                (itemCacheHeld_.gems.AsInt() + itemCacheLockbox_.gems.AsInt())) };

        if (TOTAL <= 0)
        {
            return 0;
        }

        auto const HUMANOID_COUNT{
            static_cast<int>(Game::Instance()->State().Party().GetNumHumanoid()) };

        auto const SHARED_AMOUNT{ TOTAL / HUMANOID_COUNT };
        auto const LEFTOVER_AMOUNT{ TOTAL % HUMANOID_COUNT };

        for (auto nextCreaturePtr : Game::Instance()->State().Party().Characters())
        {
            if (nextCreaturePtr->Body().IsHumanoid())
            {
                if (WHAT_IS_SHARED == ShareType::Coins)
                {
                    nextCreaturePtr->CoinsAdj(nextCreaturePtr->Inventory().Coins() * Coin_t(-1));
                    nextCreaturePtr->CoinsAdj( Coin_t(SHARED_AMOUNT) );
                }
                else
                {
                    nextCreaturePtr->GemsAdj(nextCreaturePtr->Inventory().Gems() * Gem_t(-1));
                    nextCreaturePtr->GemsAdj( Gem_t(SHARED_AMOUNT));
                }
            }
        }

        auto toHandOut{ LEFTOVER_AMOUNT };
        for (auto nextCreaturePtr : Game::Instance()->State().Party().Characters())
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
        if (displayStagePtr_ != nullptr)
        {
            displayStagePtr_->UpdateItemCaches(itemCacheHeld_, itemCacheLockbox_);
            displayStagePtr_->SetupForCollection(treasureAvailable_, treasureImageType_);
        }
    }


    bool TreasureStage::HandleKeypress_Space()
    {
        if (displayStagePtr_ != nullptr)
        {
            if (displayStagePtr_->CanTreasureChange())
            {
                displayStagePtr_->TreasureChange();
                PlaySoundEffect_KeypressValid();
                return true;
            }
        }

        PlaySoundEffect_KeypressInvalid();
        return false;
    }


    bool TreasureStage::HandleKeypress_Number(const sf::Keyboard::Key KEY)
    {
        if (displayStagePtr_ != nullptr)
        {
            PlaySoundEffect_KeypressValid();

            auto characterIndex{ [KEY]() -> std::size_t
            {
                if (KEY == sf::Keyboard::Num1) return 0;
                else if (KEY == sf::Keyboard::Num2) return 1;
                else if (KEY == sf::Keyboard::Num3) return 2;
                else if (KEY == sf::Keyboard::Num4) return 3;
                else if (KEY == sf::Keyboard::Num5) return 4;
                else return 5;
            }() };

            auto const CHARACTER_INDEX_MAX{ displayStagePtr_->CharacterIndexMax() };
            if (characterIndex > CHARACTER_INDEX_MAX)
            {
                characterIndex = CHARACTER_INDEX_MAX;
            }

            displayStagePtr_->InventoryChange(characterIndex);
            return true;
        }

        return false;
    }


    bool TreasureStage::HandleKeypress_LeftRight(const sf::Keyboard::Key KEY)
    {
        if ((KEY == sf::Keyboard::Left) && (displayStagePtr_ != nullptr))
        {
            PlaySoundEffect_KeypressValid();

            auto characterIndex{ displayStagePtr_->CharacterIndex() };

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

        if ((KEY == sf::Keyboard::Right) && (displayStagePtr_ != nullptr))
        {
            PlaySoundEffect_KeypressValid();

            auto characterIndex{ displayStagePtr_->CharacterIndex() };

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
        sfml_util::SoundManager::Instance()->
            Getsound_effect_set(sfml_util::sound_effect_set::Thock).PlayRandom();
    }


    void TreasureStage::PlaySoundEffect_KeypressInvalid() const
    {
        sfml_util::SoundManager::Instance()->
            SoundEffectPlay(sfml_util::sound_effect::PromptWarn);
    }


    void TreasureStage::TakeItem(const item::ItemPtr_t ITEM_PTR)
    {
        if (nullptr == displayStagePtr_)
        {
            return;
        }

        auto creaturePtr{ displayStagePtr_->WhichCharacterInventoryIsDisplayed() };
        auto const IS_ITEM_ADD_ALLOWED_STR{ creaturePtr->ItemIsAddAllowed(ITEM_PTR) };

        if (IS_ITEM_ADD_ALLOWED_STR.empty())
        {
            sfml_util::SoundManager::Instance()->
                Getsound_effect_set(sfml_util::sound_effect_set::Switch).PlayRandom();

            creaturePtr->ItemAdd(ITEM_PTR);

            if (displayStagePtr_->IsShowingHeldItems())
            {
                itemCacheHeld_.items_pvec.erase( std::remove(
                    itemCacheHeld_.items_pvec.begin(),
                    itemCacheHeld_.items_pvec.end(), ITEM_PTR), itemCacheHeld_.items_pvec.end());
            }
            else
            {
                itemCacheLockbox_.items_pvec.erase(std::remove(
                    itemCacheLockbox_.items_pvec.begin(),
                    itemCacheLockbox_.items_pvec.end(), ITEM_PTR), itemCacheLockbox_.items_pvec.end());
            }

            //Can't update display now because it would invalidate the this pointer.
            updateItemDisplayNeeded_ = true;
        }
        else
        {
            displayStagePtr_->CanDisplayItemDetail(false);

            std::ostringstream ss;
            ss << creaturePtr->Name() << " can't take the " << ITEM_PTR->Name() << " because:  "
                << IS_ITEM_ADD_ALLOWED_STR;

            auto const POPUP_INFO{ popup::PopupManager::Instance()->CreatePopupInfo(
                POPUP_NAME_ITEM_TAKE_REJECTION_,
                ss.str(),
                popup::PopupButtons::Cancel,
                popup::PopupImage::Regular,
                sfml_util::Justified::Center,
                sfml_util::sound_effect::PromptWarn,
                sfml_util::FontManager::Instance()->Size_Largeish()) };

            LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
        }
    }


    void TreasureStage::PutItemBack(const item::ItemPtr_t ITEM_PTR)
    {
        if (nullptr == displayStagePtr_)
        {
            return;
        }

        sfml_util::SoundManager::Instance()->
            Getsound_effect_set(sfml_util::sound_effect_set::ItemDrop).PlayRandom();

        displayStagePtr_->WhichCharacterInventoryIsDisplayed()->ItemRemove(ITEM_PTR);

        if (displayStagePtr_->IsShowingHeldItems())
        {
            itemCacheHeld_.items_pvec.push_back(ITEM_PTR);
        }
        else
        {
            itemCacheLockbox_.items_pvec.push_back(ITEM_PTR);
        }

        //Can't update display now because it would invalidate the this pointer.
        updateItemDisplayNeeded_ = true;
    }


    void TreasureStage::UpdateItemDisplay()
    {
        displayStagePtr_->UpdateItemCaches(itemCacheHeld_, itemCacheLockbox_);
        displayStagePtr_->RefreshAfterCacheUpdate();
    }


    bool TreasureStage::ProcessLockpickTitleAndPopupIfNeeded()
    {
        if (willProcessLockpickTitle_ && (creatureWhoPickedTheLockPtr_ != nullptr))
        {
            willProcessLockpickTitle_ = false;

            auto const FROM_TITLE_PTR{
                creatureWhoPickedTheLockPtr_->GetAchievements().GetCurrentTitle(
                    creature::AchievementType::LocksPicked) };

            auto const TO_TITLE_PTR{
                creatureWhoPickedTheLockPtr_->GetAchievements().Increment(
                    creature::AchievementType::LocksPicked) };

            if ((TO_TITLE_PTR != nullptr) && (FROM_TITLE_PTR != TO_TITLE_PTR))
            {
                TO_TITLE_PTR->Change(creatureWhoPickedTheLockPtr_);

                sf::Texture fromTexture;
                if (FROM_TITLE_PTR != nullptr)
                {
                    sfml_util::gui::TitleImageManager::Instance()->Get(
                        fromTexture, FROM_TITLE_PTR->Which());
                }

                sf::Texture toTexture;
                if (TO_TITLE_PTR != nullptr)
                {
                    sfml_util::gui::TitleImageManager::Instance()->Get(
                        toTexture, TO_TITLE_PTR->Which());
                }

                auto const POPUP_INFO{ popup::PopupManager::Instance()->CreateImageFadePopupInfo(
                    POPUP_NAME_TITLE_ACHIEVEMENT_,
                    creatureWhoPickedTheLockPtr_,
                    FROM_TITLE_PTR,
                    TO_TITLE_PTR,
                    & fromTexture,
                    & toTexture) };

                LoopManager::Instance()->PopupWaitBeginSpecific<popup::PopupStageImageFade>(
                    this, POPUP_INFO);

                return true;
            }
        }
        
        return false;
    }


    void TreasureStage::TransitionToAdventureStage()
    {
        combat::Encounter::Instance()->EndTreasureStageTasks(
            itemCacheHeld_,
            itemCacheLockbox_);

        LoopManager::Instance()->TransitionTo_Adventure();
    }

}
}
