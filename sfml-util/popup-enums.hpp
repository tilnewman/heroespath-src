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


    struct PopupAccent
    {
        enum Enum
        {
            Crest = 0,
            Root1,
            Root2,
            Root3,
            Root4,
            Design1,
            Design2,
            Design3,
            Design4,
            Plant1,
            Plant2,
            Plant3,
            Plant4,
            Figure1,
            Figure2,
            Figure3,
            Figure4,
            Figure5,
            Figure6,
            Figure7,
            Figure8,
            Figure9,
            Figure10,
            Figure11,
            Figure12,
            Figure13,
            Count
        };

        static const std::string ToString(const PopupAccent::Enum);
        static bool IsValid(const PopupAccent::Enum);
        static bool MustCenter(const PopupAccent::Enum);
    };

}

#endif //SFMLUTIL_POPUP_ENUMS_HPP_INCLUDE
