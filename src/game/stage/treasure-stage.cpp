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

#include "popup/popup-manager.hpp"
#include "popup/popup-stage-char-select.hpp"
#include "popup/popup-stage-treasure-trap.hpp"
#include "popup/popup-stage-item-profile-wait.hpp"
#include "popup/popup-stage-combat-over.hpp"

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

    const std::string TreasureStage::POPUP_NAME_ALL_ENEMIES_RAN_{
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
        creatureEffectIndex_(0)
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

        if (POPUP_RESPONSE.Info().Name() == POPUP_NAME_ALL_ENEMIES_RAN_)
        {
            LoopManager::Instance()->TransitionTo_Adventure();
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
                    auto const LOCK_PICKING_CREATURE_PTR{
                        Game::Instance()->State().Party().GetAtOrderPos(SELECTION) };

                    combat::Encounter::Instance()->LockPickCreaturePtr(LOCK_PICKING_CREATURE_PTR);

                    PromptPlayerWithLockPickPopup(LOCK_PICKING_CREATURE_PTR->Name());
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
                    SetupForCollection();
                    return true;
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
                SetupForCollection();
                return true;
            }
        }

        if (POPUP_RESPONSE.Info().Name() == POPUP_NAME_GEM_SHARE_)
        {
            SetupForCollection();
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
        displayStagePtr_->Draw(target, STATES);
        HandleCountdownAndPleaseWaitPopup();
        Stage::Draw(target, STATES);
    }


    bool TreasureStage::HandleListboxCallback(
        const sfml_util::gui::ListBox * const,
        const sfml_util::gui::ListBox * const,
        const sfml_util::gui::callback::ListBoxEventPackage &)
    {
        return true;
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
                    POPUP_NAME_ALL_ENEMIES_RAN_,
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


    stats::Trait_t TreasureStage::Share(const ShareType WHAT_IS_SHARED)
    {
        auto const TOTAL{
            ((WHAT_IS_SHARED == ShareType::Coins) ?
                (itemCacheHeld_.coins + itemCacheLockbox_.coins) :
                (itemCacheHeld_.gems + itemCacheLockbox_.gems)) };

        if (TOTAL <= 0)
        {
            return 0;
        }

        auto const HUMANOID_COUNT(static_cast<stats::Trait_t>(
            Game::Instance()->State().Party().GetNumHumanoid()));

        const stats::Trait_t SHARED_AMOUNT(TOTAL / HUMANOID_COUNT);
        const stats::Trait_t LEFTOVER_AMOUNT(TOTAL % HUMANOID_COUNT);

        for (auto nextCreaturePtr : Game::Instance()->State().Party().Characters())
        {
            if (nextCreaturePtr->Body().IsHumanoid())
            {
                if (WHAT_IS_SHARED == ShareType::Coins)
                {
                    nextCreaturePtr->CoinsAdj(nextCreaturePtr->Inventory().Coins() * -1);
                    nextCreaturePtr->CoinsAdj(SHARED_AMOUNT);
                }
                else
                {
                    nextCreaturePtr->GemsAdj(nextCreaturePtr->Inventory().Gems() * -1);
                    nextCreaturePtr->GemsAdj(SHARED_AMOUNT);
                }
            }
        }

        stats::Trait_t toHandOut(LEFTOVER_AMOUNT);
        for (auto nextCreaturePtr : Game::Instance()->State().Party().Characters())
        {
            if (nextCreaturePtr->Body().IsHumanoid() && (toHandOut-- > 0))
            {
                if (WHAT_IS_SHARED == ShareType::Coins)
                {
                    nextCreaturePtr->CoinsAdj(1);
                }
                else
                {
                    nextCreaturePtr->GemsAdj(1);
                }
            }
        }

        return TOTAL;
    }


    void TreasureStage::SetupForCollection()
    {
        displayStagePtr_->SetupForCollection(
            treasureAvailable_,
            treasureImageType_,
            itemCacheHeld_,
            itemCacheLockbox_);

        //TODO
    }

}
}
