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
#ifndef HEROESPATH_ITEM_ARMORDETAILS_HPP_INCLUDED
#define HEROESPATH_ITEM_ARMORDETAILS_HPP_INCLUDED
//
// armor-details.hpp
//  Code that loads detailed armor data from the GameDataFile.
//
#include "non-player/ownership-profile.hpp"
#include "stats/trait.hpp"

#include <map>
#include <memory>
#include <string>

namespace heroespath
{
namespace item
{
    namespace armor
    {

        // a simple wrapper for armor details
        struct ArmorDetails
        {
            ArmorDetails()
                : name()
                , description()
                , price(0)
                , weight(0)
                , armor_rating(0)
                , complexity(non_player::ownership::complexity_type::Count)
            {}

            std::string name;
            std::string description;
            Coin_t price;
            Weight_t weight;
            Armor_t armor_rating;
            non_player::ownership::complexity_type::Enum complexity;
        };

        // name to details mapping
        using ArmorDetailMap_t = std::map<std::string, ArmorDetails>;

        // A singleton class that loads detailed armor info from the GameDataFile.
        class ArmorDetailLoader
        {
        public:
            ArmorDetailLoader & operator=(const ArmorDetailLoader &) = delete;
            ArmorDetailLoader & operator=(const ArmorDetailLoader &&) = delete;
            ArmorDetailLoader(const ArmorDetailLoader &) = delete;
            ArmorDetailLoader(const ArmorDetailLoader &&) = delete;

            ArmorDetailLoader();
            ~ArmorDetailLoader();

            static ArmorDetailLoader * Instance();
            static void Acquire();
            static void Release();

            const ArmorDetails LookupArmorDetails(const std::string & NAME);

        private:
            void LoadArmorDeatilsFromGameDataFile();
            void LoadDetailsForKey(const std::string & ARMOR_NAME);
            int StringFieldToInt(const std::string & FIELD_NAME, const std::string & NUM_STR);
            const std::string
                CleanStringField(const std::string & FIELD_STR, const bool WILL_LOWERCASE);

        private:
            static std::unique_ptr<ArmorDetailLoader> instanceUPtr_;
            ArmorDetailMap_t armorDetailsMap_;
        };
    } // namespace armor
} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ARMORDETAILS_HPP_INCLUDED
