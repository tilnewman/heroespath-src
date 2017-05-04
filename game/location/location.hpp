#ifndef GAME_LOCATION_LOCATION_INCLUDED
#define GAME_LOCATION_LOCATION_INCLUDED
//
// location.hpp
//  A base class for all location objects.
//
#include "utilz/boost-serialize-includes.hpp"

#include "game/location/i-location.hpp"
#include "game/location/map-enum.hpp"

#include <memory>
#include <string>
#include <vector>


namespace game
{
namespace location
{

    //a base class for common code with all Location objects
    class Location : public ILocation
    {
        //prevent copy construction
        Location(const Location &);

        //prevent copy assignment
        Location & operator=(const Location &);

    public:
        Location(const map::Enum WHICH_MAP    = map::Thornberry,
                 const float MAP_POS_OFFSET_X = 0.0f,
                 const float MAP_POS_OFFSET_Y = 0.0f);

        virtual ~Location();

        //from ILocation
        inline virtual const std::string Name() const                   { return map::ToString(whichMap_); }

        inline virtual map::Enum Map() const                            { return whichMap_; }

        inline virtual void MapSet(const map::Enum MAP)                 { whichMap_ = MAP; }
        inline virtual const std::string MapFilename() const            { return map::MapFilename(whichMap_); }

        inline virtual const sf::Vector2f MapPosOffset() const          { return sf::Vector2f(mapPosOffsetX_, mapPosOffsetY_); }
        inline virtual void MapPosOffset(const float X, const float Y)  { mapPosOffsetX_ = X; mapPosOffsetY_ = Y; }

    protected:
        map::Enum whichMap_;
        float     mapPosOffsetX_;
        float     mapPosOffsetY_;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & whichMap_;
            ar & mapPosOffsetX_;
            ar & mapPosOffsetY_;
        }
    };

    using LocationSPtr_t = std::shared_ptr<Location>;
    using LocationSVec_t = std::vector<LocationSPtr_t>;

}
}
#endif //GAME_LOCATION_LOCATION_INCLUDED
