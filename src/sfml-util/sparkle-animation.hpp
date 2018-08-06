// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_SPARKLEANIMATION_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_SPARKLEANIMATION_HPP_INCLUDED
//
// sparkle-animation.hpp
//
#include "sfml-util/cached-texture.hpp"
#include "sfml-util/sliders.hpp"

#include <SFML/Graphics/Sprite.hpp>

#include <memory>
#include <vector>

namespace heroespath
{
namespace sfml_util
{
    namespace animation
    {

        // Responsible for storing all the information about a single spark
        class Sparkle : public sf::Drawable
        {
        public:
            Sparkle(
                const sf::Texture & TEXTURE,
                const sf::Vector2f & POS_V,
                const float SPEED,
                const float TARGET_SCALE);

            bool IsFinished() const { return isFinished_; }

            // Returns true if isFinished_, implying that moving/animating has
            // stopped and that this Spark can be discarded/ignored/not-drawn.
            bool Update(const float ELAPSED_TIME_SEC);

            void draw(sf::RenderTarget & target, sf::RenderStates states) const;

        private:
            bool isFinished_;
            sf::Sprite sprite_;
            sf::Vector2f posV_;
            float targetScale_;
            sliders::ZeroSliderOnce<float> slider_;
        };

        using SparkleVec_t = std::vector<Sparkle>;

        // Responsible for drawing emitted sparks
        class SparkleAnimation : public sf::Drawable
        {
            SparkleAnimation(const SparkleAnimation &);
            SparkleAnimation & operator=(const SparkleAnimation &);

        public:
            explicit SparkleAnimation(
                const sf::FloatRect & REGION,
                const float SCALE_BASE,
                const float SCALE_VARIATION_RATIO = 0.2f,
                const float EMIT_RATE_BASE_PER_SEC = 3.0f,
                const float EMIT_RATE_VARATION_RATIO = 0.75f,
                const float DURATION_SEC = 6.0f,
                const float SPEED_BASE = 2.0f,
                const float SPEED_VARIATION_RATIO = 0.5f);

            bool IsFinished() const { return isFinished_; }
            void Update(const float ELAPSED_TIME_SEC);
            void draw(sf::RenderTarget & target, sf::RenderStates states) const;

        private:
            float ValueWithRandomVariance(const float BASE, const float VARIANCE_RATIO) const;

        private:
            const sf::FloatRect REGION_;
            const float SCALE_BASE_;
            const float SCALE_VAR_RATIO_;
            const float SEC_PER_EMIT_BASE_;
            const float SEC_PER_EMIT_VAR_RATIO_;
            const float DURATION_SEC_;
            const float SPEED_BASE_;
            const float SPEED_VAR_RATIO_;
            float emitTimerSec_;
            float emitTimerDurationSec_;
            float durationTimerSec_;
            bool isFinished_;
            sfml_util::CachedTexture sparkCachedTexture1_;
            sfml_util::CachedTexture sparkCachedTexture2_;
            sfml_util::CachedTexture sparkCachedTexture3_;
            SparkleVec_t sparkleVec_;
        };

        using SparkleAnimationUPtr_t = std::unique_ptr<SparkleAnimation>;
        using SparkleAnimationUVec_t = std::vector<SparkleAnimationUPtr_t>;

    } // namespace animation
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_SPARKLEANIMATION_HPP_INCLUDED
