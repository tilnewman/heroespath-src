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

#include <exception>
#include <sstream>

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
            default:
            {
                ThrowInvalidValueForFunction(POPUP_STAGE, "ToString");
            }
        }
    }

    const std::string PopupButtons::ToString(const PopupButtons::Enum E)
    {
        switch (E)
        {
            case None:
            {
                return "None";
            }
            case Okay:
            {
                return "Okay";
            }
            case Select:
            {
                return "Select";
            }
            case Cancel:
            {
                return "Cancel";
            }
            case SelectCancel:
            {
                return "SelectCancel";
            }
            case Continue:
            {
                return "Continue";
            }
            case YesNo:
            {
                return "YesNo";
            }
            case YesNoCancel:
            {
                return "YesNoCancel";
            }
            default:
            {
                std::ostringstream ss;
                ss << "popup::PopupButtons::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

    bool PopupButtons::IsValid(const PopupButtons::Enum E)
    {
        switch (E)
        {
            case None:
            case Okay:
            case Select:
            case Cancel:
            case SelectCancel:
            case Continue:
            case YesNo:
            case YesNoCancel:
            {
                return true;
            }
            default:
            {
                return false;
            }
        }
    }

    const std::string PopupButtonColor::ToString(const PopupButtonColor::Enum E)
    {
        switch (E)
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
            default:
            {
                std::ostringstream ss;
                ss << "gui::PopupButtonColor::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

    bool PopupButtonColor::IsValid(const PopupButtonColor::Enum E)
    {
        switch (E)
        {
            case Light:
            case Dark:
            {
                return true;
            }
            case Count:
            default:
            {
                return false;
            }
        }
    }

    const std::string PopupImage::ToString(const PopupImage::Enum E)
    {
        switch (E)
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
            default:
            {
                std::ostringstream ss;
                ss << "popup::PopupImage::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

    bool PopupImage::IsValid(const PopupImage::Enum E)
    {
        switch (E)
        {
            case Banner:
            case Regular:
            case RegularSidebar:
            case Large:
            case LargeSidebar:
            case Spellbook:
            case MusicSheet:
            {
                return true;
            }
            case Count:
            default:
            {
                return false;
            }
        }
    }
} // namespace popup
} // namespace heroespath
