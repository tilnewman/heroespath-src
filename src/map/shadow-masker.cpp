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

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>

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
    const std::size_t ShadowMasker::COLOR_COMPONENT_COUNT_ { 4 };

    void ShadowMasker::ChangeColors(sf::Texture & texture, const bool IS_SHADOW_IMAGE)
    {
        sf::Image image(texture.copyToImage());
        ChangeColors(image, IS_SHADOW_IMAGE);
        texture.update(image);
    }

    void ShadowMasker::ChangeColors(sf::Image & image, const bool IS_SHADOW_IMAGE)
    {
        const sf::Vector2<std::size_t> IMAGE_SIZE_VT { image.getSize() };

        auto destPixelX { [&](const std::size_t INDEX) {
            return static_cast<unsigned>((INDEX / COLOR_COMPONENT_COUNT_) % IMAGE_SIZE_VT.x);
        } };

        auto destPixelY { [&](const std::size_t INDEX) {
            return static_cast<unsigned>((INDEX / COLOR_COMPONENT_COUNT_) / IMAGE_SIZE_VT.x);
        } };

        const sf::Uint8 * const FIRST_PIXEL_PTR { image.getPixelsPtr() };

        const auto COLOR_COMPONENTS_IN_IMAGE_COUNT {
            IMAGE_SIZE_VT.x * IMAGE_SIZE_VT.y * static_cast<std::size_t>(COLOR_COMPONENT_COUNT_)
        };

        for (std::size_t pixelIndex(0); pixelIndex < COLOR_COMPONENTS_IN_IMAGE_COUNT;
             pixelIndex += COLOR_COMPONENT_COUNT_)
        {
            const sf::Uint8 RED { *(FIRST_PIXEL_PTR + pixelIndex + 0) };
            const sf::Uint8 GREEN { *(FIRST_PIXEL_PTR + pixelIndex + 1) };
            const sf::Uint8 BLUE { *(FIRST_PIXEL_PTR + pixelIndex + 2) };

            // check for faded blue background color that should be made fully transparent
            if ((RED == TRANSPARENT_MASK_.r) && (GREEN == TRANSPARENT_MASK_.g)
                && (BLUE == TRANSPARENT_MASK_.b))
            {
                image.setPixel(
                    destPixelX(pixelIndex), destPixelY(pixelIndex), sf::Color::Transparent);
            }
            else if (IS_SHADOW_IMAGE && (GREEN == 0))
            {
                // all remaining shadow colors has a green value of zero

                if ((RED == SHADOW_MASK1_.r) && (BLUE == SHADOW_MASK1_.b))
                {
                    image.setPixel(destPixelX(pixelIndex), destPixelY(pixelIndex), SHADOW_COLOR1_);
                }
                else if (
                    ((RED == SHADOW_MASK2_.r) && (BLUE == SHADOW_MASK2_.b))
                    || ((RED == SHADOW_MASK2B_.r) && (BLUE == SHADOW_MASK2B_.b)))
                {
                    image.setPixel(destPixelX(pixelIndex), destPixelY(pixelIndex), SHADOW_COLOR2_);
                }
                else if ((RED == SHADOW_MASK3_.r) && (BLUE == SHADOW_MASK3_.b))
                {
                    image.setPixel(destPixelX(pixelIndex), destPixelY(pixelIndex), SHADOW_COLOR3_);
                }
            }
        }
    }

} // namespace map
} // namespace heroespath
