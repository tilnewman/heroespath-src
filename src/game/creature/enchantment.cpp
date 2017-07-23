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


namespace game
{
namespace creature
{

    Enchantment::Enchantment(const EnchantmentType::Enum TYPE,
                             const stats::Mana_t         MANA_ADJ,
                             const stats::Armor_t        ARMOR_RATING_ADJ,
                             const stats::StatSet &      STAT_ADJ_SET,
                             const stats::StatMultSet &  STAT_MULT_ADJ_SET,
                             const CondEnumVec_t &       CONDS_VEC)
    :
        type_               (TYPE),
        manaAdj_            (MANA_ADJ),
        armorRatingAdj_     (ARMOR_RATING_ADJ),
        statsDirectAdjSet_  (STAT_ADJ_SET),
        statsMultAdjSet_    (STAT_MULT_ADJ_SET),
        condsVec_           (CONDS_VEC)
    {}


    Enchantment::~Enchantment()
    {}

}
}
