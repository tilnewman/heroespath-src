#ifndef SFMLUTIL_POPUP_ENUMS_HPP_INCLUDE
#define SFMLUTIL_POPUP_ENUMS_HPP_INCLUDE
//
// popup-enums.hpp
//
#include "response-enum.hpp"
#include <string>


namespace sfml_util
{

    struct PopupButtons
    {
        enum Enum : unsigned
        {
            Error        = 0,
            None         = Error,
            Okay         = Response::Okay,
            Select       = Response::Select,
            Cancel       = Response::Cancel,
            SelectCancel = Response::Select | Response::Cancel,
            Continue     = Response::Continue,
            YesNo        = Response::Yes | Response::No,
            YesNoCancel  = Response::Yes | Response::No | Response::Cancel
        };

        static const std::string ToString(const PopupButtons::Enum E);
        static bool IsValid(const PopupButtons::Enum E);
    };


    struct PopupButtonColor
    {
        enum Enum
        {
            Light = 0,
            Dark,
            Count
        };

        static const std::string ToString(const PopupButtonColor::Enum);
        static bool IsValid(const PopupButtonColor::Enum);
    };


    struct PopupImage
    {
        enum Enum
        {
            Banner = 0,
            Regular,
            RegularSidebar,
            Large,
            LargeSidebar,
            Custom,
            Spellbook,
            Count
        };

        static const std::string ToString(const PopupImage::Enum);
        static bool IsValid(const PopupImage::Enum);
    };

}

#endif //SFMLUTIL_POPUP_ENUMS_HPP_INCLUDE
