//
// race-stats.cpp
//
#include "race-stats.hpp"


namespace game
{
namespace creature
{

    const stats::StatSet StatModifierByRace(const creature::race::Enum RACE)
    {
        if (RACE == race::Human)
            return stats::StatSet(  2,//str
                                    2,//acc
                                    2,//cha
                                   -6,//lck
                                    0,//spd
                                    0);//int
        else if (RACE == race::Gnome)
            return stats::StatSet( -6,//str
                                    0,//acc
                                   -6,//cha
                                    6,//lck
                                    6,//spd
                                    0);//int
        else if (RACE == race::Pixie)
            return stats::StatSet(-20,//str
                                    0,//acc
                                    4,//cha
                                    4,//lck
                                    6,//spd
                                    6);//int
        else if (RACE == race::Wolfen)
            return stats::StatSet( 12,//str
                                    2,//acc
                                  -12,//cha
                                    0,//lck
                                    2,//spd
                                   -4);//int
        else if (RACE == race::Dragon)
            return stats::StatSet( 12,//str
                                    0,//acc
                                  -20,//cha
                                    0,//lck
                                    4,//spd
                                    4);//int
        else
            return stats::StatSet(0, 0, 0, 0, 0, 0);
    }

}
}
