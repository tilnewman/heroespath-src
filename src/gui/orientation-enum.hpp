// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_ORIENTATION_ENUM_HPP_INCLUDED
#define HEROESPATH_GUI_ORIENTATION_ENUM_HPP_INCLUDED
//
// orientation-enum.hpp
//
#include "misc/enum-util.hpp"

namespace heroespath
{
namespace gui
{

    struct Orientation : public misc::EnumBaseCounting<Orientation, misc::EnumFirstValueValid>
    {
        enum Enum : misc::EnumUnderlying_t
        {
            Horiz = 0,
            Vert,
            Both,
            Count
        };

        static const std::string ToString(const Orientation::Enum);

        // if given Both and IS_OPPOSITE_OF_BOTH_COUNT==false then returns Both
        static Enum Flip(const Enum ORIENTATION, const bool IS_OPPOSITE_OF_BOTH_COUNT = false)
        {
            if (ORIENTATION == Horiz)
            {
                return Vert;
            }
            else if (ORIENTATION == Vert)
            {
                return Horiz;
            }
            else if (ORIENTATION == Both)
            {
                if (IS_OPPOSITE_OF_BOTH_COUNT)
                {
                    return Count;
                }
                else
                {
                    return Both;
                }
            }
            else
            {
                return Count;
            }
        }
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_ORIENTATION_ENUM_HPP_INCLUDED
