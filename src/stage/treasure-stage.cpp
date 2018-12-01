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
#include "gui/font-manager.hpp"
#include "gui/list-element.hpp"
#include "gui/text-region.hpp"
#include "item/item-profile-warehouse.hpp"
#include "item/item-warehouse.hpp"
#include "item/item.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/config-file.hpp"
#include "misc/random.hpp"
#include "misc/real.hpp"
#include "popup/popup-manager.hpp"
#include "popup/popup-response.hpp"
#include "popup/popup-stage-char-select.hpp"
#include "popup/popup-stage-combat-over.hpp"
#include "popup/popup-stage-image-fade.hpp"
#include "popup/popup-stage-treasure-trap.hpp"
#include "stage/stage-enum.hpp"
#include "stage/treasure-display-stage.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include <algorithm>
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
        : StageBase(
            "Treasure",
            { gui::GuiFont::Default,
              gui::GuiFont::System,
              gui::GuiFont::SystemCondensed,
              gui::GuiFont::Number,
              gui::GuiFont::DefaultBoldFlavor,
              gui::GuiFont::Handwriting })
        , displayStagePtrOpt_(boost::none)
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
        StageBase::ClearAllEntities();
    }

    const std::string TreasureStage::HandleCallback(
        const misc::PopupCallback_t::Packet_t & PACKET, const std::string & PACKET_DESCRIPTION)
    {
        if (PACKET.curently_open_popup_name == POPUP_NAME_NO_TREASURE_)
        {
            TransitionToAdventureStage();

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION, "there is no treasure so transitioned to Adventure Stage");
        }

        if (PACKET.curently_open_popup_name == POPUP_NAME_WORN_ONLY_)
        {
            SetupForCollection();

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION,
                "saw there was only worn treasure so setup treasure collection stage without a "
                "lockbox");
        }

        if ((PACKET.curently_open_popup_name == POPUP_NAME_LOCKBOX_ONLY_)
            || (PACKET.curently_open_popup_name == POPUP_NAME_LOCKBOX_AND_HELD_))
        {
            if (PACKET.type == popup::PopupButtons::Yes)
            {
                lockPicking_.PopupCharacterSelection(
                    misc::MakeNotNull(this), misc::MakeNotNull(this));

                return MakeCallbackHandlerMessage(
                    PACKET_DESCRIPTION,
                    "saw there was a lockbox and the user decided to try and open it so spawned "
                    "the lock-pick-character-selection popup");
            }
            else
            {
                SetupForCollectionWithoutLockbox();

                return MakeCallbackHandlerMessage(
                    PACKET_DESCRIPTION,
                    "saw there was a lockbox but the user decided NOT to try and open it so "
                    "setup treasure collection stage without a lockbox");
            }
        }

        if (PACKET.curently_open_popup_name == lockPicking_.POPUP_NAME_CHARACTER_SELECTION_)
        {
            if (PACKET.type == popup::PopupButtons::Select)
            {
                if (lockPicking_.HandleCharacterSelectionPopupResponse(
                        misc::MakeNotNull(this), PACKET, misc::MakeNotNull(this)))
                {
                    return MakeCallbackHandlerMessage(
                        PACKET_DESCRIPTION,
                        "saw the user decided NOT to pick a character to try and unlock the "
                        "lockbox and "
                        "spawned the lock picking popup");
                }
                else
                {
                    SetupForCollectionWithoutLockbox();

                    return MakeCallbackHandlerMessage(
                        PACKET_DESCRIPTION,
                        "saw the user decided which character to try and unlock the lockbox and "
                        "setup treasure collection stage without a lockbox");
                }
            }
            else
            {
                if (item::TreasureAvailable::LockboxOnly == treasureAvailable_)
                {
                    TransitionTo(stage::Stage::Adventure);

                    return MakeCallbackHandlerMessage(
                        PACKET_DESCRIPTION,
                        "since the player is choosing to skip the lockbox and there is no "
                        "held treasure, then simply return to the Adventure Stage");
                }
                else if (item::TreasureAvailable::HeldAndLockbox == treasureAvailable_)
                {
                    SetupForCollectionWithoutLockbox();

                    return MakeCallbackHandlerMessage(
                        PACKET_DESCRIPTION,
                        "since the player is choosing to skip the lockbox "
                        "then prevent from searching the lockbox and setup treasure collection "
                        "stage without a lockbox");
                }
            }
        }

        if (PACKET.curently_open_popup_name == lockPicking_.POPUP_NAME_NO_CHARACTER_CAN_PICK_)
        {
            SetupForCollectionWithoutLockbox();

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION,
                "since there is no character who can even attempt to pick the lock "
                "setup treasure collection stage without a lockbox");
        }

        if (PACKET.curently_open_popup_name == lockPicking_.POPUP_NAME_ATTEMPTING_)
        {
            if (lockPicking_.Attempt())
            {
                LockPickSuccess();

                return MakeCallbackHandlerMessage(
                    PACKET_DESCRIPTION,
                    "saw the player actually picked the lock so the open lockbox popup was "
                    "spawned");
            }
            else
            {
                LockPickFailure();

                return MakeCallbackHandlerMessage(
                    PACKET_DESCRIPTION,
                    "saw the player failed to pick the lock so the open trap popup was "
                    "spawned");
            }
        }

        if ((PACKET.curently_open_popup_name == POPUP_NAME_LOCK_PICK_FAILURE_)
            || (PACKET.curently_open_popup_name == POPUP_NAME_DAMAGE_REPORT_))
        {
            if (PromptPlayerWithDamagePopups() == DamagePopup::Displayed)
            {
                return MakeCallbackHandlerMessage(
                    PACKET_DESCRIPTION, "spawned the damage report popups");
            }
            else
            {
                if (CheckAndHandleAllKilledByTrap())
                {
                    return MakeCallbackHandlerMessage(
                        PACKET_DESCRIPTION,
                        "lockbox trap killed all characters so spawned end game popup");
                }
                else
                {
                    LockboxOpen();

                    return MakeCallbackHandlerMessage(
                        PACKET_DESCRIPTION,
                        "trap damaged characters but now it is open so setup treasure collection "
                        "stage with lockbox");
                }
            }
        }

        if (PACKET.curently_open_popup_name == lockPicking_.POPUP_NAME_SUCCESS_)
        {
            displayStagePtrOpt_.value()->UpdateTreasureImage(treasureImageType_);

            if (ShareAndShowPopupIfNeeded(ShareType::Coins))
            {
                return MakeCallbackHandlerMessage(
                    PACKET_DESCRIPTION,
                    "saw character actually picked the lock and spawned the 'shared coins' popup");
            }
            else
            {
                if (ShareAndShowPopupIfNeeded(ShareType::Gems))
                {
                    return MakeCallbackHandlerMessage(
                        PACKET_DESCRIPTION,
                        "saw character actually picked the lock and spawned the 'shared gems' "
                        "popup");
                }
                else
                {
                    if (ProcessLockpickTitleAndPopupIfNeeded())
                    {
                        return MakeCallbackHandlerMessage(
                            PACKET_DESCRIPTION,
                            "saw character actually picked the lock and spawned the popup "
                            "displaying that the lock picking character gained a "
                            "title for doing so");
                    }
                    else
                    {
                        SetupForCollection();

                        return MakeCallbackHandlerMessage(
                            PACKET_DESCRIPTION,
                            "saw character actually picked the lock and saw the player unlocked "
                            "the lockbox and now setup the treasure "
                            "collection stage including that lockbox");
                    }
                }
            }
        }

        if (PACKET.curently_open_popup_name == POPUP_NAME_COIN_SHARE_)
        {
            if (ShareAndShowPopupIfNeeded(ShareType::Gems))
            {
                return MakeCallbackHandlerMessage(
                    PACKET_DESCRIPTION,
                    "after the 'shared coins' popup, spawned the 'shared gems' popup");
            }
            else
            {
                if (ProcessLockpickTitleAndPopupIfNeeded())
                {
                    return MakeCallbackHandlerMessage(
                        PACKET_DESCRIPTION,
                        "after the 'shared coins' popup, displaying that the lock picking "
                        "character gained a title for doing so");
                }
                else
                {
                    SetupForCollection();

                    return MakeCallbackHandlerMessage(
                        PACKET_DESCRIPTION,
                        "after the 'shared coins' popup, setup the treasure "
                        "collection stage including that lockbox");
                }
            }
        }

        if (PACKET.curently_open_popup_name == POPUP_NAME_GEM_SHARE_)
        {
            if (ProcessLockpickTitleAndPopupIfNeeded())
            {
                return MakeCallbackHandlerMessage(
                    PACKET_DESCRIPTION,
                    "after the 'shared gems' popup, displaying that the lock picking "
                    "character gained a title for doing so");
            }
            else
            {
                SetupForCollection();

                return MakeCallbackHandlerMessage(
                    PACKET_DESCRIPTION,
                    "after the 'shared gems' popup, setup the treasure "
                    "collection stage including that lockbox");
            }
        }

        if (PACKET.curently_open_popup_name == lockPicking_.POPUP_NAME_TITLE_ARCHIEVED_)
        {
            SetupForCollection();

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION,
                "after the 'lock picking title gained' popup, setup the treasure "
                "collection stage including that lockbox");
        }

        if (PACKET.curently_open_popup_name == POPUP_NAME_ITEM_TAKE_REJECTION_)
        {
            displayStagePtrOpt_.value()->CanDisplayItemDetail(true);

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION,
                "after the player was told that a particular character could not take an item for "
                "some reason, we um...went back to treasure collecting UI stage stuff");
        }

        return MakeCallbackHandlerMessage(PACKET_DESCRIPTION, "popup callback NOT HANDLED");
    }

    void TreasureStage::Setup()
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (!displayStagePtrOpt_),
            "stage::TreasureStage::Setup() called but displayStagePtrOpt_ was invalid.  It "
            "should be set before this function is called by stage::StageFactory::Make().");

        // TODO TEMP REMOVE -once finished testing
        // create a fake collection of dead creatures, using the predetermined initial encounter
        combat::Encounter::Instance()->BeginAndEndFakeCombatForTesting();

        // one of these calls
        treasureImageType_ = combat::Encounter::Instance()->BeginTreasureStageTasks();
        itemCacheHeld_ = combat::Encounter::Instance()->TakeDeadNonPlayerItemsHeldCache();
        itemCacheLockbox_ = combat::Encounter::Instance()->TakeDeadNonPlayerItemsLockboxCache();

        // One of the Encounter functions called above code above will force the item code to
        // generate all of the items in the game.  This can take a few seconds so there might be a
        // popup saying please wait.  This is why some functions contain the words
        // "AfterPleaseWait" in this class.

        displayStagePtrOpt_.value()->SetupAfterPleaseWait(treasureImageType_);
        treasureAvailable_ = DetermineTreasureAvailableState(itemCacheHeld_, itemCacheLockbox_);
        PromptUserBasedOnTreasureAvailability(treasureAvailable_, treasureImageType_);
    }

    void TreasureStage::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        StageBase::draw(target, states);

        if (updateItemDisplayNeeded_)
        {
            updateItemDisplayNeeded_ = false;
            UpdateItemDisplay();
        }
    }

    bool TreasureStage::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if (displayStagePtrOpt_.value()->IsItemDetailMovingOrShowing())
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

        return StageBase::KeyRelease(KEY_EVENT);
    }

    const std::string TreasureStage::HandleListboxCallback(
        const ItemListBoxPtr_t & TREASURE_LISTBOX_PTR,
        const ItemListBoxPtr_t & INVENTORY_LISTBOX_PTR,
        const ItemListBox_t::Callback_t::Packet_t & PACKET,
        const std::string & PACKET_DESCRIPTION)
    {
        if ((PACKET.gui_event == gui::GuiEvent::DoubleClick)
            || (PACKET.keypress_event.code == sf::Keyboard::Return))
        {
            const auto ITEM_PTR { PACKET.selected_element_ptr->Element() };

            if (PACKET.listbox_ptr == TREASURE_LISTBOX_PTR)
            {
                TakeItem(ITEM_PTR);

                return MakeCallbackHandlerMessage(
                    PACKET_DESCRIPTION,
                    "enter-keypress or double-click causing an item to be taken");
            }
            else if (PACKET.listbox_ptr == INVENTORY_LISTBOX_PTR)
            {
                PutItemBack(ITEM_PTR);

                return MakeCallbackHandlerMessage(
                    PACKET_DESCRIPTION,
                    "enter-keypress or double-click causing an item to be put back");
            }

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION, "enter-keypress or double-click NOT HANDLED");
        }

        return MakeCallbackHandlerMessage(PACKET_DESCRIPTION, "listbox callback NOT HANDLED");
    }

    void TreasureStage::TakeAllItems()
    {
        item::ItemPVec_t & itemsPVec { (
            (displayStagePtrOpt_.value()->IsShowingHeldItems()) ? itemCacheHeld_.items_pvec
                                                                : itemCacheLockbox_.items_pvec) };

        if (itemsPVec.empty())
        {
            gui::SoundManager::Instance()->PlaySfx_Reject();
            return;
        }

        std::sort(itemsPVec.begin(), itemsPVec.end(), [](auto A_PTR, auto B_PTR) {
            return A_PTR->Weight() > B_PTR->Weight();
        });

        creature::CreaturePVec_t whoWillTakeItems
            = { game::Game::Instance()->State().Party().Characters().at(
                displayStagePtrOpt_.value()->CharacterIndexShowingInventory()) };

        for (const auto & CHARACTER_PTR : game::Game::Instance()->State().Party().Characters())
        {
            if ((CHARACTER_PTR->IsBeast() == false) && (CHARACTER_PTR != whoWillTakeItems.front()))
            {
                whoWillTakeItems.emplace_back(CHARACTER_PTR);
            }
        }

        // for each item to be taken, attempt to give it to each possible character
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
            gui::SoundManager::Instance()->PlaySfx_AckMajor();

            std::ostringstream ss;
            ss << "\n"
               << "All " << itemsToRemovePVec.size() << " items were taken.";

            const auto POPUP_INFO { popup::PopupManager::Instance()->CreatePopupInfo(
                POPUP_NAME_ALL_ITEMS_TAKEN_,
                ss.str(),
                popup::PopupButtons::Okay,
                popup::PopupImage::Regular) };

            SpawnPopup(misc::MakeNotNull(this), POPUP_INFO);
        }
        else if (itemsToRemovePVec.empty())
        {
            gui::SoundManager::Instance()->PlaySfx_Reject();

            const auto POPUP_INFO { popup::PopupManager::Instance()->CreatePopupInfo(
                POPUP_NAME_NOT_ALL_ITEMS_TAKEN_,
                "None of the items could be taken by any of your characters.",
                popup::PopupButtons::Okay,
                popup::PopupImage::Regular) };

            SpawnPopup(misc::MakeNotNull(this), POPUP_INFO);
        }
        else
        {
            gui::SoundManager::Instance()->PlaySfx_Reject();

            std::ostringstream ss;
            ss << "\n"
               << NUM_ITEMS_REMAINING << " items could not be taken by any of your characters.";

            const auto POPUP_INFO { popup::PopupManager::Instance()->CreatePopupInfo(
                POPUP_NAME_NOT_ALL_ITEMS_TAKEN_,
                ss.str(),
                popup::PopupButtons::Okay,
                popup::PopupImage::Regular) };

            SpawnPopup(misc::MakeNotNull(this), POPUP_INFO);
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

                SpawnPopup(misc::MakeNotNull(this), POPUP_INFO);
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

                SpawnPopup(misc::MakeNotNull(this), POPUP_INFO);
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

                SpawnPopup(misc::MakeNotNull(this), POPUP_INFO);
                break;
            }
            case item::TreasureAvailable::Count:
            default:
            {
                M_HP_LOG_ERR(
                    "stage::TreasureStage::PromptUserBasedOnTreasureAvailability(treasure_"
                    "available_enum="
                    << item::TreasureAvailable::ToString(TREASURE_AVAILABLE)
                    << ") but that enum valid was invalid.");

                return;
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

    gui::sound_effect::Enum TreasureStage::SelectRandomTreasureOpeningSfx() const
    {
        std::vector<gui::sound_effect::Enum> treasureOpeningSfx
            = { gui::sound_effect::TreasureOpen1,
                gui::sound_effect::TreasureOpen2,
                gui::sound_effect::TreasureOpen3,
                gui::sound_effect::TreasureOpen4,
                gui::sound_effect::TreasureOpen5 };

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

        SpawnPopup(misc::MakeNotNull(this), POPUP_INFO);
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

            SpawnPopup(misc::MakeNotNull(this), POPUP_INFO);

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
            SpawnPopup(
                misc::MakeNotNull(this),
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

        gui::SoundManager::Instance()->SoundEffectPlay(SelectRandomTreasureOpeningSfx(), 0.5f);

        lockPicking_.PopupSuccess(
            misc::MakeNotNull(this),
            item::TreasureImage::ToContainerName(treasureImageType_),
            misc::MakeNotNull(this));
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
                (WHAT_IS_SHARED == ShareType::Coins) ? gui::sound_effect_set::Coin
                                                     : gui::sound_effect_set::Gem) };

            const auto SOUND_EFFECT {
                gui::SoundManager::Instance()->GetSoundEffectSet(SOUND_EFFECT_SET).SelectRandom()
            };

            const auto POPUP_INFO { popup::PopupManager::Instance()->CreatePopupInfo(
                POPUP_NAME,
                ss.str(),
                popup::PopupButtons::Continue,
                popup::PopupImage::Regular,
                gui::Justified::Center,
                SOUND_EFFECT,
                gui::FontManager::Instance()->Size_Normal()) };

            SpawnPopup(misc::MakeNotNull(this), POPUP_INFO);
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
        displayStagePtrOpt_.value()->UpdateItemCaches(itemCacheHeld_, itemCacheLockbox_);
        displayStagePtrOpt_.value()->SetupForCollection(treasureAvailable_, treasureImageType_);
    }

    bool TreasureStage::HandleKeypress_Space()
    {
        if (displayStagePtrOpt_.value()->CanTreasureChange())
        {
            displayStagePtrOpt_.value()->TreasureChange();
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

        const auto CHARACTER_INDEX_MAX { displayStagePtrOpt_.value()->CharacterIndexMax() };
        if (characterIndex > CHARACTER_INDEX_MAX)
        {
            characterIndex = CHARACTER_INDEX_MAX;
        }

        displayStagePtrOpt_.value()->InventoryChange(characterIndex);
        return true;
    }

    bool TreasureStage::HandleKeypress_LeftRight(const sf::Keyboard::Key KEY)
    {
        if (KEY == sf::Keyboard::Left)
        {
            PlaySoundEffect_KeypressValid();

            auto characterIndex { displayStagePtrOpt_.value()->CharacterIndex() };

            if (0 == characterIndex)
            {
                characterIndex = displayStagePtrOpt_.value()->CharacterIndexMax();
            }
            else
            {
                --characterIndex;
            }

            displayStagePtrOpt_.value()->InventoryChange(characterIndex);

            return true;
        }

        if (KEY == sf::Keyboard::Right)
        {
            PlaySoundEffect_KeypressValid();

            auto characterIndex { displayStagePtrOpt_.value()->CharacterIndex() };

            if (displayStagePtrOpt_.value()->CharacterIndexMax() == characterIndex)
            {
                characterIndex = 0;
            }
            else
            {
                ++characterIndex;
            }

            displayStagePtrOpt_.value()->InventoryChange(characterIndex);

            return true;
        }

        return false;
    }

    void TreasureStage::PlaySoundEffect_KeypressValid() const
    {
        gui::SoundManager::Instance()->PlaySfx_Keypress();
    }

    void TreasureStage::PlaySoundEffect_KeypressInvalid() const
    {
        gui::SoundManager::Instance()->PlaySfx_Reject();
    }

    void TreasureStage::TakeItem(const item::ItemPtr_t ITEM_PTR)
    {
        auto creaturePtr { displayStagePtrOpt_.value()->WhichCharacterInventoryIsDisplayed() };
        const auto IS_ITEM_ADD_ALLOWED_STR { creaturePtr->ItemIsAddAllowed(ITEM_PTR) };

        if (IS_ITEM_ADD_ALLOWED_STR.empty())
        {
            gui::SoundManager::Instance()->PlaySfx_AckMajor();

            creaturePtr->ItemAdd(ITEM_PTR);

            if (displayStagePtrOpt_.value()->IsShowingHeldItems())
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
            displayStagePtrOpt_.value()->CanDisplayItemDetail(false);

            std::ostringstream ss;
            ss << creaturePtr->Name() << " can't take the " << ITEM_PTR->Name()
               << " because:  " << IS_ITEM_ADD_ALLOWED_STR;

            const auto POPUP_INFO { popup::PopupManager::Instance()->CreatePopupInfo(
                POPUP_NAME_ITEM_TAKE_REJECTION_,
                ss.str(),
                popup::PopupButtons::Cancel,
                popup::PopupImage::Regular,
                gui::Justified::Center,
                gui::sound_effect::PromptWarn,
                gui::FontManager::Instance()->Size_Largeish()) };

            SpawnPopup(misc::MakeNotNull(this), POPUP_INFO);
        }
    }

    void TreasureStage::PutItemBack(const item::ItemPtr_t ITEM_PTR)
    {
        gui::SoundManager::Instance()
            ->GetSoundEffectSet(gui::sound_effect_set::ItemDrop)
            .PlayRandom();

        displayStagePtrOpt_.value()->WhichCharacterInventoryIsDisplayed()->ItemRemove(ITEM_PTR);

        if (displayStagePtrOpt_.value()->IsShowingHeldItems())
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

    void TreasureStage::UpdateItemDisplay() const
    {
        displayStagePtrOpt_.value()->UpdateItemCaches(itemCacheHeld_, itemCacheLockbox_);
        displayStagePtrOpt_.value()->RefreshAfterCacheUpdate();
    }

    bool TreasureStage::ProcessLockpickTitleAndPopupIfNeeded()
    {
        if (willProcessLockpickTitle_)
        {
            willProcessLockpickTitle_ = false;

            return lockPicking_.HandleAchievementIncrementAndReturnTrueOnNewTitleWithPopup(
                misc::MakeNotNull(this), misc::MakeNotNull(this));
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
        TransitionTo(stage::Stage::Adventure);
    }

} // namespace stage
} // namespace heroespath
