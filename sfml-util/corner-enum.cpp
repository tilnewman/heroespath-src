//
// corner.cpp
//
#include "corner-enum.hpp"
#include <exception>
#include <sstream>


namespace sfml_util
{

    const std::string Corner::ToString(const Corner::Enum E)
    {
        switch (E)
        {
            case TopLeft:     { return "TopLeft";     }
            case TopRight:    { return "TopRight";    }
            case BottomLeft:  { return "BottomLeft";  }
            case BottomRight: { return "BottomRight"; }
            case Error:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::Corner::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    bool Corner::IsValid(const Corner::Enum E)
    {
        const unsigned MAX( None | TopLeft | TopRight | BottomLeft | BottomRight);
        return (static_cast<unsigned>(E) <= MAX);
    }

}
