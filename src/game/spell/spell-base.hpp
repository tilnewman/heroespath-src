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
#include "game/stats/types.hpp"
#include "game/spell/spell-enum.hpp"
#include "game/spell/spell-type-enum.hpp"
#include "game/target-enum.hpp"
#include "game/phase-enum.hpp"
#include "game/creature/condition-enum.hpp"

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
              const SpellType::Enum  TYPE,
              const Phase::Enum      VALID_PHASES,
              const stats::Mana_t    MANA_COST,
              const stats::Rank_t    RANK,
              const TargetType::Enum TARGET_TYPE);

        virtual ~Spell();

        inline const std::string Name() const       { return Spells::Name(which_); }

        const std::string ToString() const;

        inline const std::string Desc() const       { return Spells::ShortDesc(which_); }
        inline const std::string DescExtra() const  { return Spells::ExtraDesc(which_); }
        const std::string DescDetails() const;
        const std::string DescComplete() const;

        inline Spells::Enum Which() const       { return which_; }
        inline SpellType::Enum Type() const     { return type_; }
        inline Phase::Enum ValidPhases() const  { return validPhases_; }
        inline stats::Mana_t ManaCost() const   { return manaCost_; }
        inline stats::Rank_t Rank() const       { return rank_; }
        inline TargetType::Enum Target() const  { return targetType_; }

        //Returns a short sentance describing what the spell did.
        //For the following functions the first CreaturePtr_t is the caster and the second is the target.
        virtual const std::string ActionPhrase(creature::CreaturePtr_t,         //creatureCasting
                                               creature::CreaturePtr_t) const   //creatureCastUpon
        {
            return "TODO";
        }

        //Allows the spell to change the target creature.
        virtual const std::string EffectCreature(creature::CreaturePtr_t,              //creatureCasting
                                                 creature::CreaturePtr_t,              //creatureCastUpon
                                                 creature::ConditionEnumVec_t &) const //conditionsAdded
        {
            return Spell::EFFECT_STR_NOTHING_TO_DO_;
        }

        //Allows the spell to change the target item.
        virtual const std::string EffectItem(creature::CreaturePtr_t,   //creatureCasting
                                             item::ItemPtr_t) const  //itemCastUpon
        {
            return Spell::EFFECT_STR_NOTHING_TO_DO_;
        }

        //Returns the amount of health that the spell either gives or takes away.
        virtual stats::Health_t HealthAdj(creature::CreaturePtr_t,      //creatureCasting
                                          creature::CreaturePtr_t) const//creatureCastUpon
        {
            return 0;
        }

        //Returns the number of target creatures that will be effected.
        virtual int EffectedCreatureCount(creature::CreaturePtr_t) const
        {
            if ((TargetType::SingleCompanion == targetType_) ||
                (TargetType::SingleOpponent == targetType_))
            {
                return 1;
            }
            else if ((TargetType::AllCompanions == targetType_) ||
                     (TargetType::AllOpponents == targetType_))
            {
                return EFFECTS_ALL_CREATURES_COUNT_;
            }
            else
            {
                return 0;
            }
        }

        friend bool operator<(const Spell & L, const Spell & R);
        friend bool operator==(const Spell & L, const Spell & R);

    protected:
        int GenerateValue(const int FLOOR,
                          const int THE_RAND_MAX,
                          const int RANK         = 0,
                          const int RANK_MAX     = 0) const;

        inline stats::Health_t GenerateHealthValue(const int FLOOR,
                                                   const int THE_RAND_MAX,
                                                   const int RANK         = 0,
                                                   const int RANK_MAX     = 0) const
        {
            return static_cast<stats::Health_t>( GenerateValue(FLOOR,
                                                               THE_RAND_MAX,
                                                               RANK,
                                                               RANK_MAX) );
        }

    public:
        static const std::string EFFECT_STR_SUCCESS_;
        static const std::string EFFECT_STR_NOTHING_TO_DO_;
        static const std::string EFFECT_STR_IS_ALREADY_;
        static const std::string EFFECT_STR_RESISTED_;
        static const int         EFFECTS_ALL_CREATURES_COUNT_;

    protected:
        Spells::Enum     which_;
        stats::Rank_t    rank_;
        SpellType::Enum  type_;
        Phase::Enum      validPhases_;
        stats::Mana_t    manaCost_;
        TargetType::Enum targetType_;
    };


    using SpellPtr_t       = Spell *;
    using SpellPVec_t      = std::vector<SpellPtr_t>;
    using SpellPVecIter_t  = SpellPVec_t::iterator;
    using SpellPVecCIter_t = SpellPVec_t::const_iterator;

    using SpellSPtr_t = std::shared_ptr<Spell>;
    using SpellSVec_t = std::vector<SpellSPtr_t>;


    inline bool operator<(const Spell & L, const Spell & R)
    {
        return std::tie(L.which_, L.rank_, L.type_, L.validPhases_, L.manaCost_, L.targetType_)
                <
               std::tie(R.which_, R.rank_, R.type_, R.validPhases_, R.manaCost_, R.targetType_);
    }

    inline bool operator==(const Spell & L, const Spell & R)
    {
        return std::tie(L.which_, L.rank_, L.type_, L.validPhases_, L.manaCost_, L.targetType_)
                ==
               std::tie(R.which_, R.rank_, R.type_, R.validPhases_, R.manaCost_, R.targetType_);
    }

    inline bool operator!=(const Spell & L, const Spell & R)
    {
        return ! (L == R);
    }

}
}
#endif //GAME_SPELL_Spell_HPP_INCLUDED
