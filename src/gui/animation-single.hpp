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
#include "gui/animation-base.hpp"
#include "gui/cached-texture.hpp"
#include "misc/log-macros.hpp"

#include <vector>

namespace heroespath
{
namespace gui
{

    // An animation that gets its frames from a single image
    class AnimationSingleTexture : public Animation
    {
    public:
        AnimationSingleTexture(
            const Animations::Enum ENUM,
            const sf::FloatRect & REGION,
            const float TIME_PER_FRAME_SEC,
            const sf::BlendMode & BLEND_MODE,
            const sf::Color & COLOR_FROM,
            const sf::Color & COLOR_TO)
            : Animation(ENUM, REGION, TIME_PER_FRAME_SEC, BLEND_MODE, COLOR_FROM, COLOR_TO)
            , cachedTexture_(gui::Animations::MediaPathKey(ENUM))
            , frameRects_()
        {
            FindAllFrames(ENUM);
        }

        virtual ~AnimationSingleTexture() = default;

        std::size_t FrameCount() const final { return frameRects_.size(); }

    private:
        void FindAllFrames(const Animations::Enum ENUM)
        {
            const sf::Vector2i TEXTURE_SIZE_V(cachedTexture_.Get().getSize());
            const sf::Vector2i FRAME_SIZE_V(gui::Animations::ImageSize(ENUM));

            for (int vert(0); vert < TEXTURE_SIZE_V.y; vert += FRAME_SIZE_V.y)
            {
                for (int horiz(0); horiz < TEXTURE_SIZE_V.x; horiz += FRAME_SIZE_V.x)
                {
                    frameRects_.emplace_back(sf::IntRect(sf::Vector2i(horiz, vert), FRAME_SIZE_V));
                }
            }

            const auto RECT_COUNT_EXPECTED { static_cast<std::size_t>(
                (TEXTURE_SIZE_V.x / FRAME_SIZE_V.x) * (TEXTURE_SIZE_V.y / FRAME_SIZE_V.y)) };

            if ((RECT_COUNT_EXPECTED != frameRects_.size()) || frameRects_.empty())
            {
                M_HP_LOG_ERR(
                    "Failed to create the expected number "
                    "of frames or any frames at all."
                    + M_HP_VAR_STR(Animations::ToString(ENUM)) + M_HP_VAR_STR(frameRects_.size())
                    + M_HP_VAR_STR(RECT_COUNT_EXPECTED) + M_HP_VAR_STR(TEXTURE_SIZE_V)
                    + M_HP_VAR_STR(FRAME_SIZE_V));
            }
        }

        const sf::Texture & GetTexture(const std::size_t) const final
        {
            return cachedTexture_.Get();
        }

        const sf::IntRect GetTextureRect(const std::size_t FRAME_INDEX) const final
        {
            if (frameRects_.empty())
            {
                return sf::IntRect();
            }
            else if (FRAME_INDEX < frameRects_.size())
            {
                return frameRects_[FRAME_INDEX];
            }
            else
            {
                return frameRects_.front();
            }
        }

        CachedTexture cachedTexture_;
        std::vector<sf::IntRect> frameRects_;
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_ANIMATIONSINGLE_HPP_INCLUDED
