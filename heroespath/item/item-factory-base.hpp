#ifndef APPBASE_HEROESPATH_ITEM_ITEMFACTORYBASE_INCLUDED
#define APPBASE_HEROESPATH_ITEM_ITEMFACTORYBASE_INCLUDED
//
// item-factory-base.hpp
//  Code that is common to all item factory classes.
//
#include "heroespath/item/item-type-enum.hpp"

#include <memory>
#include <string>


namespace heroespath
{
namespace item
{

    //A base class for all item factories.
    class FactoryBase
    {
        //prevent copy construction
        FactoryBase(const FactoryBase &);

        //prevent copy assignment
        FactoryBase & operator=(const FactoryBase &);

    public:
        FactoryBase();
        virtual ~FactoryBase();

    protected:
        static const std::string Make_Name(const std::string &  BASE_NAME,
                                           const material::Enum MATERIAL_PRI,
                                           const material::Enum MATERIAL_SEC,
                                           const bool           IS_PIXIE_ITEM = false);

        static const std::string Make_Desc(const std::string &  DESC,
                                           const material::Enum MATERIAL_PRI,
                                           const material::Enum MATERIAL_SEC,
                                           const std::string &  EXTRA_NAME = "",
                                           const bool           IS_PIXIE_ITEM = false);

        static const std::string Make_DescClasped(const std::string &  DESC,
                                                  const material::Enum MATERIAL_PRI,
                                                  const material::Enum MATERIAL_SEC,
                                                  const bool           IS_PIXIE_ITEM = false);

        static void AdjustPrice(Coin_t & price,
                                const material::Enum MATERIAL_PRI,
                                const material::Enum MATERIAL_SEC,
                                const bool           IS_PIXIE_ITEM = false);

        static void AdjustWeight(Weight_t & weight,
                                 const material::Enum MATERIAL_PRI,
                                 const material::Enum MATERIAL_SEC);

        static void AdjustArmorRating(stats::Armor_t &     armorRating,
                                      const material::Enum MATERIAL_PRI,
                                      const material::Enum MATERIAL_SEC);

    };

}//namespace item
}//namespace heroespath

#endif //APPBASE_HEROESPATH_ITEM_ITEMFACTORYBASE_INCLUDED
