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
#ifndef SFMLUTIL_POS_SLIDER_INCLUDED
#define SFMLUTIL_POS_SLIDER_INCLUDED
//
//  pos-slider.hpp
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sliders.hpp"
#include "sfml-util/moving-enum.hpp"


namespace sfml_util
{
namespace sliders
{

    //responsible for moving a position back and forth between two points
    class PosSlider
    {
    public:
        explicit PosSlider(const sf::Vector2f & FROM_POS_V   = sf::Vector2f(),
                           const sf::Vector2f & TO_POS_V     = sf::Vector2f(),
                           const float          SLIDER_SPEED = 1.0f);

        virtual ~PosSlider();

        void Setup(const sf::Vector2f & FROM_POS_V   = sf::Vector2f(),
                   const sf::Vector2f & TO_POS_V     = sf::Vector2f(),
                   const float          SLIDER_SPEED = 1.0f);

        virtual void Reset();
        virtual void StartMovingToward();
        virtual void StartMovingAway();

        //returns false if IsFinishedMoving()
        virtual bool UpdateTime(const float ELAPSED_TIME_SECONDS);

        const sf::Vector2f Position() const;

        inline float                    ProgressRatio() const    { return slider_.GetCur(); }
        inline bool                     IsFinishedMoving() const { return isFinishedMoving_; }
        inline sfml_util::Moving::Enum  Moving() const           { return movingDir_; }
        inline const sf::Vector2f       To() const               { return toPosV_; }
        inline const sf::Vector2f       From() const             { return fromPosV_; }
        inline float                    Speed() const            { return slider_.GetSpd(); }
        inline void                     Speed(const float S)     { slider_.Reset(S, slider_.GetCur()); }

    private:
        void ResetSlidersAndStartMoving(const sfml_util::Moving::Enum);
        void ResetSlidersAndStopMoving();

    protected:
        sf::Vector2f origFromPosV_;
        sf::Vector2f fromPosV_;
        sf::Vector2f toPosV_;
        bool isFinishedMoving_;
        sfml_util::Moving::Enum movingDir_;
        sfml_util::sliders::ZeroSliderOnce<float> slider_;
    };

}
}

#endif //SFMLUTIL_POS_SLIDER_INCLUDED
