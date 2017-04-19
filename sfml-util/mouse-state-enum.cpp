//
// mouse-state-enum.cpp
//
#include "mouse-state-enum.hpp"
#include <exception>
#include <sstream>


namespace sfml_util
{

    const std::string MouseState::ToString(const MouseState::Enum E)
    {
        switch (E)
        {
            case Up:   { return "Up";   }
            case Over: { return "Over"; }
            case Down: { return "Down"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::MouseState::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    bool MouseState::IsValid(const MouseState::Enum E)
    {
        switch (E)
        {
            case Up:
            case Over:
            case Down: { return true; }
            case Count:
            default: { return false; }
        }
    }

}
