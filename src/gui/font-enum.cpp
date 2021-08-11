// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// font-enum.cpp
//
#include "font-enum.hpp"

#include "misc/log-macros.hpp"

namespace heroespath
{
namespace gui
{

    const std::string GuiFont::ToString(const GuiFont::Enum FONT)
    {
        switch (FONT)
        {
            case Default:
            {
                return "Default";
            }
            case DefaultBoldFlavor:
            {
                return "DefaultBoldFlavor";
            }
            case System:
            {
                return "System";
            }
            case SystemCondensed:
            {
                return "SystemCondensed";
            }
            case SignThinTallNarrow:
            {
                return "SignThinTallNarrow";
            }
            case SignBoldShortWide:
            {
                return "SignBoldShortWide";
            }
            case Handwriting:
            {
                return "Handwriting";
            }
            case DialogModern:
            {
                return "DialogModern";
            }
            case DialogMedieval:
            {
                return "DialogMedieval";
            }
            case Count:
            {
                return "(Count)";
            }
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(FONT) << " is invalid. (count="
                                  << static_cast<EnumUnderlying_t>(Count) << ")");

                return "";
            }
        }
    }

    const std::string GuiFont::Name(const GuiFont::Enum FONT)
    {
        switch (FONT)
        {
            case Default:
            {
                return "Euler";
            }
            case DefaultBoldFlavor:
            {
                return "Modern-Antiqua";
            }
            case System:
            {
                return "Gentium-Plus";
            }
            case SystemCondensed:
            {
                return "Goudy-Bookletter";
            }
            case SignThinTallNarrow:
            {
                return "Quill-Sword";
            }
            case SignBoldShortWide:
            {
                return "Queen-Country";
            }
            case Handwriting:
            {
                return "Valley-Forge";
            }
            case DialogModern:
            {
                return "Square-Antiqua";
            }
            case DialogMedieval:
            {
                return "Mops-Antiqua";
            }
            case Count:
            {
                return "(Count)";
            }
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(FONT) << " is invalid. (count="
                                  << static_cast<EnumUnderlying_t>(Count) << ")");

                return "";
            }
        }
    }

    const std::string GuiFont::Path(const GuiFont::Enum FONT)
    {
        switch (FONT)
        {
            case Default:
            {
                return "euler/euler.otf";
            }
            case DefaultBoldFlavor:
            {
                return "modern-antiqua/modern-antiqua.ttf";
            }
            case System:
            {
                return "gentium-plus/gentium-plus.ttf";
            }
            case SystemCondensed:
            {
                return "goudy-bookletter/goudy-bookletter.otf";
            }
            case SignThinTallNarrow:
            {
                return "quill-sword/quill-sword.ttf";
            }
            case SignBoldShortWide:
            {
                return "queen-country/queen-country.ttf";
            }
            case Handwriting:
            {
                return "valley-forge/valley-forge.ttf";
            }
            case DialogModern:
            {
                return "square-antiqua/square-antiqua.ttf";
            }
            case DialogMedieval:
            {
                return "mops-antiqua/mops-antiqua.ttf";
            }
            case Count:
            {
                return "(Count)";
            }
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(FONT) << " is invalid. (count="
                                  << static_cast<EnumUnderlying_t>(Count) << ")");

                return "";
            }
        }
    }

} // namespace gui
} // namespace heroespath
