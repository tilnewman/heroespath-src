//
// gui-event-enum.cpp
//
#include "gui-event-enum.hpp"
#include <exception>
#include <sstream>


namespace sfml_util
{

    const std::string GuiEvent::ToString(const GuiEvent::Enum E)
    {
        switch (E)
        {
            case None:            { return "None"; }
            case Click:           { return "Click"; }
            case DoubleClick:     { return "DoubleClick"; }
            case MouseWheel:      { return "MouseWheel"; }
            case SelectionChange: { return "SelectionChange"; }
            case Keypress:        { return "Keypress"; }
            case FocusChange:     { return "FocusChange"; }
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::GuiEvent::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    bool GuiEvent::IsValid(const GuiEvent::Enum E)
    {
        const unsigned MAX(None | Click | DoubleClick | MouseWheel | SelectionChange | Keypress | FocusChange);
        return (static_cast<unsigned>(E) <= MAX);
    }

}
