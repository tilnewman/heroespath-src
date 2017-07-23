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
#include "game/creature/condition.hpp"
#include "game/stats/stat-set.hpp"
#include "game/item/types.hpp"
#include "game/stats/types.hpp"

#include "misc/boost-serialize-includes.hpp"

#include <string>
#include <memory>


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
        Enchantment(const std::string &         NAME              = "",
                    const EnchantmentType::Enum TYPE              = EnchantmentType::None,
                    const stats::Mana_t         MANA_ADJ          = 0,
                    const stats::Armor_t        ARMOR_RATING_ADJ  = 0,
                    const stats::StatSet &      STAT_ADJ_SET      = stats::StatSet(),
                    const stats::StatMultSet &  STAT_MULT_ADJ_SET = stats::StatMultSet(),
                    const CondEnumVec_t &       CONDS_VEC         = CondEnumVec_t());

        virtual ~Enchantment();

        inline const std::string Name() const                    { return name_; }
        inline EnchantmentType::Enum Type() const                { return type_; }
        inline stats::Mana_t ManaAdj() const                     { return manaAdj_; }
        inline stats::Armor_t ArmorRatingAdj() const             { return armorRatingAdj_; }
        inline const stats::StatSet & StatDirectAdjSet() const   { return statsDirectAdjSet_; }
        inline const stats::StatMultSet & StatMultAdjSet() const { return statsMultAdjSet_; }
        inline const CondEnumVec_t & Conditions() const          { return condsVec_; }

        inline bool HasType(const EnchantmentType::Enum E) const { return (type_ & E); }

        inline bool WillAdjMana() const
        {
            return (manaAdj_ != 0);
        }

        inline bool WillAdjArmorRating() const
        {
            return (armorRatingAdj_ != 0);
        }

        inline bool WillAdjStatsDirect() const
        {
            return (statsDirectAdjSet_ != stats::StatSet());
        }

        inline bool WillAdjStatsMult() const
        {
            return (statsMultAdjSet_ != stats::StatMultSet());
        }

        inline bool WillAdjStats() const
        {
            return (WillAdjStatsDirect() || WillAdjStatsMult());
        }

        virtual inline void CreatureChangeApply(const CreaturePtr_t) {}
        virtual inline void CreatureChangeRemove(const CreaturePtr_t) {}

    private:
        std::string             name_;
        EnchantmentType::Enum   type_;
        stats::Mana_t           manaAdj_;
        stats::Armor_t          armorRatingAdj_;
        stats::StatSet          statsDirectAdjSet_;
        stats::StatMultSet      statsMultAdjSet_;
        CondEnumVec_t           condsVec_;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & name_;
            ar & type_;
            ar & manaAdj_;
            ar & armorRatingAdj_;
            ar & statsDirectAdjSet_;
            ar & statsMultAdjSet_;
            ar & condsVec_;
        }
    };

    using EnchantmentPtr_t = Enchantment *;
   

}
}

#endif //GAME_CREATURE_ENCHANTMENT_HPP_INCLUDED
