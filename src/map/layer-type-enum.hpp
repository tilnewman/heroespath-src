// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MAP_LAYERTYPEENUM_HPP_INCLUDED
#define HEROESPATH_MAP_LAYERTYPEENUM_HPP_INCLUDED
//
// layer-type-enum.hpp
//
#include <string>

namespace heroespath
{
namespace map
{

    struct LayerType
    {
        enum Enum
        {
            Ground = 0,
            Object,
            Shadow,
            Count
        };

        static const std::string ToString(const LayerType::Enum);
    };
} // namespace map
} // namespace heroespath

#endif // HEROESPATH_MAP_LAYERTYPEENUM_HPP_INCLUDED
