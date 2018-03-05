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
#ifndef HEROESPATH_SFMLUTIL_POS_SLIDER_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_POS_SLIDER_HPP_INCLUDED
//
//  pos-slider.hpp
//
#include "sfml-util/moving-enum.hpp"
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sliders.hpp"

namespace heroespath
{
namespace sfml_util
{
    namespace sliders
    {

        // Presents a simple interface for controlling a position that starts at a given FROM and
        // then moves toward a TO position.  Calling Update() periodically with the time elapsed
        // advances the position.  ChangeDirection() does not change the current position, it sets
        // FROM to the current position and sets TO to either the original FROM or TO.
        class PosSlider
        {
        public:
            PosSlider();

            explicit PosSlider(
                const sf::Vector2f & FROM_POS_V,
                const sf::Vector2f & TO_POS_V,
                const float SLIDER_SPEED);

            virtual ~PosSlider() {}

            void Setup(
                const sf::Vector2f & FROM_POS_V,
                const sf::Vector2f & TO_POS_V,
                const float SLIDER_SPEED);

            void ChangeDirection();

            // returns true if the position was changed
            virtual bool UpdateTime(const float ELAPSED_TIME_SECONDS);

            const sf::Vector2f Position() const;

            inline float ProgressRatio() const { return slider_.Current(); }
            inline sfml_util::Moving::Enum Direction() const { return direction_; }
            inline const sf::Vector2f To() const { return toPosV_; }
            inline const sf::Vector2f From() const { return fromPosV_; }
            inline float Speed() const { return slider_.Speed(); }
            inline bool IsMoving() const { return isMoving_; }
            inline void Start() { isMoving_ = true; }
            inline void Stop() { isMoving_ = false; }

            inline void Speed(const float S) { slider_.Reset(S, slider_.Current()); }

        protected:
            sf::Vector2f origFromPosV_;
            sf::Vector2f origToPosV_;
            sf::Vector2f fromPosV_;
            sf::Vector2f toPosV_;
            sfml_util::Moving::Enum direction_;
            bool isMoving_;
            sfml_util::sliders::ZeroSliderOnce<float> slider_;
        };
    }
}
}

#endif // HEROESPATH_SFMLUTIL_POS_SLIDER_HPP_INCLUDED
