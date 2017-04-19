//
// moving-enum.cpp
//
#include "moving-enum.hpp"
#include <exception>
#include <sstream>


namespace sfml_util
{

    const std::string Moving::ToString(const Moving::Enum E)
    {
        switch(E)
        {
            case Still:  { return "Still"; }
            case Toward: { return "Toward"; }
            case Away:   { return "Away"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::Moving::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        };
    }

}
