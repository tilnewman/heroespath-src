// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// popup-enums.cpp
//
#include "popup-enums.hpp"

#include "misc/enum-util.hpp"
#include "misc/log-macros.hpp"

namespace heroespath
{
namespace popup
{

    const std::string PopupStage::ToString(const PopupStage::Enum POPUP_STAGE)
    {
        switch (POPUP_STAGE)
        {
            case Generic:
            {
                return "Generic";
            }
            case CharacterSelect:
            {
                return "CharacterSelect";
            }
            case CombatOver:
            {
                return "CombatOver";
            }
            case ImageFade:
            {
                return "ImageFade";
            }
            case ImageSelect:
            {
                return "ImageSelect";
            }
            case InventoryPrompt:
            {
                return "InventoryPrompt";
            }
            case Musicsheet:
            {
                return "Musicsheet";
            }
            case NumberSelect:
            {
                return "NumberSelect";
            }
            case ResolutionChange:
            {
                return "ResolutionChange:";
            }
            case Spellbook:
            {
                return "Spellbook";
            }
            case SystemError:
            {
                return "SystemError";
            }
            case TreasureTrap:
            {
                return "TreasureTrap";
            }
            case Count:
            {
                return "(Count)";
            }
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(POPUP_STAGE)
                                  << " is invalid. (count=" << static_cast<EnumUnderlying_t>(Count)
                                  << ")");

                return "";
            }
        }
    }

    const std::string PopupButtons::ToString(const Enum ENUM, const EnumStringHow & HOW)
    {
        return EnumUtil<PopupButtons>::ToString(ENUM, HOW);
    }

    const std::string PopupButtons::ToStringPopulate(
        const EnumUnderlying_t ENUM_VALUE, const std::string & SEPARATOR)
    {
        std::string str;
        AppendNameIfBitIsSet(str, ENUM_VALUE, PopupButtons::None, "None", SEPARATOR);
        AppendNameIfBitIsSet(str, ENUM_VALUE, PopupButtons::Okay, "Okay", SEPARATOR);
        AppendNameIfBitIsSet(str, ENUM_VALUE, PopupButtons::Continue, "Continue", SEPARATOR);
        AppendNameIfBitIsSet(str, ENUM_VALUE, PopupButtons::Yes, "Yes", SEPARATOR);
        AppendNameIfBitIsSet(str, ENUM_VALUE, PopupButtons::No, "No", SEPARATOR);
        AppendNameIfBitIsSet(str, ENUM_VALUE, PopupButtons::Cancel, "Cancel", SEPARATOR);
        AppendNameIfBitIsSet(str, ENUM_VALUE, PopupButtons::Select, "Select", SEPARATOR);
        return str;
    }

    bool PopupButtons::IsAffirmative(const PopupButtons::Enum BUTTON)
    {
        return (BUTTON & (Okay | Continue | Yes | Select));
    }

    const std::string PopupButtonColor::ToString(const PopupButtonColor::Enum ENUM)
    {
        switch (ENUM)
        {
            case Light:
            {
                return "Light";
            }
            case Dark:
            {
                return "Dark";
            }
            case Count:
            {
                return "(Count)";
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

    const std::string PopupImage::ToString(const PopupImage::Enum ENUM)
    {
        switch (ENUM)
        {
            case Banner:
            {
                return "Banner";
            }
            case Regular:
            {
                return "Regular";
            }
            case RegularSidebar:
            {
                return "RegularSidebar";
            }
            case Large:
            {
                return "Large";
            }
            case LargeSidebar:
            {
                return "LargeSidebar";
            }
            case Spellbook:
            {
                return "Spellbook";
            }
            case MusicSheet:
            {
                return "MusicSheet";
            }
            case Count:
            {
                return "(Count)";
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

} // namespace popup
} // namespace heroespath
