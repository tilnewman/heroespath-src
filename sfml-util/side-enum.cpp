//
// side.cpp
//
#include "side-enum.hpp"
#include <exception>
#include <sstream>


namespace sfml_util
{

    const std::string Side::ToString(const Side::Enum E)
    {
        switch (E)
        {
            case Top:    { return "Top"; }
            case Bottom: { return "Bottom";  }
            case Left:   { return "Left"; }
            case Right:  { return "Right"; }
            case Error:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::Side::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    bool Side::IsValid(const Side::Enum E)
    {
        const unsigned MAX(None | Top | Bottom | Left | Right);
        return (static_cast<unsigned>(E) <= MAX);
    }

}
