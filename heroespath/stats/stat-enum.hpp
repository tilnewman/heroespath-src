#ifndef APPBASE_HEROESPATH_STAT_ENUM_INCLUDED
#define APPBASE_HEROESPATH_STAT_ENUM_INCLUDED
//
// stat-enum.hpp
//  An enumeration defining all stats.
//
#include <string>
#include <vector>


namespace heroespath
{
namespace stats
{

    struct stat
    {
        enum Enum
        {
            Strength = 0,
            Accuracy,
            Charm,
            Luck,
            Speed,
            Intelligence,
            Count
        };

        static const std::string Name(const stat::Enum E);
        static const std::string Desc(const stat::Enum E);
        static const std::string Abbr(const stat::Enum E, const bool WILL_CAP = false);
        inline static const std::string ToString(const stat::Enum E) { return Name(E); }
    };

    using StatEnumVec_t = std::vector<stat::Enum>;

}
}
#endif //APPBASE_HEROESPATH_STAT_ENUM_INCLUDED
