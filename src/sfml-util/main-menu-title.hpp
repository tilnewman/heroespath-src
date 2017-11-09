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
#ifndef HEROESPATH_SFMLUTIL_MAINMENUTITLE_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_MAINMENUTITLE_HPP_INCLUDED
//
// main-menu-title.hpp
//  Code that draws the title symbol and title text at the top of a main menu screen
//
#include "sfml-util/sfml-util.hpp"

#include <string>


namespace heroespath
{
namespace sfml_util
{

    //Responsible for drawing the title text and symbol at the top of a main menu stage screen.
    class MainMenuTitle : public sf::Drawable
    {
    public:
        //this constructor calls Setup() and SetPositionAndSize()
        explicit MainMenuTitle(
            const std::string & TITLE_IMAGE_FILENAME = "",
            const bool WILL_INVERT_SYMBOL = false,
            const float SYMBOL_SCALE_HORIZ = 1.0f,
            const float SYMBOL_SCALE_VERT = 1.0f);

        virtual ~MainMenuTitle();

        void Setup(
            const std::string & TITLE_IMAGE_FILENAME = "",
            const bool WILL_INVERT_SYMBOL = false,
            const float SYMBOL_SCALE_HORIZ = 1.0f,
            const float SYMBOL_SCALE_VERT = 1.0f);

        void SetPositionAndSize(
            const float SYMBOL_SCALE_HORIZ = 1.0f,
            const float SYMBOL_SCALE_VERT  = 1.0f);

        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;

        inline const sf::FloatRect Region() const { return symbolSprite_.getGlobalBounds(); }

        inline float BottomPad() const { return sfml_util::MapByRes(35.0f, 135.0f); }

        inline float Bottom(const bool WILL_PAD = true) const
        {
            return Region().top + Region().height + ((WILL_PAD) ? BottomPad() : 0.0f);
        }

    private:
        sf::Texture symbolTexture_;
        sf::Sprite  symbolSprite_;
        sf::Texture titleTexture_;
        sf::Sprite  titleSprite_;
        bool willDrawTitle_;
    };

}
}

#endif //HEROESPATH_SFMLUTIL_MAINMENUTITLE_HPP_INCLUDED
