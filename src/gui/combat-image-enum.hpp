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
#include "misc/enum-common.hpp"

#include <string>

namespace heroespath
{
namespace gui
{

    struct CombatImageType : public EnumBaseCounting<EnumFirstValue::Valid>
    {
        enum Enum : EnumUnderlying_t
        {
            Wing = 0,
            Arrow,
            Bolt,
            Dart,
            Stone,
            Note,
            Spark,
            Run,
            CrossSwords,
            CrossBones,
            Count
        };

        static const std::string ToString(const Enum);
        static std::size_t ImageCount(const Enum);
        static const std::string RandomConfigFileKey(const Enum);
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_COMBATIMAGEENUM_HPP_INCLUDED
