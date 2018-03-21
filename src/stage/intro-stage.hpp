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
#ifndef HEROESPATH_INTROSTAGE_HPP_INCLUDED
#define HEROESPATH_INTROSTAGE_HPP_INCLUDED
//
// intro-stage.hpp
//  A simple Stage class to display the intro title and start playing the theme music.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/stage.hpp"

namespace heroespath
{
namespace stage
{

    // A simple Stage class that displays the intro title
    class IntroStage : public sfml_util::Stage
    {
    public:
        IntroStage(const IntroStage &) = delete;
        IntroStage(IntroStage &&) = delete;
        IntroStage & operator=(const IntroStage &) = delete;
        IntroStage & operator=(IntroStage &&) = delete;

    public:
        IntroStage();
        virtual ~IntroStage();

        virtual void Setup();
        virtual void Draw(sf::RenderTarget & target, const sf::RenderStates & STATES);
        virtual void UpdateTime(const float ELAPSED_TIME_SECONDS);

    private:
        void PositionTitleImage();

    private:
        sf::Texture titleTexture_;
        sf::Sprite titleSprite_;
        std::size_t initialDrawHoldCounter_;
    };
} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_INTROSTAGE_HPP_INCLUDED
