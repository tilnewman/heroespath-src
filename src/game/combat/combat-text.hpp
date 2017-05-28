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
#ifndef GAME_COMBAT_COMBATTEXT_HPP_INCLUDED
#define GAME_COMBAT_COMBATTEXT_HPP_INCLUDED
//
// combat-text.hpp
//
#include "game/combat/turn-action-info.hpp"
#include "game/combat/fight-results.hpp"

#include <string>
#include <vector>
#include <memory>
#include <utility>


namespace game
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
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_ADVANCE_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_RETREAT_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_BLOCK_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_SKIP_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_FLY_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_LAND_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_ROAR_;
        static const std::string TBOX_BUTTON_MOUSEHOVER_TEXT_POUNCE_;

        static const std::string MouseOverTextAttackStr(const creature::CreaturePtrC_t, CombatDisplayCPtrC_t);
        static const std::string MouseOverTextFightStr(const creature::CreaturePtrC_t, CombatDisplayCPtrC_t);
        static const std::string MouseOverTextCastStr(const creature::CreaturePtrC_t, CombatDisplayCPtrC_t);
        static const std::string MouseOverTextAdvanceStr(const creature::CreaturePtrC_t, CombatDisplayCPtrC_t);
        static const std::string MouseOverTextRetreatStr(const creature::CreaturePtrC_t, CombatDisplayCPtrC_t);
        static const std::string MouseOverTextBlockStr(creature::CreaturePtrC_t, CombatDisplayCPtrC_t);
        static const std::string MouseOverTextFlyStr(const creature::CreaturePtrC_t, CombatDisplayCPtrC_t);
        static const std::string MouseOverTextLandStr(const creature::CreaturePtrC_t, CombatDisplayCPtrC_t);
        static const std::string MouseOverTextRoarStr(const creature::CreaturePtrC_t, CombatDisplayCPtrC_t);
        static const std::string MouseOverTextPounceStr(const creature::CreaturePtrC_t, CombatDisplayCPtrC_t);

        static const std::string ActionText(creature::CreatureCPtrC_t CREATURE_ATTACKING_CPTRC,
                                            const TurnActionInfo &    TURN_ACTION_INFO,
                                            const FightResult &       FIGHT_RESULT,
                                            const bool                WILL_USE_NAME       = false,
                                            const bool                IS_STATUS_VERSION   = false,
                                            const bool                IS_PREAMBLE_VERSION = false,
                                            const std::size_t         EFFECT_INDEX = 0,
                                            const std::size_t         HIT_INDEX = 0);

        static const std::string WeaponActionVerb(const item::ItemPtr_t WEAPON_PTR,
                                                  const bool            WILL_APPEND_ING = false);

        static const std::string HitDescription(const HitInfo & HIT_INFO);

        static const std::string DodgeDescription(const HitInfo & HIT_INFO);

        static const std::string AttackDescriptionStatusVersion(const FightResult & FIGHT_RESULT);

        static const std::string AttackDescriptionPreambleVersion(const FightResult & FIGHT_RESULT);

        static const std::string AttackDescriptionFullVersion(const FightResult & FIGHT_RESULT,
                                                              const std::size_t   EFFECT_INDEX,
                                                              const std::size_t   HIT_INDEX);

        static const std::string CastDescriptionStatusVersion(const TurnActionInfo & TURN_ACTION_INFO,
                                                              const FightResult &    FIGHT_RESULT);

        static const std::string CastDescriptionPreambleVersion(const TurnActionInfo & TURN_ACTION_INFO,
                                                                const FightResult &    FIGHT_RESULT);

        static const std::string CastDescriptionFullVersion(const TurnActionInfo & TURN_ACTION_INFO,
                                                            const FightResult &    FIGHT_RESULT,
                                                            const std::size_t      EFFECT_INDEX,
                                                            const std::size_t      HIT_INDEX);

        static const std::string WeaponActionVerbList(const FightResult & FIGHT_RESULT,
                                                      const bool          WILL_SKIP_MISSES);

        static const std::string CountPhrase(const std::size_t);

        static const std::string ConditionNameList(const creature::ConditionEnumVec_t & CONDITIONS_VEC,
                                                   const bool                           WILL_EXCLUDE_DEAD,
                                                   const std::size_t                    NUM_TO_LIST);

        static const std::string AttackConditionsSummaryList(const FightResult & FIGHT_RESULT);

        //requires FIGHT_RESULT is not empty
        static const std::string FirstTargetNamePhrase(const FightResult & FIGHT_RESULT);

        static const std::string InitialCombatStatusMessagePrefix();
    };

}
}

#endif //GAME_COMBAT_COMBATTEXT_HPP_INCLUDED
