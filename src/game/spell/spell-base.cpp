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
// spell-base.cpp
//
#include "spell-base.hpp"

#include "game/creature/creature.hpp"

#include "misc/strings.hpp"
#include "misc/random.hpp"

#include <sstream>
#include <algorithm>


namespace game
{
namespace spell
{

    Spell::Spell(const Spells::Enum     WHICH,
                 const EffectType::Enum EFFECT_TYPE,
                 const Phase::Enum      VALID_PHASES,
                 const stats::Mana_t    MANA_COST,
                 const stats::Rank_t    RANK,
                 const TargetType::Enum TARGET_TYPE)
    :
        which_      (WHICH),
        rank_       (RANK),
        effectType_ (EFFECT_TYPE),
        validPhases_(VALID_PHASES),
        manaCost_   (MANA_COST),
        targetType_ (TARGET_TYPE)
    {}


    Spell::~Spell()
    {}


    const std::string Spell::ToString() const
    {
        std::ostringstream ss;
        ss  << Name() << "  -" << DescDetails();
        return ss.str();
    }


    const std::string Spell::DescDetails() const
    {
        std::ostringstream ss;
        ss  << "A " << misc::String::DecorateNumber(rank_) << " rank"
            << " " << EffectType::Name(effectType_) << " spell"
            << " that can be cast during " << Phase::ToString(validPhases_, false)
            << ", targeting " << TargetType::Name(targetType_)
            << ", and costing " << manaCost_ << " mana.";

        return ss.str();
    }


    const std::string Spell::DescComplete() const
    {
        std::ostringstream ss;
        ss << Desc() << "  " << DescExtra() << " " << DescDetails();
        return ss.str();
    }


    const std::string Spell::EffectItem(creature::CreaturePtr_t, item::ItemPtr_t) const
    {
        return "Nothing to do.";
    }

}
}
