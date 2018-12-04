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
#include "misc/log-macros.hpp"

namespace heroespath
{
namespace gui
{

    // An animation that gets its frames from individual images.
    class AnimationMultiTexture : public Animation
    {
    public:
        AnimationMultiTexture(
            const Animations::Enum ENUM,
            const sf::FloatRect & REGION,
            const float TIME_PER_FRAME_SEC,
            const sf::BlendMode & BLEND_MODE,
            const sf::Color & COLOR_FROM,
            const sf::Color & COLOR_TO)
            : Animation(ENUM, REGION, TIME_PER_FRAME_SEC, BLEND_MODE, COLOR_FROM, COLOR_TO)
            , cachedTextures_(Animations::MediaPathKey(ENUM))
        {
            if (cachedTextures_.Empty())
            {
                M_HP_LOG_ERR(
                    "Failed to find/load any images." + M_HP_VAR_STR(Animations::ToString(ENUM))
                    + M_HP_VAR_STR(REGION) + M_HP_VAR_STR(TIME_PER_FRAME_SEC)
                    + M_HP_VAR_STR(COLOR_FROM) + M_HP_VAR_STR(COLOR_TO));
            }
        }

        virtual ~AnimationMultiTexture() = default;

        std::size_t FrameCount() const final { return cachedTextures_.Size(); }

    private:
        const sf::Texture & GetTexture(const std::size_t FRAME_INDEX) const final
        {
            if (cachedTextures_.Empty())
            {
                return sf::Texture();
            }
            else if (FRAME_INDEX < cachedTextures_.Size())
            {
                return cachedTextures_[FRAME_INDEX];
            }
            else
            {
                return cachedTextures_.At(0);
            }
        }

        const sf::IntRect GetTextureRect(const std::size_t FRAME_INDEX) const final
        {
            return sf::IntRect(sf::Vector2i(), sf::Vector2i { GetTexture(FRAME_INDEX).getSize() });
        }

        gui::CachedTextures cachedTextures_;
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_ANIMATIONMULTI_HPP_INCLUDED
