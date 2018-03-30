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
// achievement-handler.cpp
//
#include "achievement-handler.hpp"
#include "game/loop-manager.hpp"
#include "popup/popup-manager.hpp"
#include "popup/popup-stage-image-fade.hpp"
#include "sfml-util/gui/title-image-manager.hpp"

namespace heroespath
{
namespace stage
{
    bool IncrementAchievementAndReturnTrueIfProcessingRequired(
        const creature::CreaturePtr_t CHARACTER_PTR,
        const creature::AchievementType::Enum ACHIEVEMENT_TYPE,
        creature::TitlePtrOpt_t & fromTitlePtrOpt,
        creature::TitlePtrOpt_t & toTitlePtrOpt)
    {
        fromTitlePtrOpt = CHARACTER_PTR->GetAchievements().GetCurrentTitle(ACHIEVEMENT_TYPE);
        toTitlePtrOpt = CHARACTER_PTR->GetAchievements().Increment(ACHIEVEMENT_TYPE);

        if (toTitlePtrOpt)
        {
            if ((!fromTitlePtrOpt) || (fromTitlePtrOpt->Ptr() != toTitlePtrOpt->Ptr()))
            {
                return true;
            }
        }

        return false;
    }

    void ApplyTitleChangesToCreature(
        const creature::CreaturePtr_t CHARACTER_PTR, const creature::TitlePtr_t & TITLE_PTR)
    {
        TITLE_PTR->Change(CHARACTER_PTR);
    }

    void TitleTransitionPopup(
        popup::IPopupHandler_t * const popupHandlerPtr,
        const std::string & POPUP_NAME,
        const creature::CreaturePtr_t CREATURE_PTR,
        const creature::TitlePtrOpt_t & FROM_TITLE_PTR_OPT,
        const creature::TitlePtr_t TO_TITLE_PTR)
    {
        boost::optional<sf::Texture> fromTextureOpt;
        if (FROM_TITLE_PTR_OPT)
        {
            fromTextureOpt = sf::Texture();

            sfml_util::gui::TitleImageManager::Instance()->Get(
                fromTextureOpt.get(), FROM_TITLE_PTR_OPT->Obj().Which());
        }

        sf::Texture toTexture;

        sfml_util::gui::TitleImageManager::Instance()->Get(toTexture, TO_TITLE_PTR->Which());

        auto const POPUP_INFO{ popup::PopupManager::Instance()->CreateTitleFadePopupInfo(
            POPUP_NAME,
            CREATURE_PTR,
            FROM_TITLE_PTR_OPT,
            TO_TITLE_PTR,
            fromTextureOpt,
            toTexture) };

        game::LoopManager::Instance()->PopupWaitBeginSpecific<popup::PopupStageImageFade>(
            popupHandlerPtr, POPUP_INFO);
    }

    bool HandleAchievementIncrementAndReturnTrueOnNewTitleWithPopup(
        popup::IPopupHandler_t * const popupHandlerPtr,
        const std::string & POPUP_NAME,
        const creature::CreaturePtr_t CREATURE_PTR,
        const creature::AchievementType::Enum ACHIEVEMENT_TYPE)
    {
        creature::TitlePtrOpt_t fromTitlePtrOpt{ boost::none };
        creature::TitlePtrOpt_t toTitlePtrOpt{ boost::none };

        if (IncrementAchievementAndReturnTrueIfProcessingRequired(
                CREATURE_PTR, ACHIEVEMENT_TYPE, fromTitlePtrOpt, toTitlePtrOpt))
        {
            ApplyTitleChangesToCreature(CREATURE_PTR, toTitlePtrOpt.get());

            TitleTransitionPopup(
                popupHandlerPtr, POPUP_NAME, CREATURE_PTR, fromTitlePtrOpt, toTitlePtrOpt.get());

            return true;
        }
        else
        {
            return false;
        }
    }
}
}
