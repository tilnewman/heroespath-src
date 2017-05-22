//
// weapon-details.cpp
//
#include "weapon-details.hpp"

#include "utilz/boost-string-includes.hpp"

#include "game/game-data-file.hpp"
#include "utilz/assertlogandthrow.hpp"

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

    WeaponDetailLoaderSPtr_t WeaponDetailLoader::instance_(nullptr);


    WeaponDetailLoader::WeaponDetailLoader()
    :
        weaponDetailsMap_()
    {
        LoadWeaponDeatilsFromGameDataFile();
    }


    WeaponDetailLoader::~WeaponDetailLoader()
    {}


    WeaponDetailLoaderSPtr_t WeaponDetailLoader::Instance()
    {
        if (instance_.get() == nullptr)
            instance_.reset( new WeaponDetailLoader );

        return instance_;
    }


    const WeaponDetails WeaponDetailLoader::LookupWeaponDetails(const std::string & NAME)
    {
        const WeaponDetailMap_t::const_iterator CITER( weaponDetailsMap_.find(NAME) );
        if (CITER == weaponDetailsMap_.end())
        {
            std::ostringstream ss;
            ss << "game::item::weapon::WeaponDetailLoader::LookupWeaponDetails(\"" << NAME << "\")  failed to find that name in weaponDetailsMap_.";
            throw std::runtime_error(ss.str());
        }
        else
            return CITER->second;
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
        LoadDetailsForKey("Longbow");
        LoadDetailsForKey("Compositebow");
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
        LoadDetailsForKey("Breath");
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
        M_ASSERT_OR_LOGANDTHROW_SS((fieldsVec.size() == 9), "game::item::weapon::WeaponDetailsLoader::LoadDetailsForKey(weapon_name=\"" << WEAPON_NAME << "\") using key=\"" << KEY_STR << "\" found value=\"" << VALUE_STR << "\" but failed to find the required 9 comma separated fields.");

        weaponDetails.name =            CleanStringField(fieldsVec[0], false);
        weaponDetails.complexity =      non_player::ownership::complexity_type::FromString( CleanStringField(fieldsVec[1], false) );
        weaponDetails.price =           StringFieldToInt("Price", fieldsVec[2]);
        weaponDetails.weight =          static_cast<Weight_t>(StringFieldToInt("Weight", fieldsVec[3]));
        weaponDetails.damage_min =      StringFieldToInt("DamageMin", fieldsVec[4]);
        weaponDetails.damage_max =      StringFieldToInt("DamageMax", fieldsVec[5]);
        weaponDetails.handedness =      ((CleanStringField(fieldsVec[6], true) == "two-handed") ? item::category::TwoHanded : item::category::OneHanded );
        weaponDetails.image_filename =  CleanStringField(fieldsVec[7], true);
        weaponDetails.description =     CleanStringField(fieldsVec[8], false);

        //store details in the map
        weaponDetailsMap_[WEAPON_NAME] = weaponDetails;
    }


    int WeaponDetailLoader::StringFieldToInt(const std::string & FIELD_NAME, const std::string & NUM_STR)
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

        M_ASSERT_OR_LOGANDTHROW_SS((result != ERROR_VAL), "game::item::weapon::WeaponDetailsLoader::StringFieldToInt(field_name=\"" << FIELD_NAME << "\", num_str=\"" << NUM_STR << "\") failed to convert the " << FIELD_NAME << " field of \"" << NUM_STR << "\" into an int.");

        return result;
    }


    const std::string WeaponDetailLoader::CleanStringField(const std::string & FIELD_STR, const bool WILL_LOWERCASE)
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
