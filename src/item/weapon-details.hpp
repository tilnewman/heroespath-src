///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef HEROESPATH_ITEM_WEAPONFDETAILS_HPP_INCLUDED
#define HEROESPATH_ITEM_WEAPONFDETAILS_HPP_INCLUDED
//
// weapons-details.hpp
//  Code that loads detailed weapon data from the GameDataFile.
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

        // A singleton class that loads detailed weapon info from the GameDataFile.
        class WeaponDetailLoader
        {
        public:
            WeaponDetailLoader(const WeaponDetailLoader &) = delete;
            WeaponDetailLoader(WeaponDetailLoader &&) = delete;
            WeaponDetailLoader & operator=(const WeaponDetailLoader &) = delete;
            WeaponDetailLoader & operator=(WeaponDetailLoader &&) = delete;

            WeaponDetailLoader();
            ~WeaponDetailLoader();

            static misc::NotNull<WeaponDetailLoader *> Instance();
            static void Acquire();
            static void Release();

            const WeaponDetails LookupWeaponDetails(const std::string & NAME);

        private:
            void LoadWeaponDeatilsFromGameDataFile();
            void LoadDetailsForKey(const std::string & WEAPON_NAME);
            int StringFieldToInt(const std::string & FIELD_NAME, const std::string & NUM_STR);
            const std::string
                CleanStringField(const std::string & FIELD_STR, const bool WILL_LOWERCASE);

        private:
            static std::unique_ptr<WeaponDetailLoader> instanceUPtr_;
            WeaponDetailMap_t weaponDetailsMap_;
        };
    } // namespace weapon
} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_WEAPONFDETAILS_HPP_INCLUDED
