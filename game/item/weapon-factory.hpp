#ifndef GAME_ITEM_WEAPONFACTORY_INCLUDED
#define GAME_ITEM_WEAPONFACTORY_INCLUDED
//
// weapons-factory.hpp
//  Code responsible for creating weapon objects.
//
#include "sfml-util/size-enum.hpp"

#include "game/item/item-factory-base.hpp"
#include "game/item/weapon-types.hpp"

#include <memory>
#include <tuple>
#include <string>


namespace game
{

//forward declaration
namespace creature
{
    class Creature;
    using CreatureCPtrC_t = const Creature * const;
}

namespace item
{
    class Item;
    using ItemSPtr_t = std::shared_ptr<Item>;

namespace weapon
{

    //types required by the WeaponFactory singleton implementation
    class WeaponFactory;
    using WeaponFactorySPtr_t = std::shared_ptr<WeaponFactory>;


    //A singleton class that creates weapon objects.
    class WeaponFactory : public FactoryBase
    {
        //prevent copy construction
        WeaponFactory(const WeaponFactory &) =delete;

        //prevent copy assignment
        WeaponFactory & operator=(const WeaponFactory &) =delete;

        //prevent non-singleton construction
        WeaponFactory();

    public:
        virtual ~WeaponFactory();

        static WeaponFactorySPtr_t Instance();

        static ItemSPtr_t Make_Fists();

        static ItemSPtr_t Make_Claws(creature::CreatureCPtrC_t);

        static ItemSPtr_t Make_Tendrils(creature::CreatureCPtrC_t);

        static ItemSPtr_t Make_Bite(creature::CreatureCPtrC_t);

        static ItemSPtr_t Make_Breath(creature::CreatureCPtrC_t);

        static ItemSPtr_t Make_Knife(const bool                  IS_DAGGER     = false,
                                     const sfml_util::Size::Enum SIZE          = sfml_util::Size::Medium,
                                     const material::Enum        MATERIAL_PRI  = material::Steel,
                                     const material::Enum        MATERIAL_SEC  = material::Nothing,
                                     const bool                  IS_PIXIE_ITEM = false);

        static ItemSPtr_t Make_Sword(const sword_type::Enum SWORD_TYPE,
                                     const material::Enum   MATERIAL_PRI,
                                     const material::Enum   MATERIAL_SEC);

        static ItemSPtr_t Make_Axe(const axe_type::Enum AXE_TYPE,
                                   const material::Enum MATERIAL_PRI,
                                   const material::Enum MATERIAL_SEC);

        static ItemSPtr_t Make_Club(const club_type::Enum CLUB_TYPE,
                                    const material::Enum  MATERIAL_PRI,
                                    const material::Enum  MATERIAL_SEC);

        //pri material determined by WHIP_TYPE, sec material describes the handle or its decoration
        static ItemSPtr_t Make_Whip(const whip_type::Enum WHIP_TYPE,
                                     const material::Enum  MATERIAL_SEC);

        //pri material determined by PROJ_TYPE, sec material describes the handle or its decoration
        static ItemSPtr_t Make_Projectile(const projectile_type::Enum PROJ_TYPE,
                                          const material::Enum        MATERIAL_SEC);

        static ItemSPtr_t Make_Staff(const bool           IS_QUARTERSTAFF,
                                     const material::Enum MATERIAL_PRI,
                                     const material::Enum MATERIAL_SEC,
                                     const bool           IS_PIXIE_ITEM = false);

        static ItemSPtr_t Make_BladedStaff(const bladedstaff_type::Enum STAFF_TYPE,
                                           const material::Enum         MATERIAL_PRI,
                                           const material::Enum         MATERIAL_SEC);

    private:
        static WeaponFactorySPtr_t instance_;
    };

}
}
}
#endif //GAME_ITEM_WEAPONFACTORY_INCLUDED
