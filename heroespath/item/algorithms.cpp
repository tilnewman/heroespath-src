//
// algorithms.cpp
//
#include "algorithms.hpp"
#include "sfml-util/random.hpp"
#include "heroespath/item/item.hpp"


namespace heroespath
{
namespace item
{

    void Algorithms::Shuffle(ItemSVec_t & itemSVec)
    {
        sfml_util::rand::ShuffleVec(itemSVec);
    }


    ItemSPtr_t Algorithms::SelectRandom(const ItemSVec_t & ITEM_SVEC)
    {
        if (ITEM_SVEC.empty())
        {
            return ItemSPtr_t(nullptr);
        }
        else
        {
            return sfml_util::rand::SelectRandom(ITEM_SVEC);
        }
    }


    void Algorithms::Append(ItemSVec_t & a,
                            const ItemSVec_t & B,
                            const bool WILL_UNIQUE)
    {
        if (B.empty() == false)
            std::copy(B.begin(), B.end(), back_inserter(a));

        if (WILL_UNIQUE)
            std::unique(a.begin(), a.end());
    }


    const ItemSVec_t Algorithms::AppendCopy(const ItemSVec_t & A,
                                            const ItemSVec_t & B,
                                            const bool WILL_UNIQUE)
    {
        ItemSVec_t tempSVec(A);
        Append(tempSVec, B, WILL_UNIQUE);
        return tempSVec;
    }


    void Algorithms::Exclude(ItemSVec_t &       srcSVec,
                             const ItemSPtr_t & TO_EXCLUDE_SPTR)
    {
        if (srcSVec.empty() == false)
        {
            auto const FOUND_ITER{ std::find(srcSVec.begin(), srcSVec.end(), TO_EXCLUDE_SPTR) };
            if (FOUND_ITER != srcSVec.end())
                srcSVec.erase(std::remove(srcSVec.begin(), srcSVec.end(), TO_EXCLUDE_SPTR), srcSVec.end());
        }
    }


    void Algorithms::Exclude(ItemSVec_t &       srcSVec,
                             const ItemSVec_t & TO_EXCLUDE_SVEC)
    {
        if ((TO_EXCLUDE_SVEC.empty() == false) && (srcSVec.empty() == false))
        {
            for (auto const & NEXT_ITEM_SPTR : TO_EXCLUDE_SVEC)
            {
                auto const FOUND_ITER{ std::find(srcSVec.begin(), srcSVec.end(), NEXT_ITEM_SPTR) };
                if (FOUND_ITER != srcSVec.end())
                    srcSVec.erase(std::remove(srcSVec.begin(), srcSVec.end(), NEXT_ITEM_SPTR), srcSVec.end());
            }
        }
    }


    const ItemSVec_t Algorithms::ExcludeCopy(const ItemSVec_t & SRC_SVEC,
                                             const ItemSPtr_t & TO_EXCLUDE_SPTR)
    {
        ItemSVec_t tempSVec(SRC_SVEC);
        Exclude(tempSVec, TO_EXCLUDE_SPTR);
        return tempSVec;
    }


    const ItemSVec_t Algorithms::ExcludeCopy(const ItemSVec_t & SRC_SVEC,
                                             const ItemSVec_t & TO_EXCLUDE_SVEC)
    {
        ItemSVec_t tempSVec(SRC_SVEC);
        Exclude(tempSVec, TO_EXCLUDE_SVEC);
        return tempSVec;
    }


    const std::string Algorithms::Names(const ItemSVec_t & ITEM_SVEC, const bool WILL_WRAP, const bool WILL_APPEND_AND)
    {
        if (ITEM_SVEC.empty())
        {
            return "";//don't wrap the empty case
        }
        else
        {
            std::ostringstream ss;

            if (WILL_WRAP)
                ss << "(";

            auto counter = std::size_t{ 0 };
            auto appendedFirstName(false);
            auto const NUM_ITEMS{ ITEM_SVEC.size() };

            for (auto const & NEXT_ITEM_SPTR : ITEM_SVEC)
            {
                ss << ((appendedFirstName) ? ", " : "")
                    << ((WILL_APPEND_AND && (NUM_ITEMS > 1) && ((NUM_ITEMS - 1) == counter++)) ? "and " : "")
                    << NEXT_ITEM_SPTR->Name();

                appendedFirstName = true;
            }

            if (WILL_WRAP)
                ss << ")";

            return ss.str();
        }
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


    const ItemSVec_t Algorithms::FindByBroken(const ItemSVec_t ITEM_SVEC, const bool WILL_KEEP_BROKEN)
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
