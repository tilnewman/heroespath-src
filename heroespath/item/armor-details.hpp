#ifndef APPBASE_HEROESPATH_ITEM_ARMORDETAILS_INCLUDED
#define APPBASE_HEROESPATH_ITEM_ARMORDETAILS_INCLUDED
//
// armor-details.hpp
//  Code that loads detailed armor data from the GameDataFile.
//
#include "heroespath/item/types.hpp"
#include "heroespath/stats/types.hpp"
#include "heroespath/non-player/ownership-profile.hpp"

#include <memory>
#include <map>
#include <string>


namespace heroespath
{
namespace item
{
namespace armor
{

    //a simple wrapper for armor details
    struct ArmorDetails
    {
        ArmorDetails()
        :
            name            (""),
            image_filename  (""),
            description     (""),
            price           (0),
            weight          (0),
            armor_rating    (0),
            complexity      (non_player::ownership::complexity_type::Count)
        {}

        std::string     name;
        std::string     image_filename;
        std::string     description;
        item::Coin_t    price;
        item::Weight_t  weight;
        stats::Armor_t  armor_rating;
        non_player::ownership::complexity_type::Enum complexity;
    };


    //name to details mapping
    using ArmorDetailMap_t = std::map<std::string, ArmorDetails>;


    //types required by the singleton implementation
    class ArmorDetailLoader;
    using ArmorDetailLoaderSPtr_t = std::shared_ptr<ArmorDetailLoader>;


    //A singleton class that loads detailed armor info from the GameDataFile.
    class ArmorDetailLoader
    {
        //prevent copy assignment
        ArmorDetailLoader & operator=(const ArmorDetailLoader &);

        //prevent copy construction
        ArmorDetailLoader(const ArmorDetailLoader &);

        //prevent non-singleton construction
        ArmorDetailLoader();

    public:
        virtual ~ArmorDetailLoader();
        static ArmorDetailLoaderSPtr_t Instance();
        const ArmorDetails LookupArmorDetails(const std::string & NAME);

    private:
        void LoadArmorDeatilsFromGameDataFile();
        void LoadDetailsForKey(const std::string & ARMOR_NAME);
        int StringFieldToInt(const std::string & FIELD_NAME, const std::string & NUM_STR);
        const std::string CleanStringField(const std::string & FIELD_STR, const bool WILL_LOWERCASE);

    private:
        static ArmorDetailLoaderSPtr_t instance_;
        ArmorDetailMap_t armorDetailsMap_;
    };


}
}
}
#endif //APPBASE_HEROESPATH_ITEM_ARMORDETAILS_INCLUDED
