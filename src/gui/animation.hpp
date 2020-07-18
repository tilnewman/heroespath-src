// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_ANIMATION_HPP_INCLUDED
#define HEROESPATH_GUI_ANIMATION_HPP_INCLUDED
//
// animation.hpp
//
#include "gui/animation-enum.hpp"
#include "gui/cached-texture.hpp"
#include "gui/entity.hpp"

#include <SFML/Graphics/Sprite.hpp>

#include <memory>
#include <vector>

namespace heroespath
{
namespace gui
{
    // Responsible for loading animation image files (basically simple sprite sheets) and displaying
    // them in timely order to create animations.
    class Animation : public Entity
    {
        using RectsVec_t = std::vector<sf::IntRect>;
        using IndexRectPair_t = std::pair<std::size_t, sf::IntRect>;
        using IndexRectVec_t = std::vector<IndexRectPair_t>;

    public:
        Animation(
            const Animations::Enum ENUM,
            const sf::FloatRect & REGION,
            const float TIME_PER_FRAME_SEC,
            const sf::BlendMode & BLEND_MODE,
            const sf::Color & COLOR_FROM,
            const sf::Color & COLOR_TO);

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
        std::size_t FrameCount() const { return frameIndexRects_.size(); }
        std::size_t CurrentFrame() const { return frameIndex_; }
        float TimePerFrame() const { return timePerFrameSec_; }
        void TimePerFrameSet(const float TBF) { timePerFrameSec_ = TBF; }
        void TimePerFrameAdj(const float ADJ) { timePerFrameSec_ += ADJ; }
        bool IsFinished() const { return isFinished_; }
        Animations::Enum Which() const { return which_; }

        void ColorTransition(const sf::Color & FROM, const sf::Color & TO);
        void RandomVaryTimePerFrame();

    private:
        const IndexRectVec_t MakeFrameIndexRects(
            const gui::CachedTextures &,
            const sf::Vector2i & FRAME_SIZE_V,
            const sf::IntRect & TRIM) const;

        const RectsVec_t FrameRectsInTextureOfSize(
            const sf::Vector2i & TEXTURE_SIZE_V, const sf::Vector2i & FRAME_SIZE_V) const;

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
        gui::CachedTextures cachedTextures_;
        IndexRectVec_t frameIndexRects_;
    };

    using AnimationUPtr_t = std::unique_ptr<Animation>;
    using AnimationUVec_t = std::vector<AnimationUPtr_t>;
} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_ANIMATION_HPP_INCLUDED