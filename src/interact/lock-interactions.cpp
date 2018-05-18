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
#include "game/game.hpp"
#include "game/loop-manager.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/vectors.hpp"
#include "popup/popup-manager.hpp"
#include "popup/popup-stage-char-select.hpp"
#include "popup/popup-stage-image-fade.hpp"
#include "sfml-util/gui/title-image-loader.hpp"
#include "stage/achievement-handler.hpp"
#include "state/game-state.hpp"

#include <algorithm>
#include <vector>

namespace heroespath
{
namespace interact
{

    const std::string LockPicking::POPUP_NAME_CHARACTER_SELECTION_{
        "PopupName_LockPicking_CharacterSelection"
    };

    const std::string LockPicking::POPUP_NAME_NO_CHARACTER_CAN_PICK_{
        "PopupName_LockPicking_NoCharactersCanPickALock"
    };

    const std::string LockPicking::POPUP_NAME_ATTEMPTING_{ "PopupName_LockPicking_Attempting" };

    const std::string LockPicking::POPUP_NAME_SUCCESS_{ "PopupName_LockPicking_Success" };

    const std::string LockPicking::POPUP_NAME_TITLE_ARCHIEVED_{
        "PopupName_LockPicking_TitleAchieved"
    };

    LockPicking::LockPicking()
        : characterPtrOpt_{ boost::none }
    {}

    bool LockPicking::Attempt() const
    {
        if (characterPtrOpt_)
        {
            auto const DID_UNLOCK{ creature::Stats::Test(
                characterPtrOpt_.value(),
                stats::Traits::Luck,
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
        const popup::IPopupHandlerPtr_t POPUP_HANDLER_PTR) const
    {
        auto const INVALID_MSGS{ MakeInvalidLockPickCharacterMessages() };

        // clang-format off
        auto const ARE_ANY_INVALID_MSGS_EMPTY{ std::any_of(
            std::begin(INVALID_MSGS),
            std::end(INVALID_MSGS),
            [](auto const & MSG)
            {
                return MSG.empty();
            }) };
        // clang-format on

        if (ARE_ANY_INVALID_MSGS_EMPTY)
        {
            auto const POPUP_INFO{ popup::PopupManager::Instance()->CreateCharacterSelectPopupInfo(
                POPUP_NAME_CHARACTER_SELECTION_,
                "Who will attempt to pick the lock?",
                INVALID_MSGS,
                CharacterIndexWhoPrevAttempted()) };

            game::LoopManager::Instance()->PopupWaitBeginSpecific<popup::PopupStageCharacterSelect>(
                POPUP_HANDLER_PTR, POPUP_INFO);
        }
        else
        {
            auto const MSG{ std::string(
                                "There are no characters who can attempt to pick the lock!  ")
                            + "They are all incapable or incapacitated" };

            auto const POPUP_INFO{ popup::PopupManager::Instance()->CreatePopupInfo(
                POPUP_NAME_NO_CHARACTER_CAN_PICK_,
                MSG,
                popup::PopupButtons::Continue,
                popup::PopupImage::Regular) };

            game::LoopManager::Instance()->PopupWaitBegin(POPUP_HANDLER_PTR, POPUP_INFO);
        }
    }

    bool LockPicking::HandleCharacterSelectionPopupResponse(
        const popup::IPopupHandlerPtr_t POPUP_HANDLER_PTR, const popup::PopupResponse & RESPONSE)
    {
        if (RESPONSE.Response() == popup::ResponseTypes::Select)
        {
            auto const SELECTION{ RESPONSE.Selection() };

            if (SELECTION < game::Game::Instance()->State().Party().Characters().size())
            {
                characterPtrOpt_ = game::Game::Instance()->State().Party().GetAtOrderPos(SELECTION);
                combat::Encounter::Instance()->LockPickCreaturePtr(characterPtrOpt_.value());
                PopupAttempting(POPUP_HANDLER_PTR, characterPtrOpt_->Obj().Name());
                return true;
            }
        }

        characterPtrOpt_ = boost::none;
        return false;
    }

    void LockPicking::PopupAttempting(
        const popup::IPopupHandlerPtr_t POPUP_HANDLER_PTR, const std::string & CHARACTER_NAME) const
    {
        auto const POPUP_INFO{ popup::PopupManager::Instance()->CreateKeepAlivePopupInfo(
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
        const popup::IPopupHandlerPtr_t POPUP_HANDLER_PTR,
        const std::string & NAME_OF_WHAT_OPENED) const
    {
        auto const POPUP_INFO{ popup::PopupManager::Instance()->CreateKeepAlivePopupInfo(
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
        const popup::IPopupHandlerPtr_t POPUP_HANDLER_PTR)
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (!!characterPtrOpt_),
            "interact::LockPicking::HandleAchievementIncrementAndReturnTrueOnNewTitleWithPopup("
            "popupHandler="
                << POPUP_HANDLER_PTR->HandlerName()
                << ") was called when LockPicking::characterPtrOpt_ was uninitialized.");

        return stage::HandleAchievementIncrementAndReturnTrueOnNewTitleWithPopup(
            POPUP_HANDLER_PTR,
            POPUP_NAME_TITLE_ARCHIEVED_,
            characterPtrOpt_.value(),
            creature::AchievementType::LocksPicked);
    }

    const misc::StrVec_t LockPicking::MakeInvalidLockPickCharacterMessages() const
    {
        auto const NUM_CHARACTERS{ game::Game::Instance()->State().Party().Characters().size() };

        misc::StrVec_t invalidMsgsVec;
        invalidMsgsVec.resize(NUM_CHARACTERS);

        for (std::size_t i(0); i < NUM_CHARACTERS; ++i)
        {
            auto const CREATURE_PTR{ game::Game::Instance()->State().Party().GetAtOrderPos(i) };
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
        auto const NUM_CHARACTERS{ game::Game::Instance()->State().Party().Characters().size() };

        auto const PREV_LOCKPICK_CREATURE_PTR_OPT{
            combat::Encounter::Instance()->LockPickCreaturePtrOpt()
        };

        if (PREV_LOCKPICK_CREATURE_PTR_OPT)
        {
            for (std::size_t i(0); i < NUM_CHARACTERS; ++i)
            {
                auto const CREATURE_PTR{ game::Game::Instance()->State().Party().GetAtOrderPos(i) };
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
