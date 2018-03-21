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
// lock-interactions.cpp
//
#include "lock-interactions.hpp"
#include "combat/encounter.hpp"
#include "creature/creature.hpp"
#include "creature/stats.hpp"
#include "game/game.hpp"
#include "game/loop-manager.hpp"
#include "misc/vectors.hpp"
#include "player/character.hpp"
#include "player/party.hpp"
#include "popup/popup-manager.hpp"
#include "popup/popup-stage-char-select.hpp"
#include "popup/popup-stage-image-fade.hpp"
#include "sfml-util/gui/title-image-manager.hpp"
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
        : characterPtr_{ nullptr }
    {}

    bool LockPicking::Attempt() const
    {
        if (characterPtr_ != nullptr)
        {
            auto const DID_UNLOCK{ creature::Stats::Test(
                characterPtr_,
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

    void LockPicking::PopupCharacterSelection(popup::IPopupHandler_t * const popupHandlerPtr) const
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
        //clang-format on

        if (ARE_ANY_INVALID_MSGS_EMPTY)
        {
            auto const POPUP_INFO{ popup::PopupManager::Instance()->CreateCharacterSelectPopupInfo(
                POPUP_NAME_CHARACTER_SELECTION_,
                "Who will attempt to pick the lock?",
                INVALID_MSGS,
                CharacterIndexWhoPrevAttempted()) };

            game::LoopManager::Instance()->PopupWaitBeginSpecific<popup::PopupStageCharacterSelect>(
                popupHandlerPtr, POPUP_INFO);
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

            game::LoopManager::Instance()->PopupWaitBegin(popupHandlerPtr, POPUP_INFO);
        }
    }

    bool LockPicking::HandleCharacterSelectionPopupResponse(
        popup::IPopupHandler_t * const popupHandlerPtr, const popup::PopupResponse & RESPONSE)
    {
        if (RESPONSE.Response() == popup::ResponseTypes::Select)
        {
            auto const SELECTION{ RESPONSE.Selection() };

            if (SELECTION < game::Game::Instance()->State().Party().Characters().size())
            {
                characterPtr_ = game::Game::Instance()->State().Party().GetAtOrderPos(SELECTION);
                combat::Encounter::Instance()->LockPickCreaturePtr(characterPtr_);
                PopupAttempting(popupHandlerPtr, characterPtr_->Name());
                return true;
            }
        }

        characterPtr_ = nullptr;
        return false;
    }

    void LockPicking::PopupAttempting(
        popup::IPopupHandler_t * const popupHandlerPtr, const std::string & CHARACTER_NAME) const
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
            popupHandlerPtr, POPUP_INFO);
    }

    void LockPicking::PopupSuccess(
        popup::IPopupHandler_t * const popupHandlerPtr,
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
            popupHandlerPtr, POPUP_INFO);
    }

    bool LockPicking::HandleTitleAchievement(popup::IPopupHandler_t * const popupHandlerPtr)
    {
        if (nullptr != characterPtr_)
        {
            auto const FROM_TITLE_PTR{ characterPtr_->GetAchievements().GetCurrentTitle(
                creature::AchievementType::LocksPicked) };

            auto const TO_TITLE_PTR{ characterPtr_->GetAchievements().Increment(
                creature::AchievementType::LocksPicked) };

            if ((TO_TITLE_PTR != nullptr) && (FROM_TITLE_PTR != TO_TITLE_PTR))
            {
                TO_TITLE_PTR->Change(characterPtr_);

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
                    POPUP_NAME_TITLE_ARCHIEVED_,
                    characterPtr_,
                    FROM_TITLE_PTR,
                    TO_TITLE_PTR,
                    &fromTexture,
                    &toTexture) };

                game::LoopManager::Instance()->PopupWaitBeginSpecific<popup::PopupStageImageFade>(
                    popupHandlerPtr, POPUP_INFO);

                return true;
            }
        }

        return false;
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

        auto const PREV_LOCKPICK_CREATURE_PTR{
            combat::Encounter::Instance()->LockPickCreaturePtr()
        };

        if (PREV_LOCKPICK_CREATURE_PTR != nullptr)
        {
            for (std::size_t i(0); i < NUM_CHARACTERS; ++i)
            {
                auto const CREATURE_PTR{ game::Game::Instance()->State().Party().GetAtOrderPos(i) };
                if (CREATURE_PTR == PREV_LOCKPICK_CREATURE_PTR)
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
}
}
