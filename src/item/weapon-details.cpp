// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// weapon-details.cpp
//
#include "weapon-details.hpp"

#include "item/weapon-type-wrapper.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/boost-string-includes.hpp"
#include "misc/config-file.hpp"
#include "misc/strings.hpp"

#include <boost/lexical_cast.hpp>

#include <exception>
#include <sstream>
#include <vector>

namespace heroespath
{
namespace item
{
    namespace weapon
    {

        WeaponDetailMap_t WeaponDetailLoader::weaponDetailsMap_;

        const WeaponDetails WeaponDetailLoader::LookupWeaponDetails(const std::string & NAME)
        {
            WeaponDetails details;
            const auto WAS_FOUND { weaponDetailsMap_.Find(NAME, details) };

            M_HP_ASSERT_OR_LOG_AND_THROW(
                WAS_FOUND,
                "item::weapon::WeaponDetailLoader::LookupWeaponDetails(\""
                    << NAME << "\")  failed to find that name in weaponDetailsMap_.");

            return details;
        }

        void WeaponDetailLoader::LoadFromGameDataFile()
        {
            for (const auto & WEAPON_TYPE_WRAPPER : WeaponTypeWrapper::MakeCompleteSet())
            {
                if (WEAPON_TYPE_WRAPPER.IsBreath())
                {
                    LoadDetailsForKey(
                        WEAPON_TYPE_WRAPPER.DetailsKeyName()
                        + creature::role::ToString(creature::role::Firebrand));

                    LoadDetailsForKey(
                        WEAPON_TYPE_WRAPPER.DetailsKeyName()
                        + creature::role::ToString(creature::role::Sylavin));
                }
                else
                {
                    LoadDetailsForKey(WEAPON_TYPE_WRAPPER.DetailsKeyName());
                }
            }
        }

        void WeaponDetailLoader::LoadDetailsForKey(const std::string & WEAPON_NAME)
        {
            WeaponDetails weaponDetails;

            // lookup detail value string in the GameDataFile
            const auto KEY_STR { "heroespath-item-weapon-details-" + WEAPON_NAME };
            const auto VALUE_STR { misc::ConfigFile::Instance()->Value(KEY_STR) };

            // break the line of text into comma separated field strings
            const std::vector<std::string> FIELDS_VEC { misc::SplitByChars(
                VALUE_STR, misc::SplitHow(',')) };

            // verify there are eight fields
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (FIELDS_VEC.size() == 8),
                "item::weapon::WeaponDetailsLoader::LoadDetailsForKey(weapon_name=\""
                    << WEAPON_NAME << "\") using key=\"" << KEY_STR << "\" found value=\""
                    << VALUE_STR << "\" but failed to find the required 9 comma separated fields.");

            weaponDetails.name = CleanStringField(FIELDS_VEC[0], false);

            weaponDetails.complexity = static_cast<creature::nonplayer::complexity_type::Enum>(
                creature::nonplayer::complexity_type::FromString(
                    CleanStringField(FIELDS_VEC[1], false)));

            weaponDetails.price = Coin_t(StringFieldToInt("Price", FIELDS_VEC[2]));
            weaponDetails.weight = Weight_t(StringFieldToInt("Weight", FIELDS_VEC[3]));

            weaponDetails.damage_min = Health_t(StringFieldToInt("DamageMin", FIELDS_VEC[4]));
            weaponDetails.damage_max = Health_t(StringFieldToInt("DamageMax", FIELDS_VEC[5]));

            weaponDetails.handedness
                = ((CleanStringField(FIELDS_VEC[6], true) == "two-handed")
                       ? item::category::TwoHanded
                       : item::category::OneHanded);

            weaponDetails.description = CleanStringField(FIELDS_VEC[7], false);

            // store details in the map
            weaponDetailsMap_[WEAPON_NAME] = weaponDetails;
        }

        int WeaponDetailLoader::StringFieldToInt(
            const std::string & FIELD_NAME, const std::string & NUM_STR)
        {
            const int ERROR_VAL(-1);
            int result(ERROR_VAL);
            try
            {
                result = boost::lexical_cast<int>(NUM_STR);
            }
            catch (...)
            {
                result = ERROR_VAL;
            }

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (result != ERROR_VAL),
                "item::weapon::WeaponDetailsLoader::StringFieldToInt(field_name=\""
                    << FIELD_NAME << "\", num_str=\"" << NUM_STR << "\") failed to convert the "
                    << FIELD_NAME << " field of \"" << NUM_STR << "\" into an int.");

            return result;
        }

        const std::string WeaponDetailLoader::CleanStringField(
            const std::string & FIELD_STR, const bool WILL_LOWERCASE)
        {
            using namespace boost::algorithm;

            if (WILL_LOWERCASE)
            {
                return to_lower_copy(trim_copy(erase_all_copy(FIELD_STR, "\"")));
            }
            else
            {
                return trim_copy(erase_all_copy(FIELD_STR, "\""));
            }
        }

    } // namespace weapon
} // namespace item
} // namespace heroespath
