#ifndef GAME_LOCATION_ILOCATION_INCLUDED
#define GAME_LOCATION_ILOCATION_INCLUDED
//
// i-location.hpp
//  An interface for all Location objects
//
#include "sfml-util/sfml-graphics.hpp"
#include "game/location/map-enum.hpp"
#include <string>


namespace game
{
namespace location
{

    //an interface for all Location objects
    struct ILocation
    {
        virtual ~ILocation() {}

        virtual const std::string Name() const = 0;

        virtual map::Enum Map() const = 0;
        virtual void MapSet(const map::Enum) = 0;

        virtual const std::string MapFilename() const = 0;

        virtual const sf::Vector2f MapPosOffset() const = 0;
        virtual void MapPosOffset(const float OFFSET_X, const float OFFSET_Y) = 0;
    };

    using ILocationSPtr_t = std::shared_ptr<ILocation>;

}
}
#endif //GAME_LOCATION_ILOCATION_INCLUDED
