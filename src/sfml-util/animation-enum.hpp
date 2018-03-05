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
#ifndef HEROESPATH_SFMLUTIL_ANIMATIONENUM_HPP_INCLUDE
#define HEROESPATH_SFMLUTIL_ANIMATIONENUM_HPP_INCLUDE
//
// animation-enum.hpp
//  An enumeration of all animations.
//
#include "sfml-util/music-enum.hpp"
#include <string>
#include <utility> //for std::pair

namespace heroespath
{
namespace sfml_util
{

    struct Animations
    {
        enum Enum
        {
            Burst = 0,
            Campfire,
            CandleFlame,
            CandleFlame2, // a very small pixel animation for candles on the map
            DualCharge,
            DualSwirl,
            ExplosionLarge,
            ExplosionSmall,
            ExplosionMedium1,
            ExplosionMedium2,
            ExplosionMedium3,
            FireTorch,
            Flash,
            FlashSparkle,
            Inferno,
            LightningBall,
            LightningBolt,
            OrbCharge,
            OrbShimmer,
            Puff,
            PuffShort,
            SelectSwirl,
            Shimmer,
            Smoke,
            SmokeSwirl,
            SpiderFlare,
            SplashMagenta1,
            SplashMagenta2,
            SplashWhite,
            SymbolReduce,
            Count
        };

        static const std::string ToString(const Enum);
        static Enum FromString(const std::string &);
        static const std::string MediaPathKey(const Enum);
        static bool IsMultiTexture(const Enum);
        static const std::pair<int, int> SizePair(const Enum);
        static float TimePerFrameSec(const Enum);
        static const MusicEnumVec_t Sfx(const Enum);
    };
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_ANIMATIONENUM_HPP_INCLUDE
