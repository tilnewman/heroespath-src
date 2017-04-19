#ifndef APPBASE_HEROESPATH_LOCATION_MAPENUM_INCLUDED
#define APPBASE_HEROESPATH_LOCATION_MAPENUM_INCLUDED
//
// map-enum.hpp
//  An enumeration defining each mapped location in the game.
//
#include <string>


namespace heroespath
{
namespace location
{

    struct map
    {
        enum Enum
        {
            Thornberry = 0,
            ThornberryForest,
            Mudgate,
            Bridgeway,
            Count
        };

        static const std::string MAP_FILENAME_EXT;
        static const std::string ToString(const map::Enum E);
        static const std::string MapFilename(const map::Enum E);
    };

}
}
#endif //APPBASE_HEROESPATH_LOCATION_MAPENUM_INCLUDED
