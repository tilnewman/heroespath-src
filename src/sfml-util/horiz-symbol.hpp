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
#ifndef HEROESPATH_SFMLUTIL_HORIZSYMBOL_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_HORIZSYMBOL_HPP_INCLUDED
//
// horiz-symbol.hpp
//  A symbol drawn at the bottom of menu screens
//
#include "sfml-util/sfml-graphics.hpp"


namespace heroespath
{
namespace sfml_util
{

    //Responsible for drawing the horizontal symbols at the bottom of Stages.
    //A positive VERT_OFFSET_RATIO will move the symbols down, negative moves them up.
    //The Bottom() of the symbol is always the bottom of the screen, even if
    //VERT_OFFSET_RATIO is > 0 and some of the symbol is offscreen.
    class BottomSymbol : public sf::Drawable
    {
        BottomSymbol(const BottomSymbol &) =delete;
        BottomSymbol & operator=(const BottomSymbol &) =delete;

    public:
        explicit BottomSymbol(
            const float       VERT_SCALE        = 1.0f,
            const bool        WILL_INVERT_COLOR = false,
            const sf::Color & COLOR             = DEFAULT_COLOR_,
            const float       VERT_OFFSET_RATIO = 0.0f);

        virtual ~BottomSymbol();

        void Setup(
            const float       VERT_SCALE        = 1.0f,
            const bool        WILL_INVERT_COLOR = false,
            const sf::Color & COLOR             = DEFAULT_COLOR_,
            const float       VERT_OFFSET_RATIO = 0.0f);

        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

        inline float Height() const { return Bottom() - Top(); }
        inline float Top() const    { return sprite1_.getGlobalBounds().top; }
        inline float Middle() const { return Top() + (Height() * 0.5f); }
        float Bottom() const;

    public:
        static const sf::Color DEFAULT_COLOR_;

    private:
        sf::Sprite sprite1_;
        sf::Sprite sprite2_;
        sf::Sprite sprite3_;
        sf::Sprite sprite4_;
        sf::Texture texture_;
    };

}
}

#endif //HEROESPATH_SFMLUTIL_HORIZSYMBOL_HPP_INCLUDED