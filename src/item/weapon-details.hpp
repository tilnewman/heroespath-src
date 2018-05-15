// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_WEAPONFDETAILS_HPP_INCLUDED
#define HEROESPATH_ITEM_WEAPONFDETAILS_HPP_INCLUDED
//
// weapon-details.hpp
//
#include "misc/not-null.hpp"
#include "misc/vector-map.hpp"
#include "non-player/ownership-profile.hpp"

#include <memory>
#include <string>

namespace heroespath
{
namespace item
{
    namespace weapon
    {

        // a simple wrapper for weapon details
        struct WeaponDetails
        {
            WeaponDetails()
                : name()
                , description()
                , price(0)
                , weight(0)
                , damage_min(0)
                , damage_max(0)
                , handedness(item::category::OneHanded)
                , complexity(non_player::ownership::complexity_type::Count)
            {}

            std::string name;
            std::string description;
            Coin_t price;
            Weight_t weight;
            Health_t damage_min;
            Health_t damage_max;
            item::category::Enum handedness;
            non_player::ownership::complexity_type::Enum complexity;
        };

        // name to details mapping
        using WeaponDetailMap_t = misc::VectorMap<std::string, WeaponDetails>;

        // Responsible for loading detailed weapon info from the GameDataFile.
        class WeaponDetailLoader
        {
        public:
            WeaponDetailLoader(const WeaponDetailLoader &) = delete;
            WeaponDetailLoader(WeaponDetailLoader &&) = delete;
            WeaponDetailLoader & operator=(const WeaponDetailLoader &) = delete;
            WeaponDetailLoader & operator=(WeaponDetailLoader &&) = delete;
            WeaponDetailLoader() = delete;

            static const WeaponDetails LookupWeaponDetails(const std::string & NAME);

            static void LoadFromGameDataFile();

        private:
            static void LoadDetailsForKey(const std::string & WEAPON_NAME);
            static int
                StringFieldToInt(const std::string & FIELD_NAME, const std::string & NUM_STR);

            static const std::string
                CleanStringField(const std::string & FIELD_STR, const bool WILL_LOWERCASE);

        private:
            static WeaponDetailMap_t weaponDetailsMap_;
        };

    } // namespace weapon
} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_WEAPONFDETAILS_HPP_INCLUDED
