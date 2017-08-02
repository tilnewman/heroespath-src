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
#include "game/creature/enchantment-type.hpp"
#include "game/creature/condition-enum.hpp"
#include "game/stats/traits-set.hpp"
#include "game/stats/types.hpp"
#include "game/item/types.hpp"

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


    //Responsible for storing all the info of an Enchantment,
    //and for applying and removing related changes to a creature.
    class Enchantment
    {
    public:
        Enchantment(const EnchantmentType::Enum TYPE      = EnchantmentType::None,
                    const stats::TraitSet &     TRAIT_SET = stats::TraitSet(),
                    const int                   USE_COUNT = 0, //negative means infinite
                    const CondEnumVec_t &       CONDS_VEC = CondEnumVec_t());

        virtual ~Enchantment();

        inline EnchantmentType::Enum Type() const                { return type_; }
        inline int UseCountOrig() const                          { return useCountOrig_; }
        inline int UseeCountRemaining() const                    { return useCountRemaining_;}
        inline const stats::TraitSet & Traits() const            { return traitSet_; }
        inline const CondEnumVec_t & Conditions() const          { return condsVec_; }
        inline bool HasType(const EnchantmentType::Enum E) const { return (type_ & E); }

        inline bool IsUseableEver() const
        {
            //negative useCountOrig_ means infinite
            return ((type_ & EnchantmentType::WhenUsed) && (useCountOrig_ != 0));
        }

        inline bool IsUseableNow() const
        {
            return (IsUseableEver() && (useCountRemaining_ > 0));
        }

        virtual const std::string EffectStr(const CreaturePtr_t CREATURE_PTR = nullptr) const;

        virtual inline void CreatureChangeApply(const CreaturePtr_t) {}
        virtual inline void CreatureChangeRemove(const CreaturePtr_t) {}

    private:
        inline const std::string Plus(const int X) const
        {
            return ((X > 0) ? "+" : "");
        }

        inline const std::string SepIfEmpty(const std::string & S) const
        {
            return ((S.empty()) ? "" : ", ");
        }

    private:
        EnchantmentType::Enum   type_;

        //negative values mean infinite uses
        int                     useCountOrig_;
        int                     useCountRemaining_;

        stats::TraitSet         traitSet_;
        CondEnumVec_t           condsVec_;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & type_;
            ar & useCountOrig_;
            ar & useCountRemaining_;
            ar & traitSet_;
            ar & condsVec_;
        }
    };

    using EnchantmentPtr_t = Enchantment *;
    using EnchantmentPVec_t = std::vector<EnchantmentPtr_t>;

}
}

#endif //GAME_CREATURE_ENCHANTMENT_HPP_INCLUDED
