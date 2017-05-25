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
#ifndef SFMLUTIL_SOUNDEFFECTSENUM_INCLUDED
#define SFMLUTIL_SOUNDEFFECTSENUM_INCLUDED
//
// sound-effects-enum.hpp
//  An enum defining the various sound effects
//
#include <string>
#include <vector>


namespace sfml_util
{

    struct sound_effect
    {
        enum Enum
        {
            WindGust1 = 0,
            WindGust2,
            WindGust3,
            PromptGeneric,
            PromptQuestion,
            PromptWarn,
            Switch1,
            Switch2,
            Switch3,
            Switch4,
            Thock1,
            Thock2,
            Thock3,
            Thock4,
            Thock5,
            Thock6,
            TickOff1,
            TickOff2,
            TickOn1,
            TickOn2,
            Coins1,
            Coins2,
            Coins3,
            Coins4,
            Coins5,
            Coins6,
            Coins7,
            Coins8,
            Coins9,
            Coins10,
            Coins11,
            Coins12,
            Coins13,
            Coins14,
            Coins15,
            Coins16,
            Gems,
            MeteorShards,
            ItemGive,
            ItemDrop,
            BlowpipeShoot1,
            BlowpipeShoot2,
            BlowpipeShoot3,
            BlowpipeHit1,
            BlowpipeHit2,
            BlowpipeHit3,
            ArrowShoot1,
            ArrowShoot2,
            ArrowShoot3,
            ArrowShoot4,
            ArrowShoot5,
            ArrowShoot6,
            ArrowHit,
            ProjectileMiss1,
            ProjectileMiss2,
            ProjectileMiss3,
            ProjectileMiss4,
            ProjectileMiss5,
            ProjectileMiss6,
            ProjectileMiss7,
            ProjectileMiss8,
            ProjectileMiss9,
            ProjectileMiss10,
            ProjectileMiss11,
            ProjectileMiss12,
            ProjectileMiss13,
            MeleeMiss1,
            MeleeMiss2,
            MeleeMiss3,
            MeleeMiss4,
            MeleeMiss5,
            MeleeMiss6,
            MeleeMiss7,
            MeleeMiss8,
            MeleeMiss9,
            MeleeMiss10,
            MeleeMiss11,
            MeleeMiss12,
            MeleeMiss13,
            MeleeMiss14,
            MeleeMiss15,
            MeleeMiss16,
            MeleeMiss17,
            MeleeMiss18,
            MeleeMiss19,
            MeleeMiss20,
            MeleeMiss21,
            MeleeMiss22,
            MeleeMiss23,
            MeleeMiss24,
            MeleeMiss25,
            MeleeMiss26,
            MeleeMiss27,
            MeleeMiss28,
            MeleeMiss29,
            MeleeMiss30,
            MeleeMiss31,
            MeleeMiss32,
            MeleeMiss33,
            MeleeMiss34,
            MeleeMiss35,
            MeleeMiss36,
            MeleeMiss37,
            MeleeMiss38,
            MeleeMiss39,
            MeleeMiss40,
            MeleeMiss41,
            FistHit1,
            FistHit2,
            FistHit3,
            FistHit4,
            FistHit5,
            FistHit6,
            FistHit7,
            FistHit8,
            BreathHit1,
            BreathHit2,
            TendrilHit1,
            TendrilHit2,
            TendrilHit3,
            TendrilHit4,
            WhipHit,
            WhipMiss1,
            WhipMiss2,
            WhipMiss3,
            WhipMiss4,
            WhipMiss5,
            WhipMiss6,
            WhipMiss7,
            WhipMiss8,
            WhipMiss9,
            WhipMiss10,
            MaterialHitMetal1,
            MaterialHitMetal2,
            MaterialHitMetal3,
            MaterialHitMetal4,
            MaterialHitMetal5,
            MaterialHitBone,
            MaterialHitCloth,
            MaterialHitFlesh,
            MaterialHitHide,
            MaterialHitHorn,
            MaterialHitLeather,
            MaterialHitLiquid,
            MaterialHitPlant,
            MaterialHitScale,
            MaterialHitStone,
            MaterialHitTooth,
            MaterialHitWood,
            MaterialHitMisc,
            ClawTear,
            Magic1,
            Count,
            None,
            Random
        };

        static const std::string FILE_EXTENSION_OGG;
        static const std::string FILE_EXTENSION_FLAC;

        static const std::string ToString(const sound_effect::Enum);
        static const std::string FileExt(const sound_effect::Enum);
        static const std::string Filename(const sound_effect::Enum);
        static const std::string Directory(const sound_effect::Enum);
    };

    using SoundEffectEnumVec_t = std::vector<sound_effect::Enum>;

}
#endif //SFMLUTIL_SOUNDEFFECTSENUM_INCLUDED
