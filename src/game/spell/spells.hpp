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
#include "game/name-position-enum.hpp"

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
        Sparks() : Spell(Spells::Sparks,
                         EffectType::CreatureHarmDamage,
                         Phase::Combat,
                         1,
                         1,
                         TargetType::SingleOpponent,
                         "sprays with sparks",
                         "sprayed with sparks") {}

        virtual ~Sparks() {}
      
        virtual bool EffectCreature(
            creature::CreaturePtr_t   castingCreaturePtr,
            creature::CreaturePtr_t   creatureCastUponPtr,
            stats::Health_t &         healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            ContentAndNamePos &       actionPhraseCNP) const;

    private:
        virtual const ContentAndNamePos ActionPhrase() const;
    };


    class Bandage : public Spell
    {
    public:
        Bandage() : Spell(Spells::Bandage,
                          EffectType::CreatureHelpHeal,
                          static_cast<Phase::Enum>(Phase::Combat |
                                                   Phase::Exploring |
                                                   Phase::Inventory),
                          1,
                          1,
                          TargetType::SingleCompanion,
                          "bandages",
                          "bandaged") {}

        virtual ~Bandage() {}
      
        virtual bool EffectCreature(
            creature::CreaturePtr_t   castingCreaturePtr,
            creature::CreaturePtr_t   creatureCastUponPtr,
            stats::Health_t &         healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            ContentAndNamePos &       actionPhraseCNP) const;

    private:
        virtual const ContentAndNamePos ActionPhrase() const;
    };


    class Sleep : public Spell
    {
    public:
        Sleep() : Spell(Spells::Sleep,
                        EffectType::CreatureHarmMisc,
                        Phase::Combat,
                        1,
                        1,
                        TargetType::SingleOpponent,
                        "sleeps",
                        "slept") {}

        virtual ~Sleep() {}

        virtual bool EffectCreature(
            creature::CreaturePtr_t   castingCreaturePtr,
            creature::CreaturePtr_t   creatureCastUponPtr,
            stats::Health_t &         healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            ContentAndNamePos &       actionPhraseCNP) const;

    private:
        virtual const ContentAndNamePos ActionPhrase() const;
    };


    class Awaken : public Spell
    {
    public:
        Awaken() : Spell(Spells::Awaken,
                         EffectType::CreatureHelpMisc,
                         static_cast<Phase::Enum>(Phase::Combat |
                                                  Phase::Exploring |
                                                  Phase::Inventory),
                         1,
                         1,
                         TargetType::SingleCompanion,
                         "awakens",
                         "awakened") {}

        virtual ~Awaken() {}

        virtual bool EffectCreature(
            creature::CreaturePtr_t   castingCreaturePtr,
            creature::CreaturePtr_t   creatureCastUponPtr,
            stats::Health_t &         healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            ContentAndNamePos &       actionPhraseCNP) const;

    private:
        virtual const ContentAndNamePos ActionPhrase() const;
    };


    class Trip : public Spell
    {
    public:
        Trip() : Spell(Spells::Trip,
                       EffectType::CreatureHarmMisc,
                       Phase::Combat,
                       1,
                       1,
                       TargetType::SingleOpponent,
                       "trips",
                       "tripped") {}

        virtual ~Trip() {}

        virtual bool EffectCreature(
            creature::CreaturePtr_t   castingCreaturePtr,
            creature::CreaturePtr_t   creatureCastUponPtr,
            stats::Health_t &         healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            ContentAndNamePos &       actionPhraseCNP) const;

    private:
        virtual const ContentAndNamePos ActionPhrase() const;
    };


    class Lift : public Spell
    {
    public:
        Lift() : Spell(Spells::Lift,
                       EffectType::CreatureHelpMisc,
                       static_cast<Phase::Enum>(Phase::Combat |
                                                Phase::Exploring |
                                                Phase::Inventory),
                       1,
                       1,
                       TargetType::SingleCompanion,
                       "lifts",
                       "lifted") {}

        virtual ~Lift() {}

        virtual bool EffectCreature(
            creature::CreaturePtr_t   castingCreaturePtr,
            creature::CreaturePtr_t   creatureCastUponPtr,
            stats::Health_t &         healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            ContentAndNamePos &       actionPhraseCNP) const;

    private:
        virtual const ContentAndNamePos ActionPhrase(
            creature::CreaturePtr_t creatureCastUponPtr) const;
    };


    class Daze : public Spell
    {
    public:
        Daze() : Spell(Spells::Daze,
                       EffectType::CreatureHarmMisc,
                       Phase::Combat,
                       1,
                       1,
                       TargetType::SingleOpponent,
                       "dazes",
                       "dazed") {}

        virtual ~Daze() {}

        virtual bool EffectCreature(
            creature::CreaturePtr_t   castingCreaturePtr,
            creature::CreaturePtr_t   creatureCastUponPtr,
            stats::Health_t &         healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            ContentAndNamePos &       actionPhraseCNP) const;

    private:
        virtual const ContentAndNamePos ActionPhrase() const;
    };


    class Panic : public Spell
    {
    public:
        Panic() : Spell(Spells::Panic,
                        EffectType::CreatureHarmMisc,
                        Phase::Combat,
                        1,
                        1,
                        TargetType::SingleOpponent,
                        "panics",
                        "panicked") {}

        virtual ~Panic() {}

        virtual bool EffectCreature(
            creature::CreaturePtr_t   castingCreaturePtr,
            creature::CreaturePtr_t   creatureCastUponPtr,
            stats::Health_t &         healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            ContentAndNamePos &       actionPhraseCNP) const;

    private:
        virtual const ContentAndNamePos ActionPhrase() const;
    };


    class ClearMind : public Spell
    {
    public:
        ClearMind() : Spell(Spells::ClearMind,
                            EffectType::CreatureHelpMisc,
                            static_cast<Phase::Enum>(Phase::Combat |
                                                     Phase::Exploring |
                                                     Phase::Inventory),
                            1,
                            1,
                            TargetType::SingleCompanion,
                            "clears the mind",
                            "clear headed") {}

        virtual ~ClearMind() {}

        virtual bool EffectCreature(
            creature::CreaturePtr_t   castingCreaturePtr,
            creature::CreaturePtr_t   creatureCastUponPtr,
            stats::Health_t &         healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            ContentAndNamePos &       actionPhraseCNP) const;

    private:
        virtual const ContentAndNamePos ActionPhrase() const;
    };


    class Poison : public Spell
    {
    public:
        Poison() : Spell(Spells::Poison,
                         EffectType::CreatureHarmMisc,
                         Phase::Combat,
                         1,
                         1,
                         TargetType::SingleOpponent,
                         "poisons",
                         "poisoned") {}

        virtual ~Poison() {}

        virtual bool EffectCreature(
            creature::CreaturePtr_t   castingCreaturePtr,
            creature::CreaturePtr_t   creatureCastUponPtr,
            stats::Health_t &         healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            ContentAndNamePos &       actionPhraseCNP) const;

    private:
        virtual const ContentAndNamePos ActionPhrase() const;
    };


    class Antidote : public Spell
    {
    public:
        Antidote() : Spell(Spells::Antidote,
                           EffectType::CreatureHelpMisc,
                           static_cast<Phase::Enum>(Phase::Combat |
                                                    Phase::Exploring |
                                                    Phase::Inventory),
                           1,
                           1,
                           TargetType::SingleCompanion,
                           "cures",
                           "cured") {}

        virtual ~Antidote() {}

        virtual bool EffectCreature(
            creature::CreaturePtr_t   castingCreaturePtr,
            creature::CreaturePtr_t   creatureCastUponPtr,
            stats::Health_t &        healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            ContentAndNamePos &       actionPhraseCNP) const;

    private:
        virtual const ContentAndNamePos ActionPhrase() const;
    };


    class PoisonCloud : public Spell
    {
    public:
        PoisonCloud() : Spell(Spells::PoisonCloud,
                              EffectType::CreatureHarmMisc,
                              Phase::Combat,
                              3,
                              1,
                              TargetType::AllOpponents,
                              "poisons",
                              "poisoned") {}

        virtual ~PoisonCloud() {}

        virtual bool EffectCreature(
            creature::CreaturePtr_t   castingCreaturePtr,
            creature::CreaturePtr_t   creatureCastUponPtr,
            stats::Health_t &         healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            ContentAndNamePos &       actionPhraseCNP) const;

    private:
        virtual const ContentAndNamePos ActionPhrase() const;
    };

}
}

#endif //GAME_SPELL_SPELLS_HPP_INCLUDED
