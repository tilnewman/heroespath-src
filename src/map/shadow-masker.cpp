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
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software.  If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
//
// shadow-masker.cpp
//
#include "shadow-masker.hpp"

#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>

namespace heroespath
{
namespace map
{

    const sf::Color ShadowMasker::TRANSPARENT_MASK_{ sf::Color(75, 99, 127) };
    const sf::Color ShadowMasker::SHADOW_MASK1_{ sf::Color(0, 0, 0) };
    const sf::Color ShadowMasker::SHADOW_MASK2_{ sf::Color(151, 0, 147) };
    const sf::Color ShadowMasker::SHADOW_MASK2B_{ sf::Color(127, 0, 127) };
    const sf::Color ShadowMasker::SHADOW_MASK3_{ sf::Color(255, 0, 255) };
    const sf::Color ShadowMasker::SHADOW_COLOR1_{ sf::Color(0, 0, 0, 100) };
    const sf::Color ShadowMasker::SHADOW_COLOR2_{ sf::Color(0, 0, 0, 65) };
    const sf::Color ShadowMasker::SHADOW_COLOR3_{ sf::Color(0, 0, 0, 35) };
    const unsigned ShadowMasker::COLOR_COMPONENT_COUNT_{ 4 };

    void ShadowMasker::ChangeColors(const std::string & XML_ATTRIB_NAME_SHADOWS, Layout & layout)
    {
        for (auto & nextTilesPanel : layout.tiles_panel_vec)
        {
            namespace ba = boost::algorithm;

            auto const IS_SHADOW_IMAGE{ ba::contains(
                ba::to_lower_copy(nextTilesPanel.name),
                ba::to_lower_copy(XML_ATTRIB_NAME_SHADOWS)) };

            ChangeColors(layout.texture_vec[nextTilesPanel.texture_index], IS_SHADOW_IMAGE);
        }
    }

    void ShadowMasker::ChangeColors(sf::Texture & texture, const bool IS_SHADOW_IMAGE)
    {
        sf::Image srcImage(texture.copyToImage());
        sf::Image destImage(srcImage);

        const sf::Uint8 * const PIXEL_PTR{ srcImage.getPixelsPtr() };

        auto const PIXEL_COUNT{ static_cast<std::size_t>(
            srcImage.getSize().x * srcImage.getSize().y * COLOR_COMPONENT_COUNT_) };

        for (std::size_t i(0); i < PIXEL_COUNT; i += 4)
        {
            const sf::Uint8 RED{ *(PIXEL_PTR + i + 0) };
            const sf::Uint8 GREEN{ *(PIXEL_PTR + i + 1) };
            const sf::Uint8 BLUE{ *(PIXEL_PTR + i + 2) };

            auto const DEST_X{ (static_cast<unsigned>(i) / COLOR_COMPONENT_COUNT_)
                               % destImage.getSize().x };

            auto const DEST_Y{ (static_cast<unsigned>(i) / COLOR_COMPONENT_COUNT_)
                               / destImage.getSize().x };

            // check for faded blue background color that should be made fully transparent
            if ((RED == TRANSPARENT_MASK_.r) && (GREEN == TRANSPARENT_MASK_.g)
                && (BLUE == TRANSPARENT_MASK_.b))
            {
                destImage.setPixel(DEST_X, DEST_Y, sf::Color::Transparent);
            }
            else
            {
                // all remaining shadow colors has a green value of zero
                if (IS_SHADOW_IMAGE && (GREEN == 0))
                {
                    if ((RED == SHADOW_MASK1_.r) && (BLUE == SHADOW_MASK1_.b))
                    {
                        destImage.setPixel(DEST_X, DEST_Y, SHADOW_COLOR1_);
                    }
                    else if (
                        ((RED == SHADOW_MASK2_.r) && (BLUE == SHADOW_MASK2_.b))
                        || ((RED == SHADOW_MASK2B_.r) && (BLUE == SHADOW_MASK2B_.b)))
                    {
                        destImage.setPixel(DEST_X, DEST_Y, SHADOW_COLOR2_);
                    }
                    else if ((RED == SHADOW_MASK3_.r) && (BLUE == SHADOW_MASK3_.b))
                    {
                        destImage.setPixel(DEST_X, DEST_Y, SHADOW_COLOR3_);
                    }
                }
            }
        }

        texture.update(destImage);
    }
} // namespace map
} // namespace heroespath
