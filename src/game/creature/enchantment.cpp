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

#include <sstream>


namespace game
{
namespace creature
{

    Enchantment::Enchantment(const EnchantmentType::Enum TYPE,
                             const stats::TraitSet &     TRAIT_SET,
                             const int                   USE_COUNT, //negative means infinite
                             const CondEnumVec_t &       CONDS_VEC)
    :
        type_             (TYPE),
        useCountOrig_     (USE_COUNT),
        useCountRemaining_(USE_COUNT),
        traitSet_         (TRAIT_SET),
        condsVec_         (CONDS_VEC)
    {}


    Enchantment::~Enchantment()
    {}


    const std::string Enchantment::EffectStr(const CreaturePtr_t) const
    {
        std::ostringstream ss;

        ss << EnchantmentType::ToString(type_);

        if (IsUseableEver())
        {
            if (useCountRemaining_ > 0)
            {
                ss << SepIfEmpty(ss.str()) << useCountRemaining_ << " uses left";
            }
            else if (useCountRemaining_ == 0)
            {
                ss << SepIfEmpty(ss.str()) << "cannot be used again";
            }
        }

        auto const TRAITS_STR{ traitSet_.ToString(false, false, false, true) };
        if (TRAITS_STR.empty() == false)
        {
            ss << "  Traits: " << TRAITS_STR;
        }
        
        return ss.str();
    }

}
}
