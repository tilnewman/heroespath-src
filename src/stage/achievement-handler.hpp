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
#ifndef HEROESPATH_STAGE_ACHIEVEMENT_HANDLER_HPP_INCLUDED
#define HEROESPATH_STAGE_ACHIEVEMENT_HANDLER_HPP_INCLUDED
//
// achievement-handler.hpp
//
#include "creature/achievements.hpp"
#include "creature/creature.hpp"
#include "popup/i-popup-callback.hpp"
#include <string>

namespace heroespath
{
namespace stage
{

    bool IncrementAchievementAndReturnTrueIfProcessingRequired(
        const creature::CreaturePtr_t CHARACTER_PTR,
        const creature::AchievementType::Enum ACHIEVEMENT_TYPE,
        creature::TitlePtrOpt_t & fromTitlePtrOpt,
        creature::TitlePtrOpt_t & toTitlePtrOpt);

    void ApplyTitleChangesToCreature(
        const creature::CreaturePtr_t CHARACTER_PTR, const creature::TitlePtr_t & TITLE_PTR);

    void TitleTransitionPopup(
        const popup::IPopupHandlerPtr_t POPUP_HANDLER_PTR,
        const std::string & POPUP_NAME,
        const creature::CreaturePtr_t CHARACTER_PTR,
        const creature::TitlePtrOpt_t & FROM_TITLE_PTR_OPT,
        const creature::TitlePtr_t TO_TITLE_PTR);

    bool HandleAchievementIncrementAndReturnTrueOnNewTitleWithPopup(
        const popup::IPopupHandlerPtr_t POPUP_HANDLER_PTR,
        const std::string & POPUP_NAME,
        const creature::CreaturePtr_t CHARACTER_PTR,
        const creature::AchievementType::Enum ACHIEVEMENT_TYPE);

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_STAGE_ACHIEVEMENT_HANDLER_HPP_INCLUDED
