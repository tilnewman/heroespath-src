// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_ITEM_PROFILE_WAREHOUSE_HPP_INCLUDED
#define HEROESPATH_ITEM_ITEM_PROFILE_WAREHOUSE_HPP_INCLUDED
//
// item-profile-warehouse.hpp
//
#include "creature/summon-info.hpp"
#include "item/item-profile.hpp"
#include "misc/not-null.hpp"

#include <memory>
#include <utility>
#include <vector>

namespace heroespath
{
namespace item
{

    // Responsible for creating and storing an ItemProfile for each item in the game that can be
    // found after combat in the random assortment of things left behind by a group of enemy
    // creatures.
    //
    // Also responsible for calculating non-magical based scores for ItemProfiles.
    class ItemProfileWarehouse
    {
    public:
        ItemProfileWarehouse();
        ~ItemProfileWarehouse();

        ItemProfileWarehouse(const ItemProfileWarehouse &) = delete;
        ItemProfileWarehouse(ItemProfileWarehouse &&) = delete;
        ItemProfileWarehouse & operator=(const ItemProfileWarehouse &) = delete;
        ItemProfileWarehouse & operator=(ItemProfileWarehouse &&) = delete;

        static misc::NotNull<ItemProfileWarehouse *> Instance();
        static void Create();
        static void Destroy();

        void Initialize();

        const ItemProfileVec_t & Normal() const { return normal_; }
        const ItemProfileVec_t & Religious() const { return religious_; }
        const ItemProfileVec_t & Quest() const { return quest_; }
        const ItemProfileVec_t & Fallback() const { return fallback_; }

    private:
        void MakeStandardEquipment();
        void MakeMiscNonSummoning();
        void MakeMiscSummoning();
        void MakeNamed();
        void MakeSet();

        ElementList::Enum ElementsAllowed(
            const ItemTemplate & TEMPLATE, const Named::Enum NAMED, const Set::Enum SET) const;

        Misc::Enum MiscTypeOfSummoningItem(const creature::SummonInfo & SUMMON_INFO) const;

        void MakeLoopOverElements(
            const ItemTemplate & TEMPLATE,
            const Misc::Enum MISC = Misc::Count,
            const Named::Enum NAMED = Named::Count,
            const Set::Enum SET = Set::Count,
            const creature::SummonInfo & SUMMON_INFO = creature::SummonInfoEmpty);

        void MakeLoopOverMaterials(
            const ItemTemplate & TEMPLATE,
            const Misc::Enum MISC,
            const Named::Enum NAMED,
            const Set::Enum SET,
            const creature::SummonInfo & SUMMON_INFO,
            const Element::Enum ELEMENT);

        void AppendBothPixieVersionsToCollection(
            const ItemTemplate & TEMPLATE,
            const Misc::Enum MISC,
            const Named::Enum NAMED,
            const Set::Enum SET,
            const creature::SummonInfo & SUMMON_INFO,
            const Element::Enum ELEMENT,
            const MaterialPair_t & MATERIAL_PAIR);

        void AppendToCollection(const ItemProfile & PROFILE);

    private:
        static std::unique_ptr<ItemProfileWarehouse> instanceUPtr_;

        // these are the fat profiles that are stored here for use by the Treasure Stage when
        // deciding which items to give the players after defeating creatures in combat.
        ItemProfileVec_t normal_;
        ItemProfileVec_t religious_;
        ItemProfileVec_t quest_;
        ItemProfileVec_t fallback_;
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ITEM_PROFILE_WAREHOUSE_HPP_INCLUDED
