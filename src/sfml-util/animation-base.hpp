// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_ANIMATIONBASE_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_ANIMATIONBASE_HPP_INCLUDED
//
// animation-base.hpp
//  A class responsible for common state and operation of all animation objects.
//
#include "sfml-util/animation-enum.hpp"
#include "sfml-util/gui/entity.hpp"

#include <SFML/Graphics/BlendMode.hpp>
#include <SFML/Graphics/Color.hpp>

#include <memory>
#include <vector>

namespace heroespath
{
namespace sfml_util
{

    // Responsible for state and operations common to all animations in the game that work by
    // showing a changing image at a given frame rate.
    class Animation : public Entity
    {
    public:
        Animation(const Animation &) = delete;
        Animation(Animation &&) = delete;
        Animation & operator=(const Animation &) = delete;
        Animation & operator=(Animation &&) = delete;

        Animation(
            const Animations::Enum ENUM,
            const sf::FloatRect & REGION,
            const float TIME_PER_FRAME_SEC,
            const sf::BlendMode & BLEND_MODE,
            const sf::Color & COLOR_FROM,
            const sf::Color & COLOR_TO);

        virtual ~Animation();

        virtual std::size_t FrameCount() const = 0;

        virtual const sf::Vector2f OrigSize() const = 0;

        std::size_t CurrentFrame() const { return currentFrame_; }

        float TimePerFrame() const { return timePerFrameSec_; }

        void TimePerFrameSet(const float TBF) { timePerFrameSec_ = TBF; }

        void TimePerFrameAdj(const float ADJ) { timePerFrameSec_ += ADJ; }

        void ColorTransition(const sf::Color & FROM, const sf::Color & TO)
        {
            colorFrom_ = FROM;
            colorTo_ = TO;
        }

        bool IsFinished() const { return isFinished_; }

        Animations::Enum Which() const { return which_; }

        void RandomVaryTimePerFrame();

    protected:
        Animations::Enum which_;
        sf::BlendMode blendMode_;
        float timePerFrameSec_;

        // counts up to total then restarts at zero
        std::size_t currentFrame_;

        float frameTimerSec_;
        sf::Color colorFrom_;
        sf::Color colorTo_;
        bool isFinished_;
    };

    using AnimationUPtr_t = std::unique_ptr<Animation>;
    using AnimationUVec_t = std::vector<AnimationUPtr_t>;

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_ANIMATIONBASE_HPP_INCLUDED
