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
#ifndef GAME_CREATURE_ENCHANTMENT_HPP_INCLUDED
#define GAME_CREATURE_ENCHANTMENT_HPP_INCLUDED
//
// enchantment.hpp
//  Similar to a Condition, an enchantment is something that changes a creature.
//
#include "game/types.hpp"
#include "game/phase-enum.hpp"
#include "game/creature/race-enum.hpp"
#include "game/creature/role-enum.hpp"
#include "game/creature/enchantment-type.hpp"
#include "game/creature/condition-enum.hpp"
#include "game/stats/traits-set.hpp"
#include "game/stats/types.hpp"
#include "game/spell/spell-enum.hpp"

#include "misc/boost-serialize-includes.hpp"

#include <string>
#include <memory>
#include <vector>


namespace game
{
namespace creature
{
    //forward declarations
    class Creature;
    using CreaturePtr_t = Creature *;


    //all the 'use' info about an enchantment
    class UseInfo
    {
    public:
        explicit UseInfo(
            const int                   COUNT               = 0,//negative means infinite
            const Phase::Enum           PHASE_RESTRICT      = Phase::NotAPhase,
            const spell::Spells::Enum   SPELL               = spell::Spells::Count,
            const CondEnumVec_t &       CONDS_REMOVED_VEC   = CondEnumVec_t())
        :
            countOrig_      (COUNT),
            countRemain_    (COUNT),
            phase_          (PHASE_RESTRICT),
            spell_          (SPELL),
            condsRemovedVec_(CONDS_REMOVED_VEC)
        {}

        inline int CountOrig() const                            { return countOrig_; }
        inline int CountRemaining() const                       { return countRemain_; }
        inline void CountConsume()                              { --countRemain_; }
        inline Phase::Enum RestrictedToPhase() const            { return phase_; }
        inline spell::Spells::Enum Spell() const                { return spell_; }
        inline const CondEnumVec_t & ConditionsRemoved() const  { return condsRemovedVec_; }

    private:
        //negative means infinite
        int countOrig_;
        int countRemain_;

        Phase::Enum phase_;
        spell::Spells::Enum spell_;
        CondEnumVec_t condsRemovedVec_;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & countOrig_;
            ar & countRemain_;
            ar & phase_;
            ar & spell_;
            ar & condsRemovedVec_;
        }
    };


    //Responsible for storing all the info of an Enchantment,
    //and for applying and removing related changes to a creature.
    class Enchantment
    {
    public:
        explicit Enchantment(
            const EnchantmentType::Enum TYPE      = EnchantmentType::None,
            const stats::TraitSet &     TRAIT_SET = stats::TraitSet(),
            const UseInfo &             USE_INFO  = UseInfo());

        virtual ~Enchantment();

        inline EnchantmentType::Enum Type() const               { return type_; }
        inline bool IsType(const EnchantmentType::Enum E) const { return (E & type_); }
        inline const stats::TraitSet & Traits() const           { return traitSet_; }
        inline const UseInfo & Use() const                      { return useInfo_; }
        inline void UseCountConsume()                           { useInfo_.CountConsume(); }

        inline bool IsUseableEver() const
        {
            //negative useCountOrig_ means infinite
            return ((type_ & EnchantmentType::WhenUsed) && (useInfo_.CountOrig() != 0));
        }

        inline bool IsUseableNow() const
        {
            return (IsUseableEver() && (useInfo_.CountRemaining() > 0));
        }

        virtual const std::string EffectStr(const CreaturePtr_t CREATURE_PTR = nullptr) const;

        virtual inline void CreatureChangeApply(const CreaturePtr_t)    {}
        virtual inline void CreatureChangeRemove(const CreaturePtr_t)   {}
        virtual inline void UseEffect(const CreaturePtr_t)              {}

        virtual Score_t TreasureScore() const;

    private:
        inline const std::string SepIfNotEmpty(const std::string & S) const
        {
            return ((S.empty()) ? "" : ", ");
        }

    private:
        EnchantmentType::Enum   type_;
        stats::TraitSet         traitSet_;
        UseInfo                 useInfo_;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & type_;
            ar & traitSet_;
            ar & useInfo_;
        }
    };

    using EnchantmentPtr_t = Enchantment *;
    using EnchantmentPVec_t = std::vector<EnchantmentPtr_t>;

}
}

#endif //GAME_CREATURE_ENCHANTMENT_HPP_INCLUDED
