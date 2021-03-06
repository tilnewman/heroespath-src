// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GAME_MARGINS_HPP_INCLUDED
#define HEROESPATH_GAME_MARGINS_HPP_INCLUDED
//
// margins.hpp
//
#include <SFML/Graphics/Rect.hpp>

namespace heroespath
{
namespace gui
{

    // Responsible for holding the amounts of empty space that surround a region.
    struct Margins
    {
        explicit Margins(const float PAD = 0.0f)
            : left(PAD)
            , right(PAD)
            , top(PAD)
            , bottom(PAD)
        {}

        Margins(const float HORIZ, const float VERT)
            : left(HORIZ)
            , right(HORIZ)
            , top(VERT)
            , bottom(VERT)
        {}

        explicit Margins(const sf::Vector2f & MARGIN_V)
            : left(MARGIN_V.x)
            , right(MARGIN_V.x)
            , top(MARGIN_V.y)
            , bottom(MARGIN_V.y)
        {}

        Margins(const sf::Vector2f & LEFT_RIGHT_V, const sf::Vector2f & WIDTH_HEIGHT_V)
            : left(LEFT_RIGHT_V.x)
            , right(LEFT_RIGHT_V.y)
            , top(WIDTH_HEIGHT_V.x)
            , bottom(WIDTH_HEIGHT_V.y)
        {}

        Margins(const float LEFT, const float RIGHT, const float TOP, const float BOTTOM)
            : left(LEFT)
            , right(RIGHT)
            , top(TOP)
            , bottom(BOTTOM)
        {}

        Margins(const Margins &) = default;
        Margins(Margins &&) = default;
        Margins & operator=(const Margins &) = default;
        Margins & operator=(Margins &&) = default;

        void ApplyShrink(sf::FloatRect & rect, const bool WILL_PREVENT_NEGATIVE_SIZES = true) const
        {
            rect.left += left;
            rect.top += top;
            rect.width -= (left + right);
            rect.height -= (top + bottom);

            if (WILL_PREVENT_NEGATIVE_SIZES)
            {
                if (rect.width < 0.0f)
                {
                    rect.width = 0.0f;
                }

                if (rect.height < 0.0f)
                {
                    rect.height = 0.0f;
                }
            }
        }

        const sf::FloatRect ApplyShrinkCopy(
            const sf::FloatRect & RECT, const bool WILL_PREVENT_NEGATIVE_SIZES = true) const
        {
            sf::FloatRect newRect { RECT };
            ApplyShrink(newRect, WILL_PREVENT_NEGATIVE_SIZES);
            return newRect;
        }

        void ApplyGrow(sf::FloatRect & rect) const
        {
            rect.left -= left;
            rect.top -= top;
            rect.width += (left + right);
            rect.height += (top + bottom);
        }

        const sf::FloatRect ApplyGrowCopy(const sf::FloatRect & RECT) const
        {
            sf::FloatRect newRect { RECT };
            ApplyGrow(newRect);
            return newRect;
        }

        float left;
        float right;
        float top;
        float bottom;
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GAME_MARGINS_HPP_INCLUDED
