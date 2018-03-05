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
    }
}
}
