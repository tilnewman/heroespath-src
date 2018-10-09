// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// lock-interactions.cpp
//
#include "lock-interactions.hpp"

#include "combat/encounter.hpp"
#include "creature/creature.hpp"
#include "creature/player-party.hpp"
#include "creature/stats.hpp"
#include "game/game-state.hpp"
#include "game/game.hpp"
#include "game/loop-manager.hpp"
#include "misc/vectors.hpp"
#include "popup/popup-manager.hpp"
#include "popup/popup-stage-char-select.hpp"
#include "popup/popup-stage-image-fade.hpp"
#include "sfml-util/font-manager.hpp"
#include "stage/achievement-handler.hpp"

#include <algorithm>
#include <vector>

namespace heroespath
{
namespace interact
{

    const std::string LockPicking::POPUP_NAME_CHARACTER_SELECTION_ {
        "PopupName_LockPicking_CharacterSelection"
    };

    const std::string LockPicking::POPUP_NAME_NO_CHARACTER_CAN_PICK_ {
        "PopupName_LockPicking_NoCharactersCanPickALock"
    };

    const std::string LockPicking::POPUP_NAME_ATTEMPTING_ { "PopupName_LockPicking_Attempting" };

    const std::string LockPicking::POPUP_NAME_SUCCESS_ { "PopupName_LockPicking_Success" };

    const std::string LockPicking::POPUP_NAME_TITLE_ARCHIEVED_ {
        "PopupName_LockPicking_TitleAchieved"
    };

    LockPicking::LockPicking()
        : characterPtrOpt_ { boost::none }
    {}

    bool LockPicking::Attempt() const
    {
        if (characterPtrOpt_)
        {
            const auto DID_UNLOCK { creature::Stats::Test(
                characterPtrOpt_.value(),
                creature::Traits::Luck,
                static_cast<creature::Stats::With>(
                    creature::Stats::With::RaceRoleBonus | creature::Stats::With::StandardBonus)) };

            if (DID_UNLOCK)
            {
                PlaySuccessSfx();
            }
            else
            {
                PlayFailureSfx();
            }

            return DID_UNLOCK;
        }
        else
        {
            return false;
        }
    }

    void LockPicking::PopupCharacterSelection(
        const sfml_util::PopupCallback_t::IHandlerPtr_t POPUP_HANDLER_PTR) const
    {
        const auto INVALID_MSGS { MakeInvalidLockPickCharacterMessages() };

        const auto ARE_ANY_INVALID_MSGS_EMPTY { std::any_of(
            std::begin(INVALID_MSGS), std::end(INVALID_MSGS), [](const auto & MSG) {
                return MSG.empty();
            }) };

        if (ARE_ANY_INVALID_MSGS_EMPTY)
        {
            const auto POPUP_INFO { popup::PopupManager::Instance()->CreateCharacterSelectPopupInfo(
                POPUP_NAME_CHARACTER_SELECTION_,
                "Who will attempt to pick the lock?",
                INVALID_MSGS,
                CharacterIndexWhoPrevAttempted()) };

            game::LoopManager::Instance()->PopupWaitBeginSpecific<popup::PopupStageCharacterSelect>(
                POPUP_HANDLER_PTR, POPUP_INFO);
        }
        else
        {
            const auto MSG { std::string(
                                 "There are no characters who can attempt to pick the lock!  ")
                             + "They are all incapable or incapacitated" };

            const auto POPUP_INFO { popup::PopupManager::Instance()->CreatePopupInfo(
                POPUP_NAME_NO_CHARACTER_CAN_PICK_,
                MSG,
                popup::PopupButtons::Continue,
                popup::PopupImage::Regular) };

            game::LoopManager::Instance()->PopupWaitBegin(POPUP_HANDLER_PTR, POPUP_INFO);
        }
    }

    bool LockPicking::HandleCharacterSelectionPopupResponse(
        const sfml_util::PopupCallback_t::IHandlerPtr_t POPUP_HANDLER_PTR,
        const sfml_util::PopupCallback_t::PacketPtr_t & PACKET_PTR)
    {
        if (PACKET_PTR->Response() == popup::ResponseTypes::Select)
        {
            const auto SELECTION_OPT { PACKET_PTR->SelectionOpt() };

            if (SELECTION_OPT < game::Game::Instance()->State().Party().Characters().size())
            {
                characterPtrOpt_
                    = game::Game::Instance()->State().Party().GetAtOrderPos(SELECTION_OPT.value());

                combat::Encounter::Instance()->LockPickCreaturePtr(characterPtrOpt_.value());
                PopupAttempting(POPUP_HANDLER_PTR, characterPtrOpt_.value()->Name());
                return true;
            }
        }

        characterPtrOpt_ = boost::none;
        return false;
    }

    void LockPicking::PopupAttempting(
        const sfml_util::PopupCallback_t::IHandlerPtr_t POPUP_HANDLER_PTR,
        const std::string & CHARACTER_NAME) const
    {
        const auto POPUP_INFO { popup::PopupManager::Instance()->CreateKeepAlivePopupInfo(
            POPUP_NAME_ATTEMPTING_,
            CHARACTER_NAME + " is attempting to pick the lock...",
            4.0f, // the duration of the longest lockpick sfx
            sfml_util::FontManager::Instance()->Size_Normal(),
            popup::PopupButtons::None,
            popup::PopupImage::Regular,
            RandomPickingSfx()) };

        game::LoopManager::Instance()->PopupWaitBeginSpecific<popup::PopupStageGeneric>(
            POPUP_HANDLER_PTR, POPUP_INFO);
    }

    void LockPicking::PopupSuccess(
        const sfml_util::PopupCallback_t::IHandlerPtr_t POPUP_HANDLER_PTR,
        const std::string & NAME_OF_WHAT_OPENED) const
    {
        const auto POPUP_INFO { popup::PopupManager::Instance()->CreateKeepAlivePopupInfo(
            POPUP_NAME_SUCCESS_,
            "\nThe " + NAME_OF_WHAT_OPENED + " Opens!",
            4.0f, // number of seconds before the popup disapears
            sfml_util::FontManager::Instance()->Size_Large(),
            popup::PopupButtons::Continue,
            popup::PopupImage::Regular,
            sfml_util::sound_effect::None) };

        game::LoopManager::Instance()->PopupWaitBeginSpecific<popup::PopupStageGeneric>(
            POPUP_HANDLER_PTR, POPUP_INFO);
    }

    bool LockPicking::HandleAchievementIncrementAndReturnTrueOnNewTitleWithPopup(
        const sfml_util::PopupCallback_t::IHandlerPtr_t POPUP_HANDLER_PTR)
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (!!characterPtrOpt_),
            "interact::LockPicking::HandleAchievementIncrementAndReturnTrueOnNewTitleWithPopup() "
            "was called when LockPicking::characterPtrOpt_ was uninitialized.");

        return stage::HandleAchievementIncrementAndReturnTrueOnNewTitleWithPopup(
            POPUP_HANDLER_PTR,
            POPUP_NAME_TITLE_ARCHIEVED_,
            characterPtrOpt_.value(),
            creature::AchievementType::LocksPicked);
    }

    const std::vector<std::string> LockPicking::MakeInvalidLockPickCharacterMessages() const
    {
        const auto NUM_CHARACTERS { game::Game::Instance()->State().Party().Characters().size() };

        std::vector<std::string> invalidMsgsVec;
        invalidMsgsVec.resize(NUM_CHARACTERS);

        for (std::size_t i(0); i < NUM_CHARACTERS; ++i)
        {
            const auto CREATURE_PTR { game::Game::Instance()->State().Party().GetAtOrderPos(i) };
            if (CREATURE_PTR->IsBeast())
            {
                invalidMsgsVec[i] = "Beasts cannot pick locks";
            }
            else if (CREATURE_PTR->CanTakeAction() == false)
            {
                std::ostringstream ss;

                ss << creature::sex::HeSheIt(CREATURE_PTR->Sex(), true) << " is "
                   << CREATURE_PTR->CanTakeActionStr(false) << ".";

                invalidMsgsVec[i] = ss.str();
            }
            else
            {
                invalidMsgsVec[i].clear();
            }
        }

        return invalidMsgsVec;
    }

    std::size_t LockPicking::CharacterIndexWhoPrevAttempted() const
    {
        const auto NUM_CHARACTERS { game::Game::Instance()->State().Party().Characters().size() };

        const auto PREV_LOCKPICK_CREATURE_PTR_OPT {
            combat::Encounter::Instance()->LockPickCreaturePtrOpt()
        };

        if (PREV_LOCKPICK_CREATURE_PTR_OPT)
        {
            for (std::size_t i(0); i < NUM_CHARACTERS; ++i)
            {
                const auto CREATURE_PTR { game::Game::Instance()->State().Party().GetAtOrderPos(
                    i) };
                if (CREATURE_PTR == PREV_LOCKPICK_CREATURE_PTR_OPT.value())
                {
                    return i;
                }
            }
        }

        return 0;
    }

    sfml_util::sound_effect::Enum LockPicking::RandomPickingSfx() const
    {
        std::vector<sfml_util::sound_effect::Enum> lockPickingSfx
            = { sfml_util::sound_effect::LockPicking1, sfml_util::sound_effect::LockPicking2,
                sfml_util::sound_effect::LockPicking3, sfml_util::sound_effect::LockPicking4,
                sfml_util::sound_effect::LockPicking5, sfml_util::sound_effect::LockPicking6,
                sfml_util::sound_effect::LockPicking7 };

        return misc::Vector::SelectRandom(lockPickingSfx);
    }

    void LockPicking::PlaySuccessSfx() const
    {
        sfml_util::SoundManager::Instance()->SoundEffectPlay(sfml_util::sound_effect::LockUnlock);
    }

    void LockPicking::PlayFailureSfx() const
    {
        sfml_util::SoundManager::Instance()->SoundEffectPlay(
            sfml_util::sound_effect::LockPickingFail);
    }
} // namespace interact
} // namespace heroespath
