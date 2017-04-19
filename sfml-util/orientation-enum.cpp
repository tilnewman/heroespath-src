//
// orientation.cpp
//
#include "orientation-enum.hpp"
#include <exception>
#include <sstream>


namespace sfml_util
{

    const std::string Orientation::ToString(const Orientation::Enum E)
    {
        if (E == Orientation::Horiz)
        {
            return "Horizontal";
        }
        else
        {
            if (E == Orientation::Vert)
            {
                return "Vertical";
            }
            else
            {
                std::ostringstream ss;
                ss << "sfml_util::Orientation::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    bool Orientation::IsValid(const Orientation::Enum E)
    {
        return ((E == Orientation::Horiz) || (E == Orientation::Vert));
    }

}
