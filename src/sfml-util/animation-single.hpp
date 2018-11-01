// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_ANIMATIONSINGLE_HPP_INCLUDED
#define HEROESPATH_GUI_ANIMATIONSINGLE_HPP_INCLUDED
//
// animation-single.hpp
//
#include "sfml-util/animation-base.hpp"
#include "sfml-util/cached-texture.hpp"

#include <SFML/Graphics/Sprite.hpp>

#include <vector>

namespace heroespath
{
namespace gui
{

    // An animation that is sourced from a single texture
    class AnimationSingleTexture : public Animation
    {
    public:
        AnimationSingleTexture(const AnimationSingleTexture &) = delete;
        AnimationSingleTexture(AnimationSingleTexture &&) = delete;
        AnimationSingleTexture & operator=(const AnimationSingleTexture &) = delete;
        AnimationSingleTexture & operator=(AnimationSingleTexture &&) = delete;

        AnimationSingleTexture(
            const Animations::Enum ENUM,
            const sf::FloatRect & REGION,
            const float TIME_PER_FRAME_SEC,
            const sf::BlendMode & BLEND_MODE,
            const sf::Color & COLOR_FROM,
            const sf::Color & COLOR_TO);

        virtual ~AnimationSingleTexture();

        void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

        void SetEntityPos(const float LEFT, const float TOP) override;
        void SetEntityRegion(const sf::FloatRect & R) override;
        void MoveEntityPos(const float HORIZ, const float VERT) override;

        std::size_t FrameCount() const override { return rects_.size(); }

        const sf::Vector2f OrigSize() const override { return origSizeV_; }

        // returns true if frame count wrapped around back to zero
        bool UpdateTime(const float SECONDS) override;

    protected:
        CachedTexture cachedTexture_;
        sf::Sprite sprite_;
        sf::Vector2f origSizeV_;
        std::vector<sf::IntRect> rects_; // rects_.size() == frame count
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_ANIMATIONSINGLE_HPP_INCLUDED
