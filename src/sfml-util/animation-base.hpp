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
#ifndef SFMLUTIL_ANIMATIONBASE_HPP_INCLUDED
#define SFMLUTIL_ANIMATIONBASE_HPP_INCLUDED
//
// animation-base.hpp
//  A class responsible for common state and operation of all animation objects.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/gui/gui-entity.hpp"
#include "sfml-util/animation-enum.hpp"

#include <string>
#include <vector>
#include <memory>


namespace sfml_util
{

    class Animation : public gui::GuiEntity
    {
        //prevent copy construction
        Animation(const Animation &) = delete;

        //prevent copy assignment
        Animation & operator=(const Animation &) = delete;

    public:
        Animation(const Animations::Enum ENUM,
            const sf::FloatRect &  REGION,
            const float            TIME_PER_FRAME_SEC,
            const sf::BlendMode    BLEND_MODE,
            const sf::Color &      COLOR_FROM,
            const sf::Color &      COLOR_TO);

        virtual ~Animation();

        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const = 0;

        virtual void SetEntityPos(const sf::Vector2f & V) = 0;
        virtual void SetEntityPos(const float LEFT, const float TOP) = 0;

        virtual void SetEntityRegion(const sf::FloatRect & R) = 0;

        //returns true if frame count wrapped around back to zero
        virtual bool UpdateTime(const float SECONDS) = 0;

        virtual std::size_t FrameCount() const = 0;

        virtual void MoveEntityPos(const float HORIZ, const float VERT) = 0;

        virtual const sf::Vector2f OrigSize() const = 0;

        inline virtual void OnClick(const sf::Vector2f &)
        {}

        inline std::size_t CurrentFrame() const
        {
            return currentFrame_;
        }

        inline float TimePerFrame() const
        {
            return timePerFrameSec_;
        }

        inline void TimePerFrameSet(const float TBF)
        {
            timePerFrameSec_ = TBF;
        }

        inline void TimePerFrameAdj(const float ADJ)
        {
            timePerFrameSec_ += ADJ;
        }

        inline void ColorTransition(const sf::Color & FROM,
                                    const sf::Color & TO)
        {
            colorFrom_ = FROM;
            colorTo_ = TO;
        }

        inline bool IsFinished() const
        {
            return isFinished_;
        }

        inline Animations::Enum Which() const
        {
            return which_;
        }

    protected:
        Animations::Enum which_;
        sf::BlendMode blendMode_;
        float timePerFrameSec_;

        //counts up to total then restarts at zero
        std::size_t currentFrame_;

        float frameTimerSec_;
        sf::Color colorFrom_;
        sf::Color colorTo_;
        bool isFinished_;
    };


    using AnimationPtr_t = Animation *;
    using AnimationUPtr_t = std::unique_ptr<Animation>;
    using AnimationUVec_t = std::vector<AnimationUPtr_t>;

}

#endif //SFMLUTIL_ANIMATIONBASE_HPP_INCLUDED
