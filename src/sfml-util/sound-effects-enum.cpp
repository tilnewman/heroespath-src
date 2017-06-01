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
// sound-effects-enum.cpp
//
#include "sound-effects-enum.hpp"
#include <sstream>
#include <exception>


namespace sfml_util
{

    const std::string sound_effect::FILE_EXTENSION_OGG(".ogg");
    const std::string sound_effect::FILE_EXTENSION_FLAC(".flac");


    const std::string sound_effect::ToString(const sound_effect::Enum E)
    {
        switch (E)
        {
            case WindGust1:         { return "WindGust1"; }
            case WindGust2:         { return "WindGust2"; }
            case WindGust3:         { return "WindGust3"; }
            case PromptGeneric:     { return "PromptGeneric"; }
            case PromptQuestion:    { return "PromptQuestion"; }
            case PromptWarn:        { return "PromptWarn"; }
            case Switch1:           { return "Switch1"; }
            case Switch2:           { return "Switch2"; }
            case Switch3:           { return "Switch3"; }
            case Switch4:           { return "Switch4"; }
            case Thock1:            { return "Thock1"; }
            case Thock2:            { return "Thock2"; }
            case Thock3:            { return "Thock3"; }
            case Thock4:            { return "Thock4"; }
            case Thock5:            { return "Thock5"; }
            case Thock6:            { return "Thock6"; }
            case TickOff1:          { return "TickOff1"; }
            case TickOff2:          { return "TickOff2"; }
            case TickOn1:           { return "TickOn1"; }
            case TickOn2:           { return "TickOn2"; }
            case Coins1:            { return "coins-1"; }
            case Coins2:            { return "coins-2"; }
            case Coins3:            { return "coins-3"; }
            case Coins4:            { return "coins-4"; }
            case Coins5:            { return "coins-5"; }
            case Coins6:            { return "coins-6"; }
            case Coins7:            { return "coins-7"; }
            case Coins8:            { return "coins-8"; }
            case Coins9:            { return "coins-9"; }
            case Coins10:           { return "coins-10"; }
            case Coins11:           { return "coins-11"; }
            case Coins12:           { return "coins-12"; }
            case Coins13:           { return "coins-13"; }
            case Coins14:           { return "coins-14"; }
            case Coins15:           { return "coins-15"; }
            case Coins16:           { return "coins-16"; }
            case Gems:              { return "gems"; }
            case MeteorShards:      { return "meteor_shards"; }
            case ItemGive:          { return "item-give"; }
            case ItemDrop:          { return "item-drop"; }
            case BlowpipeShoot1:    { return "blowpipe-shoot-1"; }
            case BlowpipeShoot2:    { return "blowpipe-shoot-2"; }
            case BlowpipeShoot3:    { return "blowpipe-shoot-3"; }
            case BlowpipeHit1:      { return "blowpipe-hit-1"; }
            case BlowpipeHit2:      { return "blowpipe-hit-2"; }
            case BlowpipeHit3:      { return "blowpipe-hit-3"; }
            case ArrowShoot1:       { return "arrow-shoot-1"; }
            case ArrowShoot2:       { return "arrow-shoot-2"; }
            case ArrowShoot3:       { return "arrow-shoot-3"; }
            case ArrowShoot4:       { return "arrow-shoot-4"; }
            case ArrowShoot5:       { return "arrow-shoot-5"; }
            case ArrowShoot6:       { return "arrow-shoot-6"; }
            case ArrowHit:          { return "arrow-hit"; }
            case ProjectileMiss1:   { return "projectile-miss-1"; }
            case ProjectileMiss2:   { return "projectile-miss-2"; }
            case ProjectileMiss3:   { return "projectile-miss-3"; }
            case ProjectileMiss4:   { return "projectile-miss-4"; }
            case ProjectileMiss5:   { return "projectile-miss-5"; }
            case ProjectileMiss6:   { return "projectile-miss-6"; }
            case ProjectileMiss7:   { return "projectile-miss-7"; }
            case ProjectileMiss8:   { return "projectile-miss-8"; }
            case ProjectileMiss9:   { return "projectile-miss-9"; }
            case ProjectileMiss10:  { return "projectile-miss-10"; }
            case ProjectileMiss11:  { return "projectile-miss-11"; }
            case ProjectileMiss12:  { return "projectile-miss-12"; }
            case ProjectileMiss13:  { return "projectile-miss-13"; }
            case MeleeMiss1:        { return "melee-miss-1"; }
            case MeleeMiss2:        { return "melee-miss-2"; }
            case MeleeMiss3:        { return "melee-miss-3"; }
            case MeleeMiss4:        { return "melee-miss-4"; }
            case MeleeMiss5:        { return "melee-miss-5"; }
            case MeleeMiss6:        { return "melee-miss-6"; }
            case MeleeMiss7:        { return "melee-miss-7"; }
            case MeleeMiss8:        { return "melee-miss-8"; }
            case MeleeMiss9:        { return "melee-miss-9"; }
            case MeleeMiss10:       { return "melee-miss-10"; }
            case MeleeMiss11:       { return "melee-miss-11"; }
            case MeleeMiss12:       { return "melee-miss-12"; }
            case MeleeMiss13:       { return "melee-miss-13"; }
            case MeleeMiss14:       { return "melee-miss-14"; }
            case MeleeMiss15:       { return "melee-miss-15"; }
            case MeleeMiss16:       { return "melee-miss-16"; }
            case MeleeMiss17:       { return "melee-miss-17"; }
            case MeleeMiss18:       { return "melee-miss-18"; }
            case MeleeMiss19:       { return "melee-miss-19"; }
            case MeleeMiss20:       { return "melee-miss-20"; }
            case MeleeMiss21:       { return "melee-miss-21"; }
            case MeleeMiss22:       { return "melee-miss-22"; }
            case MeleeMiss23:       { return "melee-miss-23"; }
            case MeleeMiss24:       { return "melee-miss-24"; }
            case MeleeMiss25:       { return "melee-miss-25"; }
            case MeleeMiss26:       { return "melee-miss-26"; }
            case MeleeMiss27:       { return "melee-miss-27"; }
            case MeleeMiss28:       { return "melee-miss-28"; }
            case MeleeMiss29:       { return "melee-miss-29"; }
            case MeleeMiss30:       { return "melee-miss-30"; }
            case MeleeMiss31:       { return "melee-miss-31"; }
            case MeleeMiss32:       { return "melee-miss-32"; }
            case MeleeMiss33:       { return "melee-miss-33"; }
            case MeleeMiss34:       { return "melee-miss-34"; }
            case MeleeMiss35:       { return "melee-miss-35"; }
            case MeleeMiss36:       { return "melee-miss-36"; }
            case MeleeMiss37:       { return "melee-miss-37"; }
            case MeleeMiss38:       { return "melee-miss-38"; }
            case MeleeMiss39:       { return "melee-miss-39"; }
            case MeleeMiss40:       { return "melee-miss-40"; }
            case MeleeMiss41:       { return "melee-miss-41"; }
            case FistHit1:          { return "fist-hit-1"; }
            case FistHit2:          { return "fist-hit-2"; }
            case FistHit3:          { return "fist-hit-3"; }
            case FistHit4:          { return "fist-hit-4"; }
            case FistHit5:          { return "fist-hit-5"; }
            case FistHit6:          { return "fist-hit-6"; }
            case FistHit7:          { return "fist-hit-7"; }
            case FistHit8:          { return "fist-hit-8"; }
            case BreathHit1:        { return "breath-hit-1"; }
            case BreathHit2:        { return "breath-hit-2"; }
            case TendrilHit1:       { return "tendril-hit-1"; }
            case TendrilHit2:       { return "tendril-hit-2"; }
            case TendrilHit3:       { return "tendril-hit-3"; }
            case TendrilHit4:       { return "tendril-hit-4"; }
            case WhipHit:           { return "whip-hit"; }
            case WhipMiss1:         { return "whip-miss-1"; }
            case WhipMiss2:         { return "whip-miss-2"; }
            case WhipMiss3:         { return "whip-miss-3"; }
            case WhipMiss4:         { return "whip-miss-4"; }
            case WhipMiss5:         { return "whip-miss-5"; }
            case WhipMiss6:         { return "whip-miss-6"; }
            case WhipMiss7:         { return "whip-miss-7"; }
            case WhipMiss8:         { return "whip-miss-8"; }
            case WhipMiss9:         { return "whip-miss-9"; }
            case WhipMiss10:        { return "whip-miss-10"; }
            case MaterialHitMetal1: { return "material-hit-metal-1"; }
            case MaterialHitMetal2: { return "material-hit-metal-2"; }
            case MaterialHitMetal3: { return "material-hit-metal-3"; }
            case MaterialHitMetal4: { return "material-hit-metal-4"; }
            case MaterialHitMetal5: { return "material-hit-metal-5"; }
            case MaterialHitBone:   { return "material-hit-bone"; }
            case MaterialHitCloth:  { return "material-hit-cloth"; }
            case MaterialHitFlesh:  { return "material-hit-flesh"; }
            case MaterialHitHide:   { return "material-hit-hide"; }
            case MaterialHitHorn:   { return "material-hit-horn"; }
            case MaterialHitLeather:{ return "material-hit-leather"; }
            case MaterialHitLiquid: { return "material-hit-liquid"; }
            case MaterialHitPlant:  { return "material-hit-plant"; }
            case MaterialHitScale:  { return "material-hit-scale"; }
            case MaterialHitStone:  { return "material-hit-stone"; }
            case MaterialHitTooth:  { return "material-hit-tooth"; }
            case MaterialHitWood:   { return "material-hit-wood"; }
            case MaterialHitMisc:   { return "material-hit-misc"; }
            case ClawTear:          { return "claw-tear";  }
            case Magic1:            { return "magic1"; }
            case SpellSelect1:      { return "deep-pulse-a"; }
            case SpellSelect2:      { return "deep-pulse-b"; }
            case SpellSelect3:      { return "deep-pulse-c"; }
            case SpellSparks:       { return "soft-sparks"; }
            case None:              { return "None"; }
            case Count:
            case Random:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::sound_effect::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::string sound_effect::FileExt(const sound_effect::Enum)
    {
        return sound_effect::FILE_EXTENSION_OGG;
    }


    const std::string sound_effect::Filename(const sound_effect::Enum E)
    {
        std::ostringstream ss;
        ss << ToString(E) << FileExt(E);
        return ss.str();
    }


    const std::string sound_effect::Directory(const sound_effect::Enum E)
    {
        switch (E)
        {
            case WindGust1:
            case WindGust2:
            case WindGust3:             { return "sound-effects/wind-gusts"; }
            case PromptGeneric:
            case PromptQuestion:
            case PromptWarn:            { return "sound-effects/prompts"; }
            case Switch1:
            case Switch2:
            case Switch3:
            case Switch4:
            case Thock1:
            case Thock2:
            case Thock3:
            case Thock4:
            case Thock5:
            case Thock6:
            case TickOff1:
            case TickOff2:
            case TickOn1:
            case TickOn2:               { return "sound-effects/user-interface"; }
            case Coins1:
            case Coins2:
            case Coins3:
            case Coins4:
            case Coins5:
            case Coins6:
            case Coins7:
            case Coins8:
            case Coins9:
            case Coins10:
            case Coins11:
            case Coins12:
            case Coins13:
            case Coins14:
            case Coins15:
            case Coins16:               { return "sound-effects/inventory/coins"; }
            case Gems:                  { return "sound-effects/inventory/gems"; }
            case MeteorShards:          { return "sound-effects/inventory/meteor_shards"; }
            case ItemGive:
            case ItemDrop:              { return "sound-effects/inventory/items"; }
            case BlowpipeShoot1:
            case BlowpipeShoot2:
            case BlowpipeShoot3:        { return "sound-effects/combat/blowpipe-shoot"; }
            case BlowpipeHit1:
            case BlowpipeHit2:
            case BlowpipeHit3:          { return "sound-effects/combat/blowpipe-hit"; }
            case ArrowShoot1:
            case ArrowShoot2:
            case ArrowShoot3:
            case ArrowShoot4:
            case ArrowShoot5:
            case ArrowShoot6:           { return "sound-effects/combat/arrow-shoot"; }
            case ArrowHit:              { return "sound-effects/combat/arrow-hit"; }
            case ProjectileMiss1:
            case ProjectileMiss2:
            case ProjectileMiss3:
            case ProjectileMiss4:
            case ProjectileMiss5:
            case ProjectileMiss6:
            case ProjectileMiss7:
            case ProjectileMiss8:
            case ProjectileMiss9:
            case ProjectileMiss10:
            case ProjectileMiss11:
            case ProjectileMiss12:
            case ProjectileMiss13:      { return "sound-effects/combat/projectile-miss"; }
            case MeleeMiss1:
            case MeleeMiss2:
            case MeleeMiss3:
            case MeleeMiss4:
            case MeleeMiss5:
            case MeleeMiss6:
            case MeleeMiss7:
            case MeleeMiss8:
            case MeleeMiss9:
            case MeleeMiss10:
            case MeleeMiss11:
            case MeleeMiss12:
            case MeleeMiss13:
            case MeleeMiss14:
            case MeleeMiss15:
            case MeleeMiss16:
            case MeleeMiss17:
            case MeleeMiss18:
            case MeleeMiss19:
            case MeleeMiss20:
            case MeleeMiss21:
            case MeleeMiss22:
            case MeleeMiss23:
            case MeleeMiss24:
            case MeleeMiss25:
            case MeleeMiss26:
            case MeleeMiss27:
            case MeleeMiss28:
            case MeleeMiss29:
            case MeleeMiss30:
            case MeleeMiss31:
            case MeleeMiss32:
            case MeleeMiss33:
            case MeleeMiss34:
            case MeleeMiss35:
            case MeleeMiss36:
            case MeleeMiss37:
            case MeleeMiss38:
            case MeleeMiss39:
            case MeleeMiss40:
            case MeleeMiss41:           { return "sound-effects/combat/melee-miss"; }
            case FistHit1:
            case FistHit2:
            case FistHit3:
            case FistHit4:
            case FistHit5:
            case FistHit6:
            case FistHit7:
            case FistHit8:              { return "sound-effects/combat/fist-hit"; }
            case BreathHit1:
            case BreathHit2:            { return "sound-effects/combat/breath-hit"; }
            case TendrilHit1:
            case TendrilHit2:
            case TendrilHit3:
            case TendrilHit4:           { return "sound-effects/combat/tendril-hit"; }
            case WhipHit:               { return "sound-effects/combat/whip-hit"; }
            case WhipMiss1:
            case WhipMiss2:
            case WhipMiss3:
            case WhipMiss4:
            case WhipMiss5:
            case WhipMiss6:
            case WhipMiss7:
            case WhipMiss8:
            case WhipMiss9:
            case WhipMiss10:            { return "sound-effects/combat/whip-miss"; }
            case MaterialHitMetal1:
            case MaterialHitMetal2:
            case MaterialHitMetal3:
            case MaterialHitMetal4:
            case MaterialHitMetal5:
            case MaterialHitBone:
            case MaterialHitCloth:
            case MaterialHitFlesh:
            case MaterialHitHide:
            case MaterialHitHorn:
            case MaterialHitLeather:
            case MaterialHitLiquid:
            case MaterialHitPlant:
            case MaterialHitScale:
            case MaterialHitStone:
            case MaterialHitTooth:
            case MaterialHitWood:
            case MaterialHitMisc:       { return "sound-effects/combat/material-hit"; }
            case ClawTear:              { return "sound-effects/combat/claw-tear"; }
            case Magic1:                { return "sound-effects/misc"; }
            case SpellSelect1:
            case SpellSelect2:
            case SpellSelect3:
            case SpellSparks:           { return "sound-effects/spells"; }
            case Count:
            case None:
            case Random:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::sound_effect::Directory(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

}
