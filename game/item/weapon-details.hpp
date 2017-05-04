#ifndef GAME_ITEM_WEAPONFDETAILS_INCLUDED
#define GAME_ITEM_WEAPONFDETAILS_INCLUDED
//
// weapons-details.hpp
//  Code that loads detailed weapon data from the GameDataFile.
//
#include "game/non-player/ownership-profile.hpp"

#include <memory>
#include <map>
#include <string>


namespace game
{
namespace item
{
namespace weapon
{

    //a simple wrapper for weapon details
    struct WeaponDetails
    {
        WeaponDetails()
        :
            name            (""),
            image_filename  (""),
            description     (""),
            price           (0),
            weight          (0),
            damage_min      (0),
            damage_max      (0),
            handedness      (item::category::OneHanded),
            complexity      (non_player::ownership::complexity_type::Count)
        {}

        std::string     name;
        std::string     image_filename;
        std::string     description;
        item::Coin_t    price;
        item::Weight_t  weight;
        stats::Health_t damage_min;
        stats::Health_t damage_max;
        item::category::Enum handedness;
        non_player::ownership::complexity_type::Enum complexity;
    };


    //name to details mapping
    using WeaponDetailMap_t = std::map<std::string, WeaponDetails>;


    //types required by the singleton implementation
    class WeaponDetailLoader;
    using WeaponDetailLoaderSPtr_t = std::shared_ptr<WeaponDetailLoader>;


    //A singleton class that loads detailed weapon info from the GameDataFile.
    class WeaponDetailLoader
    {
        //prevent copy assignment
        WeaponDetailLoader & operator=(const WeaponDetailLoader &);

        //prevent copy construction
        WeaponDetailLoader(const WeaponDetailLoader &);

        //prevent non-singleton construction
        WeaponDetailLoader();

    public:
        virtual ~WeaponDetailLoader();
        static WeaponDetailLoaderSPtr_t Instance();
        const WeaponDetails LookupWeaponDetails(const std::string & NAME);

    private:
        void LoadWeaponDeatilsFromGameDataFile();
        void LoadDetailsForKey(const std::string & WEAPON_NAME);
        int StringFieldToInt(const std::string & FIELD_NAME, const std::string & NUM_STR);
        const std::string CleanStringField(const std::string & FIELD_STR, const bool WILL_LOWERCASE);

    private:
        static WeaponDetailLoaderSPtr_t instance_;
        WeaponDetailMap_t weaponDetailsMap_;
    };

}
}
}
#endif //GAME_ITEM_WEAPONFDETAILS_INCLUDED
