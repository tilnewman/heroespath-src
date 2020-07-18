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
#include "misc/strings.hpp"

#include <SFML/Config.hpp>

#include <string>

namespace heroespath
{
namespace gui
{

    struct MenuImage : public EnumBaseCounting<>
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

        static constexpr bool IsTitleOnly(const Enum ENUM) noexcept
        {
            switch (ENUM)
            {
                case Back:
                case New:
                case StartGame:
                case Delete:
                case Save:
                case Help:
                case Next:
                case Exit:
                case Credits:
                case Settings:
                case Resume:
                case CreateParty: return false;

                case CreateCharacters:
                case Inventory:
                case Treasure:
                case Title:
                case SymbolTop:
                case SymbolBottom:
                case Count:
                default: return true;
            }
        }

        static inline const std::string ConfigFileKey(const Enum ENUM, const bool IS_LIT = false)
        {
            std::string name;
            name.reserve(64);
            name += "media-image-misc-menu-";
            name += misc::ToLowerCopy(NAMEOF_ENUM(ENUM));

            if (!IsTitleOnly(ENUM))
            {
                if (IS_LIT)
                {
                    name += "-lit";
                }
                else
                {
                    name += "-normal";
                }
            }

            return name;
        }

        static constexpr sf::Uint8 SymbolAlpha() noexcept { return 24; }
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_MENU_IMAGE_ENUM_HPP_INCLUDE
