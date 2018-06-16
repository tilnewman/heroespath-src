// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// animation-enum.cpp
//
#include "animation-enum.hpp"

#include "misc/boost-string-includes.hpp"

namespace heroespath
{
namespace sfml_util
{

    const std::string Animations::ToString(const Enum E)
    {
        switch (E)
        {
            case Burst:
            {
                return "Burst";
            }
            case CandleFlame:
            {
                return "CandleFlame";
            }
            case CandleFlame2:
            {
                return "CandleFlame2";
            }
            case Campfire:
            {
                return "Campfire";
            }
            case DualCharge:
            {
                return "DualCharge";
            }
            case DualSwirl:
            {
                return "DualSwirl";
            }
            case ExplosionLarge:
            {
                return "ExplosionLarge";
            }
            case ExplosionSmall:
            {
                return "ExplosionSmall";
            }
            case ExplosionMedium1:
            {
                return "ExplosionMedium1";
            }
            case ExplosionMedium2:
            {
                return "ExplosionMedium2";
            }
            case ExplosionMedium3:
            {
                return "ExplosionMedium3";
            }
            case FireTorch:
            {
                return "FireTorch";
            }
            case Flash:
            {
                return "Flash";
            }
            case FlashSparkle:
            {
                return "FlashSparkle";
            }
            case Inferno:
            {
                return "Inferno";
            }
            case LightningBall:
            {
                return "LightningBall";
            }
            case LightningBolt:
            {
                return "LightningBolt";
            }
            case OrbCharge:
            {
                return "OrbCharge";
            }
            case OrbShimmer:
            {
                return "OrbShimmer";
            }
            case Puff:
            {
                return "Puff: ";
            }
            case PuffShort:
            {
                return "PuffShort";
            }
            case SelectSwirl:
            {
                return "SelectSwirl";
            }
            case Shimmer:
            {
                return "Shimmer";
            }
            case Smoke:
            {
                return "Smoke";
            }
            case SmokeSwirl:
            {
                return "SmokeSwirl";
            }
            case SpiderFlare:
            {
                return "SpiderFlare";
            }
            case SplashMagenta1:
            {
                return "SplashMagenta1";
            }
            case SplashMagenta2:
            {
                return "SplashMagenta2";
            }
            case SplashWhite:
            {
                return "SplashWhite";
            }
            case SymbolReduce:
            {
                return "SymbolReduce";
            }
            case Count:
            default:
            {
                ThrowInvalidValueForFunction(E, "ToString");
            }
        }
    }

    const std::string Animations::MediaPathKey(const Enum E)
    {
        switch (E)
        {
            case Burst:
            {
                return "media-anim-image-burst";
            }
            case Campfire:
            {
                return "media-anim-images-dir-campfire";
            }
            case CandleFlame:
            {
                return "media-anim-image-candleflame";
            }
            case CandleFlame2:
            {
                return "media-anim-images-dir-candleflame2";
            }
            case DualCharge:
            {
                return "media-anim-images-dir-dualcharge";
            }
            case DualSwirl:
            {
                return "media-anim-image-dualswirl";
            }
            case ExplosionLarge:
            {
                return "media-anim-images-dir-explosion";
            }
            case ExplosionSmall:
            {
                return "media-anim-image-explosion-small";
            }
            case ExplosionMedium1:
            {
                return "media-anim-image-explosion1";
            }
            case ExplosionMedium2:
            {
                return "media-anim-image-explosion2";
            }
            case ExplosionMedium3:
            {
                return "media-anim-image-explosion3";
            }
            case FireTorch:
            {
                return "media-anim-image-firetorch";
            }
            case Flash:
            {
                return "media-anim-image-flash";
            }
            case FlashSparkle:
            {
                return "media-anim-image-flash-sparkle";
            }
            case Inferno:
            {
                return "media-anim-images-dir-inferno";
            }
            case LightningBall:
            {
                return "media-anim-images-dir-lightningball";
            }
            case LightningBolt:
            {
                return "media-anim-images-dir-lightningbolt";
            }
            case OrbCharge:
            {
                return "media-anim-images-dir-orbcharge";
            }
            case OrbShimmer:
            {
                return "media-anim-images-dir-orbshimmer";
            }
            case Puff:
            {
                return "media-anim-image-puff";
            }
            case PuffShort:
            {
                return "media-anim-image-puffhalf";
            }
            case SelectSwirl:
            {
                return "media-anim-image-selectswirl";
            }
            case Shimmer:
            {
                return "media-anim-images-dir-shimmer";
            }
            case Smoke:
            {
                return "media-anim-image-smoke";
            }
            case SmokeSwirl:
            {
                return "media-anim-images-dir-smokesiwrl";
            }
            case SpiderFlare:
            {
                return "media-anim-images-dir-spiderflare";
            }
            case SplashMagenta1:
            {
                return "media-anim-image-splash2";
            }
            case SplashMagenta2:
            {
                return "media-anim-image-splash3";
            }
            case SplashWhite:
            {
                return "media-anim-images-dir-splash1";
            }
            case SymbolReduce:
            {
                return "media-anim-images-dir-symbolreduce";
            }
            case Count:
            default:
            {
                ThrowInvalidValueForFunction(E, "MediaPathKey");
            }
        }
    }

    bool Animations::IsMultiTexture(const Enum E)
    {
        switch (E)
        {
            case Burst:
            case CandleFlame:
            case DualSwirl:
            case ExplosionSmall:
            case ExplosionMedium1:
            case ExplosionMedium2:
            case ExplosionMedium3:
            case FireTorch:
            case Flash:
            case FlashSparkle:
            case Puff:
            case PuffShort:
            case SelectSwirl:
            case Smoke:
            case SplashMagenta1:
            case SplashMagenta2:
            {
                return false;
            }
            case Campfire:
            case CandleFlame2:
            case DualCharge:
            case ExplosionLarge:
            case LightningBall:
            case LightningBolt:
            case Inferno:
            case OrbCharge:
            case OrbShimmer:
            case Shimmer:
            case SmokeSwirl:
            case SpiderFlare:
            case SplashWhite:
            case SymbolReduce:
            {
                return true;
            }
            case Count:
            default:
            {
                ThrowInvalidValueForFunction(E, "IsMultiTexture");
            }
        }
    }

    const sf::Vector2f Animations::SizePair(const Enum E)
    {
        switch (E)
        {
            case Burst:
            {
                return sf::Vector2f(120.0f, 120.0f);
            }
            case Campfire:
            {
                return sf::Vector2f(81.0f, 64.0f);
            }
            case CandleFlame:
            {
                return sf::Vector2f(128.0f, 128.0f);
            }
            case CandleFlame2:
            {
                return sf::Vector2f(16.0f, 24.0f);
            }
            case DualCharge:
            {
                return sf::Vector2f(133.0f, 169.0f);
            }
            case DualSwirl:
            {
                return sf::Vector2f(140.0f, 140.0f);
            }
            case ExplosionLarge:
            {
                return sf::Vector2f(320.0f, 240.0f);
            }
            case ExplosionSmall:
            {
                return sf::Vector2f(128.0f, 128.0f);
            }
            case ExplosionMedium1:
            {
                return sf::Vector2f(100.0f, 100.0f);
            }
            case ExplosionMedium2:
            {
                return sf::Vector2f(100.0f, 100.0f);
            }
            case ExplosionMedium3:
            {
                return sf::Vector2f(100.0f, 100.0f);
            }
            case FireTorch:
            {
                return sf::Vector2f(128.0f, 256.0f);
            }
            case Flash:
            {
                return sf::Vector2f(128.0f, 128.0f);
            }
            case FlashSparkle:
            {
                return sf::Vector2f(128.0f, 128.0f);
            }
            case Inferno:
            {
                return sf::Vector2f(256.0f, 256.0f);
            }
            case LightningBall:
            {
                return sf::Vector2f(56.0f, 65.0f);
            }
            case LightningBolt:
            {
                return sf::Vector2f(98.0f, 203.0f);
            }
            case OrbCharge:
            {
                return sf::Vector2f(80.0f, 67.0f);
            }
            case OrbShimmer:
            {
                return sf::Vector2f(184.0f, 85.0f);
            }
            case Puff:
            {
                return sf::Vector2f(80.0f, 80.0f);
            }
            case PuffShort:
            {
                return sf::Vector2f(100.0f, 100.0f);
            }
            case SelectSwirl:
            {
                return sf::Vector2f(140.0f, 140.0f);
            }
            case Shimmer:
            {
                return sf::Vector2f(186.0f, 83.0f);
            }
            case Smoke:
            {
                return sf::Vector2f(164.0f, 164.0f);
            }
            case SmokeSwirl:
            {
                return sf::Vector2f(256.0f, 256.0f);
            }
            case SpiderFlare:
            {
                return sf::Vector2f(168.0f, 148.0f);
            }
            case SplashMagenta1:
            {
                return sf::Vector2f(120.0f, 120.0f);
            }
            case SplashMagenta2:
            {
                return sf::Vector2f(240.0f, 240.0f);
            }
            case SplashWhite:
            {
                return sf::Vector2f(50.0f, 47.0f);
            }
            case SymbolReduce:
            {
                return sf::Vector2f(177.0f, 156.0f);
            }
            case Count:
            default:
            {
                ThrowInvalidValueForFunction(E, "SizePair");
            }
        }
    }

    float Animations::TimePerFrameSec(const Animations::Enum E)
    {
        switch (E)
        {
            case CandleFlame2:
            {
                return 0.1f;
            }
            case Burst:
            case Campfire:
            case CandleFlame:
            case DualCharge:
            case DualSwirl:
            case ExplosionLarge:
            case ExplosionSmall:
            case ExplosionMedium1:
            case ExplosionMedium2:
            case ExplosionMedium3:
            case FireTorch:
            case Flash:
            case FlashSparkle:
            case Inferno:
            case LightningBall:
            case LightningBolt:
            case OrbCharge:
            case OrbShimmer:
            case Puff:
            case PuffShort:
            case SelectSwirl:
            case Shimmer:
            case Smoke:
            case SmokeSwirl:
            case SpiderFlare:
            case SplashMagenta1:
            case SplashMagenta2:
            case SplashWhite:
            case SymbolReduce:
            {
                return 0.05f;
            }
            case Count:
            default:
            {
                ThrowInvalidValueForFunction(E, "TimePerFrameSec");
            }
        }
    }

    const MusicEnumVec_t Animations::Sfx(const Animations::Enum E)
    {
        if (E == Animations::Campfire)
        {
            return { music::FireIndoorLarge };
        }
        else
        {
            return {};
        }
    }

} // namespace sfml_util
} // namespace heroespath
