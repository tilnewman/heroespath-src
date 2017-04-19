#ifndef APPBASE_HEROESPATH_CREATURE_ROLEENUM_INCLUDED
#define APPBASE_HEROESPATH_CREATURE_ROLEENUM_INCLUDED
//
// role-enum.hpp
//  An enumeration defining each Role of creature, both player and non_player.
//
#include <string>
#include <vector>


namespace heroespath
{
namespace creature
{

    struct role
    {
        enum Enum
        {
            Archer,
            Bard,
            Beastmaster,
            Cleric,
            Knight,
            Sorcerer,
            Thief,
            Firebrand,
            Sylavin,
            Wolfen,
            PlayerRoleCount,
            Thug = PlayerRoleCount,
            Mugger,
            Drunk,
            Grunt,
            Brute,
            Berserker,
            Mountain,
            Captain,
            Chieftain,
            Trader,
            Warlord,
            Shaman,
            Smasher,
            Strangler,
            Soldier,
            TwoHeaded,
            Giant,
            Elder,
            FourArmed,
            Tendrilus,
            Wing,
            Whelp,
            Pod,
            Spike,
            Skeleton,
            Ranger,
            Desert,
            Water,
            Blacksmith,
            Witch,
            Count
        };

        static const std::string ToString(const role::Enum);
        inline static const std::string Name(const role::Enum E) { return ToString(E); }
        static const std::string Desc(const role::Enum);
        static const std::string Abbr(const role::Enum);
        static bool CanFly(const role::Enum);
        static bool WillInitiallyFly(const role::Enum);
    };

    using RoleVec_t = std::vector<role::Enum>;

}
}
#endif //APPBASE_HEROESPATH_CREATURE_ROLEENUM_INCLUDED
