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

    const std::string PopupStage::ToString(const PopupStage::Enum STAGE)
    {
        switch (STAGE)
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
                    "enum_value=" << static_cast<EnumUnderlying_t>(STAGE) << " is invalid. (count="
                                  << static_cast<EnumUnderlying_t>(Count) << ")");

                return "";
            }
        }
    }

    const std::string PopupButtons::ToString(const Enum BUTTONS, const EnumStringHow & HOW)
    {
        return EnumUtil<PopupButtons>::ToString(BUTTONS, HOW);
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

    const std::string PopupButtonColor::ToString(const PopupButtonColor::Enum COLOR)
    {
        switch (COLOR)
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
                    "enum_value=" << static_cast<EnumUnderlying_t>(COLOR) << " is invalid. (count="
                                  << static_cast<EnumUnderlying_t>(Count) << ")");

                return "";
            }
        }
    }

    const std::string PopupImage::ToString(const PopupImage::Enum IMAGE)
    {
        switch (IMAGE)
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
                    "enum_value=" << static_cast<EnumUnderlying_t>(IMAGE) << " is invalid. (count="
                                  << static_cast<EnumUnderlying_t>(Count) << ")");

                return "";
            }
        }
    }

    const sf::FloatRect
        PopupImage::ContentRegionRatios(const PopupImage::Enum IMAGE, const bool IS_LEFT)
    {
        switch (IMAGE)
        {
            case Banner:
            {
                return sf::FloatRect(0.16f, 0.125f, 0.66f, 0.7f);
            }
            case Regular:
            {
                return sf::FloatRect(0.066f, 0.125f, 0.848f, 0.765f);
            }
            case RegularSidebar:
            {
                return sf::FloatRect(0.2f, 0.081f, 0.714f, 0.81f);
            }
            case Large:
            {
                return sf::FloatRect(0.094f, 0.086f, 0.786f, 0.794f);
            }
            case LargeSidebar:
            {
                return sf::FloatRect(0.168f, 0.098f, 0.712f, 0.782f);
            }
            case Spellbook:
            {
                if (IS_LEFT)
                {
                    return sf::FloatRect(0.088f, 0.12f, 0.312f, 0.74f);
                }
                else
                {
                    return sf::FloatRect(0.515f, 0.12f, 0.31f, 0.55f);
                }
            }
            case MusicSheet:
            {
                if (IS_LEFT)
                {
                    return sf::FloatRect(0.078f, 0.12f, 0.372f, 0.61f);
                }
                else
                {
                    return sf::FloatRect(0.512f, 0.12f, 0.368f, 0.61f);
                }
            }
            case Count:
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(IMAGE) << " is invalid. (count="
                                  << static_cast<EnumUnderlying_t>(Count) << ")");

                return sf::FloatRect();
            }
        }
    }

    float PopupImage::ScreenSizeRatio(const PopupImage::Enum IMAGE)
    {
        switch (IMAGE)
        {
            case Banner:
            {
                return 0.25f;
            }
            case Regular:
            case RegularSidebar:
            {
                return 0.333f;
            }
            case Large:
            case LargeSidebar:
            {
                return 0.5f;
            }
            case Spellbook:
            case MusicSheet:
            {
                // Spellbook and MusicSheet scale themselves, so return full size
                return 1.0f;
            }
            case Count:
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(IMAGE) << " is invalid. (count="
                                  << static_cast<EnumUnderlying_t>(Count) << ")");

                return 1.0f;
            }
        }
    }

    const std::string PopupImage::ImageConfigKey(const PopupImage::Enum IMAGE)
    {
        const std::string PREFIX { "media-image-background-paper-popup-" };

        switch (IMAGE)
        {
            case PopupImage::Banner:
            {
                return PREFIX + "banner";
            }
            case PopupImage::Regular:
            {
                return PREFIX + "medium";
            }
            case PopupImage::RegularSidebar:
            {
                return PREFIX + "medium-bar";
            }
            case PopupImage::Large:
            {
                return PREFIX + "large";
            }
            case PopupImage::LargeSidebar:
            {
                return PREFIX + "large-bar";
            }
            case PopupImage::Spellbook:
            {
                return PREFIX + "spell-book";
            }
            case PopupImage::MusicSheet:
            {
                return PREFIX + "music-sheet";
            }
            case PopupImage::Count:
            default:
            {
                M_HP_LOG_ERR(
                    "gui::PopupManager::BackgroundImagePath(" << popup::PopupImage::ToString(
                        IMAGE) << ") but that popup::PopupImage::Enum value was invalid.");

                return "media-image-misc-error";
            }
        }
    }

} // namespace popup
} // namespace heroespath
