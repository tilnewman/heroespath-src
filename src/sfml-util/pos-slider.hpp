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

    //responsible for moving a position vector back and forth between two points using the slider method
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

        virtual void Reset(const bool WILL_RESET_POSITION = false);
        virtual void StartMovingToward(const float DEST_POS_LEFT = -1.0f, const float DEST_POS_TOP = -1.0f);
        virtual void StartMovingAway(const bool TO_HOME = true);

        //returns false if IsFinishedMoving()
        virtual bool UpdateTime(const float ELAPSED_TIME_SECONDS);

        inline float                    CurrentValueHoriz() const        { return currHoriz_; }
        inline float                    CurrentValueVert() const         { return currVert_; }
        inline float                    CurrentAverage() const           { return (currHoriz_ + currVert_) * 0.5f; }
        inline const sf::Vector2f       Pos() const                      { return posV_; }
        inline float                    Speed() const                    { return sliderSpeed_; }
        inline void                     Speed(const float S)             { sliderSpeed_ = S; }
        inline bool                     IsFinishedMoving() const         { return (isFinishedMovingHoriz_ && isFinishedMovingVert_); }
        inline sfml_util::Moving::Enum  Moving() const                   { return movingDir_; }
        inline const sf::Vector2f       PosVTo() const                   { return toPosV_; }
        inline void                     PosVTo(const sf::Vector2f & P)   { toPosV_ = P; }
        inline const sf::Vector2f       PosVFrom() const                 { return fromPosV_; }
        inline void                     PosVFrom(const sf::Vector2f & P) { fromPosV_ = P; }
        inline const sf::Vector2f       PosVHome() const                 { return homePosV_; }
        inline void                     PosVHome(const sf::Vector2f & P) { homePosV_ = P; }

    private:
        void ResetSlidersAndStartMoving(const sfml_util::Moving::Enum);
        void ResetSlidersAndStopMoving();

    protected:
        //the actual position
        sf::Vector2f posV_;

        //one speed that is common to both the horiz and vert sliders
        float sliderSpeed_;

        //the original 'first' location of the entity
        sf::Vector2f homePosV_;

        //the location the entity is moving from
        sf::Vector2f fromPosV_;

        //the location the entity is moving to
        sf::Vector2f toPosV_;

        //used to prevent repeated calls to misc::IsRealClose()
        bool isFinishedMovingHoriz_;
        bool isFinishedMovingVert_;

        //helpful detail member
        sfml_util::Moving::Enum movingDir_;

        //chached values from the last call to slider Update()
        float currHoriz_;
        float currVert_;

        //the sliders used to move the entity
        sfml_util::sliders::ZeroSliderOnce<float> sliderHoriz_;
        sfml_util::sliders::ZeroSliderOnce<float> sliderVert_;
    };

}
}


#endif //SFMLUTIL_POS_SLIDER_INCLUDED
