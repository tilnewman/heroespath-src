//
// location.cpp
//
#include "location.hpp"


namespace heroespath
{
namespace location
{

    Location::Location(const map::Enum WHICH_MAP,
                       const float MAP_POS_OFFSET_X,
                       const float MAP_POS_OFFSET_Y)
    :
        whichMap_     (WHICH_MAP),
        mapPosOffsetX_(MAP_POS_OFFSET_X),
        mapPosOffsetY_(MAP_POS_OFFSET_Y)
    {}


    Location::~Location()
    {}

}
}
