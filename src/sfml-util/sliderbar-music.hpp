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
#ifndef SFMLUTIL_SLIDERBARMUSIC_INCLUDED
#define SFMLUTIL_SLIDERBARMUSIC_INCLUDED
//
// sliderbar-music.hpp
//  A SliderBar that controls the music volume.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/gui/sliderbar-labeled.hpp"

#include <string>
#include <memory>
#include <vector>


namespace sfml_util
{

    //forward declaration
    namespace gui
    {
        class MouseTextInfo;
    }


    //Encapsulates a gui sliderbar with a label that controls the music volume.
    class SliderBarLabeled_Music : public gui::SliderBarLabeled
    {
        SliderBarLabeled_Music(const SliderBarLabeled_Music &) =delete;
        SliderBarLabeled_Music & operator=(const SliderBarLabeled_Music &) =delete;

    public:
        SliderBarLabeled_Music(const std::string &        NAME,
                               const float                POS_LEFT,
                               const float                POS_TOP,
                               const float                LENGTH,
                               const gui::SliderStyle &   STYLE,
                               const gui::MouseTextInfo & THREE_TEXT_INFOS,
                               const float                INITIAL_VALUE           = 0.0f,
                               const float                RELATIVE_LABEL_POS_LEFT = 0.0f,
                               const float                RELATIVE_LABEL_POS_TOP  = 0.0f);//must be [0.0f, 1.0f]

        virtual ~SliderBarLabeled_Music();

        virtual void OnChange(const float NEW_VALUE);
        virtual void SetPadPosition();
        virtual bool MouseDown(const sf::Vector2f & MOUSE_POS_V);
        virtual void SetCurrentValueFromScreenCoords(const sf::Vector2f & NEW_COORD_V);
    };

    using SliderBarLabeled_MusicUPtr_t = std::unique_ptr<SliderBarLabeled_Music>;

}

#endif //SFMLUTIL_SLIDERBARMUSIC_INCLUDED
