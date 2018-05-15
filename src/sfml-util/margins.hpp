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
//  Margins code
//

namespace heroespath
{
namespace sfml_util
{

    struct Margins
    {
        explicit Margins(
            const float LEFT = 0.0f,
            const float RIGHT = 0.0f,
            const float TOP = 0.0f,
            const float BOTTOM = 0.0f)
            : left(LEFT)
            , right(RIGHT)
            , top(TOP)
            , bottom(BOTTOM)
        {}

        virtual ~Margins() = default;

        float left;
        float right;
        float top;
        float bottom;
    };
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_GAME_MARGINS_HPP_INCLUDED
