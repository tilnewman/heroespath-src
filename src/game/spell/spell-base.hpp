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
#ifndef GAME_SPELL_Spell_HPP_INCLUDED
#define GAME_SPELL_Spell_HPP_INCLUDED
//
// spell-base.hpp
//
#include "game/types.hpp"
#include "game/stats/types.hpp"
#include "game/spell/spell-enum.hpp"
#include "game/effect-type-enum.hpp"
#include "game/target-enum.hpp"
#include "game/phase-enum.hpp"
#include "game/creature/condition-enum.hpp"
#include "game/name-position-enum.hpp"

#include <string>
#include <vector>
#include <tuple>
#include <memory>


namespace game
{

//forward declarations
namespace creature
{
    class Creature;
    using CreaturePtr_t = Creature *;
}

namespace item
{
    class Item;
    using ItemPtr_t = Item *;
}

namespace spell
{

    //common base code to all spell classes
    class Spell
    {
    public:
        Spell(const Spells::Enum     WHICH,
              const EffectType::Enum EFFECT_TYPE,
              const Phase::Enum      VALID_PHASES,
              const stats::Trait_t   MANA_COST,
              const Rank_t           RANK,
              const TargetType::Enum TARGET_TYPE,
              const std::string &    VERB_THIRD_PERSON,
              const std::string &    VERB_PAST_TENSE);

        virtual ~Spell();

        inline const std::string Name() const       { return Spells::Name(which_); }

        const std::string ToString() const;

        inline const std::string VerbThirdPerson() const { return verbThirdPerson_; }
        inline const std::string VerbPastTense() const   { return verbPastTense_; }

        inline const std::string Desc() const       { return Spells::ShortDesc(which_); }
        inline const std::string DescExtra() const  { return Spells::ExtraDesc(which_); }
        const std::string DescDetails() const;
        const std::string DescComplete() const;

        inline Spells::Enum Which() const       { return which_; }
        inline EffectType::Enum Effect() const  { return effectType_; }
        inline Phase::Enum ValidPhases() const  { return validPhases_; }
        inline stats::Trait_t ManaCost() const  { return manaCost_; }
        inline Rank_t Rank() const              { return rank_; }
        inline TargetType::Enum Target() const  { return targetType_; }

        //Allows the spell to change the target creature.
        //Don't adjust creatureCastUponPtr's health, that will be done in Fight.cpp.
        //Don't check if creatureCastUponPtr is already dead, that will be done in fight.cpp
        virtual bool EffectCreature(
            creature::CreaturePtr_t   castingCreaturePtr,
            creature::CreaturePtr_t   creatureCastUponPtr,
            Health_t &                healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            ContentAndNamePos &       actionPhraseCNP) const = 0;

        //Allows the spell to change the target item.
        virtual const std::string EffectItem(
            creature::CreaturePtr_t,
            item::ItemPtr_t) const;

        friend bool operator<(const Spell & L, const Spell & R);
        friend bool operator==(const Spell & L, const Spell & R);

    public:
        static const std::string RESISTED_STR_;
        static const std::string FAILED_BECAUSE_STR_;

    protected:
        Spells::Enum     which_;
        Rank_t           rank_;
        EffectType::Enum effectType_;
        Phase::Enum      validPhases_;
        stats::Trait_t   manaCost_;
        TargetType::Enum targetType_;
        std::string      verbThirdPerson_;
        std::string      verbPastTense_;
    };


    using SpellPtr_t       = Spell *;
    using SpellPVec_t      = std::vector<SpellPtr_t>;
    using SpellPVecIter_t  = SpellPVec_t::iterator;
    using SpellPVecCIter_t = SpellPVec_t::const_iterator;

    using SpellSPtr_t = std::shared_ptr<Spell>;
    using SpellSVec_t = std::vector<SpellSPtr_t>;


    inline bool operator<(const Spell & L, const Spell & R)
    {
        return std::tie(L.which_,
                        L.rank_,
                        L.effectType_,
                        L.validPhases_,
                        L.manaCost_,
                        L.targetType_,
                        L.verbThirdPerson_,
                        L.verbPastTense_)
                <
               std::tie(R.which_,
                        R.rank_,
                        R.effectType_,
                        R.validPhases_,
                        R.manaCost_,
                        R.targetType_,
                        R.verbThirdPerson_,
                        R.verbPastTense_);
    }

    inline bool operator==(const Spell & L, const Spell & R)
    {
        return std::tie(L.which_,
                        L.rank_,
                        L.effectType_,
                        L.validPhases_,
                        L.manaCost_,
                        L.targetType_,
                        L.verbThirdPerson_,
                        L.verbPastTense_)
                ==
               std::tie(R.which_,
                        R.rank_,
                        R.effectType_,
                        R.validPhases_,
                        R.manaCost_,
                        R.targetType_,
                        R.verbThirdPerson_,
                        R.verbPastTense_);
    }

    inline bool operator!=(const Spell & L, const Spell & R)
    {
        return ! (L == R);
    }

}
}
#endif //GAME_SPELL_Spell_HPP_INCLUDED
