#ifndef GAME_ITEM_ARMORFACTORY_INCLUDED
#define GAME_ITEM_ARMORFACTORY_INCLUDED
//
// armor-factory.hpp
//  Code that is responsible for making armor objects.
//
#include "game/item/item-factory-base.hpp"
#include "game/item/armor-types.hpp"
#include "game/stats/types.hpp"

#include <memory>
#include <string>


namespace game
{
namespace item
{
    class Item;
    using ItemSPtr_t = std::shared_ptr<Item>;

namespace armor
{

    //types required by the singleton implementation
    class ArmorFactory;
    using ArmorFactorySPtr_t = std::shared_ptr<ArmorFactory>;


    //A singleton class responsible for creating armor objects.
    class ArmorFactory : public FactoryBase
    {
        //prevent copy construction
        ArmorFactory(const ArmorFactory &) =delete;

        //prevent copy assignment
        ArmorFactory & operator=(const ArmorFactory &) =delete;

        //prevent non-singleton construction
        ArmorFactory();

    public:
        virtual ~ArmorFactory();

        static ArmorFactorySPtr_t Instance();

        //primary material can be wood, secondary material can be Nothing
        static ItemSPtr_t Make_Shield(const shield_type::Enum TYPE,
                                      const material::Enum    MATERIAL_PRI,
                                      const material::Enum    MATERIAL_SEC = material::Nothing);

        //if type==Leather, then the pri and sec materials will be forced to HardLeather and Nothing
        static ItemSPtr_t Make_Helm(const helm_type::Enum TYPE,
                                    const material::Enum  MATERIAL_PRI,
                                    const material::Enum  MATERIAL_SEC = material::Nothing);

        static ItemSPtr_t Make_Gauntlets(const base_type::Enum TYPE,
                                         const material::Enum  MATERIAL_PRI,
                                         const material::Enum  MATERIAL_SEC  = material::Nothing,
                                         const bool            IS_PIXIE_ITEM = false);

        //primary material is always cloth
        static ItemSPtr_t Make_Pants(const base_type::Enum TYPE,
                                     const material::Enum  MATERIAL_SEC = material::Nothing,
                                     const bool            IS_PIXIE_ITEM = false);

        static ItemSPtr_t Make_Boots(const base_type::Enum TYPE,
                                     const material::Enum  MATERIAL_PRI,
                                     const material::Enum  MATERIAL_SEC = material::Nothing,
                                     const bool            IS_PIXIE_ITEM = false);

        //primary material is always cloth, MATERIAL_SEC can be nothing
        static ItemSPtr_t Make_Shirt(const base_type::Enum TYPE,
                                     const material::Enum  MATERIAL_SEC = material::Nothing,
                                     const bool            IS_PIXIE_ITEM = false);

        static ItemSPtr_t Make_Bracer(const base_type::Enum TYPE,
                                      const material::Enum  MATERIAL_PRI,
                                      const material::Enum  MATERIAL_SEC = material::Nothing,
                                      const bool            IS_PIXIE_ITEM = false);

        //primary material should always be metalf
        static ItemSPtr_t Make_Aventail(const base_type::Enum TYPE,
                                        const material::Enum  MATERIAL_PRI,
                                        const material::Enum  MATERIAL_SEC = material::Nothing);

        //primary material should always be either cloth/softleather/hardleather
        static ItemSPtr_t Make_Cover(const cover_type::Enum TYPE,
                                     const material::Enum   MATERIAL_PRI,
                                     const material::Enum   MATERIAL_SEC = material::Nothing,
                                     const bool             IS_PIXIE_ITEM = false);

        static ItemSPtr_t Make_Skin(const material::Enum MATERIAL,
                                    const stats::Rank_t  CREATURE_RANK,
                                    const bool           IS_PIXIE_ITEM);

    private:
        static ArmorFactorySPtr_t instance_;
    };

}//namespace armor
}//namespace item
}//namespace game

#endif //GAME_ITEM_ARMORFACTORY_INCLUDED
