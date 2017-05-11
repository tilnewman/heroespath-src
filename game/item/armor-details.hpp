#ifndef GAME_ITEM_ARMORDETAILS_INCLUDED
#define GAME_ITEM_ARMORDETAILS_INCLUDED
//
// armor-details.hpp
//  Code that loads detailed armor data from the GameDataFile.
//
#include "game/item/types.hpp"
#include "game/stats/types.hpp"
#include "game/non-player/ownership-profile.hpp"

#include <memory>
#include <map>
#include <string>


namespace game
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
        ArmorDetailLoader & operator=(const ArmorDetailLoader &) =delete;

        //prevent copy construction
        ArmorDetailLoader(const ArmorDetailLoader &) =delete;

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
#endif //GAME_ITEM_ARMORDETAILS_INCLUDED
