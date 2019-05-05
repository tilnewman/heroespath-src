// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// menu-image-enum.cpp
//
#include "menu-image-enum.hpp"

#include "misc/log-macros.hpp"
#include "misc/strings.hpp"

namespace heroespath
{
namespace gui
{

    const std::string MenuImage::ToString(const Enum ENUM)
    {
        switch (ENUM)
        {
            case Back: { return "Back";
            }
            case New: { return "New";
            }
            case StartGame: { return "Start-Game";
            }
            case Delete: { return "Delete";
            }
            case Save: { return "Save";
            }
            case Help: { return "Help";
            }
            case Next: { return "Next";
            }
            case CreateCharacters: { return "Create-Characters";
            }
            case Exit: { return "Exit";
            }
            case Credits: { return "Credits";
            }
            case Settings: { return "Settings";
            }
            case Resume: { return "Resume";
            }
            case CreateParty: { return "Create-Party";
            }
            case Inventory: { return "Inventory";
            }
            case Treasure: { return "Treasure";
            }
            case Title: { return "Title";
            }
            case SymbolTop: { return "Symbol-Top";
            }
            case SymbolBottom: { return "Symbol-Bottom";
            }
            case Count: { return "(Count)";
            }
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(ENUM) << " is invalid. (count="
                                  << static_cast<EnumUnderlying_t>(Count) << ")");

                return "";
            }
        }
    }

    bool MenuImage::IsTitleOnly(const Enum ENUM)
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
            case CreateParty: { return false;
            }
            case CreateCharacters:
            case Inventory:
            case Treasure:
            case Title:
            case SymbolTop:
            case SymbolBottom:
            case Count:
            default: { return true;
            }
        }
    }

    const std::string MenuImage::ConfigFileKey(const Enum ENUM, const bool IS_LIT)
    {
        std::string name;
        name.reserve(64);

        name += "media-image-misc-menu-";
        name += misc::ToLowerCopy(ToString(ENUM));

        if (IsTitleOnly(ENUM) == false)
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

} // namespace gui
} // namespace heroespath
