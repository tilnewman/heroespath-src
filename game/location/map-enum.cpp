//
// map-enum.cpp
//
#include "map-enum.hpp"
#include <exception>
#include <sstream>


namespace game
{
namespace location
{

    const std::string map::MAP_FILENAME_EXT(".tmx");


    const std::string map::ToString(const map::Enum E)
    {
        switch (E)
        {
            case Thornberry:        { return "Thornberry"; }
            case ThornberryForest:  { return "ThronberryForest"; }
            case Mudgate:           { return "Mudgate"; }
            case Bridgeway:         { return "Bridgeway"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::location::map::Enum::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::string map::MapFilename(const map::Enum E)
    {
        std::ostringstream ss;
        ss << ToString(E) << MAP_FILENAME_EXT;
        return ss.str();
    }

}
}
