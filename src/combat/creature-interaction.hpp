// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_COMBAT_CREATURE_INTERACTION_HPP_INCLUDED
#define HEROESPATH_COMBAT_CREATURE_INTERACTION_HPP_INCLUDED
//
// creature-interaction.hpp
//
#include "combat/fight-results.hpp"
#include "creature/trait.hpp"
#include "game/phase-enum.hpp"
#include "game/strong-types.hpp"
#include "item/armor-ratings.hpp"
#include "misc/not-null.hpp"

#include <memory>
#include <string>

namespace heroespath
{
namespace spell
{
    class Spell;
    using SpellPtr_t = misc::NotNull<Spell *>;
} // namespace spell
namespace song
{
    class Song;
    using SongPtr_t = misc::NotNull<Song *>;
} // namespace song
namespace item
{
    class Item;
    using ItemPtr_t = misc::NotNull<Item *>;
} // namespace item
namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
} // namespace creature
namespace combat
{

    class Trap;

    class CombatDisplay;
    using CombatDisplayPtr_t = misc::NotNull<CombatDisplay *>;

    // A collection of functions that implement creature interactions related to combat, or any
    // other situation where creatures can be hurt or healed or where creature conditions are at
    // play.
    class CreatureInteraction
    {
    public:
        CreatureInteraction();

        const FightResult Fight(
            const creature::CreaturePtr_t CREATURE_ATTACKING_PTR,
            const creature::CreaturePtr_t CREATURE_DEFENDING_PTR,
            const bool WILL_FORCE_HIT = false) const;

        // negative values are damaging, positive values are healing
        void HandleDamage(
            const creature::CreaturePtr_t CREATURE_DEFENDING_PTR,
            HitInfoVec_t & hitInfoVec,
            const Health_t & HEALTH_ADJ,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            const bool CAN_ADD_CONDITIONS = true) const;

        // returns true if an effect consumed the current turn
        // fightResult_OutParam is guaranteed to only have one CreatureEffect
        // if hitInfoVec_OutParam is empty return value will be false
        bool ProcessConditionEffects(
            const game::Phase::Enum HEROESPATH_PHASE,
            const creature::CreaturePtr_t CREATURE_PTR,
            HitInfoVec_t & hitInfoVec_OuParam) const;

        void AddConditionsBasedOnDamage(
            const creature::CreaturePtr_t CREATURE_DEFENDING_PTR,
            const Health_t & DAMAGE_ABS,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            HitInfoVec_t & hitInfoVec) const;

        // returns true if any conditions were removed from the creature,
        // not from the hitInfoVec
        bool RemoveAddedConditions(
            const creature::CondEnumVec_t & CONDS_VEC,
            const creature::CreaturePtr_t CREATURE_PTR,
            HitInfoVec_t & hitInfoVec,
            creature::CondEnumVec_t & condsRemovedVec) const;

        // returns true if a condition was removed from the creature,
        // not from the hitInfoVec
        bool RemoveAddedCondition(
            const creature::Conditions::Enum COND_ENUM,
            const creature::CreaturePtr_t CREATURE_PTR,
            HitInfoVec_t & hitInfoVec,
            creature::CondEnumVec_t & condsRemovedVec) const;

        const FightResult Cast(
            const spell::SpellPtr_t SPELL_PTR,
            const creature::CreaturePtr_t CREATURE_CASTING_PTR,
            const creature::CreaturePVec_t & creaturesDefendingPVec) const;

        const FightResult PlaySong(
            const song::SongPtr_t SONG_PTR,
            const creature::CreaturePtr_t CREATURE_PLAYING_PTR,
            const creature::CreaturePVec_t & creaturesListeningPVec) const;

        const FightResult Pounce(
            const creature::CreaturePtr_t CREATURE_POUNCING_PTR,
            const creature::CreaturePtr_t CREATURE_DEFENDING_PTR) const;

        const FightResult Roar(
            const creature::CreaturePtr_t CREATURE_ROARING_PTR,
            const CombatDisplayPtr_t COMBAT_DISPLAY_PTR) const;

        const creature::CreaturePtr_t FindNonPlayerCreatureToAttack(
            const creature::CreaturePtr_t CREATURE_ATTACKING_PTR,
            const CombatDisplayPtr_t COMBAT_DISPLAY_PTR) const;

        const FightResult TreasureTrap(
            const combat::Trap & TRAP,
            const creature::CreaturePtr_t CREATURE_PICKING_THE_LOCK_PTR) const;

    private:
        const creature::CreaturePVec_t RandomSelectWhoIsHurtByTrap(
            const combat::Trap & TRAP,
            const creature::CreaturePtr_t CREATURE_PICKING_THE_LOCK_PTR) const;

        const HitInfoVec_t AttackWithAllWeapons(
            const creature::CreaturePtr_t CREATURE_ATTACKING_PTR,
            const creature::CreaturePtr_t CREATURE_DEFENDING_PTR,
            const bool WILL_FORCE_HIT = false) const;

        const HitInfo AttackWithSingleWeapon(
            HitInfoVec_t & hitInfoVec,
            const item::ItemPtr_t WEAPON_PTR,
            const creature::CreaturePtr_t CREATURE_ATTACKING_PTR,
            const creature::CreaturePtr_t CREATURE_DEFENDING_PTR,
            const bool WILL_FORCE_HIT = false) const;

        Health_t DetermineDamage(
            const item::ItemPtr_t WEAPON_PTR,
            const creature::CreaturePtr_t CREATURE_ATTACKING_PTR,
            const creature::CreaturePtr_t CREATURE_DEFENDING_PTR,
            bool & isPowerHit_OutParam,
            bool & isCriticalHit_OutParam,
            bool & didArmorAbsorb_OutParam) const;

        bool AreAnyOfCondsContained(
            const creature::CondEnumVec_t & CONDS_VEC,
            const creature::CreaturePtr_t CREATURE_PTR,
            const HitInfoVec_t & HIT_INFO_VEC) const;

        creature::Trait_t IsValuetHigherThanRatioOfStat(
            const creature::Trait_t STAT_VALUE,
            const creature::Trait_t STAT_MAX,
            const float RATIO) const;

        item::ArmorRatings armorRatings_;
    };

} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_COMBAT_CREATURE_INTERACTION_HPP_INCLUDED
