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
// animation-enum.cpp
//
#include "animation-enum.hpp"

#include <sstream>
#include <exception>


namespace sfml_util
{

    const std::string Animations::ToString(const Enum E)
    {
        switch (E)
        {
            case Burst:             { return "Burst"; }
            case CandleFlame:       { return "CandleFlame"; }
            case DualCharge:        { return "DualCharge"; }
            case DualSwirl:         { return "DualSwirl"; }
            case ExplosionLarge:    { return "ExplosionLarge"; }
            case ExplosionSmall:    { return "ExplosionSmall"; }
            case ExplosionMedium1:  { return "ExplosionMedium1"; }
            case ExplosionMedium2:  { return "ExplosionMedium2"; }
            case ExplosionMedium3:  { return "ExplosionMedium3"; }
            case FireTorch:         { return "FireTorch"; }
            case Flash:             { return "Flash"; }
            case FlashSparkle:      { return "FlashSparkle"; }
            case Inferno:           { return "Inferno"; }
            case LightningBall:     { return "LightningBall"; }
            case LightningBolt:     { return "LightningBolt"; }
            case OrbCharge:         { return "OrbCharge"; }
            case OrbShimmer:        { return "OrbShimmer"; }
            case Puff:              { return "Puff: "; }
            case PuffShort:         { return "PuffShort"; }
            case SelectSwirl:       { return "SelectSwirl"; }
            case Shimmer:           { return "Shimmer"; }
            case Smoke:             { return "Smoke"; }
            case SmokeSwirl:        { return "SmokeSwirl"; }
            case SpiderFlare:       { return "SpiderFlare"; }
            case SplashMagenta1:    { return "SplashMagenta1"; }
            case SplashMagenta2:    { return "SplashMagenta2"; }
            case SplashWhite:       { return "SplashWhite"; }
            case SymbolReduce:      { return "SymbolReduce"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::Animations::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::string Animations::MediaPathKey(const Enum E)
    {
        switch (E)
        {
            case Burst:             { return "media-anim-image-burst"; }
            case CandleFlame:       { return "media-anim-images-candleflame"; }
            case DualCharge:        { return "media-anim-images-dir-dualcharge"; }
            case DualSwirl:         { return "media-anim-image-dualswirl"; }
            case ExplosionLarge:    { return "media-anim-images-dir-explosion"; }
            case ExplosionSmall:    { return "media-anim-images-explosion-small"; }
            case ExplosionMedium1:  { return "media-anim-image-explosion1"; }
            case ExplosionMedium2:  { return "media-anim-image-explosion2"; }
            case ExplosionMedium3:  { return "media-anim-image-explosion3"; }
            case FireTorch:         { return "media-anim-image-firetorch"; }
            case Flash:             { return "media-anim-image-flash"; }
            case FlashSparkle:      { return "media-anim-image-flash-sparkle"; }
            case Inferno:           { return "media-anim-images-dir-inferno"; }
            case LightningBall:     { return "media-anim-images-dir-lightningball"; }
            case LightningBolt:     { return "media-anim-images-dir-lightningbolt"; }
            case OrbCharge:         { return "media-anim-images-dir-orbcharge"; }
            case OrbShimmer:        { return "media-anim-images-dir-orbshimmer"; }
            case Puff:              { return "media-anim-image-puff"; }
            case PuffShort:         { return "media-anim-image-puffhalf"; }
            case SelectSwirl:       { return "media-anim-image-selectswirl"; }
            case Shimmer:           { return "media-anim-images-dir-shimmer"; }
            case Smoke:             { return "media-anim-image-smoke"; }
            case SmokeSwirl:        { return "media-anim-images-dir-smokesiwrl"; }
            case SpiderFlare:       { return "media-anim-images-dir-spiderflare"; }
            case SplashMagenta1:    { return "media-anim-image-splash2"; }
            case SplashMagenta2:    { return "media-anim-image-splash3"; }
            case SplashWhite:       { return "media-anim-images-dir-splash1"; }
            case SymbolReduce:      { return "media-anim-images-dir-symbolreduce"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::Animations::MediaPathKey(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
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
            case SplashMagenta2: { return false; }
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
            case SymbolReduce: { return true; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::Animations::IsMultiTexture(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::pair<int, int> Animations::SizePair(const Enum E)
    {
        switch (E)
        {
            case Burst:             { return std::make_pair(120, 120); }
            case CandleFlame:       { return std::make_pair(128, 128); }
            case DualCharge:        { return std::make_pair(133, 169); }
            case DualSwirl:         { return std::make_pair(140, 140); }
            case ExplosionLarge:    { return std::make_pair(320, 240); }
            case ExplosionSmall:    { return std::make_pair(128, 128); }
            case ExplosionMedium1:  { return std::make_pair(100, 100); }
            case ExplosionMedium2:  { return std::make_pair(100, 100); }
            case ExplosionMedium3:  { return std::make_pair(100, 100); }
            case FireTorch:         { return std::make_pair(128, 256); }
            case Flash:             { return std::make_pair(128, 128); }
            case FlashSparkle:      { return std::make_pair(128, 128); }
            case Inferno:           { return std::make_pair(256, 256); }
            case LightningBall:     { return std::make_pair( 56,  65); }
            case LightningBolt:     { return std::make_pair( 98, 203); }
            case OrbCharge:         { return std::make_pair( 80,  67); }
            case OrbShimmer:        { return std::make_pair(184,  85); }
            case Puff:              { return std::make_pair( 80,  80); }
            case PuffShort:         { return std::make_pair(100, 100); }
            case SelectSwirl:       { return std::make_pair(140, 140); }
            case Shimmer:           { return std::make_pair(186,  83); }
            case Smoke:             { return std::make_pair(164, 164); }
            case SmokeSwirl:        { return std::make_pair(256, 256); }
            case SpiderFlare:       { return std::make_pair(168, 148); }
            case SplashMagenta1:    { return std::make_pair(120, 120); }
            case SplashMagenta2:    { return std::make_pair(240, 240); }
            case SplashWhite:       { return std::make_pair( 50, 47);  }
            case SymbolReduce:      { return std::make_pair(177, 156); }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::Animations::SizePair(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

}
