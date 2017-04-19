#ifndef SFMLUTIL_GUIEVENT_ENUM_HPP_INCLUDED
#define SFMLUTIL_GUIEVENT_ENUM_HPP_INCLUDED
//
// gui-event-enum.hpp
//
#include <string>


namespace sfml_util
{

    struct GuiEvent
    {
        enum Enum : unsigned int
        {
            None             = 0,
            Click            = 1 << 0,
            DoubleClick      = 1 << 1,
            MouseWheel       = 1 << 2,
            SelectionChange  = 1 << 3,
            Keypress         = 1 << 4,
            FocusChange      = 1 << 5
        };

        static const std::string ToString(const GuiEvent::Enum);
        static bool IsValid(const GuiEvent::Enum);
    };

}

#endif //SFMLUTIL_GUIEVENT_ENUM_HPP_INCLUDED
