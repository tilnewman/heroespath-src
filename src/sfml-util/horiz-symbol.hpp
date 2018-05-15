// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_HORIZSYMBOL_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_HORIZSYMBOL_HPP_INCLUDED
//
// horiz-symbol.hpp
//  A symbol drawn at the bottom of menu screens
//
#include "sfml-util/sfml-graphics.hpp"

namespace heroespath
{
namespace sfml_util
{

    // Responsible for drawing the horizontal symbols at the bottom of Stages.
    // A positive VERT_OFFSET_RATIO will move the symbols down, negative moves them up.
    // The Bottom() of the symbol is always the bottom of the screen, even if
    // VERT_OFFSET_RATIO is > 0 and some of the symbol is offscreen.
    class BottomSymbol : public sf::Drawable
    {
    public:
        BottomSymbol(const BottomSymbol &) = delete;
        BottomSymbol(BottomSymbol &&) = delete;
        BottomSymbol & operator=(const BottomSymbol &) = delete;
        BottomSymbol & operator=(BottomSymbol &&) = delete;

    public:
        explicit BottomSymbol(
            const float VERT_SCALE = 1.0f,
            const bool WILL_INVERT_COLOR = false,
            const sf::Color & COLOR = DEFAULT_COLOR_,
            const float VERT_OFFSET_RATIO = 0.0f);

        virtual ~BottomSymbol();

        void Setup(
            const float VERT_SCALE = 1.0f,
            const bool WILL_INVERT_COLOR = false,
            const sf::Color & COLOR = DEFAULT_COLOR_,
            const float VERT_OFFSET_RATIO = 0.0f);

        void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

        float Height() const { return Bottom() - Top(); }
        float Top() const { return sprite1_.getGlobalBounds().top; }
        float Middle() const { return Top() + (Height() * 0.5f); }
        float Bottom() const;

    public:
        static const sf::Color DEFAULT_COLOR_;

    private:
        sf::Sprite sprite1_;
        sf::Sprite sprite2_;
        sf::Sprite sprite3_;
        sf::Sprite sprite4_;
        sf::Texture texture_;
    };
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_HORIZSYMBOL_HPP_INCLUDED
