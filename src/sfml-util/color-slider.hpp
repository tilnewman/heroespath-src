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
#ifndef SFMLUTIL_COLOR_SLIDER_HPP_INCLUDED
#define SFMLUTIL_COLOR_SLIDER_HPP_INCLUDED
//
// color-slider.hpp
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/pos-slider.hpp"
#include "sfml-util/moving-enum.hpp"
#include "sfml-util/sliders.hpp"


namespace sfml_util
{

    //Similar to the PositionSlider, ColorSlider presents a simple interface for controlling a
    //color that starts at a given FROM and then moves toward a TO color.  Calling Update()
    //periodically with the time elapsed advances the position.  ChangeDirection() does not
    //change the current position, it sets FROM to the current position and sets TO to either
    //the original FROM or TO.
    class ColorSlider
    {
    public:
        ColorSlider();

        explicit ColorSlider(
            const sf::Color & FROM_COLOR,
            const sf::Color & TO_COLOR,
            const float SPEED);

        void Setup(
            const sf::Color & FROM_COLOR,
            const sf::Color & TO_COLOR,
            const float SPEED);

        void ChangeDirection();

        //returns true if the position was changed 
        bool UpdateTime(const float ELAPSED_TIME_SECONDS);

        const sf::Color Current() const;

        inline float                    ProgressRatio() const   { return slider_.Current(); }
        inline sfml_util::Moving::Enum  Direction() const       { return direction_; }
        inline const sf::Color          To() const              { return toColor_; }
        inline const sf::Color          From() const            { return fromColor_; }
        inline float                    Speed() const           { return slider_.Speed(); }
        inline bool                     IsMoving() const        { return isMoving_; }
        inline void                     Start()                 { isMoving_ = true; }
        inline void                     Stop()                  { isMoving_ = false; }

        inline void Speed(const float S)
        {
            slider_.Reset(S, slider_.Current());
        }

    private:
        sf::Color fromColor_;
        sf::Color toColor_;
        sf::Color origFromColor_;
        sf::Color origToColor_;
        sfml_util::Moving::Enum direction_;
        bool isMoving_;
        sfml_util::sliders::ZeroSliderOnce<float> slider_;
    };

}

#endif //SFMLUTIL_COLOR_SLIDER_HPP_INCLUDED
