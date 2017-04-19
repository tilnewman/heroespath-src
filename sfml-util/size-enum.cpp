//
// size-enum.cpp
//
#include "size-enum.hpp"
#include <exception>
#include <sstream>


namespace sfml_util
{

    namespace Size
    {

        const std::string ToString(const Size::Enum E)
        {
            switch (E)
            {
                case Small:  { return "Small"; }
                case Medium: { return "Medium"; }
                case Large:  { return "Large"; }
                case Count:
                default:
                {
                    std::ostringstream ss;
                    ss << "sfml_util::Size::ToString(" << E << ")_InvalidValueError.";
                    throw std::range_error(ss.str());
                }
            }
        }

    }

}
