//
// justified-enum.cpp
//
#include "justified-enum.hpp"
#include <exception>
#include <sstream>


namespace sfml_util
{

    const std::string Justified::ToString(const Justified::Enum E)
    {
        switch (E)
        {
            case Left:   { return "Left";   }
            case Right:  { return "Right";  }
            case Center: { return "Center"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::Justified::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    bool Justified::IsValid(const Justified::Enum E)
    {
        switch (E)
        {
            case Left:
            case Right:
            case Center: { return true; }
            case Count:
            default: { return false; }
        }
    }

}
