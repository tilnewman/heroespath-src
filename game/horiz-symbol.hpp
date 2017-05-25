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
#ifndef GAME_HORIZSYMBOL_INCLUDED
#define GAME_HORIZSYMBOL_INCLUDED
//
// horiz-symbol.hpp
//  A symbol drawn at the bottom of menu screens
//
#include "sfml-util/sfml-graphics.hpp"


namespace game
{

    //draws the horizontal symbols at the bottom of menu stages
    class BottomSymbol
    {
        //prevent copy construction
        BottomSymbol(const BottomSymbol &) =delete;

        //prevent copy assignment
        BottomSymbol & operator=(const BottomSymbol &) =delete;

    public:
        explicit BottomSymbol(const float       VERT_SCALE        = 1.0f,
                              const bool        WILL_INVERT_COLOR = false,
                              const float       SCREEN_WIDTH      = DEFAULT_INVALID_DIMM_,
                              const float       SCREEN_HEIGHT     = DEFAULT_INVALID_DIMM_,
                              const float       HORIZ_POS         = DEFAULT_HORIZ_POS_,
                              const float       VERT_POS_OFFSET   = DEFAULT_VERT_POS_OFFSET_,
                              const sf::Color & COLOR             = DEFAULT_COLOR_);

        virtual ~BottomSymbol();

        void Setup(const float       VERT_SCALE        = 1.0f,
                   const bool        WILL_INVERT_COLOR = false,
                   const float       SCREEN_WIDTH      = DEFAULT_INVALID_DIMM_,
                   const float       SCREEN_HEIGHT     = DEFAULT_INVALID_DIMM_,
                   const float       HORIZ_POS         = DEFAULT_HORIZ_POS_,
                   const float       VERT_POS_OFFSET   = DEFAULT_VERT_POS_OFFSET_,
                   const sf::Color & COLOR             = DEFAULT_COLOR_);

        void Draw(sf::RenderTarget & target, const sf::RenderStates & STATES);

        //these functions must be called after Setup() to return the correct values
        inline float VertPosTop() const        { return posTop_; }
        inline float VertPosMiddle() const     { return posTop_ + (sprite_.getGlobalBounds().height * 0.5f); }
        inline float VertPosBottom() const     { return posTop_ + sprite_.getGlobalBounds().height; }

    public:
        static const float     DEFAULT_INVALID_DIMM_;
        static const float     DEFAULT_HORIZ_POS_;
        static const float     DEFAULT_VERT_POS_OFFSET_;
        static const sf::Color DEFAULT_COLOR_;
    private:
        float       halfScreenWidth_;
        float       screenHeight_;
        float       horizPos_;
        float       vertPosOffset_;
        sf::Sprite  sprite_;
        sf::Texture texture_;
        float       posTop_;
    };

}
#endif //GAME_HORIZSYMBOL_INCLUDED
