#ifndef GAME_ITEM_ARMORINFO_INCLUDED
#define GAME_ITEM_ARMORINFO_INCLUDED
//
// armor-info.hpp
//  Code that completely describes a singl piece of armor.
//
#include "utilz/boost-serialize-includes.hpp"

#include "game/item/item-type-enum.hpp"
#include "game/item/armor-types.hpp"

#include <tuple>
#include <string>


namespace game
{
namespace item
{
namespace armor
{

    //A simple wrapper for all the info needed to describe one piece of armor
    struct ArmorInfo
    {
        ArmorInfo(const armor_type::Enum TYPE = armor_type::NotArmor);

        //when used here in this class, only one bit is ever set.
        item::armor_type::Enum type;

        bool is_aventail;
        bool is_bracer;
        bool is_shirt;
        bool is_boots;
        bool is_pants;
        bool is_gauntlets;

        base_type::Enum   base;
        cover_type::Enum  cover;
        helm_type::Enum   helm;
        shield_type::Enum shield;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & type;
            ar & is_aventail;
            ar & is_bracer;
            ar & is_shirt;
            ar & is_boots;
            ar & is_pants;
            ar & is_gauntlets;
            ar & base;
            ar & cover;
            ar & helm;
            ar & shield;
        }
    };


    inline bool operator==(const ArmorInfo & L, const ArmorInfo & R)
    {
        return std::tie(L.type,
                        L.is_aventail,
                        L.is_bracer,
                        L.is_shirt,
                        L.is_boots,
                        L.is_pants,
                        L.is_gauntlets,
                        L.base,
                        L.cover,
                        L.helm,
                        L.shield)
            ==
               std::tie(R.type,
                        R.is_aventail,
                        R.is_bracer,
                        R.is_shirt,
                        R.is_boots,
                        R.is_pants,
                        R.is_gauntlets,
                        R.base,
                        R.cover,
                        R.helm,
                        R.shield);
    }


    inline bool operator!=(const ArmorInfo & L, const ArmorInfo & R)
    {
        return ! (L == R);
    }


    inline bool operator<(const ArmorInfo & L, const ArmorInfo & R)
    {
        return std::tie(L.type,
                        L.is_aventail,
                        L.is_bracer,
                        L.is_shirt,
                        L.is_boots,
                        L.is_pants,
                        L.is_gauntlets,
                        L.base,
                        L.cover,
                        L.helm,
                        L.shield)
            <
               std::tie(R.type,
                        R.is_aventail,
                        R.is_bracer,
                        R.is_shirt,
                        R.is_boots,
                        R.is_pants,
                        R.is_gauntlets,
                        R.base,
                        R.cover,
                        R.helm,
                        R.shield);
    }

}//namespace armor
}//namespace item
}//namespace game

#endif //GAME_ITEM_ARMORINFO_INCLUDED
