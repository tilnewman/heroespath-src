//
// role-stats.cpp
//
#include "role-stats.hpp"


namespace heroespath
{
namespace creature
{

    const stats::StatSet StatModifierByRole(const role::Enum ROLE)
    {
        if (ROLE == role::Archer)
            return stats::StatSet(   0, //str
                                     2, //acc
                                     0, //cha
                                     0, //lck
                                     0, //spd
                                    -2);//int
        else if (ROLE == role::Bard)
            return stats::StatSet(   2, //str
                                     0, //acc
                                     2, //cha
                                     0, //lck
                                    -6, //spd
                                     2);//int
        else if (ROLE == role::Beastmaster)
            return stats::StatSet (  2, //str
                                     0, //acc
                                    -2, //cha
                                     0, //lck
                                     0, //spd
                                     0);//int
        else if (ROLE == role::Cleric)
            return stats::StatSet(   0, //str
                                     0, //acc
                                     2, //cha
                                     0, //lck
                                    -2, //spd
                                     0);//int
        else if (ROLE == role::Firebrand)
            return stats::StatSet(   0, //str
                                     0, //acc
                                    -2, //cha
                                     0, //lck
                                     0, //spd
                                     2);//int
        else if (ROLE == role::Knight)
            return stats::StatSet(   2, //str
                                     2, //acc
                                     0, //cha
                                     0, //lck
                                     0, //spd
                                     -4);//int
        else if (ROLE == role::Sorcerer)
            return stats::StatSet(  -2, //str
                                     0, //acc
                                     0, //cha
                                     0, //lck
                                     0, //spd
                                     2);//int
        else if (ROLE == role::Sylavin)
            return stats::StatSet(   2, //str
                                     0, //acc
                                    -4, //cha
                                     0, //lck
                                     0, //spd
                                     2);//int
        else if (ROLE == role::Thief)
            return stats::StatSet(   0, //str
                                     0, //acc
                                    -4, //cha
                                     4, //lck
                                     2, //spd
                                    -2);//int
        else if (ROLE == role::Wolfen)
            return stats::StatSet(   0, //str
                                     0, //acc
                                     0, //cha
                                     0, //lck
                                     0, //spd
                                     0);//int
        else
            return stats::StatSet(0, 0, 0, 0, 0, 0);
    }

}
}
