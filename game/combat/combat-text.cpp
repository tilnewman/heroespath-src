//
// combat-text.cpp
//
#include "combat-text.hpp"

#include "game/creature/creature.hpp"
#include "game/creature/conditions.hpp"
#include "game/creature/condition-algorithms.hpp"
#include "game/combat/combat-display.hpp"
#include "game/combat/encounter.hpp"
#include "game/item/item.hpp"
#include "game/item/algorithms.hpp"
#include "game/spell/spell-base.hpp"

#include "utilz/vectors.hpp"


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
    const std::string Text::TBOX_BUTTON_MOUSEHOVER_TEXT_ROAR_    ("Click or press 'r' to roar and attempt to frighten enemies.");
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


    const std::string Text::ActionText(creature::CreatureCPtrC_t      CREATURE_ATTACKING_CPTRC,
                                       const combat::TurnActionInfo & TURN_ACTION_INFO,
                                       const combat::FightResult &    FIGHT_RESULT,
                                       const bool                     WILL_USE_NAME,
                                       const bool                     IS_STATUS_VERSION,
                                       const bool                     IS_PREAMBLE_VERSION,
                                       const std::size_t              EFFECT_INDEX,
                                       const std::size_t              HIT_INDEX)

    {
        std::ostringstream ss;

        if (WILL_USE_NAME)
        {
            ss << CREATURE_ATTACKING_CPTRC->Name();
        }
        else
        {
            ss << creature::sex::HeSheIt(CREATURE_ATTACKING_CPTRC->Sex(), true);
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
                else
                {
                    ss << AttackDescriptionFullVersion(FIGHT_RESULT, EFFECT_INDEX, HIT_INDEX);
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
                else
                {
                    ss << CastDescriptionFullVersion(TURN_ACTION_INFO, FIGHT_RESULT, EFFECT_INDEX, HIT_INDEX);
                }
            }

            case combat::TurnAction::ChangeWeapon:
            {
                auto const CURRENT_WEAPONS_SVEC{ CREATURE_ATTACKING_CPTRC->CurrentWeaponsCopy() };

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
                ss << strVec.at(utilz::random::Int(strVec.size() - 1));

                if (utilz::random::Bool())
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


    const std::string Text::WeaponActionVerb(const item::ItemSPtr_t & WEAPON_SPTR, const bool WILL_APPEND_ING)
    {
        std::vector<std::string> strVec;

        if (WEAPON_SPTR->WeaponType() & item::weapon_type::Axe)
        {
            if (WILL_APPEND_ING) strVec.push_back("chopping"); else strVec.push_back("chops");
            if (WILL_APPEND_ING) strVec.push_back("hacking"); else strVec.push_back("hacks");
            if (WILL_APPEND_ING) strVec.push_back("swinging"); else strVec.push_back("swings");
        }

        if (WEAPON_SPTR->WeaponType() & item::weapon_type::Bite)
        {
            if (WILL_APPEND_ING) strVec.push_back("bitting"); else strVec.push_back("bites");
            if (WILL_APPEND_ING) strVec.push_back("snapping"); else strVec.push_back("snaps");
            if (WILL_APPEND_ING) strVec.push_back("gnawing"); else strVec.push_back("gnaws");
            if (WILL_APPEND_ING) strVec.push_back("chewing"); else strVec.push_back("chews");
        }

        if ((WEAPON_SPTR->WeaponType() & item::weapon_type::Bladed) ||
            (WEAPON_SPTR->WeaponType() & item::weapon_type::BladedStaff) ||
            (WEAPON_SPTR->WeaponType() & item::weapon_type::Knife) ||
            (WEAPON_SPTR->WeaponType() & item::weapon_type::Sword))
        {
            if (WILL_APPEND_ING) strVec.push_back("slicing"); else strVec.push_back("slices");
            if (WILL_APPEND_ING) strVec.push_back("slashing"); else strVec.push_back("slashes");
            if (WILL_APPEND_ING) strVec.push_back("cutting"); else strVec.push_back("cuts");
            if (WILL_APPEND_ING) strVec.push_back("swinging"); else strVec.push_back("swings");
        }

        if ((WEAPON_SPTR->WeaponType() & item::weapon_type::BladedStaff) ||
            (WEAPON_SPTR->WeaponType() & item::weapon_type::Pointed) ||
            (WEAPON_SPTR->WeaponType() & item::weapon_type::Knife) ||
            (WEAPON_SPTR->WeaponType() & item::weapon_type::Spear))
        {
            if (WILL_APPEND_ING) strVec.push_back("stabbing"); else strVec.push_back("stabs");
            if (WILL_APPEND_ING) strVec.push_back("thrusting"); else strVec.push_back("thrusts");
            if (WILL_APPEND_ING) strVec.push_back("jabbing"); else strVec.push_back("jabs");
        }

        if (WEAPON_SPTR->WeaponType() & item::weapon_type::Blowpipe)
        {
            if (WILL_APPEND_ING) strVec.push_back("darting"); else strVec.push_back("darts");
        }

        if ((WEAPON_SPTR->WeaponType() & item::weapon_type::Blowpipe) ||
            (WEAPON_SPTR->WeaponType() & item::weapon_type::Bow) ||
            (WEAPON_SPTR->WeaponType() & item::weapon_type::Crossbow) ||
            (WEAPON_SPTR->WeaponType() & item::weapon_type::Projectile))
        {
            if (WILL_APPEND_ING) strVec.push_back("shooting"); else strVec.push_back("shoots");
            if (WILL_APPEND_ING) strVec.push_back("firing"); else strVec.push_back("fires");
        }

        if (WEAPON_SPTR->WeaponType() & item::weapon_type::Breath)
        {
            if (WILL_APPEND_ING) strVec.push_back("breathing"); else strVec.push_back("breathes");
        }

        if (WEAPON_SPTR->WeaponType() & item::weapon_type::Claws)
        {
            if (WILL_APPEND_ING) strVec.push_back("tearing"); else strVec.push_back("tears");
            if (WILL_APPEND_ING) strVec.push_back("slashing"); else strVec.push_back("slashes");
            if (WILL_APPEND_ING) strVec.push_back("clawing"); else strVec.push_back("claws");
            if (WILL_APPEND_ING) strVec.push_back("ripping"); else strVec.push_back("rips");
        }

        if ((WEAPON_SPTR->WeaponType() & item::weapon_type::Club) ||
            (WEAPON_SPTR->WeaponType() & item::weapon_type::Staff))
        {
            if (WILL_APPEND_ING) strVec.push_back("swinging"); else strVec.push_back("swings");
            if (WILL_APPEND_ING) strVec.push_back("bashing"); else strVec.push_back("bashes");
            if (WILL_APPEND_ING) strVec.push_back("smashing"); else strVec.push_back("smashes");
            if (WILL_APPEND_ING) strVec.push_back("slamming"); else strVec.push_back("slams");
            if (WILL_APPEND_ING) strVec.push_back("crushing"); else strVec.push_back("crushes");
        }

        if (WEAPON_SPTR->WeaponType() & item::weapon_type::Sling)
        {
            if (WILL_APPEND_ING) strVec.push_back("slinging"); else strVec.push_back("slings");
        }

        if (WEAPON_SPTR->WeaponType() & item::weapon_type::Whip)
        {
            if (WILL_APPEND_ING) strVec.push_back("whipping"); else strVec.push_back("whips");
        }

        if (WEAPON_SPTR->WeaponType() & item::weapon_type::Tendrils)
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

        std::unique(strVec.begin(), strVec.end());

        return strVec.at(utilz::random::Int(strVec.size() - 1));
    }


    const std::string Text::HitDescription(const combat::HitInfo & HIT_INFO)
    {
        switch (HIT_INFO.HitKind())
        {
            case combat::HitType::AmazingAccuracy:  { return "with amazing Accuracy"; }
            case combat::HitType::Luck:             { return "by a lucky shot"; }
            case combat::HitType::AmazingLuck:      { return "with amazing luck"; }
            case combat::HitType::Count:
            case combat::HitType::Accuracy:
            case combat::HitType::Spell:
            default:                                { return ""; }
        }
    }


    const std::string Text::DodgeDescription(const combat::HitInfo & HIT_INFO)
    {
        switch (HIT_INFO.DodgeKind())
        {
            case combat::DodgeType::AmazingSpeed:   { return "amazing speed"; }
            case combat::DodgeType::Luck:           { return "by a lucky move"; }
            case combat::DodgeType::AmazingLuck:    { return "with amazing luck"; }
            case combat::DodgeType::Count:
            case combat::DodgeType::Speed:
            default:                                { return ""; }
        }
    }


    const std::string Text::AttackDescriptionStatusVersion(const combat::FightResult & FIGHT_RESULT)
    {
        auto const CREATURES_FOUGHT_COUNT{ FIGHT_RESULT.Count() };
        M_ASSERT_OR_LOGANDTHROW_SS((CREATURES_FOUGHT_COUNT != 0), "game::combat::Text::AttackDescriptionStatusVersion() was given an empty FIGHT_RESULT.");

        std::ostringstream ss;
        if (CREATURES_FOUGHT_COUNT == 1)
        {
            ss << WeaponActionVerbList(FIGHT_RESULT, false) << " " << FirstTargetNamePhrase(FIGHT_RESULT) << " ";

            auto const HIT_COUNT{ FIGHT_RESULT.HitCount() };
            if (HIT_COUNT > 0)
            {
                ss << "hitting " << CountPhrase(HIT_COUNT) << " ";

                if (FIGHT_RESULT.FirstEffect().WasKill())
                {
                    ss << "killing " << creature::sex::HimHerIt(FIGHT_RESULT.FirstCreature()->Sex(), false);
                }
                else
                {
                    ss << "for " << FIGHT_RESULT.DamageTotal() << " damage";
                }

                ss << AttackConditionsSummaryList(FIGHT_RESULT);
            }
            else
            {
                ss << "but misses";
            }
        }
        else
        {
            ss << WeaponActionVerbList(FIGHT_RESULT, false) << " " << CREATURES_FOUGHT_COUNT << " creatures";

            auto const HIT_COUNT{ FIGHT_RESULT.HitCount() };
            if (HIT_COUNT > 0)
            {
                ss << "hitting " << CountPhrase(HIT_COUNT) << " ";

                std::size_t killCount{ 0 };
                auto const CREATURE_EFFECTS_VEC{ FIGHT_RESULT.Effects() };
                for (auto const & NEXT_CREATURE_EFFECT : CREATURE_EFFECTS_VEC)
                    if (NEXT_CREATURE_EFFECT.WasKill())
                        ++killCount;

                if (FIGHT_RESULT.DamageTotal() > 0)
                {
                    ss << "for " << FIGHT_RESULT.DamageTotal() << " damage";
                }

                if (killCount > 0)
                {
                    ss << ", killing " << killCount << ", ";
                }

                ss << AttackConditionsSummaryList(FIGHT_RESULT);
            }
            else
            {
                ss << " but missed them all";
            }
        }

        ss << ".";
        return ss.str();
    }


    const std::string Text::AttackDescriptionPreambleVersion(const FightResult & FIGHT_RESULT)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((FIGHT_RESULT.Count() != 0), "game::combat::Text::AttackDescriptionPreambleVersion() was given an empty FIGHT_RESULT.");

        std::ostringstream ss;
        ss << WeaponActionVerbList(FIGHT_RESULT, false) << " " << FirstTargetNamePhrase(FIGHT_RESULT) << "...";
        return ss.str();
    }


    const std::string Text::AttackDescriptionFullVersion(const FightResult & FIGHT_RESULT,
                                                         const std::size_t   EFFECT_INDEX,
                                                         const std::size_t   HIT_INDEX)
    {
        auto const CREATURE_EFFECTS_VEC{ FIGHT_RESULT.Effects() };

        M_ASSERT_OR_LOGANDTHROW_SS((CREATURE_EFFECTS_VEC.empty() == false),      "game::combat::Text::AttackDescriptionFullVersion(effect_index=" << EFFECT_INDEX << ", hit_index=" << HIT_INDEX << ") was given an empty FIGHT_RESULT.");
        M_ASSERT_OR_LOGANDTHROW_SS((EFFECT_INDEX < CREATURE_EFFECTS_VEC.size()), "game::combat::Text::AttackDescriptionFullVersion(effect_index=" << EFFECT_INDEX << ", hit_index=" << HIT_INDEX << ") effect_index was out of bounds with FIGHT_RESULT.Effects().size()="  << CREATURE_EFFECTS_VEC.size() << ".");

        auto const CREATURE_EFFECT{ CREATURE_EFFECTS_VEC.at(EFFECT_INDEX) };
        auto const HIT_INFO_VEC{ CREATURE_EFFECT.GetHitInfoVec() };

        M_ASSERT_OR_LOGANDTHROW_SS((HIT_INFO_VEC.empty() == false),   "game::combat::Text::AttackDescriptionFullVersion(effect_index=" << EFFECT_INDEX << ", hit_index=" << HIT_INDEX << ") was given an empty HIT_INFO_VEC.");
        M_ASSERT_OR_LOGANDTHROW_SS((HIT_INDEX < HIT_INFO_VEC.size()), "game::combat::Text::AttackDescriptionFullVersion(effect_index=" << EFFECT_INDEX << ", hit_index=" << HIT_INDEX << ") hit_index was out of bounds with FIGHT_RESULT.Effects()[effect_index].GetCount()=" << HIT_INFO_VEC.size() << ".");

        auto const HIT_INFO{ HIT_INFO_VEC.at(HIT_INDEX) };

        std::ostringstream ss;
        ss << HIT_INFO.ActionVerb() << " " << FirstTargetNamePhrase(FIGHT_RESULT) << "...\n";

        if (HIT_INFO.WasHit())
        {
            if ((HIT_INFO.IsPowerHit() == true) && (HIT_INFO.IsCritical() == false))
            {
                ss << "POWER HIT ";
            }
            else if ((HIT_INFO.IsPowerHit() == false) && (HIT_INFO.IsCritical() == true))
            {
                ss << "CRITICAL HIT ";
            }
            else if ((HIT_INFO.IsPowerHit() == true) && (HIT_INFO.IsCritical() == true))
            {
                ss << "POWER AND CRITICAL HIT ";
            }
            else
            {
                if (HIT_INFO.Weapon()->IsBodypart() == false)
                {
                    ss << "and hits ";
                }
            }

            if (HIT_INFO.Damage() > 0)
            {
                ss << "for " << HIT_INFO.Damage() << " damage";
            }
            else
            {
                ss << "but does no damage";
            }

            auto const CONDITIONS_VEC{ HIT_INFO.Conditions() };
            if (CONDITIONS_VEC.empty() == false)
            {
                if (HIT_INFO.WasKill())
                {
                    ss << " and kills";
                }
                else
                {
                    ss << " and causes " << creature::condition::Algorithms::Names(CONDITIONS_VEC, ", ", false, 0, 0, true);
                }
            }
        }
        else
        {
            ss << "but misses";
        }

        return ss.str();
    }


    const std::string Text::CastDescriptionStatusVersion(const TurnActionInfo & TURN_ACTION_INFO, const FightResult & FIGHT_RESULT)
    {
        std::ostringstream ss;

        ss << "casts the " << TURN_ACTION_INFO.Spell()->Name() << "spell " << TargetType::ActionPhrase(TURN_ACTION_INFO.Spell()->TargetType());

        if ((TURN_ACTION_INFO.Spell()->TargetType() == TargetType::AllCharacters) ||
            (TURN_ACTION_INFO.Spell()->TargetType() == TargetType::AllEnemies))
        {
            ss << " effecting " << FIGHT_RESULT.Count();
        }

        ss << ".";
        return ss.str();
    }


    const std::string Text::CastDescriptionPreambleVersion(const TurnActionInfo & TURN_ACTION_INFO,
                                                           const FightResult &)
    {
        std::ostringstream ss;
        ss << "casts the " << TURN_ACTION_INFO.Spell()->Name() << "spell " << TargetType::ActionPhrase(TURN_ACTION_INFO.Spell()->TargetType()) << "...";
        return ss.str();
    }



    const std::string Text::CastDescriptionFullVersion(const TurnActionInfo &,
                                                       const FightResult &,
                                                       const std::size_t,
                                                       const std::size_t)
    {
        std::ostringstream ss;

        ss << "CastDescription TODO.";

        return ss.str();
    }


    const std::string Text::WeaponActionVerbList(const FightResult & FIGHT_RESULT, const bool WILL_SKIP_MISSES)
    {
        std::vector<std::string> strVec;
        auto const CREATURE_EFFECTS_VEC{ FIGHT_RESULT.Effects() };

        for (auto const & NEXT_CREATURE_EFFECT : CREATURE_EFFECTS_VEC)
        {
            auto const HIT_INFO_VEC{ NEXT_CREATURE_EFFECT.GetHitInfoVec() };
            for (auto const & NEXT_HIT_INFO : HIT_INFO_VEC)
            {
                if ((WILL_SKIP_MISSES == false) || (WILL_SKIP_MISSES && (NEXT_HIT_INFO.WasHit() == true)))
                {
                    strVec.push_back(NEXT_HIT_INFO.ActionVerb());
                }
            }
        }

        if (strVec.size() > 1)
        {
            std::unique(strVec.begin(), strVec.end());
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


    const std::string Text::AttackConditionsSummaryList(const FightResult & FIGHT_RESULT)
    {
        std::ostringstream ss;

        const std::size_t NUM_CONDITIONS_TO_LIST{ 3 };
        auto const CONDITIONS_EXCLUDING_DEAD_SVEC{ utilz::Vector::Exclude(FIGHT_RESULT.Conditions(true), creature::Conditions::Dead) };
        auto const NUM_CONDITIONS{ CONDITIONS_EXCLUDING_DEAD_SVEC.size() };
        if (NUM_CONDITIONS != 0)
        {
            ss << " and causing " << creature::condition::Algorithms::Names(CONDITIONS_EXCLUDING_DEAD_SVEC, ", ", false, NUM_CONDITIONS_TO_LIST, 0, true);

            if (NUM_CONDITIONS > NUM_CONDITIONS_TO_LIST)
            {
                ss << "...";
            }
        }

        return ss.str();
    }


    const std::string Text::FirstTargetNamePhrase(const FightResult & FIGHT_RESULT)
    {
        std::ostringstream ss;

        auto const CREATURE_PTR{ FIGHT_RESULT.FirstCreature() };
        if (CREATURE_PTR->IsPlayerCharacter())
        {
            ss << "at " << CREATURE_PTR->NameOrRaceAndClass();
        }
        else
        {
            ss << "at a " << CREATURE_PTR->NameOrRaceAndClass();
        }

        return ss.str();
    }


    const std::string Text::InitialCombatStatusMessagePrefix()
    {
        switch (utilz::random::Int(4))
        {
            case 0:  { return "You face"; }
            case 1:  { return "Before you rage"; }
            case 2:  { return "Before you stand"; }
            case 3:  { return "Attacking you are"; }
            case 4:
            default: { return "You encounter"; }
        }
    }

}
}
