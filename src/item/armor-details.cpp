// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// armor-details.cpp
//
#include "armor-details.hpp"

#include "item/armor-type-wrapper.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/boost-string-includes.hpp"
#include "misc/config-file.hpp"
#include "misc/strings-split-by-char.hpp"
#include "misc/strings.hpp"

#include <vector>

namespace heroespath
{
namespace item
{
    namespace armor
    {

        ArmorDetailMap_t ArmorDetailLoader::armorDetailsMap_;

        const ArmorDetails ArmorDetailLoader::LookupArmorDetails(const std::string & NAME)
        {
            ArmorDetails details;
            const auto WAS_FOUND { armorDetailsMap_.Find(NAME, details) };

            M_HP_ASSERT_OR_LOG_AND_THROW(
                WAS_FOUND,
                "item::armor::ArmorDetailLoader::LookupArmorDetails(\""
                    << NAME << "\")  failed to find that name in armorDetailsMap_.");

            return details;
        }

        void ArmorDetailLoader::LoadFromGameDataFile()
        {
            for (const auto & ARMOR_TYPE_WRAPPER : ArmorTypeWrapper::MakeCompleteSet())
            {
                if (ARMOR_TYPE_WRAPPER.IsSkin() == false)
                {
                    LoadDetailsForKey(ARMOR_TYPE_WRAPPER.DetailsKeyName());
                }
            }
        }

        void ArmorDetailLoader::LoadDetailsForKey(const std::string & ARMOR_NAME)
        {
            ArmorDetails armorDetails;

            // lookup detail value string in the GameDataFile
            const std::string KEY_STR("heroespath-item-armor-details-" + ARMOR_NAME);
            const std::string VALUE_STR(misc::ConfigFile::Instance()->Value(KEY_STR));

            // break the line of text into comma separated field strings
            std::vector<std::string> fieldsVec;
            misc::SplitByChar(VALUE_STR, fieldsVec, ',', true, true);

            // verify there are eight fields
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (fieldsVec.size() == 6),
                "item::armor::ArmorDetailsLoader::LoadDetailsForKey(armor_name=\""
                    << ARMOR_NAME << "\") using key=\"" << KEY_STR << "\" found value=\""
                    << VALUE_STR << "\" but failed to find the required 7 comma separated fields.");

            armorDetails.name = CleanStringField(fieldsVec[0], false);

            armorDetails.complexity = static_cast<creature::nonplayer::complexity_type::Enum>(
                creature::nonplayer::complexity_type::FromString(
                    CleanStringField(fieldsVec[1], false)));

            armorDetails.price = Coin_t(StringFieldToInt("Price", fieldsVec[2]));
            armorDetails.weight = Weight_t(StringFieldToInt("Weight", fieldsVec[3]));
            armorDetails.armor_rating = Armor_t(StringFieldToInt("ArmorRating", fieldsVec[4]));
            armorDetails.description = CleanStringField(fieldsVec[5], false);

            // store details in the map
            armorDetailsMap_[ARMOR_NAME] = armorDetails;
        }

        int ArmorDetailLoader::StringFieldToInt(
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
                "item::armor::ArmorDetailsLoader::StringFieldToInt(field_name=\""
                    << FIELD_NAME << "\", num_str=\"" << NUM_STR << "\") failed to convert the "
                    << FIELD_NAME << " field of \"" << NUM_STR << "\" into an int.");

            return result;
        }

        const std::string ArmorDetailLoader::CleanStringField(
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

    } // namespace armor
} // namespace item
} // namespace heroespath
