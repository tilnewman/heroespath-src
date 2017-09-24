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
// weapon-details.cpp
//
#include "weapon-details.hpp"

#include "game/game-data-file.hpp"

#include "misc/boost-string-includes.hpp"
#include "misc/assertlogandthrow.hpp"

#include "stringutil/stringhelp.hpp"

#include <boost/lexical_cast.hpp>

#include <vector>
#include <sstream>
#include <exception>


namespace game
{
namespace item
{
namespace weapon
{

    std::unique_ptr<WeaponDetailLoader> WeaponDetailLoader::instanceUPtr_{ nullptr };


    WeaponDetailLoader::WeaponDetailLoader()
    :
        weaponDetailsMap_()
    {
        M_HP_LOG_DBG("Singleton Construction: WeaponDetailLoader");
        LoadWeaponDeatilsFromGameDataFile();
    }


    WeaponDetailLoader::~WeaponDetailLoader()
    {
        M_HP_LOG_DBG("Singleton Destruction: WeaponDetailLoader");
    }


    WeaponDetailLoader * WeaponDetailLoader::Instance()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            M_HP_LOG_WRN("Singleton Instance() before Acquire(): WeaponDetailLoader");
            Acquire();
        }

        return instanceUPtr_.get();
    }


    void WeaponDetailLoader::Acquire()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            instanceUPtr_.reset(new WeaponDetailLoader);
        }
        else
        {
            M_HP_LOG_WRN("Singleton Acquire() after Construction: WeaponDetailLoader");
        }
    }


    void WeaponDetailLoader::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((instanceUPtr_.get() != nullptr),
            "game::item::weapon::WeaponDetailLoader::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }


    const WeaponDetails WeaponDetailLoader::LookupWeaponDetails(const std::string & NAME)
    {
        const WeaponDetailMap_t::const_iterator CITER( weaponDetailsMap_.find(NAME) );
        if (CITER == weaponDetailsMap_.end())
        {
            std::ostringstream ss;
            ss << "game::item::weapon::WeaponDetailLoader::LookupWeaponDetails(\""
                << NAME << "\")  failed to find that name in weaponDetailsMap_.";

            throw std::runtime_error(ss.str());
        }
        else
        {
            return CITER->second;
        }
    }


    void WeaponDetailLoader::LoadWeaponDeatilsFromGameDataFile()
    {
        LoadDetailsForKey("Claymore");
        LoadDetailsForKey("Longsword");
        LoadDetailsForKey("Flamberg");
        LoadDetailsForKey("KnightlySword");
        LoadDetailsForKey("Broadsword");
        LoadDetailsForKey("Falcata");
        LoadDetailsForKey("Saber");
        LoadDetailsForKey("Cutlass");
        LoadDetailsForKey("Rapier");
        LoadDetailsForKey("Gladius");
        LoadDetailsForKey("Shortsword");
        LoadDetailsForKey("Handaxe");
        LoadDetailsForKey("Sickle");
        LoadDetailsForKey("Battleaxe");
        LoadDetailsForKey("Waraxe");
        LoadDetailsForKey("Spiked");
        LoadDetailsForKey("Maul");
        LoadDetailsForKey("Mace");
        LoadDetailsForKey("Warhammer");
        LoadDetailsForKey("Bullwhip");
        LoadDetailsForKey("Flail");
        LoadDetailsForKey("MaceAndChain");
        LoadDetailsForKey("Blowpipe");
        LoadDetailsForKey("Sling");
        LoadDetailsForKey("Shortbow");
        LoadDetailsForKey("Longbow");
        LoadDetailsForKey("CompositeBow");
        LoadDetailsForKey("Crossbow");
        LoadDetailsForKey("Spear");
        LoadDetailsForKey("ShortSpear");
        LoadDetailsForKey("Scythe");
        LoadDetailsForKey("Pike");
        LoadDetailsForKey("Partisan");
        LoadDetailsForKey("Halberd");
        LoadDetailsForKey("Staff");
        LoadDetailsForKey("Quarterstaff");
        LoadDetailsForKey("Bite");
        LoadDetailsForKey("Claws");
        LoadDetailsForKey("Fists");
        LoadDetailsForKey("Tendrils");
        LoadDetailsForKey("Knife");
        LoadDetailsForKey("BreathFirebrand");
        LoadDetailsForKey("BreathSylavin");
    }


    void WeaponDetailLoader::LoadDetailsForKey(const std::string & WEAPON_NAME)
    {
        using StrVec_t = std::vector<std::string>;

        WeaponDetails weaponDetails;

        //lookup detail value string in the GameDataFile
        const std::string KEY_STR("heroespath-item-weapon-details-" + WEAPON_NAME);
        const std::string VALUE_STR( GameDataFile::Instance()->GetCopyStr(KEY_STR) );

        //break the line of text into comma separated field strings
        StrVec_t fieldsVec;
        appbase::stringhelp::SplitByChar(VALUE_STR, fieldsVec, ',', true, true);

        //verify there are eight fields
        M_ASSERT_OR_LOGANDTHROW_SS((fieldsVec.size() == 8),
            "game::item::weapon::WeaponDetailsLoader::LoadDetailsForKey(weapon_name=\""
            << WEAPON_NAME << "\") using key=\"" << KEY_STR << "\" found value=\""
            << VALUE_STR << "\" but failed to find the required 9 comma separated fields.");

        weaponDetails.name = CleanStringField(fieldsVec[0], false);

        weaponDetails.complexity = non_player::ownership::complexity_type::FromString(
            CleanStringField(fieldsVec[1], false) );

        weaponDetails.price = StringFieldToInt("Price", fieldsVec[2]);
        weaponDetails.weight = static_cast<stats::Trait_t>(StringFieldToInt("Weight", fieldsVec[3]));

        weaponDetails.damage_min = StringFieldToInt("DamageMin", fieldsVec[4]);
        weaponDetails.damage_max = StringFieldToInt("DamageMax", fieldsVec[5]);

        weaponDetails.handedness = ((CleanStringField(fieldsVec[6], true) == "two-handed") ?
            item::category::TwoHanded : item::category::OneHanded );

        weaponDetails.description = CleanStringField(fieldsVec[7], false);

        //store details in the map
        weaponDetailsMap_[WEAPON_NAME] = weaponDetails;
    }


    int WeaponDetailLoader::StringFieldToInt(const std::string & FIELD_NAME,
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
            "game::item::weapon::WeaponDetailsLoader::StringFieldToInt(field_name=\""
            << FIELD_NAME << "\", num_str=\"" << NUM_STR << "\") failed to convert the "
            << FIELD_NAME << " field of \"" << NUM_STR << "\" into an int.");

        return result;
    }


    const std::string WeaponDetailLoader::CleanStringField(const std::string & FIELD_STR,
                                                           const bool          WILL_LOWERCASE)
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
}
}
}
