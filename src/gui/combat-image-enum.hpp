// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_COMBATIMAGEENUM_HPP_INCLUDED
#define HEROESPATH_GUI_COMBATIMAGEENUM_HPP_INCLUDED
//
// combat-image-enum.hpp
//
#include <string>

namespace heroespath
{
namespace gui
{

    struct CombatImageType
    {
        enum Enum
        {
            Wing = 0,
            Arrow1,
            Arrow2,
            Arrow3,
            Bolt,
            Dart,
            Stone1,
            Stone2,
            Stone3,
            Stone4,
            Count
        };

        static const std::string ToString(const Enum);
        static const std::string ImageFilename(const Enum);
    };
} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_COMBATIMAGEENUM_HPP_INCLUDED
