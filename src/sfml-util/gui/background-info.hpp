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
#ifndef SFMLUTIL_BACKGROUNDINFO_INCLUDED
#define SFMLUTIL_BACKGROUNDINFO_INCLUDED
//
// background-info.hpp
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/gradient.hpp"

#include <memory>
#include <string>


namespace sfml_util
{
namespace gui
{

    //Encapsulates all the info needed to display a background.
    //Note:  Region width and height can be more or less than the image dimmensions
    //Note:  The default background color is white because that is the default
    //       overlay color for a sprite.  Transparent would make the sprite image
    //       disapear.
    //Note:  Designed so that using the default constructor (with no params) is
    //       safe and causes nothing to be drawn.
    struct BackgroundInfo
    {
        //use this constructor when no background is desired
        BackgroundInfo();

        //use this constructor when only a filled-rectangle background is desired
        explicit BackgroundInfo(const sf::Color &    FILL_COLOR,
                                const GradientInfo & GRADIENT_INFO = GradientInfo());

        //use this constructor when the sf::Texture of the background image already exists
        explicit BackgroundInfo(const sf::Texture &   TEXTURE,
                                const sf::FloatRect & REGION        = sf::FloatRect(),
                                const sf::Color &     COLOR         = sf::Color::White,
                                const GradientInfo &  GRADIENT_INFO = GradientInfo());

        //use this constructor when only the path to the background image is known
        explicit BackgroundInfo(const std::string &   TEXTURE_PATH,
                                const sf::FloatRect & REGION        = sf::FloatRect(),
                                const sf::Color &     COLOR         = sf::Color::White,
                                const GradientInfo &  GRADIENT_INFO = GradientInfo());

        //returns false if either region dimmension is zero or less
        inline bool IsValid() const { return ((region.width > 0.0f) && (region.height > 0.0f)); }

        sf::FloatRect region;
        sf::Texture   texture;
        sf::Color     color;
        std::string   path;
        GradientInfo  gradient_info;
        bool          hasTexture;
    };


    bool operator==(const BackgroundInfo & L, const BackgroundInfo & R);

    inline bool operator!=(const BackgroundInfo & L, const BackgroundInfo & R)
    {
        return ! (L == R);
    }

}
}

#endif //SFMLUTIL_BACKGROUNDINFO_INCLUDED
