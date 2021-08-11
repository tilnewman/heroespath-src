// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_MENU_IMAGE_ENUM_HPP_INCLUDE
#define HEROESPATH_GUI_MENU_IMAGE_ENUM_HPP_INCLUDE
//
// menu-image-enum.hpp
//
#include "misc/enum-common.hpp"

#include <SFML/Config.hpp>

#include <string>

namespace heroespath
{
namespace gui
{

    struct MenuImage : public EnumBaseCounting<EnumFirstValue::Valid>
    {
        enum Enum : EnumUnderlying_t
        {
            Back = 0,
            New,
            StartGame,
            Delete,
            Save,
            Help,
            Next,
            CreateCharacters,
            Exit,
            Credits,
            Settings,
            Resume,
            CreateParty,
            Inventory,
            Treasure,
            Title,
            SymbolTop,
            SymbolBottom,
            Count
        };

        static const std::string ToString(const Enum);
        static bool IsTitleOnly(const Enum);
        static const std::string ConfigFileKey(const Enum, const bool IS_LIT = false);
        static sf::Uint8 SymbolAlpha() { return 24; }
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_MENU_IMAGE_ENUM_HPP_INCLUDE
