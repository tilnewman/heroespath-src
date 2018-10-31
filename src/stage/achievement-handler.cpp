// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// achievement-handler.cpp
//
#include "achievement-handler.hpp"

#include "popup/popup-manager.hpp"
#include "popup/popup-stage-image-fade.hpp"

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
        const sfml_util::PopupCallback_t::IHandlerPtr_t POPUP_HANDLER_PTR,
        IStagePtr_t iStagePtr,
        const std::string & POPUP_NAME,
        const creature::CreaturePtr_t CREATURE_PTR,
        const creature::TitlePtrOpt_t & FROM_TITLE_PTR_OPT,
        const creature::TitlePtr_t TO_TITLE_PTR)
    {
        const auto POPUP_INFO { popup::PopupManager::Instance()->CreateTitleFadePopupInfo(
            POPUP_NAME, CREATURE_PTR, FROM_TITLE_PTR_OPT, TO_TITLE_PTR) };

        iStagePtr->SpawnPopup(POPUP_HANDLER_PTR, POPUP_INFO);
    }

    bool HandleAchievementIncrementAndReturnTrueOnNewTitleWithPopup(
        const sfml_util::PopupCallback_t::IHandlerPtr_t POPUP_HANDLER_PTR,
        IStagePtr_t iStagePtr,
        const std::string & POPUP_NAME,
        const creature::CreaturePtr_t CREATURE_PTR,
        const creature::AchievementType::Enum ACHIEVEMENT_TYPE)
    {
        creature::TitlePtrOpt_t fromTitlePtrOpt { boost::none };
        creature::TitlePtrOpt_t toTitlePtrOpt { boost::none };

        if (IncrementAchievementAndReturnTrueIfProcessingRequired(
                CREATURE_PTR, ACHIEVEMENT_TYPE, fromTitlePtrOpt, toTitlePtrOpt))
        {
            ApplyTitleChangesToCreature(CREATURE_PTR, toTitlePtrOpt.get());

            TitleTransitionPopup(
                POPUP_HANDLER_PTR,
                iStagePtr,
                POPUP_NAME,
                CREATURE_PTR,
                fromTitlePtrOpt,
                toTitlePtrOpt.get());

            return true;
        }
        else
        {
            return false;
        }
    }

} // namespace stage
} // namespace heroespath
