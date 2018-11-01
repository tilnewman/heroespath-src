// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_CLOUDANIMATION_HPP_INCLUDED
#define HEROESPATH_GUI_CLOUDANIMATION_HPP_INCLUDED
//
// cloud-animation.hpp
//
#include "sfml-util/cached-texture.hpp"
#include "sfml-util/sliders.hpp"

#include <SFML/Graphics/Sprite.hpp>

#include <memory>
#include <vector>

namespace heroespath
{
namespace gui
{
    namespace animation
    {

        // Responsible for storing all the information about a single cloud
        class Cloud : public sf::Drawable
        {
        public:
            Cloud(
                const sf::Texture & TEXTURE,
                const sf::Vector2f & START_POS_V,
                const sf::Vector2f & END_POS_V,
                const float BASE_SPEED,
                const float START_SCALE,
                const float END_SCALE,
                const float ROTATION_SPEED,
                const sf::Color & START_COLOR,
                const sf::Color & END_COLOR);

            bool IsFinished() const { return isFinished_; }

            // Returns true if isFinished_, implying that moving/animating has
            // stopped and that this Spark can be discarded/ignored/not-drawn.
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
            SliderZeroToOne slider_;
        };

        using CloudVec_t = std::vector<Cloud>;

        // Responsible for drawing emitted clouds
        class CloudAnimation : public sf::Drawable
        {
            CloudAnimation(const CloudAnimation &);
            CloudAnimation & operator=(const CloudAnimation &);

        public:
            explicit CloudAnimation(
                const sf::FloatRect & REGION,
                const float CENTER_VARATION_RATIO = 0.33f,
                const float START_SCALE_BASE = 0.5f,
                const float START_SCALE_VARIATION_RATIO = 0.25f,
                const float END_SCALE_BASE = 0.75f,
                const float END_SCALE_VARIATION_RATIO = 0.25f,
                const float EMIT_RATE_BASE_PER_SEC = 3.0f,
                const float EMIT_RATE_VARATION_RATIO = 0.75f,
                const float DURATION_SEC = 6.0f,
                const float SPEED_BASE = 2.0f,
                const float SPEED_VARIATION_RATIO = 0.5f,
                const float ROTATION_SPEED_BASE = 2.0f,
                const float ROTATION_SPEED_VARIATION_RATIO = 0.5f);

            bool IsFinished() const { return isFinished_; }
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
            gui::CachedTexture cloudCachedTexture1_;
            gui::CachedTexture cloudCachedTexture2_;
            gui::CachedTexture cloudCachedTexture3_;
            CloudVec_t cloudVec_;
        };

        using CloudAnimationUPtr_t = std::unique_ptr<CloudAnimation>;
        using CloudAnimationUVec_t = std::vector<CloudAnimationUPtr_t>;

    } // namespace animation
} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_CLOUDANIMATION_HPP_INCLUDED
