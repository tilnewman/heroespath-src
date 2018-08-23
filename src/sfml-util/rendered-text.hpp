// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_RENDERED_TEXT_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_RENDERED_TEXT_HPP_INCLUDED
//
// rendered-text.hpp
//
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"

#include <SFML/Graphics/Text.hpp>

#include <vector>

namespace heroespath
{
namespace sfml_util
{

    // used to represent a single line of text, possibly with multiple fonts
    using SfTextVec_t = std::vector<sf::Text>;

    // used to represent multiple lines of text, possibly with multiple fonts
    using SfTextVecVec_t = std::vector<SfTextVec_t>;

    // Responsible for wrapping the result of rendering one TextInfo object into multiple sf::Text
    // objects, possibly with multiple fonts.  A blank line, the space between two paragraphs for
    // example, is represented by a SfTextVec_t with one sf::Text object with string set to " ".
    struct RenderedText
    {
        RenderedText();
        RenderedText(const SfTextVecVec_t & SF_TEXT_VEC_VEC);

        RenderedText(const RenderedText &) = default;
        RenderedText(RenderedText &&) = default;
        RenderedText & operator=(const RenderedText &) = default;
        RenderedText & operator=(RenderedText &&) = default;

        void Clear();
        void SyncRegion();
        void AppendLines(const RenderedText & RENDERED_TEXT_TO_APPEND);
        void SetColor(const sf::Color & NEW_COLOR);
        void SetPos(const float LEFT, const float TOP);
        void MovePos(const float HORIZ, const float VERT);

        // Initially had a real problem with text rendered to sf::RenderTextures looking fuzzy and
        // wrong.  I solved that problem by making all offscreen surfaces (sf::RenderTextures) have
        // dimmensions/sizes that were powers of two.
        //
        // Now it is 2018-8 and I've come across this comment but the code that ensures that a
        // length/size is aligned to a power of two has long been removed from the codebase and yet
        // the text looks fine.  I don't understand what happened or why it is working, but I'm
        // going to leave this comment here in the code in case the problem returns.
        //
        //  Wait...This might have been solved by setting the BlendMode to None...
        void CreateTextureAndRenderOffscreen(sf::RenderTexture &) const;

        SfTextVecVec_t text_vecs;
        std::vector<sf::FloatRect> regions;
        sf::FloatRect region;
    };

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_RENDERED_TEXT_HPP_INCLUDED
