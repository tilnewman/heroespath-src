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
#include "sfml-util/gui/gui-entity.hpp"
#include "sfml-util/sfml-graphics.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{

    class Animation : public gui::GuiEntity
    {
    public:
        Animation(const Animation &) = delete;
        Animation(Animation &&) = delete;
        Animation & operator=(const Animation &) = delete;
        Animation & operator=(Animation &&) = delete;

    public:
        Animation(
            const Animations::Enum ENUM,
            const sf::FloatRect & REGION,
            const float TIME_PER_FRAME_SEC,
            const sf::BlendMode & BLEND_MODE,
            const sf::Color & COLOR_FROM,
            const sf::Color & COLOR_TO);

        virtual ~Animation();

        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const = 0;

        virtual void SetEntityPos(const sf::Vector2f & V) = 0;
        virtual void SetEntityPos(const float LEFT, const float TOP) = 0;

        virtual void SetEntityRegion(const sf::FloatRect & R) = 0;

        // returns true if frame count wrapped around back to zero
        virtual bool UpdateTime(const float SECONDS) = 0;

        virtual std::size_t FrameCount() const = 0;

        virtual void MoveEntityPos(const float HORIZ, const float VERT) = 0;

        virtual const sf::Vector2f OrigSize() const = 0;

        virtual const sf::Sprite Sprite() const = 0;

        virtual void OnClick(const sf::Vector2f &) {}

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
