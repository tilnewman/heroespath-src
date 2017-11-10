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
#ifndef HEROESPATH_GAME_COMBAT_COMBATTEXT_HPP_INCLUDED
#define HEROESPATH_GAME_COMBAT_COMBATTEXT_HPP_INCLUDED
//
// combat-text.hpp
//
#include "combat/turn-action-info.hpp"
#include "combat/fight-results.hpp"

#include <string>
#include <vector>
#include <memory>
#include <utility>


namespace heroespath
{
namespace item
{
    class Item;
    using ItemPtr_t = Item *;
}
namespace creature
{
    //forward declarations
    class Creature;
    using CreaturePtr_t = Creature *;
    using CreatureCPtr_t = const Creature *;
    using CreaturePtrC_t = Creature * const;
    using CreatureCPtrC_t = const Creature * const;
    using CreaturePVec_t = std::vector<CreaturePtr_t>;
}
namespace combat
{
    //forward declarations
    class CombatDisplay;
    using CombatDisplayPtr_t = CombatDisplay *;
    using CombatDisplayCPtr_t = const CombatDisplay *;
    using CombatDisplayPtrC_t = CombatDisplay * const;
    using CombatDisplayCPtrC_t = const CombatDisplay * const;


    //A collection of functions that generate text used during combat.
    struct Text
    {
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_ATTACK_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_FIGHT_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_CAST_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_PLAY_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_ADVANCE_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_RETREAT_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_BLOCK_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_SKIP_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_FLY_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_LAND_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_ROAR_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_POUNCE_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_RUN_;

        static const std::string MouseOverTextAttackStr(
            const creature::CreaturePtrC_t, CombatDisplayCPtrC_t);

        static const std::string MouseOverTextFightStr(
            const creature::CreaturePtrC_t, CombatDisplayCPtrC_t);

        static const std::string MouseOverTextCastStr(
            const creature::CreaturePtrC_t, CombatDisplayCPtrC_t);

        static const std::string MouseOverTextPlayStr(
            const creature::CreaturePtrC_t, CombatDisplayCPtrC_t);

        static const std::string MouseOverTextAdvanceStr(
            const creature::CreaturePtrC_t, CombatDisplayCPtrC_t);

        static const std::string MouseOverTextRetreatStr(
            const creature::CreaturePtrC_t, CombatDisplayCPtrC_t);

        static const std::string MouseOverTextBlockStr(
            creature::CreaturePtrC_t, CombatDisplayCPtrC_t);

        static const std::string MouseOverTextFlyStr(
            const creature::CreaturePtrC_t, CombatDisplayCPtrC_t);

        static const std::string MouseOverTextLandStr(
            const creature::CreaturePtrC_t, CombatDisplayCPtrC_t);

        static const std::string MouseOverTextRoarStr(
            const creature::CreaturePtrC_t, CombatDisplayCPtrC_t);

        static const std::string MouseOverTextPounceStr(
            const creature::CreaturePtrC_t, CombatDisplayCPtrC_t);

        static const std::string MouseOverTextRunStr(
            const creature::CreaturePtrC_t, CombatDisplayCPtrC_t);

        static const std::string MouseOverTextSkipStr(
            const creature::CreaturePtrC_t, CombatDisplayCPtrC_t);

        static const std::string ActionText(
            const creature::CreaturePtr_t CREATURE_ATTACKING_PTR,
            const TurnActionInfo &        TURN_ACTION_INFO,
            const FightResult &           FIGHT_RESULT,
            const bool                    WILL_USE_NAME       = false,
            const bool                    IS_STATUS_VERSION   = false,
            const bool                    IS_PREAMBLE_VERSION = false);

        static const std::string ActionTextIndexed(
            const creature::CreaturePtr_t CREATURE_ATTACKING_PTR,
            const TurnActionInfo &        TURN_ACTION_INFO,
            const FightResult &           FIGHT_RESULT,
            const bool                    WILL_USE_NAME,
            const std::size_t             EFFECT_INDEX,
            const std::size_t             HIT_INDEX,
            bool &                        wasCollapsed);

        static const std::string WeaponActionVerb(
            const item::ItemPtr_t WEAPON_PTR,
            const bool            WILL_APPEND_ING = false);

        static const std::string AttackDescriptionStatusVersion(
            const FightResult & FIGHT_RESULT);

        static const std::string AttackDescriptionPreambleVersion(
            const FightResult & FIGHT_RESULT);

        static const std::string AttackDescriptionFullVersion(
            const FightResult & FIGHT_RESULT,
            const std::size_t   EFFECT_INDEX,
            const std::size_t   HIT_INDEX);

        static const std::string CastDescriptionStatusVersion(
            const TurnActionInfo & TURN_ACTION_INFO,
            const FightResult &    FIGHT_RESULT);

        static const std::string CastDescriptionPreambleVersion(
            const TurnActionInfo & TURN_ACTION_INFO,
            const FightResult &    FIGHT_RESULT);

        static const std::string CastDescriptionFullVersion(
            const creature::CreaturePtr_t CREATURE_CASTING_PTR,
            const TurnActionInfo &        TURN_ACTION_INFO,
            const FightResult &           FIGHT_RESULT,
            const std::size_t             EFFECT_INDEX,
            const std::size_t             HIT_INDEX,
            bool &                        wasCollapsed);

        static const std::string PlaySongDescriptionStatusVersion(
            const TurnActionInfo & TURN_ACTION_INFO,
            const FightResult &    FIGHT_RESULT);

        static const std::string PlaySongDescriptionPreambleVersion(
            const TurnActionInfo & TURN_ACTION_INFO,
            const FightResult &    FIGHT_RESULT);

        static const std::string PlaySongDescriptionFullVersion(
            const creature::CreaturePtr_t CREATURE_PLAYINGING_PTR,
            const TurnActionInfo &        TURN_ACTION_INFO,
            const FightResult &           FIGHT_RESULT,
            const std::size_t             EFFECT_INDEX,
            const std::size_t             HIT_INDEX,
            bool &                        wasCollapsed);

        static const std::string RoarDescriptionFullVersion(
            const creature::CreaturePtr_t CREATURE_ROARING_PTR,
            const TurnActionInfo &        TURN_ACTION_INFO,
            const FightResult &           FIGHT_RESULT,
            const std::size_t             EFFECT_INDEX,
            const std::size_t             HIT_INDEX,
            bool &                        wasCollapsed);

        static const std::string TrapDescriptionFullVersion(
            const creature::CreaturePtr_t CREATURE_UNLOCKING_PTR,
            const FightResult &           FIGHT_RESULT,
            const std::size_t             EFFECT_INDEX,
            const std::size_t             HIT_INDEX);

        static const std::string WeaponActionVerbList(
            const HitInfoVec_t & HIT_INFO_VEC,
            const bool           WILL_SKIP_MISSES);

        static const std::string CountPhrase(const HitInfoVec_t &);

        static const std::string CountPhrase(const std::size_t);

        static const std::string ConditionNameList(
            const creature::CondEnumVec_t & CONDITIONS_VEC,
            const bool                           WILL_EXCLUDE_DEAD,
            const std::size_t                    NUM_TO_LIST);

        static const std::string AttackConditionsSummaryList(
            const CreatureEffect & CREATURE_EFFECT);

        static const std::string NamePhrase(const creature::CreaturePtr_t);

        static const std::string InitialCombatStatusMessagePrefix();

        static const FightResultSummary SummarizeFightResult(
            const creature::CreaturePtr_t CREATURE_INITIATING_PTR,
            const FightResult &);

        static bool SummarizeCreatureEffect(FightResultSummary &   frs,
                                            const CreatureEffect & CREATURE_EFFECT);
    };

}
}

#endif //HEROESPATH_GAME_COMBAT_COMBATTEXT_HPP_INCLUDED
