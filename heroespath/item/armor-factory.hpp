#ifndef APPBASE_HEROESPATH_ITEM_ARMORFACTORY_INCLUDED
#define APPBASE_HEROESPATH_ITEM_ARMORFACTORY_INCLUDED
//
// armor-factory.hpp
//  Code that is responsible for making armor objects.
//
#include "heroespath/item/item-factory-base.hpp"
#include "heroespath/item/armor-types.hpp"

#include <memory>
#include <string>


namespace heroespath
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
        ArmorFactory(const ArmorFactory &);

        //prevent copy assignment
        ArmorFactory & operator=(const ArmorFactory &);

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

        //primary material should always be metal
        static ItemSPtr_t Make_Aventail(const base_type::Enum TYPE,
                                        const material::Enum  MATERIAL_PRI,
                                        const material::Enum  MATERIAL_SEC = material::Nothing);

        //primary material should always be either cloth/softleather/hardleather
        static ItemSPtr_t Make_Cover(const cover_type::Enum TYPE,
                                     const material::Enum   MATERIAL_PRI,
                                     const material::Enum   MATERIAL_SEC = material::Nothing,
                                     const bool             IS_PIXIE_ITEM = false);

    private:
        static ArmorFactorySPtr_t instance_;
    };

}//namespace armor
}//namespace item
}//namespace heroespath

#endif //APPBASE_HEROESPATH_ITEM_ARMORFACTORY_INCLUDED
