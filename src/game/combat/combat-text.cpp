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
// combat-text.cpp
//
#include "combat-text.hpp"

#include "game/log-macros.hpp"
#include "game/creature/creature.hpp"
#include "game/creature/conditions.hpp"
#include "game/creature/condition-algorithms.hpp"
#include "game/combat/combat-display.hpp"
#include "game/combat/encounter.hpp"
#include "game/item/item.hpp"
#include "game/item/algorithms.hpp"
#include "game/spell/spell-base.hpp"
#include "game/song/song.hpp"

#include "misc/vectors.hpp"
#include "misc/boost-string-includes.hpp"

#include <boost/algorithm/algorithm.hpp>


namespace game
{
namespace combat
{

    const std::string Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ATTACK_  ("Click or press 'enter' to automatically fight the enemy who appears weakest.");
    const std::string Text::TBOX_BUTTON_MOUSEHOVER_TEXT_FIGHT_   ("Click or press 'f' to fight a creature of your choice.");
    const std::string Text::TBOX_BUTTON_MOUSEHOVER_TEXT_CAST_    ("Click or press 'c' to cast a spell.");
    const std::string Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ADVANCE_ ("Click or press 'a' to move forward.");
    const std::string Text::TBOX_BUTTON_MOUSEHOVER_TEXT_RETREAT_ ("Click or press 'r' to move backward.");
    const std::string Text::TBOX_BUTTON_MOUSEHOVER_TEXT_BLOCK_   ("Click, press 'b', or press the spacebar to prepare for attack and prevent enemies from passing.");
    const std::string Text::TBOX_BUTTON_MOUSEHOVER_TEXT_SKIP_    ("Click, press 's', or press the spacebar to take no action and skip this turn.");
    const std::string Text::TBOX_BUTTON_MOUSEHOVER_TEXT_FLY_     ("Click or press 'y' to fly into the air.");
    const std::string Text::TBOX_BUTTON_MOUSEHOVER_TEXT_LAND_    ("Click or press 'l' to stop flying and land.");
    const std::string Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ROAR_    ("Click or press 'r' to roar and attempt to panic enemies.");
    const std::string Text::TBOX_BUTTON_MOUSEHOVER_TEXT_POUNCE_  ("Click or press 'p' to attempt to pounce on an enemy.");


    const std::string Text::MouseOverTextAttackStr(const creature::CreaturePtrC_t CREATURE_CPTRC, CombatDisplayCPtrC_t COMBAT_DISPLAY_CPTRC)
    {
        auto const CAN_TAKE_ACTION_STR(CREATURE_CPTRC->CanTakeActionStr());
        if (CAN_TAKE_ACTION_STR.empty() == false)
            return CAN_TAKE_ACTION_STR;

        creature::CreaturePVec_t ignoredPVec;
        if (COMBAT_DISPLAY_CPTRC->FindCreaturesThatCanBeAttackedOfType(ignoredPVec, CREATURE_CPTRC, ! CREATURE_CPTRC->IsPlayerCharacter()) == 0)
            return "Cannot Attack because there are no enemies in range.";

        if (CREATURE_CPTRC->HasWeaponsHeld() == false)
            return "Cannot Attack because no weapons are held.";

        return TBOX_BUTTON_MOUSEHOVER_TEXT_ATTACK_;
    }


    const std::string Text::MouseOverTextFightStr(const creature::CreaturePtrC_t CREATURE_CPTRC, CombatDisplayCPtrC_t COMBAT_DISPLAY_CPTRC)
    {
        auto const CAN_TAKE_ACTION_STR(CREATURE_CPTRC->CanTakeActionStr());
        if (CAN_TAKE_ACTION_STR.empty() == false)
            return CAN_TAKE_ACTION_STR;

        creature::CreaturePVec_t ignoredPVec;
        if (COMBAT_DISPLAY_CPTRC->FindCreaturesThatCanBeAttackedOfType(ignoredPVec, CREATURE_CPTRC, !CREATURE_CPTRC->IsPlayerCharacter()) == 0)
            return "Cannot Fight because there are no enemies in range.";

        if (CREATURE_CPTRC->HasWeaponsHeld() == false)
            return "Cannot Fight because no weapons are held.";

        return TBOX_BUTTON_MOUSEHOVER_TEXT_FIGHT_;
    }


    const std::string Text::MouseOverTextCastStr(const creature::CreaturePtrC_t CREATURE_CPTRC, CombatDisplayCPtrC_t)
    {
        auto const CAN_CAST_STR(CREATURE_CPTRC->CanCastSpellsStr());
        if (CAN_CAST_STR.empty() == false)
            return CAN_CAST_STR;

        return TBOX_BUTTON_MOUSEHOVER_TEXT_CAST_;
    }


    const std::string Text::MouseOverTextAdvanceStr(const creature::CreaturePtrC_t CREATURE_CPTRC, CombatDisplayCPtrC_t COMBAT_DISPLAY_CPTRC)
    {
        auto const CAN_TAKE_ACTION_STR(CREATURE_CPTRC->CanTakeActionStr());
        if (CAN_TAKE_ACTION_STR.empty() == false)
            return CAN_TAKE_ACTION_STR;

        auto const CAN_ADVANCE_STR(COMBAT_DISPLAY_CPTRC->CanAdvanceOrRetreat(CREATURE_CPTRC, true));
        if (CAN_ADVANCE_STR.empty() == false)
            return CAN_ADVANCE_STR;

        return TBOX_BUTTON_MOUSEHOVER_TEXT_ADVANCE_;
    }


    const std::string Text::MouseOverTextRetreatStr(const creature::CreaturePtrC_t CREATURE_CPTRC, CombatDisplayCPtrC_t COMBAT_DISPLAY_CPTRC)
    {
        auto const CAN_TAKE_ACTION_STR(CREATURE_CPTRC->CanTakeActionStr());
        if (CAN_TAKE_ACTION_STR.empty() == false)
            return CAN_TAKE_ACTION_STR;

        auto const CAN_RETREAT_STR(COMBAT_DISPLAY_CPTRC->CanAdvanceOrRetreat(CREATURE_CPTRC, false));
        if (CAN_RETREAT_STR.empty() == false)
            return CAN_RETREAT_STR;

        return TBOX_BUTTON_MOUSEHOVER_TEXT_RETREAT_;
    }


    const std::string Text::MouseOverTextBlockStr(const creature::CreaturePtrC_t CREATURE_CPTRC, CombatDisplayCPtrC_t)
    {
        auto const CAN_TAKE_ACTION_STR(CREATURE_CPTRC->CanTakeActionStr());
        if (CAN_TAKE_ACTION_STR.empty() == false)
            return CAN_TAKE_ACTION_STR;

        return TBOX_BUTTON_MOUSEHOVER_TEXT_BLOCK_;
    }


    const std::string Text::MouseOverTextFlyStr(const creature::CreaturePtrC_t CREATURE_CPTRC, CombatDisplayCPtrC_t)
    {
        auto const CAN_TAKE_ACTION_STR(CREATURE_CPTRC->CanTakeActionStr());
        if (CAN_TAKE_ACTION_STR.empty() == false)
            return CAN_TAKE_ACTION_STR;

        if (Encounter::Instance()->GetTurnInfoCopy(CREATURE_CPTRC).GetIsFlying())
            return "Cannot Fly because already flying.";

        if (CREATURE_CPTRC->CanFly() == false)
            return CREATURE_CPTRC->Race().Name() + "s cannot fly.";

        return TBOX_BUTTON_MOUSEHOVER_TEXT_FLY_;
    }


    const std::string Text::MouseOverTextLandStr(const creature::CreaturePtrC_t CREATURE_CPTRC, CombatDisplayCPtrC_t)
    {
        auto const CAN_TAKE_ACTION_STR(CREATURE_CPTRC->CanTakeActionStr());
        if (CAN_TAKE_ACTION_STR.empty() == false)
            return CAN_TAKE_ACTION_STR;

        if (Encounter::Instance()->GetTurnInfoCopy(CREATURE_CPTRC).GetIsFlying() == false)
            return "Cannot Land because not currently flying.";

        return TBOX_BUTTON_MOUSEHOVER_TEXT_FLY_;
    }


    const std::string Text::MouseOverTextRoarStr(const creature::CreaturePtrC_t CREATURE_CPTRC, CombatDisplayCPtrC_t)
    {
        auto const CAN_TAKE_ACTION_STR(CREATURE_CPTRC->CanTakeActionStr());
        if (CAN_TAKE_ACTION_STR.empty() == false)
            return CAN_TAKE_ACTION_STR;

        if (CREATURE_CPTRC->IsBeast() == false)
            return "Cannot Roar because " + CREATURE_CPTRC->Name() + " is not a beast.";

        return TBOX_BUTTON_MOUSEHOVER_TEXT_ROAR_;
    }


    const std::string Text::MouseOverTextPounceStr(const creature::CreaturePtrC_t CREATURE_CPTRC, CombatDisplayCPtrC_t)
    {
        auto const CAN_TAKE_ACTION_STR(CREATURE_CPTRC->CanTakeActionStr());
        if (CAN_TAKE_ACTION_STR.empty() == false)
            return CAN_TAKE_ACTION_STR;

        if (CREATURE_CPTRC->IsBeast() == false)
            return "Cannot Pounce because " + CREATURE_CPTRC->Name() + " is a not a beast.";

        return TBOX_BUTTON_MOUSEHOVER_TEXT_ROAR_;
    }


    const std::string Text::ActionText(const creature::CreaturePtr_t  CREATURE_ATTACKING_PTR,
                                       const combat::TurnActionInfo & TURN_ACTION_INFO,
                                       const combat::FightResult &    FIGHT_RESULT,
                                       const bool                     WILL_USE_NAME,
                                       const bool                     IS_STATUS_VERSION,
                                       const bool                     IS_PREAMBLE_VERSION)
    {
        std::ostringstream ss;

        if (WILL_USE_NAME)
        {
            ss << CREATURE_ATTACKING_PTR->Name();
        }
        else
        {
            ss << creature::sex::HeSheIt(CREATURE_ATTACKING_PTR->Sex(), true);
        }

        ss << " ";

        switch (TURN_ACTION_INFO.Action())
        {
            case combat::TurnAction::Attack:
            {
                if (IS_PREAMBLE_VERSION)
                {
                    ss << AttackDescriptionPreambleVersion(FIGHT_RESULT);
                }
                else if (IS_STATUS_VERSION)
                {
                    ss << AttackDescriptionStatusVersion(FIGHT_RESULT);
                }

                break;
            }

            case combat::TurnAction::Cast:
            {
                if (IS_PREAMBLE_VERSION)
                {
                    ss << CastDescriptionPreambleVersion(TURN_ACTION_INFO, FIGHT_RESULT);
                }
                else if (IS_STATUS_VERSION)
                {
                    ss << CastDescriptionStatusVersion(TURN_ACTION_INFO, FIGHT_RESULT);
                }
                
                break;
            }

            case combat::TurnAction::PlaySong:
            {
                if (IS_PREAMBLE_VERSION)
                {
                    ss << PlaySongDescriptionPreambleVersion(TURN_ACTION_INFO, FIGHT_RESULT);
                }
                else if (IS_STATUS_VERSION)
                {
                    ss << PlaySongDescriptionStatusVersion(TURN_ACTION_INFO, FIGHT_RESULT);
                }
                
                break;
            }

            case combat::TurnAction::ChangeWeapon:
            {
                auto const CURRENT_WEAPONS_SVEC{ CREATURE_ATTACKING_PTR->CurrentWeaponsCopy() };

                if (CURRENT_WEAPONS_SVEC.empty())
                {
                    ss << "fumbles around looking for a weapon";
                }
                else
                {
                    ss << "changes weapon";

                    if (CURRENT_WEAPONS_SVEC.size() > 1)
                    {
                        ss << "s";
                    }

                    ss << " to " << item::Algorithms::Names(CURRENT_WEAPONS_SVEC, false, true);
                }

                break;
            }

            case combat::TurnAction::LandPounce:
            {
                std::vector<std::string> strVec{ "surges", "charages", "leaps", "rushes", "springs" };
                ss << strVec.at(static_cast<std::size_t>(misc::random::Int(static_cast<int>(strVec.size()) - 1)));

                if (misc::random::Bool())
                {
                    ss << " forward";
                }

                ss << " Pouncing ";

                ss << "LandPounce Action Text TODO";
                break;
            }

            case combat::TurnAction::SkyPounce:
            {
                ss << "SkyPounce Action Text TODO";
                break;
            }

            case combat::TurnAction::Roar:
            {
                ss << "Roar Action Text TODO";
                break;
            }

            case combat::TurnAction::Fly:
            case combat::TurnAction::Land:
            case combat::TurnAction::Advance:
            case combat::TurnAction::Retreat:
            case combat::TurnAction::Block:
            {
                ss << combat::TurnAction::Name(TURN_ACTION_INFO.Action());
                break;
            }

            case combat::TurnAction::Nothing:
            case combat::TurnAction::Count:
            default:
            {
                ss << "does nothing";
                break;
            }
        }

        return ss.str();
    }


    const std::string Text::ActionTextIndexed(
        const creature::CreaturePtr_t  CREATURE_ATTACKING_PTR,
        const combat::TurnActionInfo & TURN_ACTION_INFO,
        const combat::FightResult &    FIGHT_RESULT,
        const bool                     WILL_USE_NAME,
        const std::size_t              EFFECT_INDEX,
        const std::size_t              HIT_INDEX,
        bool &                         wasCollapsed)
    {
        wasCollapsed = false;

        auto const TURN_ACTION{ TURN_ACTION_INFO.Action() };

        if (TURN_ACTION == combat::TurnAction::Attack)
        {
            std::ostringstream ss;

            if (WILL_USE_NAME)
            {
                ss << CREATURE_ATTACKING_PTR->Name();
            }
            else
            {
                ss << creature::sex::HeSheIt(CREATURE_ATTACKING_PTR->Sex(), true);
            }

            ss << " " << AttackDescriptionFullVersion(FIGHT_RESULT, EFFECT_INDEX, HIT_INDEX);
            return ss.str();
        }
        else if (TURN_ACTION == combat::TurnAction::Cast)
        {
            return CastDescriptionFullVersion(CREATURE_ATTACKING_PTR,
                                                    TURN_ACTION_INFO,
                                                    FIGHT_RESULT,
                                                    EFFECT_INDEX,
                                                    HIT_INDEX,
                                                    wasCollapsed);
        }
        else if (TURN_ACTION == combat::TurnAction::PlaySong)
        {
            return PlaySongDescriptionFullVersion(CREATURE_ATTACKING_PTR,
                                                        TURN_ACTION_INFO,
                                                        FIGHT_RESULT,
                                                        EFFECT_INDEX,
                                                        HIT_INDEX,
                                                        wasCollapsed);
        }
        else
        {
            std::ostringstream ss;
            ss << "(error: invalid TurnAction (" << combat::TurnAction::ToString(TURN_ACTION)
                << ") for indexed version of game::combat::Text::ActionText)";

            return ss.str();
        }
    }


    const std::string Text::WeaponActionVerb(const item::ItemPtr_t WEAPON_PTR, const bool WILL_APPEND_ING)
    {
        std::vector<std::string> strVec;

        if (WEAPON_PTR->WeaponType() & item::weapon_type::Axe)
        {
            if (WILL_APPEND_ING) strVec.push_back("chopping"); else strVec.push_back("chops");
            if (WILL_APPEND_ING) strVec.push_back("hacking"); else strVec.push_back("hacks");
            if (WILL_APPEND_ING) strVec.push_back("swinging"); else strVec.push_back("swings");
        }

        if (WEAPON_PTR->WeaponType() & item::weapon_type::Bite)
        {
            if (WILL_APPEND_ING) strVec.push_back("bitting"); else strVec.push_back("bites");
            if (WILL_APPEND_ING) strVec.push_back("snapping"); else strVec.push_back("snaps");
            if (WILL_APPEND_ING) strVec.push_back("gnawing"); else strVec.push_back("gnaws");
            if (WILL_APPEND_ING) strVec.push_back("chewing"); else strVec.push_back("chews");
        }

        if ((WEAPON_PTR->WeaponType() & item::weapon_type::Bladed) ||
            (WEAPON_PTR->WeaponType() & item::weapon_type::BladedStaff) ||
            (WEAPON_PTR->WeaponType() & item::weapon_type::Knife) ||
            (WEAPON_PTR->WeaponType() & item::weapon_type::Sword))
        {
            if (WILL_APPEND_ING) strVec.push_back("slicing"); else strVec.push_back("slices");
            if (WILL_APPEND_ING) strVec.push_back("slashing"); else strVec.push_back("slashes");
            if (WILL_APPEND_ING) strVec.push_back("cutting"); else strVec.push_back("cuts");
            if (WILL_APPEND_ING) strVec.push_back("swinging"); else strVec.push_back("swings");
        }

        if ((WEAPON_PTR->WeaponType() & item::weapon_type::BladedStaff) ||
            (WEAPON_PTR->WeaponType() & item::weapon_type::Pointed) ||
            (WEAPON_PTR->WeaponType() & item::weapon_type::Knife) ||
            (WEAPON_PTR->WeaponType() & item::weapon_type::Spear))
        {
            if (WILL_APPEND_ING) strVec.push_back("stabbing"); else strVec.push_back("stabs");
            if (WILL_APPEND_ING) strVec.push_back("thrusting"); else strVec.push_back("thrusts");
            if (WILL_APPEND_ING) strVec.push_back("jabbing"); else strVec.push_back("jabs");
        }

        if (WEAPON_PTR->WeaponType() & item::weapon_type::Blowpipe)
        {
            if (WILL_APPEND_ING) strVec.push_back("darting"); else strVec.push_back("darts");
        }

        if ((WEAPON_PTR->WeaponType() & item::weapon_type::Blowpipe) ||
            (WEAPON_PTR->WeaponType() & item::weapon_type::Bow) ||
            (WEAPON_PTR->WeaponType() & item::weapon_type::Crossbow) ||
            (WEAPON_PTR->WeaponType() & item::weapon_type::Projectile))
        {
            if (WILL_APPEND_ING) strVec.push_back("shooting"); else strVec.push_back("shoots");
            if (WILL_APPEND_ING) strVec.push_back("firing"); else strVec.push_back("fires");
        }

        if (WEAPON_PTR->WeaponType() & item::weapon_type::Breath)
        {
            if (WILL_APPEND_ING) strVec.push_back("breathing"); else strVec.push_back("breathes");
        }

        if (WEAPON_PTR->WeaponType() & item::weapon_type::Claws)
        {
            if (WILL_APPEND_ING) strVec.push_back("tearing"); else strVec.push_back("tears");
            if (WILL_APPEND_ING) strVec.push_back("slashing"); else strVec.push_back("slashes");
            if (WILL_APPEND_ING) strVec.push_back("clawing"); else strVec.push_back("claws");
            if (WILL_APPEND_ING) strVec.push_back("ripping"); else strVec.push_back("rips");
        }

        if ((WEAPON_PTR->WeaponType() & item::weapon_type::Club) ||
            (WEAPON_PTR->WeaponType() & item::weapon_type::Staff))
        {
            if (WILL_APPEND_ING) strVec.push_back("swinging"); else strVec.push_back("swings");
            if (WILL_APPEND_ING) strVec.push_back("bashing"); else strVec.push_back("bashes");
            if (WILL_APPEND_ING) strVec.push_back("smashing"); else strVec.push_back("smashes");
            if (WILL_APPEND_ING) strVec.push_back("slamming"); else strVec.push_back("slams");
            if (WILL_APPEND_ING) strVec.push_back("crushing"); else strVec.push_back("crushes");
        }

        if (WEAPON_PTR->WeaponType() & item::weapon_type::Sling)
        {
            if (WILL_APPEND_ING) strVec.push_back("slinging"); else strVec.push_back("slings");
        }

        if (WEAPON_PTR->WeaponType() & item::weapon_type::Whip)
        {
            if (WILL_APPEND_ING) strVec.push_back("whipping"); else strVec.push_back("whips");
        }

        if (WEAPON_PTR->WeaponType() & item::weapon_type::Tendrils)
        {
            if (WILL_APPEND_ING) strVec.push_back("whipping"); else strVec.push_back("whips");
            if (WILL_APPEND_ING) strVec.push_back("sliming"); else strVec.push_back("slimes");
            if (WILL_APPEND_ING) strVec.push_back("slapping"); else strVec.push_back("slaps");
            if (WILL_APPEND_ING) strVec.push_back("squeezing"); else strVec.push_back("squeezes");
        }

        if (strVec.empty())
        {
            if (WILL_APPEND_ING) strVec.push_back("attacking"); else strVec.push_back("attacks");
        }

        strVec.erase(std::unique(strVec.begin(), strVec.end()), strVec.end());

        return strVec.at(static_cast<std::size_t>(misc::random::Int(
            static_cast<int>(strVec.size()) - 1)));
    }


    const std::string Text::AttackDescriptionStatusVersion(
        const combat::FightResult & FIGHT_RESULT)
    {
        //assume weapon attacks can only effect one target creature
        M_ASSERT_OR_LOGANDTHROW_SS((FIGHT_RESULT.Count() == 1),
            "game::combat::Text::AttackDescriptionStatusVersion() was given "
            << "a FIGHT_RESULT with " << FIGHT_RESULT.Count()
            << " CreatureEffects, when only 1 is supported.");

        auto const CREATURE_EFFECT{ FIGHT_RESULT.Effects()[0] };

        std::ostringstream ss;
        ss << WeaponActionVerbList(CREATURE_EFFECT.GetHitInfoVec(), false) << " "
            << NamePhrase(CREATURE_EFFECT.GetCreature()) << " ";

        if (CREATURE_EFFECT.GetWasHit())
        {
            ss << "hitting " << CountPhrase(CREATURE_EFFECT.GetHitInfoVec()) << " ";

            if (CREATURE_EFFECT.WasKill())
            {
                ss << "killing " << creature::sex::HimHerIt(
                    CREATURE_EFFECT.GetCreature()->Sex(), false);
            }
            else
            {
                ss << "for " << CREATURE_EFFECT.GetDamageTotal() * -1 << " damage";
            }

            ss << AttackConditionsSummaryList(CREATURE_EFFECT);
        }
        else
        {
            ss << "but misses";
        }
        
        ss << ".";
        return ss.str();
    }


    const std::string Text::AttackDescriptionPreambleVersion(const FightResult & FIGHT_RESULT)
    {
        //assume weapon attacks can only effect one target creature
        M_ASSERT_OR_LOGANDTHROW_SS((FIGHT_RESULT.Count() == 1),
            "game::combat::Text::AttackDescriptionPreambleVersion() was given "
            << "a FIGHT_RESULT with " << FIGHT_RESULT.Count()
            << " CreatureEffects, when only 1 is supported.");

        std::ostringstream ss;

        auto const CREATURE_EFFECT{ FIGHT_RESULT.Effects()[0] };
        ss << WeaponActionVerbList(CREATURE_EFFECT.GetHitInfoVec(), false) << " "
            << NamePhrase(CREATURE_EFFECT.GetCreature()) << "...";

        return ss.str();
    }


    const std::string Text::AttackDescriptionFullVersion(const FightResult & FIGHT_RESULT,
                                                         const std::size_t   EFFECT_INDEX,
                                                         const std::size_t   HIT_INDEX)
    {
        //assume weapon attacks can only effect one target creature
        M_ASSERT_OR_LOGANDTHROW_SS((EFFECT_INDEX == 0),
            "game::combat::Text::AttackDescriptionFullVersion() was given "
            << "a EFFECT_INDEX=" << EFFECT_INDEX << " when only 0 is supported.");
        //
        M_ASSERT_OR_LOGANDTHROW_SS((FIGHT_RESULT.Effects().size() == 1),
            "game::combat::Text::AttackDescriptionFullVersion() was given "
            << "a FIGHT_RESULT with " << FIGHT_RESULT.Count()
            << " CreatureEffects, when only 1 is supported.");

        auto const CREATURE_EFFECT{ FIGHT_RESULT.Effects()[EFFECT_INDEX] };
        auto const HIT_INFO_VEC{ CREATURE_EFFECT.GetHitInfoVec() };

        M_ASSERT_OR_LOGANDTHROW_SS((HIT_INFO_VEC.empty() == false),
            "game::combat::Text::AttackDescriptionFullVersion(effect_index="
            << EFFECT_INDEX << ", hit_index=" << HIT_INDEX
            << ") was given an empty HIT_INFO_VEC.");

        M_ASSERT_OR_LOGANDTHROW_SS((HIT_INDEX < HIT_INFO_VEC.size()),
            "game::combat::Text::AttackDescriptionFullVersion(effect_index="
            << EFFECT_INDEX << ", hit_index=" << HIT_INDEX
            <<") hit_index was out of bounds with HIT_INFO_VEC.size()="
            << HIT_INFO_VEC.size() << ".");

        auto const HIT_INFO{ HIT_INFO_VEC[HIT_INDEX] };

        std::ostringstream ss;
        ss << HIT_INFO.ActionVerb() << " "
            << NamePhrase(CREATURE_EFFECT.GetCreature()) << "...\n";

        if (HIT_INFO.WasHit())
        {
            if ((HIT_INFO.IsPowerHit() == true) && (HIT_INFO.IsCritical() == false))
            {
                ss << "POWER hit ";
            }
            else if ((HIT_INFO.IsPowerHit() == false) && (HIT_INFO.IsCritical() == true))
            {
                ss << "CRITICAL hit ";
            }
            else if ((HIT_INFO.IsPowerHit() == true) && (HIT_INFO.IsCritical() == true))
            {
                ss << "POWER and CRITICAL hit ";
            }
            else
            {
                if (HIT_INFO.Weapon()->IsBodypart() == false)
                {
                    ss << "and hits ";
                }
            }

            if (HIT_INFO.Damage() < 0)
            {
                ss << "for " << HIT_INFO.Damage() * -1 << " damage";
            }
            else
            {
                if (HIT_INFO.DidArmorAbsorb())
                {
                    ss << " but ";

                    auto const CREATURE_PTR{ CREATURE_EFFECT.GetCreature() };
                    
                    if (CREATURE_PTR->IsPlayerCharacter())
                    {
                        ss << CREATURE_PTR->Name() << "'s";
                    }
                    else
                    {
                        ss << "the " << CREATURE_EFFECT.GetCreature()->NameOrRaceAndRole() << "'s";
                    }
                    
                    ss << " armor absorbed ALL the damage";
                }
                else
                {
                    ss << "but does no damage";
                }
            }

            if (HIT_INFO.WasKill())
            {
                ss << " and kills ";
            }
            else
            {
                ss << AttackConditionsSummaryList(CREATURE_EFFECT);
            }
        }
        else
        {
            ss << "but misses";
        }

        return ss.str();
    }


    const std::string Text::CastDescriptionStatusVersion(
        const TurnActionInfo & TURN_ACTION_INFO,
        const FightResult &    FIGHT_RESULT)
    {
        std::ostringstream ss;

        auto const SPELL_PTR{ TURN_ACTION_INFO.Spell() };
        ss << "casts the " << SPELL_PTR->Name() << " spell "
            << TargetType::ActionPhrase(SPELL_PTR->Target());

        if ((SPELL_PTR->Target() == TargetType::AllCompanions) ||
            (SPELL_PTR->Target() == TargetType::AllOpponents))
        {
            ss << " effecting " << FIGHT_RESULT.Count();
        }

        ss << ".";
        return ss.str();
    }


    const std::string Text::CastDescriptionPreambleVersion(
        const TurnActionInfo & TURN_ACTION_INFO,
        const FightResult &    FIGHT_RESULT)
    {
        std::ostringstream ss;
        ss << "casts the " << TURN_ACTION_INFO.Spell()->Name() << " spell ";
        if (FIGHT_RESULT.Count() == 1)
        {
            auto const CREATURE_EFFECT{ FIGHT_RESULT.Effects()[0] };
            auto const CREATURE_PTR{ CREATURE_EFFECT.GetCreature() };
            ss << "on ";
            if (CREATURE_PTR->IsPlayerCharacter())
            {
                ss << CREATURE_PTR->Name();
            }
            else
            {
                ss << "a " << CREATURE_PTR->NameOrRaceAndRole();
            }
        }
        else if (FIGHT_RESULT.Count() > 1)
        {
            ss << "effecting " << FIGHT_RESULT.Count();

            auto const FIRST_CREATURE_PTR{ FIGHT_RESULT.Effects()[0].GetCreature() };
            if (FIRST_CREATURE_PTR->IsPlayerCharacter())
            {
                ss << " characters";
            }
            else
            {
                ss << " creatures";
            }
        }
        else
        {
            ss << TargetType::ActionPhrase(TURN_ACTION_INFO.Spell()->Target());
        }

        ss << "...";
        return ss.str();
    }


    const std::string Text::CastDescriptionFullVersion(
        const creature::CreaturePtr_t CREATURE_ATTACKING_PTR,
        const TurnActionInfo &        TURN_ACTION_INFO,
        const FightResult &           FIGHT_RESULT,
        const std::size_t             EFFECT_INDEX,
        const std::size_t             HIT_INDEX,
        bool &                        wasCollapsed)
    {
        wasCollapsed = false;

        if (TURN_ACTION_INFO.Spell() == nullptr)
        {
            return "(error: TURN_ACTION_INFO.Spell() null)";
        }

        auto const FIGHT_RESULT_SUMMARY{ SummarizeFightResult(FIGHT_RESULT) };
        if (FIGHT_RESULT_SUMMARY.IsValid())
        {
            wasCollapsed = true;
            return FIGHT_RESULT_SUMMARY.Compose(CREATURE_ATTACKING_PTR->Name(),
                TURN_ACTION_INFO.Spell()->VerbPastTense());
        }
        
        if (EFFECT_INDEX >= FIGHT_RESULT.Effects().size())
        {
            return "(error: EFFECT_INDEX out of range)";
        }

        auto const CREATURE_EFFECT{ FIGHT_RESULT.Effects()[EFFECT_INDEX] };

        if (HIT_INDEX >= CREATURE_EFFECT.GetHitInfoVec().size())
        {
            return "(error: HIT_INDEX out of range)";
        }

        auto const HIT_INFO{ CREATURE_EFFECT.GetHitInfoVec()[HIT_INDEX] };

        std::ostringstream ss;

        ss << HIT_INFO.ActionPhrase().Compose(CREATURE_ATTACKING_PTR->Name(),
                                              CREATURE_EFFECT.GetCreature()->Name());

        auto const DAMAGE{ HIT_INFO.Damage() };
        
        if (DAMAGE > 0)
        {
            ss << " healing for " << DAMAGE;
        }
        else if (DAMAGE < 0)
        {
            ss << " doing " << std::abs(DAMAGE) << " damage.";
        }

        return ss.str();
    }


    const std::string Text::PlaySongDescriptionStatusVersion(
        const TurnActionInfo & TURN_ACTION_INFO,
        const FightResult &    FIGHT_RESULT)
    {
        std::ostringstream ss;

        auto const SONG_PTR{ TURN_ACTION_INFO.Song() };
        ss << "plays the " << SONG_PTR->Name() << " " << SONG_PTR->TypeToNoun() << " "
            << TargetType::ActionPhrase(SONG_PTR->Target());

        if ((SONG_PTR->Target() == TargetType::AllCompanions) ||
            (SONG_PTR->Target() == TargetType::AllOpponents))
        {
            ss << " effecting " << FIGHT_RESULT.Count();
        }

        ss << ".";
        return ss.str();
    }


    const std::string Text::PlaySongDescriptionPreambleVersion(
        const TurnActionInfo & TURN_ACTION_INFO,
        const FightResult &    FIGHT_RESULT)
    {
        std::ostringstream ss;
        ss << "plays the " << TURN_ACTION_INFO.Song()->Name() << " "
            << TURN_ACTION_INFO.Song()->TypeToNoun() << " ";

        if (FIGHT_RESULT.Count() == 1)
        {
            auto const CREATURE_EFFECT{ FIGHT_RESULT.Effects()[0] };
            auto const CREATURE_PTR{ CREATURE_EFFECT.GetCreature() };
            ss << "on ";
            if (CREATURE_PTR->IsPlayerCharacter())
            {
                ss << CREATURE_PTR->Name();
            }
            else
            {
                ss << "a " << CREATURE_PTR->NameOrRaceAndRole();
            }
        }
        else if (FIGHT_RESULT.Count() > 1)
        {
            ss << "effecting " << FIGHT_RESULT.Count();

            auto const FIRST_CREATURE_PTR{ FIGHT_RESULT.Effects()[0].GetCreature() };
            if (FIRST_CREATURE_PTR->IsPlayerCharacter())
            {
                ss << " characters";
            }
            else
            {
                ss << " creatures";
            }
        }
        else
        {
            ss << TargetType::ActionPhrase(TURN_ACTION_INFO.Song()->Target());
        }

        ss << "...";
        return ss.str();
    }


    const std::string Text::PlaySongDescriptionFullVersion(
        const creature::CreaturePtr_t CREATURE_ATTACKING_PTR,
        const TurnActionInfo &        TURN_ACTION_INFO,
        const FightResult &           FIGHT_RESULT,
        const std::size_t             EFFECT_INDEX,
        const std::size_t             HIT_INDEX,
        bool &                        wasCollapsed)
    {
        wasCollapsed = false;

        if (TURN_ACTION_INFO.Song() == nullptr)
        {
            return "(error: TURN_ACTION_INFO.Song() null)";
        }

        auto const FIGHT_RESULT_SUMMARY{ SummarizeFightResult(FIGHT_RESULT) };
        if (FIGHT_RESULT_SUMMARY.IsValid())
        {
            wasCollapsed = true;
            return FIGHT_RESULT_SUMMARY.Compose(CREATURE_ATTACKING_PTR->Name(),
                TURN_ACTION_INFO.Song()->VerbPastTense());
        }
        
        if (EFFECT_INDEX >= FIGHT_RESULT.Effects().size())
        {
            return "(error: EFFECT_INDEX out of range)";
        }

        auto const CREATURE_EFFECT{ FIGHT_RESULT.Effects()[EFFECT_INDEX] };

        if (HIT_INDEX >= CREATURE_EFFECT.GetHitInfoVec().size())
        {
            return "(error: HIT_INDEX out of range)";
        }

        auto const HIT_INFO{ CREATURE_EFFECT.GetHitInfoVec()[HIT_INDEX] };

        std::ostringstream ss;

        ss << HIT_INFO.ActionPhrase().Compose(CREATURE_ATTACKING_PTR->Name(),
                                              CREATURE_EFFECT.GetCreature()->Name());

        auto const DAMAGE{ HIT_INFO.Damage() };
        
        if (DAMAGE > 0)
        {
            ss << " healing for " << DAMAGE;
        }
        else if (DAMAGE < 0)
        {
            ss << " doing " << std::abs(DAMAGE) << " damage.";
        }

        return ss.str();
    }


    const std::string Text::WeaponActionVerbList(const HitInfoVec_t & HIT_INFO_VEC,
                                                 const bool           WILL_SKIP_MISSES)
    {
        std::vector<std::string> strVec;
        
        for (auto const & NEXT_HIT_INFO : HIT_INFO_VEC)
        {
            if ((WILL_SKIP_MISSES == false) ||
                (WILL_SKIP_MISSES && (NEXT_HIT_INFO.WasHit() == true)))
            {
                strVec.push_back(NEXT_HIT_INFO.ActionVerb());
            }
        }
    
        if (strVec.size() > 1)
        {
            strVec.erase(std::unique(strVec.begin(), strVec.end()), strVec.end());
            return boost::algorithm::join(strVec, " and ");
        }
        else if (strVec.size() == 1)
        {
            return strVec[0];
        }
        else
        {
            return "";
        }
    }


    const std::string Text::CountPhrase(const HitInfoVec_t & HIT_INFO_VEC)
    {
        std::size_t count{ 0 };
        for (auto const & NEXT_HIT_INFO : HIT_INFO_VEC)
        {
            if (NEXT_HIT_INFO.WasHit())
            {
                ++count;
            }
        }

        return CountPhrase(count);
    }


    const std::string Text::CountPhrase(const std::size_t COUNT)
    {
        if (COUNT == 1)
        {
            return "once";
        }
        else if (COUNT == 2)
        {
            return "twice";
        }
        else
        {
            std::ostringstream ss;
            ss << COUNT << " times";
            return ss.str();
        }
    }


    const std::string Text::AttackConditionsSummaryList(const CreatureEffect & CREATURE_EFFECT)
    {
        std::ostringstream ss;

        const std::size_t NUM_CONDS_TO_LIST{ 3 };

        auto const ADDED_CONDS_EXCLUDING_DEAD_VEC{
            misc::Vector::Exclude(CREATURE_EFFECT.GetAllCondsAdded(), creature::Conditions::Dead) };

        auto const NUM_ADDED_CONDS{ ADDED_CONDS_EXCLUDING_DEAD_VEC.size() };
        if (NUM_ADDED_CONDS > 0)
        {
            ss << ", causing " << creature::condition::Algorithms::Names(
                ADDED_CONDS_EXCLUDING_DEAD_VEC, false, true, NUM_CONDS_TO_LIST);

            if (NUM_ADDED_CONDS > NUM_CONDS_TO_LIST)
            {
                ss << " (more...)";
            }
        }

        auto const REMOVED_CONDS_SVEC{ CREATURE_EFFECT.GetAllCondsRemoved() };
        auto const NUM_REMOVED_CONDS{ REMOVED_CONDS_SVEC.size() };

        if (NUM_REMOVED_CONDS > 0)
        {
            ss << ", and eliminating " << creature::condition::Algorithms::Names(
                REMOVED_CONDS_SVEC, false, true, NUM_CONDS_TO_LIST);

            if (NUM_REMOVED_CONDS > NUM_CONDS_TO_LIST)
            {
                ss << " (more...)";
            }
        }

        return ss.str();
    }


    const std::string Text::NamePhrase(const creature::CreaturePtr_t CREATURE_PTR)
    {
        std::ostringstream ss;

        if (CREATURE_PTR->IsPlayerCharacter())
        {
            ss << "at " << CREATURE_PTR->NameOrRaceAndRole();
        }
        else
        {
            ss << "at a " << CREATURE_PTR->NameOrRaceAndRole();
        }

        return ss.str();
    }


    const std::string Text::InitialCombatStatusMessagePrefix()
    {
        switch (misc::random::Int(4))
        {
        case 0: { return "You face"; }
        case 1: { return "Before you rage"; }
        case 2: { return "Before you stand"; }
        case 3: { return "Attacking you are"; }
        case 4:
        default: { return "You encounter"; }
        }
    }


    const FightResultSummary Text::SummarizeFightResult(const FightResult & FIGHT_RESULT)
    {
        auto const & CREATURE_EFFECT_VEC{ FIGHT_RESULT.Effects() };
        auto const CREATURE_EFFECTS_COUNT{ CREATURE_EFFECT_VEC.size() };

        if (CREATURE_EFFECTS_COUNT < 2)
        {
            return FightResultSummary();
        }

        auto const FIRST_HIT_INFO{ FIGHT_RESULT.GetHitInfo(0, 0) };
        if (FIRST_HIT_INFO.IsValid() == false)
        {
            return FightResultSummary();
        }

        FightResultSummary frs;

        frs.hit_type = FIRST_HIT_INFO.TypeOfHit();

        if (FIRST_HIT_INFO.TypeOfHit() == HitType::Spell)
        {
            frs.spell_ptr = FIRST_HIT_INFO.SpellPtr();
        }
        else if (FIRST_HIT_INFO.TypeOfHit() == HitType::Song)
        {
            frs.song_ptr = FIRST_HIT_INFO.SongPtr();
        }

        for (std::size_t i(1); i<CREATURE_EFFECTS_COUNT; ++i)
        {
            if (SummarizeCreatureEffect(frs,
                                        FIRST_HIT_INFO,
                                        CREATURE_EFFECT_VEC[i]) == false)
            {
                return FightResultSummary();
            }
        }

        return frs;
    }


    bool Text::SummarizeCreatureEffect(FightResultSummary &   frs,
                                       const HitInfo &        FIRST_HIT_INFO,
                                       const CreatureEffect & CREATURE_EFFECT)
    {
        auto const HIT_INFO_VEC{ CREATURE_EFFECT.GetHitInfoVec() };
        if (HIT_INFO_VEC.size() != 1)
        {
            return false;
        }

        auto const HIT_INFO{ HIT_INFO_VEC[0] };

        if (HIT_INFO.IsCloseEnoughToEqual(FIRST_HIT_INFO) == false)
        {
            return false;
        }

        auto const ACTION_STR{ HIT_INFO.ActionPhrase().Compose("", "") };

        if ((boost::algorithm::contains(ACTION_STR, spell::Spell::FAILED_BECAUSE_STR_)) ||
            (boost::algorithm::contains(ACTION_STR, song::Song::FAILED_STR_)))
        {
            if (boost::algorithm::contains(ACTION_STR, "already"))
            {
                frs.already_pvec.push_back(CREATURE_EFFECT.GetCreature());
            }
            else if ((boost::algorithm::contains(ACTION_STR, spell::Spell::RESISTED_STR_)) ||
                     (boost::algorithm::contains(ACTION_STR, song::Song::RESISTED_STR_)))
            {
                frs.resisted_pvec.push_back(CREATURE_EFFECT.GetCreature());
            }
            else
            {
                return false;
            }
        }
        else
        {
            frs.effected_pvec.push_back(CREATURE_EFFECT.GetCreature());
        }

        return true;
    }

}
}
