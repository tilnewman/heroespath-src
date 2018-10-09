// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// shadow-masker.cpp
//
#include "shadow-masker.hpp"

#include "misc/boost-string-includes.hpp"

namespace heroespath
{
namespace map
{

    const sf::Color ShadowMasker::TRANSPARENT_MASK_ { sf::Color(75, 99, 127) };
    const sf::Color ShadowMasker::SHADOW_MASK1_ { sf::Color(0, 0, 0) };
    const sf::Color ShadowMasker::SHADOW_MASK2_ { sf::Color(151, 0, 147) };
    const sf::Color ShadowMasker::SHADOW_MASK2B_ { sf::Color(127, 0, 127) };
    const sf::Color ShadowMasker::SHADOW_MASK3_ { sf::Color(255, 0, 255) };
    const sf::Color ShadowMasker::SHADOW_COLOR1_ { sf::Color(0, 0, 0, 100) };
    const sf::Color ShadowMasker::SHADOW_COLOR2_ { sf::Color(0, 0, 0, 65) };
    const sf::Color ShadowMasker::SHADOW_COLOR3_ { sf::Color(0, 0, 0, 35) };
    const unsigned ShadowMasker::COLOR_COMPONENT_COUNT_ { 4 };

    void ShadowMasker::ChangeColors(
        const std::string & XML_ATTRIB_NAME_SHADOWS, Layout & layout) const
    {
        for (auto & nextTilesPanel : layout.tiles_panel_vec)
        {
            namespace ba = boost::algorithm;

            const auto IS_SHADOW_IMAGE { ba::contains(
                ba::to_lower_copy(nextTilesPanel.name),
                ba::to_lower_copy(XML_ATTRIB_NAME_SHADOWS)) };

            ChangeColors(layout.texture_vec[nextTilesPanel.texture_index], IS_SHADOW_IMAGE);
        }
    }

    void ShadowMasker::ChangeColors(sf::Texture & texture, const bool IS_SHADOW_IMAGE) const
    {
        sf::Image srcImage(texture.copyToImage());
        sf::Image destImage(srcImage);

        const auto DEST_IMAGE_SIZE_X { destImage.getSize().x };

        auto destPixelX { [&](const unsigned INDEX) {
            return ((INDEX / COLOR_COMPONENT_COUNT_) % DEST_IMAGE_SIZE_X);
        } };

        auto destPixelY { [&](const unsigned INDEX) {
            return ((INDEX / COLOR_COMPONENT_COUNT_) / DEST_IMAGE_SIZE_X);
        } };

        const sf::Uint8 * const PIXEL_PTR { srcImage.getPixelsPtr() };

        const auto PIXEL_COUNT { srcImage.getSize().x * srcImage.getSize().y
                                 * COLOR_COMPONENT_COUNT_ };

        for (unsigned i(0); i < PIXEL_COUNT; i += 4)
        {
            const sf::Uint8 RED { *(PIXEL_PTR + i + 0) };
            const sf::Uint8 GREEN { *(PIXEL_PTR + i + 1) };
            const sf::Uint8 BLUE { *(PIXEL_PTR + i + 2) };

            // check for faded blue background color that should be made fully transparent
            if ((RED == TRANSPARENT_MASK_.r) && (GREEN == TRANSPARENT_MASK_.g)
                && (BLUE == TRANSPARENT_MASK_.b))
            {
                destImage.setPixel(destPixelX(i), destPixelY(i), sf::Color::Transparent);
            }
            else if (IS_SHADOW_IMAGE && (GREEN == 0))
            {
                // all remaining shadow colors has a green value of zero

                if ((RED == SHADOW_MASK1_.r) && (BLUE == SHADOW_MASK1_.b))
                {
                    destImage.setPixel(destPixelX(i), destPixelY(i), SHADOW_COLOR1_);
                }
                else if (
                    ((RED == SHADOW_MASK2_.r) && (BLUE == SHADOW_MASK2_.b))
                    || ((RED == SHADOW_MASK2B_.r) && (BLUE == SHADOW_MASK2B_.b)))
                {
                    destImage.setPixel(destPixelX(i), destPixelY(i), SHADOW_COLOR2_);
                }
                else if ((RED == SHADOW_MASK3_.r) && (BLUE == SHADOW_MASK3_.b))
                {
                    destImage.setPixel(destPixelX(i), destPixelY(i), SHADOW_COLOR3_);
                }
            }
        }

        texture.update(destImage);
    }

} // namespace map
} // namespace heroespath
