// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_ANIMATIONBASE_HPP_INCLUDED
#define HEROESPATH_GUI_ANIMATIONBASE_HPP_INCLUDED
//
// animation-base.hpp
//  A class responsible for common state and operation of all animation objects.
//
#include "gui/animation-enum.hpp"
#include "gui/entity.hpp"

#include <SFML/Graphics/Sprite.hpp>

#include <memory>
#include <vector>

namespace heroespath
{
namespace gui
{

    // A base class for both types of animations.
    class Animation : public Entity
    {
    public:
        Animation(
            const Animations::Enum ENUM,
            const sf::FloatRect & REGION,
            const float TIME_PER_FRAME_SEC,
            const sf::BlendMode & BLEND_MODE,
            const sf::Color & COLOR_FROM,
            const sf::Color & COLOR_TO);

        virtual ~Animation();

        Animation(const Animation &) = delete;
        Animation(Animation &&) = delete;
        Animation & operator=(const Animation &) = delete;
        Animation & operator=(Animation &&) = delete;

        void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

        void SetEntityPos(const sf::Vector2f & V) override { SetEntityPos(V.x, V.y); }
        void SetEntityPos(const float LEFT, const float TOP) override;
        void SetEntityRegion(const sf::FloatRect & R) override;
        void MoveEntityPos(const float HORIZ, const float VERT) override;

        // returns true if frame count wrapped around back to zero
        bool UpdateTime(const float SECONDS) override;

        const sf::Vector2f OrigSize() const { return origSizeV_; }

        virtual std::size_t FrameCount() const = 0;

        std::size_t CurrentFrame() const { return frameIndex_; }

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

    private:
        virtual const sf::Texture & GetTexture(const std::size_t FRAME_INDEX) const = 0;
        virtual const sf::IntRect GetTextureRect(const std::size_t FRAME_INDEX) const = 0;
        void SetupSprite();

        Animations::Enum which_;
        sf::BlendMode blendMode_;
        float timePerFrameSec_;
        std::size_t frameIndex_;
        float frameTimerSec_;
        sf::Color colorFrom_;
        sf::Color colorTo_;
        sf::Sprite sprite_;
        sf::Vector2f origSizeV_;
        bool isFinished_;
        bool hasAlreadyUpdated_;
    };

    using AnimationUPtr_t = std::unique_ptr<Animation>;
    using AnimationUVec_t = std::vector<AnimationUPtr_t>;

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_ANIMATIONBASE_HPP_INCLUDED
