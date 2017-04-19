#ifndef HEROESPATH_COMBAT_STRATEGYENUMS_HPP_INCLUDED
#define HEROESPATH_COMBAT_STRATEGYENUMS_HPP_INCLUDED
//
//  strategy-enums.hpp
//
#include <string>
#include <map>


namespace heroespath
{
namespace combat
{
namespace strategy
{

    struct SelectType
    {
        enum Enum : unsigned int
        {
            None            = 0,
            Pixie           = 1 << 0,
            Dragon          = 1 << 1,
            Human           = 1 << 2,
            Gnome           = 1 << 3,
            Wolfen          = 1 << 4,
            Archer          = 1 << 5,
            Sorcerer        = 1 << 6,
            Knight          = 1 << 7,
            Beastmaster     = 1 << 8,
            Cleric          = 1 << 9,
            Theif           = 1 << 10,
            Bard            = 1 << 11,
            FireBrand       = 1 << 12,
            Sylavin         = 1 << 13,
            Projectile      = 1 << 14,
            Caster          = 1 << 15,
            CanFly          = 1 << 16,
            CantFly         = 1 << 17,
            Beast           = 1 << 18,
            NotBeast        = 1 << 19,
        };

        static const std::string ToString(const Enum, const bool WILL_WRAP = false);
        static Enum FromString(const std::string &);
    };


    struct RefineType
    {
        enum Enum : unsigned int
        {
            None         = 0,
            Murderer     = 1 << 0,  //selects whoever is unconcious
            Bloodthirsty = 1 << 1,  //selects whoever has the least health current
            Coward       = 1 << 2,  //selects whoever can't fight back
            Hit          = 1 << 3,  //selects whoever first hits him/her/it
            Attack       = 1 << 4,  //selects whoever first attacks him/her/it
            MusicMaker   = 1 << 5,  //selects whoever was last to play a song/music/bard-spell
            Caster	     = 1 << 6,  //selects whoever casts spells
            Enchanted    = 1 << 7,  //selects whoever is enchanted
            NotEnchanted = 1 << 8,  //selects whoever is NOT enchanted
            Steadfast    = 1 << 9,  //won't change selected target until they die
            LastTo       = 1 << 10, //will change selection/target to whoever last triggers the refinement type
            MostDamage   = 1 << 11  //selects whoever is doing the most damage
        };

        static const std::string ToString(const Enum, const bool WILL_WRAP = false);
        static Enum FromString(const std::string &);
    };


    struct AdvanceType
    {
        enum Enum
        {
            None = 0,      //advances toward most desired target but stops whenever able to attack
            Cowardly,     //Never advances
            Hesitant,    //only advances if being hit and can't hit back
            Charger,    //always advances until reaching most desired target
            Count
        };

        static const std::string ToString(const Enum);
        static Enum FromString(const std::string &);
    };


    struct RetreatType
    {
        enum Enum
        {
            None = 0,    //never retreats
            Wary,       //retreats if melee is outnumbered
            Coward,    //retreats if there are any opponents able to attack in the melee
            Count
        };

        static const std::string ToString(const Enum);
        static Enum FromString(const std::string &);
    };


    struct FrequencyType
    {
        enum Enum
        {
            Never = 0,
            Once,
            Twice,
            Thrice,
            Rarely,
            Seldom,
            Neutral,
            Commonly,
            Often,
            Always,
            Count
        };

        static const std::string ToString(const Enum);
        static Enum FromString(const std::string &);
    };


    using SelectChanceMap_t    = std::map<SelectType::Enum, float>;
    using RefineChanceMap_t    = std::map<RefineType::Enum, float>;
    using AdvanceChanceMap_t   = std::map<AdvanceType::Enum, float>;
    using RetreatChanceMap_t   = std::map<RetreatType::Enum, float>;
    using OutnumberRetreatChanceMap_t = std::map<std::size_t, float>;
    using FrequencyChanceMap_t = std::map<FrequencyType::Enum, float>;

}
}
}

#endif //HEROESPATH_COMBAT_STRATEGYENUMS_HPP_INCLUDED
