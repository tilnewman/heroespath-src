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
// sfx-set-enum.cpp
//
#include "sfx-set-enum.hpp"

#include <sstream>
#include <exception>


namespace heroespath
{
namespace sfml_util
{

    const std::string sound_effect_set::ToString(const sound_effect_set::Enum E)
    {
        switch (E)
        {
            case Prompt:            { return "Prompt"; }
            case Switch:            { return "Switch"; }
            case TickOn:            { return "TickOn"; }
            case TickOff:           { return "TickOff"; }
            case Thock:             { return "Thock"; }
            case Coin:              { return "Coin"; }
            case Gem:               { return "Gem"; }
            case MeteorShard:       { return "MeteorShard"; }
            case ItemGive:          { return "ItemGive"; }
            case ItemDrop:          { return "ItemDrop"; }
            case BlowpipeShoot:     { return "BlowpipeShoot"; }
            case BlowpipeHit:       { return "BlowpipeHit"; }
            case ArrowShoot:        { return "ArrowShoot"; }
            case ArrowHit:          { return "ArrowHit"; }
            case ProjectileMiss:    { return "ProjectileMiss"; }
            case MeleeMiss:         { return "MeleeMiss"; }
            case FistHit:           { return "FistHit"; }
            case TendrilHit:        { return "TendrilHit"; }
            case WhipHit:           { return "WhipHit"; }
            case WhipMiss:          { return "WhipMiss"; }
            case MaterialHitMetal:  { return "MaterialHitMetal"; }
            case MaterialHitMisc:   { return "MaterialHitMisc"; }
            case ClawHit:           { return "ClawHit"; }
            case Wind:              { return "Wind"; }
            case SpellSelect:       { return "SpellSelect"; }
            case SpellBandage:      { return "SpellBandage"; }
            case DrumBlip:          { return "DrumBlip"; }
            case GuitarStrum:       { return "GuitarStrum"; }
            case CombatWin:         { return "CombatWin"; }
            case CombatLose:        { return "CombatLose"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::sound_effect_set::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

}
}
