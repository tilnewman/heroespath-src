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
#ifndef SFMLUTIL_CLOUDANIMATION_HPP_INCLUDED
#define SFMLUTIL_CLOUDANIMATION_HPP_INCLUDED
//
// cloud-animation.hpp
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sliders.hpp"

#include <vector>
#include <memory>


namespace sfml_util
{
namespace animation
{

    //Responsible for storing all the information about a single spark
    class Cloud : public sf::Drawable
    {
    public:
        Cloud(const sf::Texture &  TEXTURE,
              const sf::Vector2f & START_POS_V,
              const sf::Vector2f & END_POS_V,
              const float          BASE_SPEED,
              const float          START_SCALE,
              const float          END_SCALE,
              const float          ROTATION_SPEED,
              const sf::Color &    START_COLOR,
              const sf::Color &    END_COLOR);

        inline bool IsFinished() const { return isFinished_; }

        //Returns true if isFinished_, implying that moving/animating has
        //stopped and that this Spark can be discarded/ignored/not-drawn.
        bool Update(const float ELAPSED_TIME_SEC);

        void draw(sf::RenderTarget & target, sf::RenderStates states) const;

    private:
        bool isFinished_;
        sf::Sprite sprite_;
        sf::Vector2f startPosV_;
        sf::Vector2f endPosV_;
        float startScale_;
        float endScale_;
        float rotationSpeed_;
        sf::Color startColor_;
        sf::Color endColor_;
        sliders::ZeroSliderOnce<float> slider_;
    };

    using CloudVec_t = std::vector<Cloud>;


    //Responsible for drawing emitted sparks
    class CloudAnimation : public sf::Drawable
    {
        //prevent copy construction
        CloudAnimation(const CloudAnimation &);

        //prevent copy assignment
        CloudAnimation & operator=(const CloudAnimation &);

    public:
        explicit CloudAnimation(const sf::FloatRect & REGION,
                                const float           CENTER_VARATION_RATIO          = 0.33f,
                                const float           START_SCALE_BASE               = 0.5f,
                                const float           START_SCALE_VARIATION_RATIO    = 0.25f,
                                const float           END_SCALE_BASE                 = 0.75f,
                                const float           END_SCALE_VARIATION_RATIO      = 0.25f,
                                const float           EMIT_RATE_BASE_PER_SEC         = 3.0f,
                                const float           EMIT_RATE_VARATION_RATIO       = 0.75f,
                                const float           DURATION_SEC                   = 6.0f,
                                const float           SPEED_BASE                     = 2.0f,
                                const float           SPEED_VARIATION_RATIO          = 0.5f,
                                const float           ROTATION_SPEED_BASE            = 2.0f,
                                const float           ROTATION_SPEED_VARIATION_RATIO = 0.5f);

        inline bool IsFinished() const { return isFinished_; }
        void Update(const float ELAPSED_TIME_SEC);
        void draw(sf::RenderTarget & target, sf::RenderStates states) const;

    private:
        float ValueWithRandomVariance(const float BASE, const float VARIANCE_RATIO) const;

    private:
        const sf::FloatRect REGION_;
        const float CENTER_VAR_RATIO_;
        const float START_SCALE_BASE_;
        const float START_SCALE_VAR_RATIO_;
        const float END_SCALE_BASE_;
        const float END_SCALE_VAR_RATIO_;
        const float SEC_PER_EMIT_BASE_;
        const float SEC_PER_EMIT_VAR_RATIO_;
        const float DURATION_SEC_;
        const float SPEED_BASE_;
        const float SPEED_VAR_RATIO_;
        const float ROTATION_SPEED_BASE_;
        const float ROTATION_SPEED_VAR_RATIO_;
        float emitTimerSec_;
        float emitTimerDurationSec_;
        float durationTimerSec_;
        bool isFinished_;
        sf::Texture cloudTexture1_;
        sf::Texture cloudTexture2_;
        sf::Texture cloudTexture3_;
        CloudVec_t cloudVec_;
    };

    using CloudAnimationUPtr_t = std::unique_ptr<CloudAnimation>;
    using CloudAnimationUVec_t = std::vector<CloudAnimationUPtr_t>;

}
}

#endif //SFMLUTIL_CLOUDANIMATION_HPP_INCLUDED
