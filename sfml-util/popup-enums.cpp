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
            case Custom: { return true; }
            case Count:
            default: { return false; }
        }
    }


    const std::string PopupAccent::ToString(const PopupAccent::Enum E)
    {
        switch(E)
        {
            case Crest:     { return "Crest"; }
            case Root1:     { return "Root1"; }
            case Root2:     { return "Root2"; }
            case Root3:     { return "Root3"; }
            case Root4:     { return "Root4"; }
            case Design1:   { return "Design1"; }
            case Design2:   { return "Design2"; }
            case Design3:   { return "Design3"; }
            case Design4:   { return "Design4"; }
            case Plant1:    { return "Plant1"; }
            case Plant2:    { return "Plant2"; }
            case Plant3:    { return "Plant3"; }
            case Plant4:    { return "Plant4"; }
            case Figure1:   { return "Figure1"; }
            case Figure2:   { return "Figure2"; }
            case Figure3:   { return "Figure3"; }
            case Figure4:   { return "Figure4"; }
            case Figure5:   { return "Figure5"; }
            case Figure6:   { return "Figure6"; }
            case Figure7:   { return "Figure7"; }
            case Figure8:   { return "Figure8"; }
            case Figure9:   { return "Figure9"; }
            case Figure10:  { return "Figure10"; }
            case Figure11:  { return "Figure11"; }
            case Figure12:  { return "Figure12"; }
            case Figure13:  { return "Figure13"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::PopupAccent::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    bool PopupAccent::IsValid(const PopupAccent::Enum E)
    {
        switch(E)
        {
            case Crest:
            case Root1:
            case Root2:
            case Root3:
            case Root4:
            case Design1:
            case Design2:
            case Design3:
            case Design4:
            case Plant1:
            case Plant2:
            case Plant3:
            case Plant4:
            case Figure1:
            case Figure2:
            case Figure3:
            case Figure4:
            case Figure5:
            case Figure6:
            case Figure7:
            case Figure8:
            case Figure9:
            case Figure10:
            case Figure11:
            case Figure12:
            case Figure13: { return true; }
            case Count:
            default: { return false; }
        }
    }


    bool PopupAccent::MustCenter(const PopupAccent::Enum E)
    {
        switch(E)
        {
            case Crest:
            case Root1:
            case Root2:
            case Root3:
            case Root4:
            case Design1:
            case Design2:
            case Design3:
            case Design4:
            case Plant1:
            case Plant2:
            case Plant3:
            case Plant4: { return true; }
            case Figure1:
            case Figure2:
            case Figure3:
            case Figure4:
            case Figure5: { return false; }
            case Figure6: { return true; }
            case Figure7: { return false; }
            case Figure8:
            case Figure9:
            case Figure10:
            case Figure11:
            case Figure12:
            case Figure13: { return true; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::PopupAccent::MustCenter(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

}
