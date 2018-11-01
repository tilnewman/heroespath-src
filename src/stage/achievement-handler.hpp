// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_STAGE_ACHIEVEMENT_HANDLER_HPP_INCLUDED
#define HEROESPATH_STAGE_ACHIEVEMENT_HANDLER_HPP_INCLUDED
//
// achievement-handler.hpp
//
#include "creature/achievements.hpp"
#include "creature/creature.hpp"
#include "sfml-util/callback.hpp"

#include <string>

namespace heroespath
{

namespace stage
{
    struct IStage;
    using IStagePtr_t = misc::NotNull<IStage *>;
} // namespace stage

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
        const gui::PopupCallback_t::IHandlerPtr_t POPUP_HANDLER_PTR,
        IStagePtr_t iStagePtr,
        const std::string & POPUP_NAME,
        const creature::CreaturePtr_t CHARACTER_PTR,
        const creature::TitlePtrOpt_t & FROM_TITLE_PTR_OPT,
        const creature::TitlePtr_t TO_TITLE_PTR);

    bool HandleAchievementIncrementAndReturnTrueOnNewTitleWithPopup(
        const gui::PopupCallback_t::IHandlerPtr_t POPUP_HANDLER_PTR,
        IStagePtr_t iStagePtr,
        const std::string & POPUP_NAME,
        const creature::CreaturePtr_t CHARACTER_PTR,
        const creature::AchievementType::Enum ACHIEVEMENT_TYPE);

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_STAGE_ACHIEVEMENT_HANDLER_HPP_INCLUDED
