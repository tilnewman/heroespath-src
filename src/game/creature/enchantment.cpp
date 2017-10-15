// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// enchantment.cpp
//
#include "enchantment.hpp"

#include "game/creature/creature.hpp"
#include "game/creature/condition-algorithms.hpp"
#include "game/item/item-profile-warehouse.hpp"

#include <sstream>


namespace game
{
namespace creature
{

    Enchantment::Enchantment(const EnchantmentType::Enum TYPE,
                             const stats::TraitSet &     TRAIT_SET,
                             const UseInfo &             USE_INFO)
    :
        type_    (TYPE),
        traitSet_(TRAIT_SET),
        useInfo_ (USE_INFO)
    {}


    Enchantment::~Enchantment()
    {}


    const std::string Enchantment::EffectStr(const CreaturePtr_t) const
    {
        std::ostringstream ss;

        ss << EnchantmentType::ToString(type_);

        if (IsUseableEver())
        {
            auto const COUNT_REMAIN{ useInfo_.CountRemaining() };
            if (COUNT_REMAIN > 0)
            {
                ss << SepIfNotEmpty(ss.str()) << COUNT_REMAIN << " uses left";
            }
            else if (COUNT_REMAIN == 0)
            {
                ss << SepIfNotEmpty(ss.str()) << "cannot be used again";
            }
            else
            {
                ss << SepIfNotEmpty(ss.str()) << "";
            }

            auto const SPELL{ useInfo_.Spell() };
            if (SPELL != spell::Spells::Count)
            {
                ss << SepIfNotEmpty(ss.str()) << "casts the "
                    << spell::Spells::Name(SPELL) << " spell";
            }

            auto const CONDS_REMOVED_VEC{ useInfo_.ConditionsRemoved() };
            if (CONDS_REMOVED_VEC.empty())
            {
                ss << SepIfNotEmpty(ss.str()) << "removes the conditions: "
                    << creature::condition::Algorithms::Names(
                        CONDS_REMOVED_VEC, misc::Vector::JoinOpt::And);
            }
        }

        if (useInfo_.RestrictedToPhase() != Phase::NotAPhase)
        {
            ss << SepIfNotEmpty(ss.str()) << ", use only during "
                << Phase::ToString(useInfo_.RestrictedToPhase(), false);
        }

        auto const TRAITS_STR{ traitSet_.ToString(false, false, false, true) };
        if (TRAITS_STR.empty() == false)
        {
            ss << SepIfNotEmpty(ss.str()) << "Traits: " << TRAITS_STR;
        }

        return ss.str();
    }


    Score_t Enchantment::TreasureScore() const
    {
        auto score{ item::ItemProfileWarehouse::Score(traitSet_) };

        if (type_ & EnchantmentType::WhenHeld)
        {
            score += 350_score;
        }

        if (type_ & EnchantmentType::AllowsFlight)
        {
            score += 200_score;
        }

        if ((type_ & EnchantmentType::BlessWithoutItem) ||
            (type_ & EnchantmentType::CurseWithoutItem))
        {
            score += 300_score;
        }

        return score;
    }

}
}
