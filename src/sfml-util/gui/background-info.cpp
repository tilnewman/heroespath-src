// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// background-info.cpp
//
#include "background-info.hpp"

#include <string>
#include <tuple>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        BackgroundInfo::BackgroundInfo()
            : region(sf::FloatRect())
            , texture()
            , color(sf::Color::Transparent)
            , path("")
            , gradient_info()
            , hasTexture(false)
        {}

        BackgroundInfo::BackgroundInfo(
            const sf::Color & FILL_COLOR, const GradientInfo & GRADIENT_INFO)
            : region(sf::FloatRect())
            , texture()
            , color(FILL_COLOR)
            , path("")
            , gradient_info(GRADIENT_INFO)
            , hasTexture(false)
        {}

        BackgroundInfo::BackgroundInfo(
            const sf::Texture & TEXTURE,
            const sf::FloatRect & REGION,
            const sf::Color & COLOR,
            const GradientInfo & GRADIENT_INFO)
            : region(REGION)
            , texture(TEXTURE)
            , color(COLOR)
            , path("")
            , gradient_info(GRADIENT_INFO)
            , hasTexture(true)
        {}

        BackgroundInfo::BackgroundInfo(
            const std::string & TEXTURE_PATH,
            const sf::FloatRect & REGION,
            const sf::Color & COLOR,
            const GradientInfo & GRADIENT_INFO)
            : region(REGION)
            , texture()
            , color(COLOR)
            , path(TEXTURE_PATH)
            , gradient_info(GRADIENT_INFO)
            , hasTexture(false)
        {}

        bool operator==(const BackgroundInfo & L, const BackgroundInfo & R)
        {
            return std::tie(
                       L.region,
                       // L.texture,
                       L.color,
                       L.path,
                       L.gradient_info,
                       L.hasTexture)
                == std::tie(
                       R.region,
                       // R.texture,
                       R.color,
                       R.path,
                       R.gradient_info,
                       R.hasTexture);
        }
    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
