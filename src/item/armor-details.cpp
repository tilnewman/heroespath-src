// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// armor-details.cpp
//
#include "armor-details.hpp"

#include "game/game-data-file.hpp"

#include "misc/boost-string-includes.hpp"
#include "misc/assertlogandthrow.hpp"

#include "stringutil/stringhelp.hpp"

#include <vector>


namespace heroespath
{
namespace item
{
namespace armor
{

    std::unique_ptr<ArmorDetailLoader> ArmorDetailLoader::instanceUPtr_{ nullptr };


    ArmorDetailLoader::ArmorDetailLoader()
    :
        armorDetailsMap_()
    {
        M_HP_LOG_DBG("Singleton Construction: ArmorDetailLoader");
        LoadArmorDeatilsFromGameDataFile();
    }


    ArmorDetailLoader::~ArmorDetailLoader()
    {
        M_HP_LOG_DBG("Singleton Destruction: ArmorDetailLoader");
    }


    ArmorDetailLoader * ArmorDetailLoader::Instance()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            M_HP_LOG_WRN("Singleton Instance() before Acquire(): ArmorDetailLoader");
            Acquire();
        }

        return instanceUPtr_.get();
    }


    void ArmorDetailLoader::Acquire()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            instanceUPtr_ = std::make_unique<ArmorDetailLoader>();
        }
        else
        {
            M_HP_LOG_WRN("Singleton Acquire() after Construction: ArmorDetailLoader");
        }
    }


    void ArmorDetailLoader::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((instanceUPtr_.get() != nullptr),
            "item::armor::ArmorDetailLoader::Release() found instanceUPtr that was null.");
        instanceUPtr_.reset();
    }


    const ArmorDetails ArmorDetailLoader::LookupArmorDetails(const std::string & NAME)
    {
        const ArmorDetailMap_t::const_iterator CITER(armorDetailsMap_.find(NAME));
        if (CITER == armorDetailsMap_.end())
        {
            std::ostringstream ss;
            ss << "item::armor::ArmorDetailLoader::LookupArmorDetails(\"" << NAME
                << "\")  failed to find that name in armorDetailsMap_.";

            throw std::runtime_error(ss.str());
        }
        else
        {
            return CITER->second;
        }
    }


    void ArmorDetailLoader::LoadArmorDeatilsFromGameDataFile()
    {
        LoadDetailsForKey("Buckler");
        LoadDetailsForKey("Kite");
        LoadDetailsForKey("Heater");
        LoadDetailsForKey("Pavis");
        LoadDetailsForKey("Leather");
        LoadDetailsForKey("Kettle");
        LoadDetailsForKey("MailCoif");
        LoadDetailsForKey("Archers");
        LoadDetailsForKey("Bascinet");
        LoadDetailsForKey("Great");
        LoadDetailsForKey("Gloves");
        LoadDetailsForKey("MailGauntlets");
        LoadDetailsForKey("ScaleGauntlets");
        LoadDetailsForKey("PlateGauntlets");
        LoadDetailsForKey("PlainPants");
        LoadDetailsForKey("MailPants");
        LoadDetailsForKey("ScalePants");
        LoadDetailsForKey("PlatePants");
        LoadDetailsForKey("PlainBoots");
        LoadDetailsForKey("MailBoots");
        LoadDetailsForKey("ScaleBoots");
        LoadDetailsForKey("PlateBoots");
        LoadDetailsForKey("PlainShirt");
        LoadDetailsForKey("MailShirt");
        LoadDetailsForKey("ScaleShirt");
        LoadDetailsForKey("PlateShirt");
        LoadDetailsForKey("PlainBracers");
        LoadDetailsForKey("MailBracers");
        LoadDetailsForKey("ScaleBracers");
        LoadDetailsForKey("PlateBracers");
        LoadDetailsForKey("PlainAventail");
        LoadDetailsForKey("MailAventail");
        LoadDetailsForKey("ScaleAventail");
        LoadDetailsForKey("PlateAventail");
        LoadDetailsForKey("Cape");
        LoadDetailsForKey("Vest");
        LoadDetailsForKey("Robe");
        LoadDetailsForKey("Cloak");
    }


    void ArmorDetailLoader::LoadDetailsForKey(const std::string & ARMOR_NAME)
    {
        using StrVec_t = std::vector<std::string>;

        ArmorDetails armorDetails;

        //lookup detail value string in the GameDataFile
        const std::string KEY_STR("heroespath-item-armor-details-" + ARMOR_NAME);
        const std::string VALUE_STR( game::GameDataFile::Instance()->GetCopyStr(KEY_STR) );

        //break the line of text into comma separated field strings
        StrVec_t fieldsVec;
        appbase::stringhelp::SplitByChar(VALUE_STR, fieldsVec, ',', true, true);

        //verify there are eight fields
        M_ASSERT_OR_LOGANDTHROW_SS((fieldsVec.size() == 6),
            "item::armor::ArmorDetailsLoader::LoadDetailsForKey(armor_name=\""
            << ARMOR_NAME << "\") using key=\"" << KEY_STR << "\" found value=\""
            << VALUE_STR << "\" but failed to find the required 7 comma separated fields.");

        armorDetails.name = CleanStringField(fieldsVec[0], false);

        armorDetails.complexity = non_player::ownership::complexity_type::FromString(
            CleanStringField(fieldsVec[1], false) );

        armorDetails.price = Coin_t(StringFieldToInt("Price", fieldsVec[2]));
        armorDetails.weight = Weight_t(StringFieldToInt("Weight", fieldsVec[3]));
        armorDetails.armor_rating = Armor_t(StringFieldToInt("ArmorRating", fieldsVec[4]));
        armorDetails.description = CleanStringField(fieldsVec[5], false);

        //store details in the map
        armorDetailsMap_[ARMOR_NAME] = armorDetails;
    }


    int ArmorDetailLoader::StringFieldToInt(
        const std::string & FIELD_NAME,
        const std::string & NUM_STR)
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

        M_ASSERT_OR_LOGANDTHROW_SS((result != ERROR_VAL),
            "item::armor::ArmorDetailsLoader::StringFieldToInt(field_name=\""
            << FIELD_NAME << "\", num_str=\"" << NUM_STR << "\") failed to convert the "
            << FIELD_NAME << " field of \"" << NUM_STR << "\" into an int.");

        return result;
    }


    const std::string ArmorDetailLoader::CleanStringField(
        const std::string & FIELD_STR,
        const bool          WILL_LOWERCASE)
    {
        using namespace boost::algorithm;

        if (WILL_LOWERCASE)
            return to_lower_copy(trim_copy(erase_all_copy(FIELD_STR, "\"")));
        else
            return trim_copy(erase_all_copy(FIELD_STR, "\""));
    }
}
}
}
