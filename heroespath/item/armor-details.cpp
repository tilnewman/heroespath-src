//
// armor-details.cpp
//
#include "armor-details.hpp"

#include "utilz/boost-string-includes.hpp"

#include "heroespath/game-data-file.hpp"
#include "utilz/assertlogandthrow.hpp"

#include "stringutil/stringhelp.hpp"

#include <vector>


namespace heroespath
{
namespace item
{
namespace armor
{

    ArmorDetailLoaderSPtr_t ArmorDetailLoader::instance_(nullptr);


    ArmorDetailLoader::ArmorDetailLoader()
    :
        armorDetailsMap_()
    {
        LoadArmorDeatilsFromGameDataFile();
    }


    ArmorDetailLoader::~ArmorDetailLoader()
    {}


    ArmorDetailLoaderSPtr_t ArmorDetailLoader::Instance()
    {
        if (instance_.get() == nullptr)
            instance_.reset( new ArmorDetailLoader );

        return instance_;
    }


    const ArmorDetails ArmorDetailLoader::LookupArmorDetails(const std::string & NAME)
    {
        const ArmorDetailMap_t::const_iterator CITER(armorDetailsMap_.find(NAME));
        if (CITER == armorDetailsMap_.end())
        {
            std::ostringstream ss;
            ss << "heroespath::item::armor::ArmorDetailLoader::LookupArmorDetails(\"" << NAME << "\")  failed to find that name in armorDetailsMap_.";
            throw std::runtime_error(ss.str());
        }
        else
            return CITER->second;
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
        const std::string VALUE_STR( GameDataFile::Instance()->GetCopyStr(KEY_STR) );

        //break the line of text into comma separated field strings
        StrVec_t fieldsVec;
        appbase::stringhelp::SplitByChar(VALUE_STR, fieldsVec, ',', true, true);

        //verify there are eight fields
        M_ASSERT_OR_LOGANDTHROW_SS((fieldsVec.size() == 7), "heroespath::item::armor::ArmorDetailsLoader::LoadDetailsForKey(armor_name=\"" << ARMOR_NAME << "\") using key=\"" << KEY_STR << "\" found value=\"" << VALUE_STR << "\" but failed to find the required 7 comma separated fields.");

        armorDetails.name =            CleanStringField(fieldsVec[0], false);
        armorDetails.complexity =      non_player::ownership::complexity_type::FromString( CleanStringField(fieldsVec[1], false) );
        armorDetails.price =           StringFieldToInt("Price", fieldsVec[2]);
        armorDetails.weight =          StringFieldToInt("Weight", fieldsVec[3]);
        armorDetails.armor_rating =    StringFieldToInt("ArmorRating", fieldsVec[4]);
        armorDetails.image_filename =  CleanStringField(fieldsVec[5], true);
        armorDetails.description =     CleanStringField(fieldsVec[6], false);

        //store details in the map
        armorDetailsMap_[ARMOR_NAME] = armorDetails;
    }


    int ArmorDetailLoader::StringFieldToInt(const std::string & FIELD_NAME, const std::string & NUM_STR)
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

        M_ASSERT_OR_LOGANDTHROW_SS((result != ERROR_VAL), "heroespath::item::armor::ArmorDetailsLoader::StringFieldToInt(field_name=\"" << FIELD_NAME << "\", num_str=\"" << NUM_STR << "\") failed to convert the " << FIELD_NAME << " field of \"" << NUM_STR << "\" into an int.");

        return result;
    }


    const std::string ArmorDetailLoader::CleanStringField(const std::string & FIELD_STR, const bool WILL_LOWERCASE)
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
