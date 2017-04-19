//
// brightness.cpp
//
#include "brightness-enum.hpp"
#include <exception>
#include <sstream>


namespace sfml_util
{

    const std::string Brightness::ToString(const Brightness::Enum E)
    {
        switch (E)
        {
            case Bright: { return "Bright"; }
            case Medium: { return "Medium"; }
            case Dark:   { return "Dark"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::Brightness::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    bool Brightness::IsValid(const Brightness::Enum E)
    {
        switch (E)
        {
            case Bright:
            case Medium:
            case Dark:  { return true; }
            case Count:
            default:    { return false; }
        }
    }

}
