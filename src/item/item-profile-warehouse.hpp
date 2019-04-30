// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_ITEMPROFILEWAREHOUSE_HPP_INCLUDED
#define HEROESPATH_ITEM_ITEMPROFILEWAREHOUSE_HPP_INCLUDED
//
// item-profile-warehouse.hpp
//
#include "creature/traits-set.hpp"
#include "game/strong-types.hpp"
#include "item/item-profile-thin-factory.hpp"
#include "item/item-profile-thin.hpp"
#include "item/item-profile.hpp"
#include "item/materials-factory.hpp"
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
        ItemProfileWarehouse(const ItemProfileWarehouse &) = delete;
        ItemProfileWarehouse(ItemProfileWarehouse &&) = delete;
        ItemProfileWarehouse & operator=(const ItemProfileWarehouse &) = delete;
        ItemProfileWarehouse & operator=(ItemProfileWarehouse &&) = delete;

        ItemProfileWarehouse();
        ~ItemProfileWarehouse();

        static misc::NotNull<ItemProfileWarehouse *> Instance();
        static void Acquire();
        static void Release();

        void Initialize();

        const ItemProfileVec_t & GetNormalProfiles()
        {
            EnsureInitialized();
            return profiles_;
        }

        const ItemProfileVec_t & GetReligiousProfiles()
        {
            EnsureInitialized();
            return religiousProfiles_;
        }

        const misc::VectorMap<misc_type::Enum, ItemProfile> & GetQuestProfiles()
        {
            EnsureInitialized();
            return questItemProfilesMap_;
        }

    private:
        void EnsureInitialized();
        void Setup_StandardEquipment();
        void Setup_MiscItems();
        void Setup_NamedEquipment();
        void Setup_SetEquipment();
        void Setup_SummoningItems();

        const ElementEnumVec_t ElementTypesIncludingNone(
            const ItemProfileThin & THIN_PROFILE,
            const named_type::Enum NAMED_TYPE,
            const set_type::Enum SET_TYPE);

        void MakeLoopOverMaterialsAndElementsForEquipment(
            const ItemProfileThin & THIN_PROFILE,
            const named_type::Enum = named_type::Not,
            const set_type::Enum = set_type::Not);

        void MakeForEquipment(
            const ItemProfileThin & THIN_PROFILE,
            const named_type::Enum NAMED_TYPE,
            const set_type::Enum SET_TYPE,
            const element_type::Enum ELEMENT_TYPE,
            const material::Enum MATERIAL_PRIMARY,
            const material::Enum MATERIAL_SECONDARY);

        void MakeLoopOverMaterialsForMisc(
            const misc_type::Enum,
            const element_type::Enum ELEMENT_TYPE,
            const creature::SummonInfo & SUMMON_INFO = creature::SummonInfo());

        void AppendToCorrectProfileCollection(const ItemProfile & PROFILE)
        {
            if (misc_type::IsQuestItem(PROFILE.MiscType()))
            {
                questItemProfilesMap_[PROFILE.MiscType()] = PROFILE;
            }
            else if (PROFILE.IsReligious())
            {
                religiousProfiles_.emplace_back(PROFILE);
            }
            else
            {
                profiles_.emplace_back(PROFILE);
            }
        }

    private:
        static std::unique_ptr<ItemProfileWarehouse> instanceUPtr_;

        MaterialFactory materialFactory_;
        ItemProfileThinFactory thinProfileFactory_;

        // these are the fat profiles that are stored here for use by the Treasure Stage when
        // deciding which items to give the players after defeating creatures in combat.
        ItemProfileVec_t profiles_;
        ItemProfileVec_t religiousProfiles_;

        misc::VectorMap<misc_type::Enum, ItemProfile> questItemProfilesMap_;

        bool hasInitialized_;
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ITEMPROFILEWAREHOUSE_HPP_INCLUDED
