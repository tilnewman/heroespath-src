// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_ANIMATIONENUM_HPP_INCLUDE
#define HEROESPATH_GUI_ANIMATIONENUM_HPP_INCLUDE
//
// animation-enum.hpp
//  An enumeration of all animations.
//
#include "gui/music-enum.hpp"
#include "misc/enum-common.hpp"

#include <SFML/System/Vector2.hpp>

#include <string>
#include <utility> //for std::pair

namespace heroespath
{
namespace gui
{

    struct Animations : public EnumBaseCounting<EnumFirstValue::Valid>
    {
        enum Enum : EnumUnderlying_t
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
        static const std::string MediaPathKey(const Enum);
        static bool IsMultiTexture(const Enum);
        static float TimePerFrameSec(const Enum);
        static const MusicEnumVec_t Sfx(const Enum);

        // returns frame size for AnimationSingle and the common full texture size for
        // AnimationMults
        static const sf::Vector2f ImageSize(const Enum);
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_ANIMATIONENUM_HPP_INCLUDE
