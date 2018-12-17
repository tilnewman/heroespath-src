// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_HORIZSYMBOL_HPP_INCLUDED
#define HEROESPATH_GUI_HORIZSYMBOL_HPP_INCLUDED
//
// horiz-symbol.hpp
//
#include "gui/cached-texture.hpp"
#include "misc/not-null.hpp"

#include <SFML/Graphics/Sprite.hpp>

namespace heroespath
{
namespace gui
{

    // Responsible for drawing Angela's faded curvy rune-like symbol that spans (horizontal) the
    // bottom of the screen of Stages.
    class BottomSymbol : public sf::Drawable
    {
    public:
        explicit BottomSymbol(
            const bool WILL_INVERT_COLOR_TO_WHITE = true,
            const float VERT_RESCALE = 1.0f,
            const sf::Color & COLOR = DEFAULT_COLOR_,
            const float HEIGHT_SCREEN_RATIO = DEFAULT_HEIGHT_SCREEN_RATIO_);

        BottomSymbol(const BottomSymbol &) = default;
        BottomSymbol(BottomSymbol &&) = default;
        BottomSymbol & operator=(const BottomSymbol &) = default;
        BottomSymbol & operator=(BottomSymbol &&) = default;

        void Setup(
            const bool WILL_INVERT_COLOR_TO_WHITE = true,
            const float VERT_RESCALE = 1.0f,
            const sf::Color & COLOR = DEFAULT_COLOR_,
            const float HEIGHT_SCREEN_RATIO = DEFAULT_HEIGHT_SCREEN_RATIO_);

        void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

        // Bottom(Region()) is always the bottom of the screen.
        const sf::FloatRect Region() const { return region_; }

        // this horizontal symbol image has tendrils that reach up higher than those that reach
        // below, making the true vertical center look too high.  This corrects for that, and
        // returns the vertical position that LOOKS like the center.
        float VisibleVerticalCenter() const;

        const sf::Color Color() const;

        // if COLOR==Transparent then the default color is used
        void Color(const sf::Color & NEW_COLOR);

        void SetPos(const float POS_LEFT, const float POS_TOP);
        void MovePos(const float HORIZ, const float VERT);

    private:
        gui::ImageOpt::Enum MakeImageOpt(const bool WILL_INVERT_COLOR_TO_WHITE) const;

    public:
        static const float DEFAULT_HEIGHT_SCREEN_RATIO_;
        static const float VISIBLE_VERT_POS_RATIO_;
        static const sf::Color DEFAULT_COLOR_;

    private:
        const std::string IMAGE_CONFIG_FILE_KEY_;
        CachedTexture cachedTexture_;
        std::vector<sf::Sprite> sprites_;
        sf::FloatRect region_;
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_HORIZSYMBOL_HPP_INCLUDED
