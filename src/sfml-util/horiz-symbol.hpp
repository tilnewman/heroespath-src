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
#include "misc/not-null.hpp"
#include "sfml-util/cached-texture.hpp"

#include <SFML/Graphics/Sprite.hpp>

namespace heroespath
{
namespace gui
{

    // Responsible for drawing the horizontal symbols at the bottom of Stages. Bottom(Region()) is
    // always the bottom of the screen.
    class BottomSymbol : public sf::Drawable
    {
    public:
        explicit BottomSymbol(
            const float VERT_SCALE = 1.0f,
            const bool WILL_INVERT_COLOR = false,
            const sf::Color & COLOR = DEFAULT_COLOR_);

        BottomSymbol(const BottomSymbol &) = default;
        BottomSymbol(BottomSymbol &&) = default;
        BottomSymbol & operator=(const BottomSymbol &) = default;
        BottomSymbol & operator=(BottomSymbol &&) = default;

        void Setup(
            const float VERT_SCALE = 1.0f,
            const bool WILL_INVERT_COLOR = false,
            const sf::Color & COLOR = DEFAULT_COLOR_);

        void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

        const sf::FloatRect Region() const { return region_; }

        // this horizontal symbol image has tendrils that reach up higher than those that reach
        // below, making the true vertical center look too high.  This corrects for that.
        float VisibleVerticalCenter() const { return (region_.top + (region_.height * 0.575f)); }

        const sf::Color Color() const;

        void Color(const sf::Color & NEW_COLOR);

        void SetPos(const float POS_LEFT, const float POS_TOP);
        void MovePos(const float HORIZ, const float VERT);

    public:
        static const sf::Color DEFAULT_COLOR_;
        static const std::string IMAGE_PATH_KEY_;

    private:
        gui::ImageOpt::Enum MakeImageOpt(const bool WILL_INVERT_COLOR) const;

        CachedTexture cachedTexture_;
        std::vector<sf::Sprite> sprites_;
        sf::FloatRect region_;
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_HORIZSYMBOL_HPP_INCLUDED
