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
#ifndef SFMLUTIL_GUI_BOXINFO_INCLUDED
#define SFMLUTIL_GUI_BOXINFO_INCLUDED
//
// box-info.hpp
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/gui/color-set.hpp"
#include "sfml-util/gui/background-info.hpp"

#include <memory>
#include <string>


namespace sfml_util
{
namespace gui
{
namespace box
{

    //Encapsulates everything needed to draw a box, either with the gui
    //gold bars or with simple lines.
    //
    //Note: Designed so that using the default constructor (with all default params)
    //      is safe and will cause nothing to be drawn.
    class Info
    {
    public:
        //use as default constructor to draw nothing, or for specific construction
        explicit Info(const ColorSet &       COLOR_SET        = ColorSet(),
                      const sf::FloatRect &  REGION           = sf::FloatRect(),
                      const bool             GOLD_BARS        = false,
                      const float            LINE_THICKNESS   = 0.0f,
                      const bool             WILL_FRAME_LINES = false,
                      const bool             WILL_GROW        = false,
                      const BackgroundInfo & BACKGROUND_INFO  = BackgroundInfo());

        //use this constructor when only lines are desired
        Info(const float            LINE_THICKNESS,
             const bool             WILL_FRAME_LINES,
             const sf::FloatRect &  REGION,
             const ColorSet &       COLOR_SET       = ColorSet(),
             const BackgroundInfo & BACKGROUND_INFO = BackgroundInfo());

        //use this constructor when only lines are desired, and theree is incomplete background info
        Info(const float            LINE_THICKNESS,
             const bool             WILL_FRAME_LINES,
             const sf::FloatRect &  REGION,
             const std::string &    BG_TEXTURE_PATH,
             const sf::Color &      BG_GRADIENT_TO_COLOR = sf::Color::Transparent,
             const unsigned int     BG_GRADIENT_SIDES    = 0,
             const unsigned int     BG_GRADIENT_CORNERS  = 0);

        //use this constructor when gold bars are desired
        Info(const bool             WILL_GROW,
             const sf::FloatRect &  REGION,
             const ColorSet &       COLOR_SET       = ColorSet(),
             const BackgroundInfo & BACKGROUND_INFO = BackgroundInfo());

        //use this constructor when gold bars are desired, and there is incomplete background info
        Info(const bool             WILL_GROW,
             const sf::FloatRect &  REGION,
             const std::string &    BG_TEXTURE_PATH,
             const sf::Color &      BG_GRADIENT_TO_COLOR = sf::Color::Transparent,
             const unsigned int     BG_GRADIENT_SIDES    = 0,
             const unsigned int     BG_GRADIENT_CORNERS  = 0);

        inline void SetBoxAndBackgroundRegion(const sf::FloatRect & R) { region = R; bg_info.region = R; }

        inline bool IsValid() const { return ((region.width > 0.0f) && (region.height > 0.0f)); }

        //returns true if either gold bars or lines are configured to be drawn, returns false if nothing will be drawn
        inline bool WillBox() const { return ((gold_bars || (line_thickness > 0.0f)) && IsValid()); }

        static const sf::Uint8 DEFAULT_FRAME_LINES_ALPHA_;

        bool           gold_bars;
        bool           will_grow;
        bool           frame_lines;
        float          line_thickness;
        ColorSet       color_set;
        sf::FloatRect  region;
        BackgroundInfo bg_info;
    };


    bool operator==(const Info & L, const Info & R);

    inline bool operator!=(const Info & L, const Info & R)
    {
        return ! (L == R);
    }

}
}
}

#endif //SFMLUTIL_GUI_BOXINFO_INCLUDED
