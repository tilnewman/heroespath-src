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
#ifndef GAME_EFFECTTYPEENUM_HPP_INCLUDED
#define GAME_EFFECTTYPEENUM_HPP_INCLUDED
//
// effect-type-enum.hpp
//
#include <string>
#include <vector>


namespace game
{

    struct EffectType
    {
        enum Enum
        {
            CreatureHarmDamage = 0,
            CreatureHarmMisc,
            CreatureHelpHeal,
            CreatureHelpMisc,
            ItemHarmBreak,
            ItemHarmMisc,
            ItemHelpFix,
            ItemHelpMisc,
            Misc,
            Count
        };

        static const std::string ToString(const Enum);
        static const std::string Name(const Enum);
    };

    using EffectTypeVec_t = std::vector<EffectType::Enum>;

}

#endif //GAME_EFFECTTYPEENUM_HPP_INCLUDED
