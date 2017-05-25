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
#ifndef GAME_SPELL_SPELLS_HPP_INCLUDED
#define GAME_SPELL_SPELLS_HPP_INCLUDED
//
// spells.hpp
//
#include "game/spell/spell-base.hpp"
#include "game/phase-enum.hpp"
#include "game/creature/condition-enum.hpp"

#include "misc/random.hpp"

#include <string>


namespace game
{

//forward declarations
namespace creature
{
    class Creature;
    using CreaturePtr_t = Creature *;
}

namespace spell
{

    class Sparks : public Spell
    {
    public:
        Sparks() : Spell(Spells::Sparks, SpellType::Attack, Phase::Combat, 1, 1, TargetType::SingleOpponent) {}
        virtual ~Sparks() {}
        virtual const std::string ActionPhrase(creature::CreaturePtr_t castingCreaturePtr, creature::CreaturePtr_t effectedCreaturePtr) const;
        virtual stats::Health_t HealthAdj(creature::CreaturePtr_t castingCreaturePtr, creature::CreaturePtr_t effectedCreaturePtr) const;
    };


    class Bandage : public Spell
    {
    public:
        Bandage() : Spell(Spells::Bandage, SpellType::Heal, static_cast<Phase::Enum>(Phase::Combat | Phase::Exploring | Phase::Inventory), 1, 1, TargetType::SingleCompanion) {}
        virtual ~Bandage() {}
        virtual const std::string ActionPhrase(creature::CreaturePtr_t castingCreaturePtr, creature::CreaturePtr_t effectedCreaturePtr) const;
        virtual stats::Health_t HealthAdj(creature::CreaturePtr_t castingCreaturePtr, creature::CreaturePtr_t effectedCreaturePtr) const;
    };


    class Sleep : public Spell
    {
    public:
        Sleep() : Spell(Spells::Sleep, SpellType::EffectCreature, Phase::Combat, 1, 1, TargetType::SingleOpponent) {}
        virtual ~Sleep() {}
        virtual const std::string EffectCreature(creature::CreaturePtr_t castingCreaturePtr, creature::CreaturePtr_t effectedCreaturePtr, creature::ConditionEnumVec_t & conditionsAddedVec) const;
    };


    class Awaken : public Spell
    {
    public:
        Awaken() : Spell(Spells::Awaken, SpellType::EffectCreature, static_cast<Phase::Enum>(Phase::Combat | Phase::Exploring | Phase::Inventory), 1, 1, TargetType::SingleCompanion) {}
        virtual ~Awaken() {}
        virtual const std::string EffectCreature(creature::CreaturePtr_t castingCreaturePtr, creature::CreaturePtr_t effectedCreaturePtr, creature::ConditionEnumVec_t & conditionsAddedVec) const;
    };


    class Trip : public Spell
    {
    public:
        Trip() : Spell(Spells::Trip, SpellType::EffectCreature, static_cast<Phase::Enum>(Phase::Combat), 1, 1, TargetType::SingleOpponent) {}
        virtual ~Trip() {}
        virtual const std::string EffectCreature(creature::CreaturePtr_t castingCreaturePtr, creature::CreaturePtr_t effectedCreaturePtr, creature::ConditionEnumVec_t & conditionsAddedVec) const;
    };


    class Lift : public Spell
    {
    public:
        Lift() : Spell(Spells::Lift, SpellType::EffectCreature, static_cast<Phase::Enum>(Phase::Combat | Phase::Exploring | Phase::Inventory), 1, 1, TargetType::SingleCompanion) {}
        virtual ~Lift() {}
        virtual const std::string EffectCreature(creature::CreaturePtr_t castingCreaturePtr, creature::CreaturePtr_t effectedCreaturePtr, creature::ConditionEnumVec_t & conditionsAddedVec) const;
    };


    class Daze : public Spell
    {
    public:
        Daze() : Spell(Spells::Daze, SpellType::EffectCreature, static_cast<Phase::Enum>(Phase::Combat), 1, 1, TargetType::SingleOpponent) {}
        virtual ~Daze() {}
        virtual const std::string EffectCreature(creature::CreaturePtr_t castingCreaturePtr, creature::CreaturePtr_t effectedCreaturePtr, creature::ConditionEnumVec_t & conditionsAddedVec) const;
    };


    class Frighten : public Spell
    {
    public:
        Frighten() : Spell(Spells::Frighten, SpellType::EffectCreature, static_cast<Phase::Enum>(Phase::Combat), 1, 1, TargetType::SingleOpponent) {}
        virtual ~Frighten() {}
        virtual const std::string EffectCreature(creature::CreaturePtr_t castingCreaturePtr, creature::CreaturePtr_t effectedCreaturePtr, creature::ConditionEnumVec_t & conditionsAddedVec) const;
    };


    class ClearMind : public Spell
    {
    public:
        ClearMind() : Spell(Spells::ClearMind, SpellType::EffectCreature, static_cast<Phase::Enum>(Phase::Combat | Phase::Exploring | Phase::Inventory), 1, 1, TargetType::SingleCompanion) {}
        virtual ~ClearMind() {}
        virtual const std::string EffectCreature(creature::CreaturePtr_t castingCreaturePtr, creature::CreaturePtr_t effectedCreaturePtr, creature::ConditionEnumVec_t & conditionsAddedVec) const;
    };


    class Poison : public Spell
    {
    public:
        Poison() : Spell(Spells::Poison, SpellType::EffectCreature, static_cast<Phase::Enum>(Phase::Combat), 1, 1, TargetType::SingleOpponent) {}
        virtual ~Poison() {}
        virtual const std::string EffectCreature(creature::CreaturePtr_t castingCreaturePtr, creature::CreaturePtr_t effectedCreaturePtr, creature::ConditionEnumVec_t & conditionsAddedVec) const;
    };


    class Antidote : public Spell
    {
    public:
        Antidote() : Spell(Spells::Antidote, SpellType::EffectCreature, static_cast<Phase::Enum>(Phase::Combat | Phase::Exploring | Phase::Inventory), 1, 1, TargetType::SingleCompanion) {}
        virtual ~Antidote() {}
        virtual const std::string EffectCreature(creature::CreaturePtr_t castingCreaturePtr, creature::CreaturePtr_t effectedCreaturePtr, creature::ConditionEnumVec_t & conditionsAddedVec) const;
    };


    class PoisonCloud : public Spell
    {
    public:
        PoisonCloud() : Spell(Spells::PoisonCloud, SpellType::EffectCreature, static_cast<Phase::Enum>(Phase::Combat), 3, 1, TargetType::AllOpponents) {}
        virtual ~PoisonCloud() {}
        virtual const std::string EffectCreature(creature::CreaturePtr_t castingCreaturePtr, creature::CreaturePtr_t effectedCreaturePtr, creature::ConditionEnumVec_t & conditionsAddedVec) const;
    };

}
}

#endif //GAME_SPELL_SPELLS_HPP_INCLUDED
