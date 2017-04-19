#ifndef APPBASE_HEROESPATH_ITEM_WEAPONTYPES_INCLUDED
#define APPBASE_HEROESPATH_ITEM_WEAPONTYPES_INCLUDED
//
// weapons-types.hpp
//  A set of enumerations that together define a weapon.
//
#include <string>


namespace heroespath
{
namespace item
{
namespace weapon
{

    struct sword_type
    {
        enum Enum
        {
            Shortsword = 0,
            Gladius,
            Cutlass,
            Falcata,
            Rapier,
            Saber,
            Broadsword,
            Longsword,
            Knightlysword,
            Flamberg,
            Claymore,
            Count
        };

        static const std::string ToString(const sword_type::Enum);
    };


    struct axe_type
    {
        enum Enum
        {
            Handaxe = 0,
            Sickle,
            Battleaxe,
            Waraxe,
            Count
        };

        static const std::string ToString(const axe_type::Enum);
    };


    struct club_type
    {
        enum Enum
        {
            Spiked = 0,
            Maul,
            Mace,
            Warhammer,
            Count
        };

        static const std::string ToString(const club_type::Enum);
    };


    struct whip_type
    {
        enum Enum
        {
            Bullwhip = 0,
            Flail,
            MaceAndChain,
            Count
        };

        static const std::string ToString(const whip_type::Enum);
    };


    struct projectile_type
    {
        enum Enum
        {
            Blowpipe = 0,
            Sling,
            Longbow,
            Crossbow,
            CompositeBow,
            Count
        };

        static const std::string ToString(const projectile_type::Enum);
    };


    struct bladedstaff_type
    {
        enum Enum
        {
            ShortSpear = 0,
            Spear,
            Scythe,
            Pike,
            Partisan,
            Halberd,
            Count
        };

        static const std::string ToString(const bladedstaff_type::Enum);
    };

}
}
}
#endif //APPBASE_HEROESPATH_ITEM_WEAPONTYPES_INCLUDED
