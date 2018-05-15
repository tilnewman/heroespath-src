// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_ARMORDETAILS_HPP_INCLUDED
#define HEROESPATH_ITEM_ARMORDETAILS_HPP_INCLUDED
//
// armor-details.hpp
//  Code that loads detailed armor data from the GameDataFile.
//
#include "misc/not-null.hpp"
#include "misc/vector-map.hpp"
#include "non-player/ownership-profile.hpp"
#include "stats/trait.hpp"

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
        using ArmorDetailMap_t = misc::VectorMap<std::string, ArmorDetails>;

        // Loads and holds detailed armor info from the GameDataFile.
        class ArmorDetailLoader
        {
        public:
            ArmorDetailLoader & operator=(const ArmorDetailLoader &) = delete;
            ArmorDetailLoader & operator=(ArmorDetailLoader &&) = delete;
            ArmorDetailLoader(const ArmorDetailLoader &) = delete;
            ArmorDetailLoader(ArmorDetailLoader &&) = delete;
            ArmorDetailLoader() = delete;

            static const ArmorDetails LookupArmorDetails(const std::string & NAME);

            static void LoadFromGameDataFile();

        private:
            static void LoadDetailsForKey(const std::string & ARMOR_NAME);

            static int
                StringFieldToInt(const std::string & FIELD_NAME, const std::string & NUM_STR);

            static const std::string
                CleanStringField(const std::string & FIELD_STR, const bool WILL_LOWERCASE);

        private:
            static ArmorDetailMap_t armorDetailsMap_;
        };

    } // namespace armor
} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ARMORDETAILS_HPP_INCLUDED
