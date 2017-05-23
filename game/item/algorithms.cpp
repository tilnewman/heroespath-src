//
// algorithms.cpp
//
#include "algorithms.hpp"

#include "utilz/random.hpp"
#include "utilz/vectors.hpp"

#include "game/item/item.hpp"


namespace game
{
namespace item
{

    const std::string Algorithms::Names(const ItemSVec_t & ITEM_SVEC,
                                        const bool         WILL_WRAP,
                                        const bool         WILL_APPEND_AND,
                                        const std::size_t  MAX_COUNT,
                                        const bool         WILL_ELLIPSIS)
    {
        return utilz::Vector::Join<ItemSPtr_t>(ITEM_SVEC,
                                               WILL_WRAP,
                                               WILL_APPEND_AND,
                                               MAX_COUNT,
                                               WILL_ELLIPSIS,
                                               [](const ItemSPtr_t SPTR) -> const std::string
                                                {
                                                    return SPTR->Name();
                                                });
    }


    const ItemSVec_t Algorithms::FindByCategory(const ItemSVec_t &   ITEM_SVEC,
                                                const category::Enum CATEGORY_TYPE,
                                                const bool           IS_MATCH)
    {
        ItemSVec_t resultSVec;

        if (ITEM_SVEC.empty() == false)
            std::copy_if(ITEM_SVEC.begin(),
                         ITEM_SVEC.end(),
                         back_inserter(resultSVec),
                         [CATEGORY_TYPE, IS_MATCH] (const ItemSPtr_t & SPTR) { return ((SPTR->IsCategoryType(CATEGORY_TYPE)) == IS_MATCH); });

        return resultSVec;
    }


    const ItemSVec_t Algorithms::FindByWeaponType(const ItemSVec_t &      ITEM_SVEC,
                                                  const weapon_type::Enum WEAPON_TYPE,
                                                  const bool              IS_MATCH)
    {
        ItemSVec_t resultSVec;

        if (ITEM_SVEC.empty() == false)
            std::copy_if(ITEM_SVEC.begin(),
                         ITEM_SVEC.end(),
                         back_inserter(resultSVec),
                         [WEAPON_TYPE, IS_MATCH] (const ItemSPtr_t & SPTR) { return ((SPTR->IsWeaponType(WEAPON_TYPE)) == IS_MATCH); });

        return resultSVec;
    }


    const ItemSVec_t Algorithms::FindByArmorType(const ItemSVec_t &     ITEM_SVEC,
                                                 const armor_type::Enum ARMOR_TYPE,
                                                 const bool             IS_MATCH)
    {
        ItemSVec_t resultSVec;

        if (ITEM_SVEC.empty() == false)
            std::copy_if(ITEM_SVEC.begin(),
                         ITEM_SVEC.end(),
                         back_inserter(resultSVec),
                         [ARMOR_TYPE, IS_MATCH] (const ItemSPtr_t & SPTR) { return ((SPTR->IsArmorType(ARMOR_TYPE)) == IS_MATCH); });

        return resultSVec;
    }


    const ItemSVec_t Algorithms::FindByMiscType(const ItemSVec_t &    ITEM_SVEC,
                                                const misc_type::Enum MISC_TYPE,
                                                const bool            IS_MATCH)
    {
        ItemSVec_t resultSVec;

        if (ITEM_SVEC.empty() == false)
            std::copy_if(ITEM_SVEC.begin(),
                         ITEM_SVEC.end(),
                         back_inserter(resultSVec),
                         [MISC_TYPE, IS_MATCH] (const ItemSPtr_t & SPTR) { return ((SPTR->IsMiscType(MISC_TYPE)) == IS_MATCH); });

        return resultSVec;
    }


    const ItemSVec_t Algorithms::FindByPixie(const ItemSVec_t & ITEM_SVEC, const bool IS_MATCH)
    {
        ItemSVec_t resultSVec;

        if (ITEM_SVEC.empty() == false)
            std::copy_if(ITEM_SVEC.begin(),
                         ITEM_SVEC.end(),
                         back_inserter(resultSVec),
                         [IS_MATCH] (const ItemSPtr_t & SPTR) { return (SPTR->IsPixie() == IS_MATCH); });

        return resultSVec;
    }


    const ItemSVec_t Algorithms::FindByBroken(const ItemSVec_t & ITEM_SVEC, const bool WILL_KEEP_BROKEN)
    {
        ItemSVec_t resultSVec;

        if (ITEM_SVEC.empty() == false)
            std::copy_if(ITEM_SVEC.begin(),
                         ITEM_SVEC.end(),
                         back_inserter(resultSVec),
                         [WILL_KEEP_BROKEN](const ItemSPtr_t & SPTR) { return (SPTR->IsBroken() == WILL_KEEP_BROKEN); });

        return resultSVec;
    }

}
}
