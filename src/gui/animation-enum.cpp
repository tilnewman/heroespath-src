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

#include "misc/config-file.hpp"
#include "misc/filesystem.hpp"
#include "misc/log-macros.hpp"

namespace heroespath
{
namespace gui
{

    const std::string Animations::ToString(const Enum ANIM)
    {
        switch (ANIM)
        {
            case Burst:
            {
                return "Burst";
            }
            case CandleFlame:
            {
                return "Candle-Flame";
            }
            case CandleFlamePixel:
            {
                return "Candle-Flame-Pixel";
            }
            case Campfire:
            {
                return "Campfire";
            }
            case DualCharge:
            {
                return "Dual-Charge";
            }
            case DualSwirl:
            {
                return "Dual-Swirl";
            }
            case ExplosionLarge:
            {
                return "Explosion-Large";
            }
            case ExplosionSmall:
            {
                return "Explosion-Small";
            }
            case ExplosionMedium1:
            {
                return "Explosion-Medium-1";
            }
            case ExplosionMedium2:
            {
                return "Explosion-Medium-2";
            }
            case ExplosionMedium3:
            {
                return "Explosion-Medium-3";
            }
            case FireTorch:
            {
                return "Torch";
            }
            case Flash:
            {
                return "Flash";
            }
            case FlashSparkle:
            {
                return "Flash-Sparkle";
            }
            case Inferno:
            {
                return "Inferno";
            }
            case LightningBall:
            {
                return "Lightning-Ball";
            }
            case LightningBolt:
            {
                return "Lightning-Bolt";
            }
            case OrbCharge:
            {
                return "Orb-Charge";
            }
            case OrbShimmer:
            {
                return "Orb-Shimmer";
            }
            case Puff:
            {
                return "Puff";
            }
            case PuffShort:
            {
                return "Puff-Half";
            }
            case Swirl:
            {
                return "Swirl";
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
                return "Smoke-Swirl";
            }
            case SpiderFlare:
            {
                return "Spider-Flare";
            }
            case Splash1:
            {
                return "Splash-1";
            }
            case Splash2:
            {
                return "Splash-2";
            }
            case Splash3:
            {
                return "Splash-3";
            }
            case SymbolReduce:
            {
                return "Symbol-Reduce";
            }
            case Count:
            {
                return "(Count)";
            }
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(ANIM) << " is invalid. (count="
                                  << static_cast<EnumUnderlying_t>(Count) << ")");

                return "";
            }
        }
    }

    const std::string Animations::MediaPath(const Enum ANIM)
    {
        static std::string dirPath;

        if (dirPath.empty())
        {
            dirPath = misc::ConfigFile::Instance()->GetMediaPath("media-image-animation-dir");
        }

        return misc::filesystem::CombinePathsAndClean(dirPath, misc::ToLowerCopy(ToString(ANIM)));
    }

    const sf::Vector2i Animations::ImageSize(const Enum ANIM)
    {
        switch (ANIM)
        {
            case Burst:
            {
                return sf::Vector2i(128, 128);
            }
            case Campfire:
            {
                return sf::Vector2i(64, 64);
            }
            case CandleFlame:
            {
                return sf::Vector2i(64, 128);
            }
            case CandleFlamePixel:
            {
                return sf::Vector2i(16, 32);
            }
            case DualCharge:
            {
                return sf::Vector2i(128, 256);
            }
            case DualSwirl:
            {
                return sf::Vector2i(128, 64);
            }
            case ExplosionLarge:
            {
                return sf::Vector2i(256, 256);
            }
            case ExplosionSmall:
            {
                return sf::Vector2i(64, 64);
            }
            case ExplosionMedium1:
            {
                return sf::Vector2i(128, 128);
            }
            case ExplosionMedium2:
            {
                return sf::Vector2i(128, 128);
            }
            case ExplosionMedium3:
            {
                return sf::Vector2i(128, 128);
            }
            case FireTorch:
            {
                return sf::Vector2i(128, 256);
            }
            case Flash:
            {
                return sf::Vector2i(128, 128);
            }
            case FlashSparkle:
            {
                return sf::Vector2i(128, 128);
            }
            case Inferno:
            {
                return sf::Vector2i(128, 256);
            }
            case LightningBall:
            {
                return sf::Vector2i(64, 64);
            }
            case LightningBolt:
            {
                return sf::Vector2i(128, 256);
            }
            case OrbCharge:
            {
                return sf::Vector2i(64, 64);
            }
            case OrbShimmer:
            {
                return sf::Vector2i(128, 64);
            }
            case Puff:
            {
                return sf::Vector2i(64, 64);
            }
            case PuffShort:
            {
                return sf::Vector2i(128, 128);
            }
            case Swirl:
            {
                return sf::Vector2i(128, 128);
            }
            case Shimmer:
            {
                return sf::Vector2i(128, 64);
            }
            case Smoke:
            {
                return sf::Vector2i(128, 128);
            }
            case SmokeSwirl:
            {
                return sf::Vector2i(256, 256);
            }
            case SpiderFlare:
            {
                return sf::Vector2i(128, 128);
            }
            case Splash1:
            {
                return sf::Vector2i(64, 64);
            }
            case Splash2:
            {
                return sf::Vector2i(128, 128);
            }
            case Splash3:
            {
                return sf::Vector2i(256, 256);
            }
            case SymbolReduce:
            {
                return sf::Vector2i(128, 128);
            }
            case Count:
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(ANIM) << " is invalid. (count="
                                  << static_cast<EnumUnderlying_t>(Count) << ")");

                return sf::Vector2i(0, 0);
            }
        }
    }

    float Animations::TimePerFrameSec(const Animations::Enum ANIM)
    {
        const auto ANIM_TIME_BETWEEN_FRAMES_DEFAULT { 0.055f };

        switch (ANIM)
        {
            case CandleFlamePixel:
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
            case Swirl:
            case Shimmer:
            case Smoke:
            case SmokeSwirl:
            {
                return 0.035f;
            }
            case SpiderFlare:
            case Splash1:
            case Splash2:
            case Splash3:
            case SymbolReduce:
            {
                return ANIM_TIME_BETWEEN_FRAMES_DEFAULT;
            }
            case Count:
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(ANIM) << " is invalid. (count="
                                  << static_cast<EnumUnderlying_t>(Count) << ")");

                return ANIM_TIME_BETWEEN_FRAMES_DEFAULT;
            }
        }
    }

    const MusicEnumVec_t Animations::Sfx(const Animations::Enum ANIM)
    {
        if (ANIM == Animations::Campfire)
        {
            return { music::FireIndoorLarge };
        }
        else
        {
            return {};
        }
    }

    const sf::IntRect Animations::TextureRectTrim(const Enum ANIM)
    {
        if (ANIM == Campfire)
        {
            return sf::IntRect(7, 7, 7, 7);
        }

        return sf::IntRect();
    }

} // namespace gui
} // namespace heroespath
