///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
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
        Location(const Location &) =delete;

        //prevent copy assignment
        Location & operator=(const Location &) =delete;

    public:
        explicit Location(const map::Enum WHICH_MAP    = map::Thornberry,
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
