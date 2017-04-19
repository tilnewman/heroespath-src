#ifndef APPBASE_HEROESPATH_CREATURE_RACE_INCLUDED
#define APPBASE_HEROESPATH_CREATURE_RACE_INCLUDED
//
// race.hpp
//  A base class for all Race types
//
#include "sfml-util/boost-serialize-includes.hpp"

#include "heroespath/creature/race-enum.hpp"
#include "heroespath/stats/stat-set.hpp"

#include <string>
#include <tuple>


namespace heroespath
{
namespace creature
{

    //A base class for all Race types
    class Race
    {
    public:
        Race(const race::Enum    RACE_ENUM = race::Count_PlayerRaces);

        virtual ~Race();

        inline const std::string Name() const                   { return race::Name(race_); }
        inline const std::string Desc() const                   { return race::Desc(race_); }
        inline race::Enum Which() const                         { return race_; }
        inline const stats::StatSet StatModifiersCopy() const   { return statModifiers_; }

        friend bool operator<(const Race & L, const Race & R);
        friend bool operator==(const Race & L, const Race & R);

    protected:
        race::Enum     race_;
        stats::StatSet statModifiers_;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & race_;
            ar & statModifiers_;
        }
    };


    inline bool operator<(const Race & L, const Race & R)
    {
        return std::tie(L.race_, L.statModifiers_)
                <
               std::tie(R.race_, R.statModifiers_);
    }

    inline bool operator==(const Race & L, const Race & R)
    {
        return std::tie(L.race_, L.statModifiers_)
               ==
               std::tie(R.race_, R.statModifiers_);
    }

    inline bool operator!=(const Race & L, const Race & R)
    {
        return ! (L == R);
    }

}
}
#endif //APPBASE_HEROESPATH_CREATURE_RACE_INCLUDED
