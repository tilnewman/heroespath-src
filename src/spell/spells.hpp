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
#ifndef HEROESPATH_SPELL_SPELLS_HPP_INCLUDED
#define HEROESPATH_SPELL_SPELLS_HPP_INCLUDED
//
// spells.hpp
//
#include "combat/name-position-enum.hpp"
#include "creature/condition-enum.hpp"
#include "game/phase-enum.hpp"
#include "spell/spell-base.hpp"

#include "misc/random.hpp"

#include <string>

namespace heroespath
{

// forward declarations
namespace creature
{
    class Creature;
    using CreaturePtr_t = Creature *;
} // namespace creature

namespace spell
{

    class Sparks : public Spell
    {
    public:
        Sparks()
            : Spell(
                  Spells::Sparks,
                  combat::EffectType::CreatureHarmDamage,
                  game::Phase::Combat,
                  2_mana,
                  1_rank,
                  combat::TargetType::SingleOpponent,
                  "sprays with sparks",
                  "sprayed with sparks")
        {}

        virtual ~Sparks() {}

        virtual bool EffectCreature(
            creature::CreaturePtr_t castingCreaturePtr,
            creature::CreaturePtr_t creatureCastUponPtr,
            Health_t & healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            combat::ContentAndNamePos & actionPhraseCNP) const;

    private:
        virtual const combat::ContentAndNamePos ActionPhrase() const;
    };

    class Bandage : public Spell
    {
    public:
        Bandage()
            : Spell(
                  Spells::Bandage,
                  combat::EffectType::CreatureHelpHeal,
                  static_cast<game::Phase::Enum>(
                      game::Phase::Combat | game::Phase::Exploring | game::Phase::Inventory),
                  1_mana,
                  1_rank,
                  combat::TargetType::SingleCompanion,
                  "bandages",
                  "bandaged")
        {}

        virtual ~Bandage() {}

        virtual bool EffectCreature(
            creature::CreaturePtr_t castingCreaturePtr,
            creature::CreaturePtr_t creatureCastUponPtr,
            Health_t & healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            combat::ContentAndNamePos & actionPhraseCNP) const;

    private:
        virtual const combat::ContentAndNamePos ActionPhrase() const;
    };

    class Sleep : public Spell
    {
    public:
        Sleep()
            : Spell(
                  Spells::Sleep,
                  combat::EffectType::CreatureHarmMisc,
                  game::Phase::Combat,
                  1_mana,
                  1_rank,
                  combat::TargetType::SingleOpponent,
                  "sleeps",
                  "slept")
        {}

        virtual ~Sleep() {}

        virtual bool EffectCreature(
            creature::CreaturePtr_t castingCreaturePtr,
            creature::CreaturePtr_t creatureCastUponPtr,
            Health_t & healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            combat::ContentAndNamePos & actionPhraseCNP) const;

    private:
        virtual const combat::ContentAndNamePos ActionPhrase() const;
    };

    class Awaken : public Spell
    {
    public:
        Awaken()
            : Spell(
                  Spells::Awaken,
                  combat::EffectType::CreatureHelpMisc,
                  static_cast<game::Phase::Enum>(
                      game::Phase::Combat | game::Phase::Exploring | game::Phase::Inventory),
                  1_mana,
                  1_rank,
                  combat::TargetType::SingleCompanion,
                  "awakens",
                  "awakened")
        {}

        virtual ~Awaken() {}

        virtual bool EffectCreature(
            creature::CreaturePtr_t castingCreaturePtr,
            creature::CreaturePtr_t creatureCastUponPtr,
            Health_t & healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            combat::ContentAndNamePos & actionPhraseCNP) const;

    private:
        virtual const combat::ContentAndNamePos ActionPhrase() const;
    };

    class Trip : public Spell
    {
    public:
        Trip()
            : Spell(
                  Spells::Trip,
                  combat::EffectType::CreatureHarmMisc,
                  game::Phase::Combat,
                  1_mana,
                  1_rank,
                  combat::TargetType::SingleOpponent,
                  "trips",
                  "tripped")
        {}

        virtual ~Trip() {}

        virtual bool EffectCreature(
            creature::CreaturePtr_t castingCreaturePtr,
            creature::CreaturePtr_t creatureCastUponPtr,
            Health_t & healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            combat::ContentAndNamePos & actionPhraseCNP) const;

    private:
        virtual const combat::ContentAndNamePos ActionPhrase() const;
    };

    class Lift : public Spell
    {
    public:
        Lift()
            : Spell(
                  Spells::Lift,
                  combat::EffectType::CreatureHelpMisc,
                  static_cast<game::Phase::Enum>(
                      game::Phase::Combat | game::Phase::Exploring | game::Phase::Inventory),
                  1_mana,
                  1_rank,
                  combat::TargetType::SingleCompanion,
                  "lifts",
                  "lifted")
        {}

        virtual ~Lift() {}

        virtual bool EffectCreature(
            creature::CreaturePtr_t castingCreaturePtr,
            creature::CreaturePtr_t creatureCastUponPtr,
            Health_t & healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            combat::ContentAndNamePos & actionPhraseCNP) const;

    private:
        virtual const combat::ContentAndNamePos
            ActionPhrase(creature::CreaturePtr_t creatureCastUponPtr) const;
    };

    class Daze : public Spell
    {
    public:
        Daze()
            : Spell(
                  Spells::Daze,
                  combat::EffectType::CreatureHarmMisc,
                  game::Phase::Combat,
                  1_mana,
                  1_rank,
                  combat::TargetType::SingleOpponent,
                  "dazes",
                  "dazed")
        {}

        virtual ~Daze() {}

        virtual bool EffectCreature(
            creature::CreaturePtr_t castingCreaturePtr,
            creature::CreaturePtr_t creatureCastUponPtr,
            Health_t & healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            combat::ContentAndNamePos & actionPhraseCNP) const;

    private:
        virtual const combat::ContentAndNamePos ActionPhrase() const;
    };

    class Panic : public Spell
    {
    public:
        Panic()
            : Spell(
                  Spells::Panic,
                  combat::EffectType::CreatureHarmMisc,
                  game::Phase::Combat,
                  1_mana,
                  1_rank,
                  combat::TargetType::SingleOpponent,
                  "panics",
                  "panicked")
        {}

        virtual ~Panic() {}

        virtual bool EffectCreature(
            creature::CreaturePtr_t castingCreaturePtr,
            creature::CreaturePtr_t creatureCastUponPtr,
            Health_t & healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            combat::ContentAndNamePos & actionPhraseCNP) const;

    private:
        virtual const combat::ContentAndNamePos ActionPhrase() const;
    };

    class ClearMind : public Spell
    {
    public:
        ClearMind()
            : Spell(
                  Spells::ClearMind,
                  combat::EffectType::CreatureHelpMisc,
                  static_cast<game::Phase::Enum>(
                      game::Phase::Combat | game::Phase::Exploring | game::Phase::Inventory),
                  1_mana,
                  1_rank,
                  combat::TargetType::SingleCompanion,
                  "clears the mind",
                  "clear headed")
        {}

        virtual ~ClearMind() {}

        virtual bool EffectCreature(
            creature::CreaturePtr_t castingCreaturePtr,
            creature::CreaturePtr_t creatureCastUponPtr,
            Health_t & healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            combat::ContentAndNamePos & actionPhraseCNP) const;

    private:
        virtual const combat::ContentAndNamePos ActionPhrase() const;
    };

    class Poison : public Spell
    {
    public:
        Poison()
            : Spell(
                  Spells::Poison,
                  combat::EffectType::CreatureHarmMisc,
                  game::Phase::Combat,
                  1_mana,
                  1_rank,
                  combat::TargetType::SingleOpponent,
                  "poisons",
                  "poisoned")
        {}

        virtual ~Poison() {}

        virtual bool EffectCreature(
            creature::CreaturePtr_t castingCreaturePtr,
            creature::CreaturePtr_t creatureCastUponPtr,
            Health_t & healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            combat::ContentAndNamePos & actionPhraseCNP) const;

    private:
        virtual const combat::ContentAndNamePos ActionPhrase() const;
    };

    class Antidote : public Spell
    {
    public:
        Antidote()
            : Spell(
                  Spells::Antidote,
                  combat::EffectType::CreatureHelpMisc,
                  static_cast<game::Phase::Enum>(
                      game::Phase::Combat | game::Phase::Exploring | game::Phase::Inventory),
                  1_mana,
                  1_rank,
                  combat::TargetType::SingleCompanion,
                  "cures",
                  "cured")
        {}

        virtual ~Antidote() {}

        virtual bool EffectCreature(
            creature::CreaturePtr_t castingCreaturePtr,
            creature::CreaturePtr_t creatureCastUponPtr,
            Health_t & healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            combat::ContentAndNamePos & actionPhraseCNP) const;

    private:
        virtual const combat::ContentAndNamePos ActionPhrase() const;
    };

    class PoisonCloud : public Spell
    {
    public:
        PoisonCloud()
            : Spell(
                  Spells::PoisonCloud,
                  combat::EffectType::CreatureHarmMisc,
                  game::Phase::Combat,
                  3_mana,
                  1_rank,
                  combat::TargetType::AllOpponents,
                  "poisons",
                  "poisoned")
        {}

        virtual ~PoisonCloud() {}

        virtual bool EffectCreature(
            creature::CreaturePtr_t castingCreaturePtr,
            creature::CreaturePtr_t creatureCastUponPtr,
            Health_t & healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            combat::ContentAndNamePos & actionPhraseCNP) const;

    private:
        virtual const combat::ContentAndNamePos ActionPhrase() const;
    };
} // namespace spell
} // namespace heroespath

#endif // HEROESPATH_SPELL_SPELLS_HPP_INCLUDED
