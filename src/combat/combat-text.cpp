// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// combat-text.cpp
//
#include "combat-text.hpp"

#include "combat/combat-display.hpp"
#include "combat/encounter.hpp"
#include "creature/condition-algorithms.hpp"
#include "creature/condition.hpp"
#include "creature/creature.hpp"
#include "item/algorithms.hpp"
#include "item/item.hpp"
#include "misc/boost-string-includes.hpp"
#include "misc/log-macros.hpp"
#include "misc/strings.hpp"
#include "misc/vectors.hpp"
#include "song/song.hpp"
#include "spell/spell.hpp"

#include <algorithm>

namespace heroespath
{
namespace combat
{

    const std::string Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ATTACK_(
        "Click or press 'a' to automatically fight the enemy who appears weakest.");

    const std::string Text::TBOX_BUTTON_MOUSEHOVER_TEXT_FIGHT_(
        "Click or press 'f' to fight a creature of your choice.");

    const std::string
        Text::TBOX_BUTTON_MOUSEHOVER_TEXT_CAST_("Click or press 's' to cast a spell.");

    const std::string
        Text::TBOX_BUTTON_MOUSEHOVER_TEXT_PLAY_("Click or press 's' to play a magical song.");

    const std::string Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ADVANCE_(
        "Click, press the right arrow, or press 'd' to move forward.");

    const std::string Text::TBOX_BUTTON_MOUSEHOVER_TEXT_RETREAT_(
        "Click, press the left arrow, or press 'e' to move backward.");

    const std::string Text::TBOX_BUTTON_MOUSEHOVER_TEXT_BLOCK_(
        "Click or press 'b' to prepare for attack and prevent enemies from passing.");

    const std::string Text::TBOX_BUTTON_MOUSEHOVER_TEXT_SKIP_(
        "Click or press 'k' to take no action and skip this turn.");

    const std::string
        Text::TBOX_BUTTON_MOUSEHOVER_TEXT_FLY_("Click or press 'y' to fly into the air.");

    const std::string
        Text::TBOX_BUTTON_MOUSEHOVER_TEXT_LAND_("Click or press 'l' to stop flying and land.");

    const std::string Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ROAR_(
        "Click or press 'r' to roar and attempt to panic enemies.");

    const std::string Text::TBOX_BUTTON_MOUSEHOVER_TEXT_POUNCE_(
        "Click or press 'p' to attempt to pounce on an enemy.");

    const std::string Text::TBOX_BUTTON_MOUSEHOVER_TEXT_RUN_ {
        "Click or press 'u' to attempt to run away."
    };

    const std::string Text::MouseOverTextAttackStr(
        const creature::CreaturePtr_t CREATURE_PTR,
        const CombatDisplayPtr_t COMBAT_DISPLAY_PTR) const
    {
        const auto CAN_TAKE_ACTION_STR { CREATURE_PTR->CanTakeActionStr() };
        if (CAN_TAKE_ACTION_STR.empty() == false)
        {
            return CAN_TAKE_ACTION_STR;
        }

        if (COMBAT_DISPLAY_PTR
                ->FindCreaturesThatCanBeAttackedOfType(
                    CREATURE_PTR, !CREATURE_PTR->IsPlayerCharacter())
                .empty())
        {
            return "Cannot Attack because there are no enemies in range.";
        }

        if (CREATURE_PTR->HasWeaponsHeld() == false)
        {
            return "Cannot Attack because no weapons are held.";
        }

        return TBOX_BUTTON_MOUSEHOVER_TEXT_ATTACK_;
    }

    const std::string Text::MouseOverTextFightStr(
        const creature::CreaturePtr_t CREATURE_PTR,
        const CombatDisplayPtr_t COMBAT_DISPLAY_PTR) const
    {
        const auto CAN_TAKE_ACTION_STR { CREATURE_PTR->CanTakeActionStr() };

        if (CAN_TAKE_ACTION_STR.empty() == false)
        {
            return CAN_TAKE_ACTION_STR;
        }

        if (COMBAT_DISPLAY_PTR
                ->FindCreaturesThatCanBeAttackedOfType(
                    CREATURE_PTR, !CREATURE_PTR->IsPlayerCharacter())
                .empty())
        {
            return "Cannot Fight because there are no enemies in range.";
        }

        if (CREATURE_PTR->HasWeaponsHeld() == false)
        {
            return "Cannot Fight because no weapons are held.";
        }

        return TBOX_BUTTON_MOUSEHOVER_TEXT_FIGHT_;
    }

    const std::string Text::MouseOverTextPlayStr(
        const creature::CreaturePtr_t CREATURE_PTR, const CombatDisplayPtr_t) const
    {
        const auto CAN_PLAY_STR(CREATURE_PTR->CanPlaySongsStr());
        if (CAN_PLAY_STR.empty() == false)
        {
            return CAN_PLAY_STR;
        }

        return TBOX_BUTTON_MOUSEHOVER_TEXT_PLAY_;
    }

    const std::string Text::MouseOverTextCastStr(
        const creature::CreaturePtr_t CREATURE_PTR, const CombatDisplayPtr_t) const
    {
        const auto CAN_CAST_STR(CREATURE_PTR->CanCastSpellsStr());
        if (CAN_CAST_STR.empty() == false)
        {
            return CAN_CAST_STR;
        }

        return TBOX_BUTTON_MOUSEHOVER_TEXT_CAST_;
    }

    const std::string Text::MouseOverTextAdvanceStr(
        const creature::CreaturePtr_t CREATURE_PTR,
        const CombatDisplayPtr_t COMBAT_DISPLAY_PTR) const
    {
        const auto CAN_TAKE_ACTION_STR(CREATURE_PTR->CanTakeActionStr());
        if (CAN_TAKE_ACTION_STR.empty() == false)
        {
            return CAN_TAKE_ACTION_STR;
        }

        const auto CAN_ADVANCE_STR(COMBAT_DISPLAY_PTR->CanAdvanceOrRetreat(CREATURE_PTR, true));

        if (CAN_ADVANCE_STR.empty() == false)
        {
            return CAN_ADVANCE_STR;
        }

        return TBOX_BUTTON_MOUSEHOVER_TEXT_ADVANCE_;
    }

    const std::string Text::MouseOverTextRetreatStr(
        const creature::CreaturePtr_t CREATURE_PTR,
        const CombatDisplayPtr_t COMBAT_DISPLAY_PTR) const
    {
        const auto CAN_TAKE_ACTION_STR(CREATURE_PTR->CanTakeActionStr());
        if (CAN_TAKE_ACTION_STR.empty() == false)
        {
            return CAN_TAKE_ACTION_STR;
        }

        const auto CAN_RETREAT_STR(COMBAT_DISPLAY_PTR->CanAdvanceOrRetreat(CREATURE_PTR, false));

        if (CAN_RETREAT_STR.empty() == false)
        {
            return CAN_RETREAT_STR;
        }

        return TBOX_BUTTON_MOUSEHOVER_TEXT_RETREAT_;
    }

    const std::string Text::MouseOverTextBlockStr(
        const creature::CreaturePtr_t CREATURE_PTR, const CombatDisplayPtr_t) const
    {
        const auto CAN_TAKE_ACTION_STR(CREATURE_PTR->CanTakeActionStr());
        if (CAN_TAKE_ACTION_STR.empty() == false)
        {
            return CAN_TAKE_ACTION_STR;
        }

        return TBOX_BUTTON_MOUSEHOVER_TEXT_BLOCK_;
    }

    const std::string Text::MouseOverTextFlyStr(
        const creature::CreaturePtr_t CREATURE_PTR, const CombatDisplayPtr_t) const
    {
        const auto CAN_TAKE_ACTION_STR(CREATURE_PTR->CanTakeActionStr());
        if (CAN_TAKE_ACTION_STR.empty() == false)
        {
            return CAN_TAKE_ACTION_STR;
        }

        if (Encounter::Instance()->GetTurnInfoCopy(CREATURE_PTR).GetIsFlying())
        {
            return "Cannot Fly because already flying.";
        }

        if (CREATURE_PTR->CanFly() == false)
        {
            return std::string(CREATURE_PTR->RaceName()).append("s cannot fly.");
        }

        return TBOX_BUTTON_MOUSEHOVER_TEXT_FLY_;
    }

    const std::string Text::MouseOverTextLandStr(
        const creature::CreaturePtr_t CREATURE_PTR, const CombatDisplayPtr_t) const
    {
        const auto CAN_TAKE_ACTION_STR(CREATURE_PTR->CanTakeActionStr());
        if (CAN_TAKE_ACTION_STR.empty() == false)
        {
            return CAN_TAKE_ACTION_STR;
        }

        if (Encounter::Instance()->GetTurnInfoCopy(CREATURE_PTR).GetIsFlying() == false)
        {
            return "Cannot Land because not currently flying.";
        }

        return TBOX_BUTTON_MOUSEHOVER_TEXT_LAND_;
    }

    const std::string Text::MouseOverTextRoarStr(
        const creature::CreaturePtr_t CREATURE_PTR, const CombatDisplayPtr_t) const
    {
        const auto CAN_TAKE_ACTION_STR(CREATURE_PTR->CanTakeActionStr());
        if (CAN_TAKE_ACTION_STR.empty() == false)
        {
            return CAN_TAKE_ACTION_STR;
        }

        if (CREATURE_PTR->IsBeast() == false)
        {
            return "Cannot Roar because " + CREATURE_PTR->Name() + " is not a beast.";
        }

        return TBOX_BUTTON_MOUSEHOVER_TEXT_ROAR_;
    }

    const std::string
        Text::MouseOverTextPounceStr(const creature::CreaturePtr_t, const CombatDisplayPtr_t) const
    {
        /*
        const auto CAN_TAKE_ACTION_STR(CREATURE_PTR->CanTakeActionStr());
        if (CAN_TAKE_ACTION_STR.empty() == false)
        {
            return CAN_TAKE_ACTION_STR;
        }

        if (CREATURE_PTR->IsBeast() == false)
        {
            return "Cannot Pounce because " + CREATURE_PTR->Name() + " is a not a beast.";
        }

        return TBOX_BUTTON_MOUSEHOVER_TEXT_POUNCE_;
        */

        return "Pounce is not yet implemented.";
    }

    const std::string Text::MouseOverTextRunStr(
        const creature::CreaturePtr_t CREATURE_PTR, const CombatDisplayPtr_t) const
    {
        const auto CAN_TAKE_ACTION_STR(CREATURE_PTR->CanTakeActionStr());
        if (CAN_TAKE_ACTION_STR.empty() == false)
        {
            return CAN_TAKE_ACTION_STR;
        }

        return TBOX_BUTTON_MOUSEHOVER_TEXT_RUN_;
    }

    const std::string Text::MouseOverTextSkipStr(
        const creature::CreaturePtr_t CREATURE_PTR, const CombatDisplayPtr_t) const
    {
        const auto CAN_TAKE_ACTION_STR(CREATURE_PTR->CanTakeActionStr());
        if (CAN_TAKE_ACTION_STR.empty())
        {
            return "Cannot skip a turn if able to block.  Try blocking instead.";
        }
        else
        {
            return TBOX_BUTTON_MOUSEHOVER_TEXT_SKIP_;
        }
    }

    const std::string Text::ActionText(
        const creature::CreaturePtr_t CREATURE_ATTACKING_PTR,
        const combat::TurnActionInfo & TURN_ACTION_INFO,
        const combat::FightResult & FIGHT_RESULT,
        const bool WILL_USE_NAME,
        const bool IS_STATUS_VERSION,
        const bool IS_PREAMBLE_VERSION) const
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
                M_HP_ASSERT_OR_LOG_AND_THROW(
                    (!!TURN_ACTION_INFO.Spell()),
                    "combat::Text::ActionText(creature_attacking="
                        << CREATURE_ATTACKING_PTR->Name()
                        << ", turn_action_info=Cast, will_use_name=" << std::boolalpha
                        << WILL_USE_NAME << ", is_status=" << IS_STATUS_VERSION
                        << ", is_preamble=" << IS_PREAMBLE_VERSION
                        << ") was of cast type but TURN_ACTION_INFO.Spell() was uninitialized.");

                if (IS_PREAMBLE_VERSION)
                {
                    ss << CastDescriptionPreambleVersion(
                        TURN_ACTION_INFO.Spell().value(), FIGHT_RESULT);
                }
                else if (IS_STATUS_VERSION)
                {
                    ss << CastDescriptionStatusVersion(
                        TURN_ACTION_INFO.Spell().value(), FIGHT_RESULT);
                }

                break;
            }

            case combat::TurnAction::PlaySong:
            {
                M_HP_ASSERT_OR_LOG_AND_THROW(
                    (!!TURN_ACTION_INFO.Song()),
                    "combat::Text::ActionText(creature_attacking="
                        << CREATURE_ATTACKING_PTR->Name()
                        << ", turn_action_info=PlaySong, will_use_name=" << std::boolalpha
                        << WILL_USE_NAME << ", is_status=" << IS_STATUS_VERSION
                        << ", is_preamble=" << IS_PREAMBLE_VERSION
                        << ") was of cast type but TURN_ACTION_INFO.Song() was uninitialized.");

                if (IS_PREAMBLE_VERSION)
                {
                    ss << PlaySongDescriptionPreambleVersion(
                        TURN_ACTION_INFO.Song().get(), FIGHT_RESULT);
                }
                else if (IS_STATUS_VERSION)
                {
                    ss << PlaySongDescriptionStatusVersion(
                        TURN_ACTION_INFO.Song().get(), FIGHT_RESULT);
                }

                break;
            }

            case combat::TurnAction::ChangeWeapon:
            {
                const auto HELD_WEAPONS_PVEC { CREATURE_ATTACKING_PTR->HeldWeapons() };

                if (HELD_WEAPONS_PVEC.empty())
                {
                    ss << "fumbles around looking for a weapon";
                }
                else
                {
                    ss << "changes weapon";

                    if (HELD_WEAPONS_PVEC.size() > 1)
                    {
                        ss << "s";
                    }

                    ss << " to " << item::Algorithms::Names(HELD_WEAPONS_PVEC, misc::JoinOpt::And);
                }

                break;
            }

            case combat::TurnAction::LandPounce:
            {
                std::vector<std::string> strVec {
                    "surges", "charges", "leaps", "rushes", "springs"
                };

                ss << misc::RandomSelect(strVec);

                if (misc::RandomBool())
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
                ss << CREATURE_ATTACKING_PTR->Name() << " Roars...";
                break;
            }

            case combat::TurnAction::Fly:
            case combat::TurnAction::Land:
            case combat::TurnAction::Advance:
            case combat::TurnAction::Retreat:
            case combat::TurnAction::Block:
            case combat::TurnAction::Run:
            case combat::TurnAction::TreasureUnlock:
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
        const creature::CreaturePtr_t CREATURE_ATTACKING_PTR,
        const combat::TurnActionInfo & TURN_ACTION_INFO,
        const combat::FightResult & FIGHT_RESULT,
        const bool WILL_USE_NAME,
        const std::size_t EFFECT_INDEX,
        const std::size_t HIT_INDEX,
        bool & wasCollapsed) const
    {
        wasCollapsed = false;

        const auto TURN_ACTION { TURN_ACTION_INFO.Action() };

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
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (!!TURN_ACTION_INFO.Spell()),
                "combat::Text::ActionTextIndexed(creature_attacking="
                    << CREATURE_ATTACKING_PTR->Name()
                    << ", turn_action_info=" << TURN_ACTION_INFO.ToString()
                    << ", will_use_name=" << std::boolalpha << WILL_USE_NAME
                    << ", effect_index=" << EFFECT_INDEX << ", hit_index=" << HIT_INDEX
                    << ") was cast but TURN_ACTION_INFO.Spell() was uninitialized.");

            return CastDescriptionFullVersion(
                CREATURE_ATTACKING_PTR,
                TURN_ACTION_INFO.Spell().value(),
                FIGHT_RESULT,
                EFFECT_INDEX,
                HIT_INDEX,
                wasCollapsed);
        }
        else if (TURN_ACTION == combat::TurnAction::PlaySong)
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (!!TURN_ACTION_INFO.Song()),
                "combat::Text::ActionTextIndexed(creature_attacking="
                    << CREATURE_ATTACKING_PTR->Name()
                    << ", turn_action_info=" << TURN_ACTION_INFO.ToString()
                    << ", will_use_name=" << std::boolalpha << WILL_USE_NAME
                    << ", effect_index=" << EFFECT_INDEX << ", hit_index=" << HIT_INDEX
                    << ") was cast but TURN_ACTION_INFO.Song() was uninitialized.");

            return PlaySongDescriptionFullVersion(
                CREATURE_ATTACKING_PTR,
                TURN_ACTION_INFO.Song().value(),
                FIGHT_RESULT,
                EFFECT_INDEX,
                HIT_INDEX,
                wasCollapsed);
        }
        else if (TURN_ACTION == combat::TurnAction::Block)
        {
            return ActionText(
                CREATURE_ATTACKING_PTR,
                TURN_ACTION_INFO,
                FIGHT_RESULT,
                WILL_USE_NAME,
                false,
                false);
        }
        else if (TURN_ACTION == combat::TurnAction::Roar)
        {
            return RoarDescriptionFullVersion(
                CREATURE_ATTACKING_PTR,
                TURN_ACTION_INFO,
                FIGHT_RESULT,
                EFFECT_INDEX,
                HIT_INDEX,
                wasCollapsed);
        }
        else if (TURN_ACTION == combat::TurnAction::TreasureUnlock)
        {
            return TrapDescriptionFullVersion(
                CREATURE_ATTACKING_PTR, FIGHT_RESULT, EFFECT_INDEX, HIT_INDEX);
        }
        else
        {
            std::ostringstream ss;
            ss << "(error: invalid TurnAction (" << NAMEOF_ENUM(TURN_ACTION)
               << ") for indexed version of combat::Text::ActionText)";

            return ss.str();
        }
    }

    const std::string
        Text::WeaponActionVerb(const item::ItemPtr_t WEAPON_PTR, const bool WILL_APPEND_ING) const
    {
        std::vector<std::string> strVec;

        const auto & WEAPON_INFO { WEAPON_PTR->WeaponInfo() };

        if (WEAPON_INFO.IsAxe())
        {
            if (WILL_APPEND_ING)
            {
                strVec.emplace_back("chopping");
            }
            else
            {
                strVec.emplace_back("chops");
            }

            if (WILL_APPEND_ING)
            {
                strVec.emplace_back("hacking");
            }
            else
            {
                strVec.emplace_back("hacks");
            }

            if (WILL_APPEND_ING)
            {
                strVec.emplace_back("swinging");
            }
            else
            {
                strVec.emplace_back("swings");
            }
        }

        if (WEAPON_INFO.IsMinor<item::BodyPartWeapons>(item::BodyPartWeapons::Bite))
        {
            if (WILL_APPEND_ING)
            {
                strVec.emplace_back("bitting");
            }
            else
            {
                strVec.emplace_back("bites");
            }

            if (WILL_APPEND_ING)
            {
                strVec.emplace_back("snapping");
            }
            else
            {
                strVec.emplace_back("snaps");
            }

            if (WILL_APPEND_ING)
            {
                strVec.emplace_back("gnawing");
            }
            else
            {
                strVec.emplace_back("gnaws");
            }

            if (WILL_APPEND_ING)
            {
                strVec.emplace_back("chewing");
            }
            else
            {
                strVec.emplace_back("chews");
            }
        }

        if (WEAPON_INFO.IsMinor<item::BodyPartWeapons>(item::BodyPartWeapons::Claws))
        {
            if (WILL_APPEND_ING)
            {
                strVec.emplace_back("tearing");
            }
            else
            {
                strVec.emplace_back("tears");
            }

            if (WILL_APPEND_ING)
            {
                strVec.emplace_back("slashing");
            }
            else
            {
                strVec.emplace_back("slashes");
            }

            if (WILL_APPEND_ING)
            {
                strVec.emplace_back("clawing");
            }
            else
            {
                strVec.emplace_back("claws");
            }

            if (WILL_APPEND_ING)
            {
                strVec.emplace_back("ripping");
            }
            else
            {
                strVec.emplace_back("rips");
            }
        }

        if (WEAPON_INFO.IsBladed())
        {
            if (WILL_APPEND_ING)
            {
                strVec.emplace_back("slicing");
            }
            else
            {
                strVec.emplace_back("slices");
            }

            if (WILL_APPEND_ING)
            {
                strVec.emplace_back("slashing");
            }
            else
            {
                strVec.emplace_back("slashes");
            }

            if (WILL_APPEND_ING)
            {
                strVec.emplace_back("cutting");
            }
            else
            {
                strVec.emplace_back("cuts");
            }

            if (WILL_APPEND_ING)
            {
                strVec.emplace_back("swinging");
            }
            else
            {
                strVec.emplace_back("swings");
            }
        }

        if (WEAPON_INFO.IsPointed())
        {
            if (WILL_APPEND_ING)
            {
                strVec.emplace_back("stabbing");
            }
            else
            {
                strVec.emplace_back("stabs");
            }

            if (WILL_APPEND_ING)
            {
                strVec.emplace_back("thrusting");
            }
            else
            {
                strVec.emplace_back("thrusts");
            }

            if (WILL_APPEND_ING)
            {
                strVec.emplace_back("jabbing");
            }
            else
            {
                strVec.emplace_back("jabs");
            }
        }

        if (WEAPON_INFO.IsProjectile())
        {
            if (WILL_APPEND_ING)
            {
                strVec.emplace_back("shooting");
            }
            else
            {
                strVec.emplace_back("shoots");
            }

            if (WILL_APPEND_ING)
            {
                strVec.emplace_back("firing");
            }
            else
            {
                strVec.emplace_back("fires");
            }

            if (WEAPON_INFO.IsMinor<item::Projectiles>(item::Projectiles::Blowpipe))
            {
                if (WILL_APPEND_ING)
                {
                    strVec.emplace_back("darting");
                }
                else
                {
                    strVec.emplace_back("darts");
                }
            }

            if (WEAPON_INFO.IsMinor<item::Projectiles>(item::Projectiles::Sling))
            {
                if (WILL_APPEND_ING)
                {
                    strVec.emplace_back("slinging");
                }
                else
                {
                    strVec.emplace_back("slings");
                }
            }
        }

        if (WEAPON_INFO.IsBodyPartBase())
        {
            if (WILL_APPEND_ING)
            {
                strVec.emplace_back("whipping");
            }
            else
            {
                strVec.emplace_back("whips");
            }

            if (WILL_APPEND_ING)
            {
                strVec.emplace_back("sliming");
            }
            else
            {
                strVec.emplace_back("slimes");
            }

            if (WILL_APPEND_ING)
            {
                strVec.emplace_back("slapping");
            }
            else
            {
                strVec.emplace_back("slaps");
            }

            if (WILL_APPEND_ING)
            {
                strVec.emplace_back("squeezing");
            }
            else
            {
                strVec.emplace_back("squeezes");
            }

            if (WEAPON_INFO.IsBreath())
            {
                if (WILL_APPEND_ING)
                {
                    strVec.emplace_back("breathing");
                }
                else
                {
                    strVec.emplace_back("breathes");
                }
            }
        }

        if (WEAPON_INFO.IsClub() || WEAPON_INFO.IsAnyStaff())
        {
            if (WILL_APPEND_ING)
            {
                strVec.emplace_back("swinging");
            }
            else
            {
                strVec.emplace_back("swings");
            }

            if (WILL_APPEND_ING)
            {
                strVec.emplace_back("bashing");
            }
            else
            {
                strVec.emplace_back("bashes");
            }

            if (WILL_APPEND_ING)
            {
                strVec.emplace_back("smashing");
            }
            else
            {
                strVec.emplace_back("smashes");
            }

            if (WILL_APPEND_ING)
            {
                strVec.emplace_back("slamming");
            }
            else
            {
                strVec.emplace_back("slams");
            }

            if (WILL_APPEND_ING)
            {
                strVec.emplace_back("crushing");
            }
            else
            {
                strVec.emplace_back("crushes");
            }
        }

        if (WEAPON_INFO.IsWhip())
        {
            if (WILL_APPEND_ING)
            {
                strVec.emplace_back("whipping");
            }
            else
            {
                strVec.emplace_back("whips");
            }
        }

        if (strVec.empty())
        {
            if (WILL_APPEND_ING)
            {
                strVec.emplace_back("attacking");
            }
            else
            {
                strVec.emplace_back("attacks");
            }
        }

        std::sort(strVec.begin(), strVec.end());
        strVec.erase(std::unique(strVec.begin(), strVec.end()), strVec.end());
        return misc::RandomSelect(strVec);
    }

    const std::string
        Text::AttackDescriptionStatusVersion(const combat::FightResult & FIGHT_RESULT) const
    {
        // assume weapon attacks can only effect one target creature
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (FIGHT_RESULT.Count() == 1),
            "combat::Text::AttackDescriptionStatusVersion() was given "
                << "a FIGHT_RESULT with " << FIGHT_RESULT.Count()
                << " CreatureEffects, when only 1 is supported.");

        const auto CREATURE_EFFECT { FIGHT_RESULT.Effects()[0] };

        std::ostringstream ss;
        ss << WeaponActionVerbList(CREATURE_EFFECT.GetHitInfoVec(), false) << " "
           << NamePhrase(CREATURE_EFFECT.GetCreature()) << " ";

        if (CREATURE_EFFECT.GetWasHit())
        {
            ss << "hitting " << CountPhrase(CREATURE_EFFECT.GetHitInfoVec()) << " ";

            if (CREATURE_EFFECT.WasKill())
            {
                ss << "killing "
                   << creature::sex::HimHerIt(CREATURE_EFFECT.GetCreature()->Sex(), false);
            }
            else
            {
                ss << "for " << -CREATURE_EFFECT.GetDamageTotal() << " damage";
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

    const std::string Text::AttackDescriptionPreambleVersion(const FightResult & FIGHT_RESULT) const
    {
        // assume weapon attacks can only effect one target creature
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (FIGHT_RESULT.Count() == 1),
            "combat::Text::AttackDescriptionPreambleVersion() was given "
                << "a FIGHT_RESULT with " << FIGHT_RESULT.Count()
                << " CreatureEffects, when only 1 is supported.");

        std::ostringstream ss;

        const auto CREATURE_EFFECT { FIGHT_RESULT.Effects()[0] };
        ss << WeaponActionVerbList(CREATURE_EFFECT.GetHitInfoVec(), false) << " "
           << NamePhrase(CREATURE_EFFECT.GetCreature()) << "...";

        return ss.str();
    }

    const std::string Text::AttackDescriptionFullVersion(
        const FightResult & FIGHT_RESULT,
        const std::size_t EFFECT_INDEX,
        const std::size_t HIT_INDEX) const
    {
        // assume weapon attacks can only effect one target creature
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (EFFECT_INDEX == 0),
            "combat::Text::AttackDescriptionFullVersion() was given "
                << "a EFFECT_INDEX=" << EFFECT_INDEX << " when only 0 is supported.");
        //
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (FIGHT_RESULT.Effects().size() == 1),
            "combat::Text::AttackDescriptionFullVersion() was given "
                << "a FIGHT_RESULT with " << FIGHT_RESULT.Count()
                << " CreatureEffects, when only 1 is supported.");

        const auto CREATURE_EFFECT { FIGHT_RESULT.Effects()[EFFECT_INDEX] };
        const auto & HIT_INFO_VEC { CREATURE_EFFECT.GetHitInfoVec() };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (HIT_INFO_VEC.empty() == false),
            "combat::Text::AttackDescriptionFullVersion(effect_index="
                << EFFECT_INDEX << ", hit_index=" << HIT_INDEX
                << ") was given an empty HIT_INFO_VEC.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (HIT_INDEX < HIT_INFO_VEC.size()),
            "combat::Text::AttackDescriptionFullVersion(effect_index="
                << EFFECT_INDEX << ", hit_index=" << HIT_INDEX
                << ") hit_index was out of bounds with HIT_INFO_VEC.size()=" << HIT_INFO_VEC.size()
                << ".");

        const auto HIT_INFO { HIT_INFO_VEC[HIT_INDEX] };

        std::ostringstream ss;
        ss << HIT_INFO.ActionVerb() << " " << NamePhrase(CREATURE_EFFECT.GetCreature()) << "...\n";

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
                if (HIT_INFO.Weapon().value()->IsBodyPart() == false)
                {
                    ss << "and hits ";
                }
            }

            if (HIT_INFO.Damage() < 0_health)
            {
                ss << "for " << -HIT_INFO.Damage() << " damage";
            }
            else
            {
                if (HIT_INFO.DidArmorAbsorb())
                {
                    ss << " but ";

                    const auto CREATURE_PTR { CREATURE_EFFECT.GetCreature() };

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
        const spell::SpellPtr_t & SPELL_PTR, const FightResult & FIGHT_RESULT) const
    {
        std::ostringstream ss;
        ss << "casts the " << SPELL_PTR->Name() << " spell ";

        const auto SPELL_TARGET { SPELL_PTR->Target() };

        ss << TargetType::ActionPhrase(SPELL_TARGET);

        if ((SPELL_TARGET == TargetType::AllCompanions)
            || (SPELL_TARGET == TargetType::AllOpponents))
        {
            ss << " at " << FIGHT_RESULT.Count();
        }

        ss << ".";
        return ss.str();
    }

    const std::string Text::CastDescriptionPreambleVersion(
        const spell::SpellPtr_t & SPELL_PTR, const FightResult & FIGHT_RESULT) const
    {
        std::ostringstream ss;
        ss << "casts the " << SPELL_PTR->Name() << " spell ";

        if (FIGHT_RESULT.Count() == 1)
        {
            const auto CREATURE_EFFECT { FIGHT_RESULT.Effects()[0] };
            const auto CREATURE_PTR { CREATURE_EFFECT.GetCreature() };
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
            ss << "at " << FIGHT_RESULT.Count();

            const auto FIRST_CREATURE_PTR { FIGHT_RESULT.Effects()[0].GetCreature() };
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
            ss << TargetType::ActionPhrase(SPELL_PTR->Target());
        }

        ss << "...";
        return ss.str();
    }

    const std::string Text::CastDescriptionFullVersion(
        const creature::CreaturePtr_t CREATURE_CASTING_PTR,
        const spell::SpellPtr_t & SPELL_PTR,
        const FightResult & FIGHT_RESULT,
        const std::size_t EFFECT_INDEX,
        const std::size_t HIT_INDEX,
        bool & wasCollapsed) const
    {
        wasCollapsed = false;

        const auto FIGHT_RESULT_SUMMARY { SummarizeFightResult(
            CREATURE_CASTING_PTR, FIGHT_RESULT) };

        if (FIGHT_RESULT_SUMMARY.IsValid())
        {
            wasCollapsed = true;
            return FIGHT_RESULT_SUMMARY.Compose(
                CREATURE_CASTING_PTR->Name(), SPELL_PTR->VerbPastTense());
        }

        if (EFFECT_INDEX >= FIGHT_RESULT.Effects().size())
        {
            return "(error: cast EFFECT_INDEX out of range)";
        }

        const auto CREATURE_EFFECT { FIGHT_RESULT.Effects()[EFFECT_INDEX] };

        if (HIT_INDEX >= CREATURE_EFFECT.GetHitInfoVec().size())
        {
            return "(error: cast HIT_INDEX out of range)";
        }

        const auto HIT_INFO { CREATURE_EFFECT.GetHitInfoVec()[HIT_INDEX] };

        std::ostringstream ss;

        ss << HIT_INFO.ActionPhrase().Compose(
            CREATURE_CASTING_PTR->Name(), CREATURE_EFFECT.GetCreature()->Name());

        const auto DAMAGE { HIT_INFO.Damage() };

        if (DAMAGE > 0_health)
        {
            ss << " healing for " << DAMAGE;
        }
        else if (DAMAGE < 0_health)
        {
            ss << " doing " << DAMAGE.MakePositiveCopy() << " damage.";
        }

        return ss.str();
    }

    const std::string Text::PlaySongDescriptionStatusVersion(
        const song::SongPtr_t & SONG_PTR, const FightResult & FIGHT_RESULT) const
    {
        std::ostringstream ss;

        ss << "plays the " << SONG_PTR->Name() << " " << SONG_PTR->TypeToNoun() << " "
           << TargetType::ActionPhrase(SONG_PTR->Target());

        if ((SONG_PTR->Target() == TargetType::AllCompanions)
            || (SONG_PTR->Target() == TargetType::AllOpponents))
        {
            ss << " at " << FIGHT_RESULT.Count();
        }

        ss << ".";
        return ss.str();
    }

    const std::string Text::PlaySongDescriptionPreambleVersion(
        const song::SongPtr_t & SONG_PTR, const FightResult & FIGHT_RESULT) const
    {
        std::ostringstream ss;
        ss << "plays the " << SONG_PTR->Name() << " " << SONG_PTR->TypeToNoun() << " ";

        if (FIGHT_RESULT.Count() == 1)
        {
            const auto CREATURE_EFFECT { FIGHT_RESULT.Effects()[0] };
            const auto CREATURE_PTR { CREATURE_EFFECT.GetCreature() };
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
            ss << "at " << FIGHT_RESULT.Count();

            const auto FIRST_CREATURE_PTR { FIGHT_RESULT.Effects()[0].GetCreature() };
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
            ss << TargetType::ActionPhrase(SONG_PTR->Target());
        }

        ss << "...";
        return ss.str();
    }

    const std::string Text::PlaySongDescriptionFullVersion(
        const creature::CreaturePtr_t CREATURE_PLAYINGING_PTR,
        const song::SongPtr_t & SONG_PTR,
        const FightResult & FIGHT_RESULT,
        const std::size_t EFFECT_INDEX,
        const std::size_t HIT_INDEX,
        bool & wasCollapsed) const
    {
        wasCollapsed = false;

        const auto FIGHT_RESULT_SUMMARY { SummarizeFightResult(
            CREATURE_PLAYINGING_PTR, FIGHT_RESULT) };

        if (FIGHT_RESULT_SUMMARY.IsValid())
        {
            wasCollapsed = true;
            return FIGHT_RESULT_SUMMARY.Compose(
                CREATURE_PLAYINGING_PTR->Name(), SONG_PTR->VerbPastTense());
        }

        if (EFFECT_INDEX >= FIGHT_RESULT.Effects().size())
        {
            return "(error: song EFFECT_INDEX out of range)";
        }

        const auto CREATURE_EFFECT { FIGHT_RESULT.Effects()[EFFECT_INDEX] };

        if (HIT_INDEX >= CREATURE_EFFECT.GetHitInfoVec().size())
        {
            return "(error: song HIT_INDEX out of range)";
        }

        const auto HIT_INFO { CREATURE_EFFECT.GetHitInfoVec()[HIT_INDEX] };

        std::ostringstream ss;

        ss << HIT_INFO.ActionPhrase().Compose(
            CREATURE_PLAYINGING_PTR->Name(), CREATURE_EFFECT.GetCreature()->Name());

        const auto DAMAGE { HIT_INFO.Damage() };

        if (DAMAGE > 0_health)
        {
            ss << " healing for " << DAMAGE;
        }
        else if (DAMAGE < 0_health)
        {
            ss << " doing " << DAMAGE.MakePositiveCopy() << " damage.";
        }

        return ss.str();
    }

    const std::string Text::RoarDescriptionFullVersion(
        const creature::CreaturePtr_t CREATURE_ROARING_PTR,
        const TurnActionInfo &,
        const FightResult & FIGHT_RESULT,
        const std::size_t EFFECT_INDEX,
        const std::size_t HIT_INDEX,
        bool & wasCollapsed) const
    {
        wasCollapsed = false;

        const auto FIGHT_RESULT_SUMMARY { SummarizeFightResult(
            CREATURE_ROARING_PTR, FIGHT_RESULT) };

        if (FIGHT_RESULT_SUMMARY.IsValid())
        {
            wasCollapsed = true;
            return FIGHT_RESULT_SUMMARY.Compose(CREATURE_ROARING_PTR->Name(), "roared");
        }

        if (EFFECT_INDEX >= FIGHT_RESULT.Effects().size())
        {
            return "(error: roar EFFECT_INDEX out of range)";
        }

        const auto CREATURE_EFFECT { FIGHT_RESULT.Effects()[EFFECT_INDEX] };

        if (HIT_INDEX >= CREATURE_EFFECT.GetHitInfoVec().size())
        {
            return "(error: roar HIT_INDEX out of range)";
        }

        const auto HIT_INFO { CREATURE_EFFECT.GetHitInfoVec()[HIT_INDEX] };

        std::ostringstream ss;

        ss << HIT_INFO.ActionPhrase().Compose(
            CREATURE_ROARING_PTR->Name(), CREATURE_EFFECT.GetCreature()->Name());

        return ss.str();
    }

    const std::string Text::TrapDescriptionFullVersion(
        const creature::CreaturePtr_t CREATURE_UNLOCKING_PTR,
        const FightResult & FIGHT_RESULT,
        const std::size_t EFFECT_INDEX,
        const std::size_t HIT_INDEX) const
    {
        const auto & CREATURE_EFFECTS { FIGHT_RESULT.Effects() };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (EFFECT_INDEX < CREATURE_EFFECTS.size()),
            "combat::Text::TrapDescriptionFullVersion(creature="
                << CREATURE_UNLOCKING_PTR->NameAndRaceAndRole() << ", effect_index=" << EFFECT_INDEX
                << ", hit_index=" << HIT_INDEX << ") but there were only "
                << CREATURE_EFFECTS.size() << " creature effects.");

        const auto CREATURE_EFFECT { CREATURE_EFFECTS.at(EFFECT_INDEX) };

        const auto & HIT_INFOS { CREATURE_EFFECT.GetHitInfoVec() };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (HIT_INDEX < HIT_INFOS.size()),
            "combat::Text::TrapDescriptionFullVersion(creature="
                << CREATURE_UNLOCKING_PTR->NameAndRaceAndRole() << ", effect_index=" << EFFECT_INDEX
                << ", hit_index=" << HIT_INDEX << ") but there were only " << HIT_INFOS.size()
                << " hit infos.");

        const auto HIT_INFO { HIT_INFOS.at(HIT_INDEX) };

        std::ostringstream ss;
        ss << CREATURE_UNLOCKING_PTR->Name() << " is " << HIT_INFO.ActionVerb() << " for "
           << HIT_INFO.Damage().MakePositiveCopy() << " damage"
           << AttackConditionsSummaryList(CREATURE_EFFECT);

        return ss.str();
    }

    const std::string Text::InitialCombatStatusMessagePrefix() const
    {
        switch (misc::Random(4))
        {
            case 0:
            {
                return "You face";
            }
            case 1:
            {
                return "Before you rage";
            }
            case 2:
            {
                return "Before you stand";
            }
            case 3:
            {
                return "Attacking you are";
            }
            case 4:
            default:
            {
                return "You encounter";
            }
        }
    }

    const std::string Text::WeaponActionVerbList(
        const HitInfoVec_t & HIT_INFO_VEC, const bool WILL_SKIP_MISSES) const
    {
        std::vector<std::string> strVec;

        for (const auto & NEXT_HIT_INFO : HIT_INFO_VEC)
        {
            if ((WILL_SKIP_MISSES == false)
                || (WILL_SKIP_MISSES && (NEXT_HIT_INFO.WasHit() == true)))
            {
                strVec.emplace_back(NEXT_HIT_INFO.ActionVerb());
            }
        }

        if (strVec.size() > 1)
        {
            std::sort(strVec.begin(), strVec.end());
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

    const std::string Text::CountPhrase(const HitInfoVec_t & HIT_INFO_VEC) const
    {
        std::size_t count { 0 };
        for (const auto & NEXT_HIT_INFO : HIT_INFO_VEC)
        {
            if (NEXT_HIT_INFO.WasHit())
            {
                ++count;
            }
        }

        return CountPhrase(count);
    }

    const std::string Text::CountPhrase(const std::size_t COUNT) const
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
            return std::to_string(COUNT) + " times";
        }
    }

    const std::string Text::NamePhrase(const creature::CreaturePtr_t CREATURE_PTR) const
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

    const std::string
        Text::AttackConditionsSummaryList(const CreatureEffect & CREATURE_EFFECT) const
    {
        std::ostringstream ss;

        const std::size_t NUM_CONDS_TO_LIST { 3 };

        const auto ADDED_CONDS_EXCLUDING_DEAD_VEC { misc::Vector::Exclude(
            CREATURE_EFFECT.GetAllCondsAdded(), creature::Conditions::Dead) };

        const auto NUM_ADDED_CONDS { ADDED_CONDS_EXCLUDING_DEAD_VEC.size() };
        if (NUM_ADDED_CONDS > 0)
        {
            ss << ", causing "
               << creature::condition::Algorithms::Names(
                      ADDED_CONDS_EXCLUDING_DEAD_VEC,
                      misc::JoinHow(misc::JoinOpt::And, NUM_CONDS_TO_LIST));

            if (NUM_ADDED_CONDS > NUM_CONDS_TO_LIST)
            {
                ss << " (more...)";
            }
        }

        const auto REMOVED_CONDS_SVEC { CREATURE_EFFECT.GetAllCondsRemoved() };
        const auto NUM_REMOVED_CONDS { REMOVED_CONDS_SVEC.size() };

        if (NUM_REMOVED_CONDS > 0)
        {
            ss << ", and eliminating "
               << creature::condition::Algorithms::Names(
                      REMOVED_CONDS_SVEC, misc::JoinHow(misc::JoinOpt::And, NUM_CONDS_TO_LIST));

            if (NUM_REMOVED_CONDS > NUM_CONDS_TO_LIST)
            {
                ss << " (more...)";
            }
        }

        return ss.str();
    }

    const FightResultSummary Text::SummarizeFightResult(
        const creature::CreaturePtr_t CREATURE_INITIATING_PTR,
        const FightResult & FIGHT_RESULT) const
    {
        const auto & CREATURE_EFFECT_VEC { FIGHT_RESULT.Effects() };
        const auto CREATURE_EFFECTS_COUNT { CREATURE_EFFECT_VEC.size() };

        if (CREATURE_EFFECTS_COUNT <= 1)
        {
            return FightResultSummary();
        }

        const auto FIRST_HIT_INFO { FIGHT_RESULT.GetHitInfo(0, 0) };
        if (FIRST_HIT_INFO.IsValid() == false)
        {
            return FightResultSummary();
        }

        creature::CreaturePVec_t effectedCreaturesPVec;
        FIGHT_RESULT.EffectedCreatures(effectedCreaturesPVec);

        if (effectedCreaturesPVec.empty())
        {
            return FightResultSummary();
        }

        FightResultSummary frs;

        frs.areResistedNotEffected_
            = (CREATURE_INITIATING_PTR->IsPlayerCharacter()
               == effectedCreaturesPVec[0]->IsPlayerCharacter());

        frs.hit_type = FIRST_HIT_INFO.TypeOfHit();

        if ((HitType::Roar != frs.hit_type) && (HitType::Song != frs.hit_type)
            && (HitType::Spell != frs.hit_type))
        {
            return FightResultSummary();
        }

        if (FIRST_HIT_INFO.TypeOfHit() == HitType::Spell)
        {
            frs.spell_ptr_opt = FIRST_HIT_INFO.SpellPtrOpt();
        }
        else if (FIRST_HIT_INFO.TypeOfHit() == HitType::Song)
        {
            frs.song_ptr_opt = FIRST_HIT_INFO.SongPtrOpt();
        }

        for (std::size_t i(0); i < CREATURE_EFFECTS_COUNT; ++i)
        {
            if (SummarizeCreatureEffect(frs, CREATURE_EFFECT_VEC[i]) == false)
            {
                return FightResultSummary();
            }
        }

        return frs;
    }

    bool Text::SummarizeCreatureEffect(
        FightResultSummary & frs, const CreatureEffect & CREATURE_EFFECT) const
    {
        const auto & NEXT_HIT_INFO_VEC { CREATURE_EFFECT.GetHitInfoVec() };
        if (NEXT_HIT_INFO_VEC.size() != 1)
        {
            return false;
        }

        const auto NEXT_HIT_INFO { NEXT_HIT_INFO_VEC[0] };
        const auto NEXT_ACTION_STR { NEXT_HIT_INFO.ActionPhrase().Compose("", "") };

        if ((boost::algorithm::contains(NEXT_ACTION_STR, spell::Spell::FAILED_BECAUSE_STR_))
            || (boost::algorithm::contains(NEXT_ACTION_STR, song::Song::FAILED_STR_))
            || (boost::algorithm::contains(NEXT_ACTION_STR, "resisted")))
        {
            if (boost::algorithm::contains(NEXT_ACTION_STR, "already"))
            {
                if (frs.already_vec.empty()
                    || (frs.already_vec[0].second.IsCloseEnoughToEqual(NEXT_HIT_INFO)))
                {
                    frs.already_vec.emplace_back(
                        std::make_pair(CREATURE_EFFECT.GetCreature(), NEXT_HIT_INFO));

                    return true;
                }
            }
            else if (
                (boost::algorithm::contains(NEXT_ACTION_STR, spell::Spell::RESISTED_STR_))
                || (boost::algorithm::contains(NEXT_ACTION_STR, song::Song::RESISTED_STR_))
                || (boost::algorithm::contains(NEXT_ACTION_STR, "resisted")))
            {
                if (frs.resisted_vec.empty()
                    || (frs.resisted_vec[0].second.IsCloseEnoughToEqual(NEXT_HIT_INFO)))
                {
                    frs.resisted_vec.emplace_back(
                        std::make_pair(CREATURE_EFFECT.GetCreature(), NEXT_HIT_INFO));

                    return true;
                }
            }
        }
        else
        {
            if (frs.effected_vec.empty()
                || (frs.effected_vec[0].second.IsCloseEnoughToEqual(NEXT_HIT_INFO)))
            {
                frs.effected_vec.emplace_back(
                    std::make_pair(CREATURE_EFFECT.GetCreature(), NEXT_HIT_INFO));

                return true;
            }
        }

        return false;
    }

} // namespace combat
} // namespace heroespath
