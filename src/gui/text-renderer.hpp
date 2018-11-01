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

        // if returns true then then renderTextureUPtr and *renderTextureUPtr were not changed,
        // REGION.height is ignored/not-enforced, REGION.width is ignored/not-enforced if <=
        // 0.0f, the caller is responsible for checking if Result.region != REGION because
        // Justification can change position, the RenderTexture might have been re-used so its
        // size might not match Size(REGION) or Size(Result.region)
        static bool ToTexture(
            const TextInfo &,
            const sf::FloatRect & BOUNDING_REGION,
            RenderTextureUPtr_t &,
            sf::FloatRect & finalRegion);

        // same as Render() above only this version will setup the sprite on success
        static bool ToSprite(
            const TextInfo &,
            const sf::FloatRect & BOUNDING_REGION,
            RenderTextureUPtr_t &,
            sf::Sprite &);

    private:
        // does not destroy/re-create the RenderTexture if it is already big enough
        static void Draw(const text_rendering::RenderedLines &, sf::RenderTexture &);
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_TEXT_RENDERER_HPP_INCLUDED
