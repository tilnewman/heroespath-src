// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MAP_INTERACTION_TEXT_ENUM_HPP_INCLUDED
#define HEROESPATH_MAP_INTERACTION_TEXT_ENUM_HPP_INCLUDED
//
// interaction-text-enum.hpp
//
#include "gui/font-enum.hpp"
#include "misc/enum-common.hpp"
#include "misc/not-null.hpp"

#include <string>

namespace sf
{
class Font;
}

namespace heroespath
{
namespace interact
{

    struct Text : public EnumBaseCounting<EnumFirstValue::Valid>
    {
        enum Enum : EnumUnderlying_t
        {
            System = 0,
            Dialog,
            Count
        };

        static const std::string ToString(const Enum);
        static gui::GuiFont::Enum Font(const Enum);
    };

} // namespace interact
} // namespace heroespath

#endif // HEROESPATH_MAP_INTERACTION_TEXT_ENUM_HPP_INCLUDED
