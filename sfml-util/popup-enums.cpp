//
// popup-enums.cpp
//
#include "popup-enums.hpp"
#include <exception>
#include <sstream>


namespace sfml_util
{

    const std::string PopupButtons::ToString(const PopupButtons::Enum E)
    {
        switch (E)
        {
            case Okay:         { return "Okay";         }
            case Select:       { return "Select";       }
            case Cancel:       { return "Cancel";       }
            case SelectCancel: { return "SelectCancel"; }
            case Continue:     { return "Continue";     }
            case YesNo:        { return "YesNo";        }
            case YesNoCancel:  { return "YesNoCancel";  }
            case Error:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::PopupButtons::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    bool PopupButtons::IsValid(const PopupButtons::Enum E)
    {
        const unsigned MAX(None | Okay | Select | Cancel | SelectCancel | Continue | YesNo | YesNoCancel);
        return (static_cast<unsigned>(E) <= MAX);
    }


    const std::string PopupButtonColor::ToString(const PopupButtonColor::Enum E)
    {
        switch(E)
        {
            case Light: { return "Light"; }
            case Dark:  { return "Dark"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::PopupButtonColor::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    bool PopupButtonColor::IsValid(const PopupButtonColor::Enum E)
    {
        switch (E)
        {
            case Light:
            case Dark:  { return true; }
            case Count:
            default:    { return false; }
        }
    }


    const std::string PopupImage::ToString(const PopupImage::Enum E)
    {
        switch (E)
        {
            case Banner:            { return "Banner"; }
            case Regular:           { return "Regular"; }
            case RegularSidebar:    { return "RegularSidebar"; }
            case Large:             { return "Large"; }
            case LargeSidebar:      { return "LargeSidebar"; }
            case Custom:            { return "Custom"; }
            case Spellbook:         { return "Spellbook"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::PopupImage::ToString(" << E << ")_InvalidValueError.";
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
            case Custom:
            case Spellbook: { return true; }
            case Count:
            default: { return false; }
        }
    }

}
