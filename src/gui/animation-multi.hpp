// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_ANIMATIONMULTI_HPP_INCLUDED
#define HEROESPATH_GUI_ANIMATIONMULTI_HPP_INCLUDED
//
// animation-multi.hpp
//  An animation class sourced from multiples textures.
//
#include "gui/animation-base.hpp"
#include "gui/cached-texture.hpp"

#include <SFML/Graphics/Sprite.hpp>

namespace heroespath
{
namespace gui
{

    // An animation class sourced from multiple textures.
    class AnimationMultiTexture : public Animation
    {
    public:
        AnimationMultiTexture(const AnimationMultiTexture &) = delete;
        AnimationMultiTexture(AnimationMultiTexture &&) = delete;
        AnimationMultiTexture & operator=(const AnimationMultiTexture &) = delete;
        AnimationMultiTexture & operator=(AnimationMultiTexture &&) = delete;

        AnimationMultiTexture(
            const Animations::Enum ENUM,
            const sf::FloatRect & REGION,
            const float TIME_PER_FRAME_SEC,
            const sf::BlendMode & BLEND_MODE,
            const sf::Color & COLOR_FROM,
            const sf::Color & COLOR_TO);

        virtual ~AnimationMultiTexture();

        void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

        void SetEntityPos(const float LEFT, const float TOP) override;
        void SetEntityRegion(const sf::FloatRect & R) override;
        void MoveEntityPos(const float HORIZ, const float VERT) override;

        // returns true if frame count wrapped around back to zero
        bool UpdateTime(const float SECONDS) override;

        std::size_t FrameCount() const override { return cachedTextures_.Size(); }

        const sf::Vector2f OrigSize() const override { return origSizeV_; }

    protected:
        gui::CachedTextures cachedTextures_;
        sf::Sprite sprite_;
        sf::Vector2f origSizeV_;
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_ANIMATIONMULTI_HPP_INCLUDED
