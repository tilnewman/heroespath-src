#ifndef SFMLUTIL_SPARKSANIMATION_HPP_INCLUDED
#define SFMLUTIL_SPARKSANIMATION_HPP_INCLUDED
//
// sparks-animation.hpp
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
    class Spark : public sf::Drawable
    {
    public:
        Spark(const sf::Texture &  TEXTURE,
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

    using SparkVec_t = std::vector<Spark>;


    //Responsible for drawing emitted sparks
    class SparksAnimation : public sf::Drawable
    {
        //prevent copy construction
        SparksAnimation(const SparksAnimation &);

        //prevent copy assignment
        SparksAnimation & operator=(const SparksAnimation &);

    public:
        explicit SparksAnimation(const bool            WILL_EMIT_RIGHT,
                                 const sf::FloatRect & REGION,
                                 const float           SPRAY_RATIO                    = 0.25f,
                                 const float           SCALE_BASE                     = 1.0f,
                                 const float           SCALE_VARIATION_RATIO          = 0.25f,
                                 const float           EMIT_RATE_PER_SEC              = 200.0f,
                                 const float           DURATION_SEC                   = 3.0f,
                                 const float           SPEED_BASE                     = 4.0f,
                                 const float           SPEED_VARIATION_RATIO          = 0.25f,
                                 const float           ROTATION_SPEED_BASE            = 20.0f,
                                 const float           ROTATION_SPEED_VARIATION_RATIO = 0.5f);

        inline bool IsFinished() const { return isFinished_; }
        void Update(const float ELAPSED_TIME_SEC);
        void draw(sf::RenderTarget & target, sf::RenderStates states) const;

    private:
        float ValueWithRandomVariance(const float BASE, const float VARIANCE_RATIO) const;

    private:
        const bool WILL_EMIT_RIGHT_;
        const sf::FloatRect REGION_;
        const float SPRAY_RATIO_MINOR_;
        const float SPRAY_RATIO_MAJOR_;
        const float SCALE_BASE_;
        const float SCALE_VAR_RATIO_;
        const float SEC_PER_EMIT_;
        const float DURATION_SEC_;
        const float SPEED_BASE_;
        const float SPEED_VAR_RATIO_;
        const float ROTATION_SPEED_BASE_;
        const float ROTATION_SPEED_VAR_RATIO_;
        float emitTimerSec_;
        float durationTimerSec_;
        bool isFinished_;
        TextureSPtr_t sparkTextureSPtr_;
        SparkVec_t sparkVec_;
    };

    using SparksAnimationUPtr_t = std::unique_ptr<SparksAnimation>;


}
}

#endif //SFMLUTIL_SPARKSANIMATION_HPP_INCLUDED