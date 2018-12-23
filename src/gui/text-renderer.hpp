// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_TEXT_RENDERER_HPP_INCLUDED
#define HEROESPATH_GUI_TEXT_RENDERER_HPP_INCLUDED
//
// text-renderer.hpp
//
#include "gui/text-info.hpp"

#include <SFML/Graphics/Rect.hpp>

#include <memory>

namespace sf
{
class RenderTexture;
class Sprite;
} // namespace sf

namespace heroespath
{
namespace gui
{
    namespace text_rendering
    {
        class RenderedLines;
    }

    using RenderTextureUPtr_t = std::unique_ptr<sf::RenderTexture>;

    // Responsible for rendering/drawing TextInfo objects to sfml RenderTextures or Sprites
    struct TextRenderer
    {
        TextRenderer() = delete;

        // Returns all zeros on failure with RenderTextureUPtr_t reset() to null otherwise width
        // and height are both > zero and RenderTextureUPtr_t is not null.  This function cannot
        // create a Sliderbar, so REGION.height is always ignored and the returned height might be
        // greater than BOUNDING_REGION.height.  If REGION.width <= 0.0f then it is ignored.  The
        // RenderTexture will be re-used if it is big enough, so the returned size is the actual
        // minimally enclosing size.
        static const sf::FloatRect ToTexture(
            const TextInfo &, const sf::FloatRect & BOUNDING_REGION, RenderTextureUPtr_t &);

        // same as ToTexture() above only this function will setup the sprite on success with global
        // bounds equal to what ToTexture() returns
        static bool ToSprite(
            const TextInfo &,
            const sf::FloatRect & BOUNDING_REGION,
            RenderTextureUPtr_t &,
            sf::Sprite &);

    private:
        // does not destroy/re-create the RenderTexture if it is already big enough
        static bool DrawRenderedLinesOffScreen(
            const text_rendering::RenderedLines &, RenderTextureUPtr_t &);
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_TEXT_RENDERER_HPP_INCLUDED
