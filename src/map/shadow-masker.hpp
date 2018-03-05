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
#ifndef HEROESPATH_SHADOWMASKER_HPP_INCLUDED
#define HEROESPATH_SHADOWMASKER_HPP_INCLUDED
//
// shadow-masker.hpp
//
#include "map/layout.hpp"
#include "sfml-util/sfml-graphics.hpp"

namespace heroespath
{
namespace map
{

    // Responsible for changing shadow place-holder colors into shades of gray.
    class ShadowMasker
    {
        ShadowMasker() = delete;
        ShadowMasker(const ShadowMasker &) = delete;
        ShadowMasker(const ShadowMasker &&) = delete;
        ShadowMasker & operator=(const ShadowMasker &) = delete;

    public:
        static void ChangeColors(const std::string & XML_ATTRIB_NAME_SHADOWS, Layout & layout);
        static void ChangeColors(sf::Texture &, const bool IS_SHADOW_IMAGE);

    private:
        // The tileset I found online uses this color as a background,
        // so it needs to be changed to transparen.t
        static const sf::Color TRANSPARENT_MASK_;

        // the tileset I found online uses these magenta colors as shades for shadows
        static const sf::Color SHADOW_MASK1_;
        static const sf::Color SHADOW_MASK2_;
        static const sf::Color SHADOW_MASK2B_;
        static const sf::Color SHADOW_MASK3_;

        // these are the shades of semi-transparent black I chose for shadows
        static const sf::Color SHADOW_COLOR1_;
        static const sf::Color SHADOW_COLOR2_;
        static const sf::Color SHADOW_COLOR3_;

        static const unsigned COLOR_COMPONENT_COUNT_;
    };
} // namespace map
} // namespace heroespath

#endif // HEROESPATH_SHADOWMASKER_HPP_INCLUDED
