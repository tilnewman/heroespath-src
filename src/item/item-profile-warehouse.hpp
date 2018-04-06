///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef HEROESPATH_ITEM_ITEMPROFILEWAREHOUSE_HPP_INCLUDED
#define HEROESPATH_ITEM_ITEMPROFILEWAREHOUSE_HPP_INCLUDED
//
// item-profile-warehouse.hpp
//
#include "item/item-profile.hpp"
#include "misc/not-null.hpp"
#include "misc/types.hpp"
#include "stats/traits-set.hpp"

#include <memory>
#include <utility>
#include <vector>

namespace heroespath
{
namespace item
{

    using BaseMaterialVecPair_t = std::pair<armor::base_type::Enum, MaterialVecPair_t>;
    using BaseMaterialVecPairVec_t = std::vector<BaseMaterialVecPair_t>;

    // Responsible for storing an ItemProfile for each possible item in the game.
    class ItemProfileWarehouse
    {
    public:
        ItemProfileWarehouse(const ItemProfileWarehouse &) = delete;
        ItemProfileWarehouse(ItemProfileWarehouse &&) = delete;
        ItemProfileWarehouse & operator=(const ItemProfileWarehouse &) = delete;
        ItemProfileWarehouse & operator=(ItemProfileWarehouse &&) = delete;

    public:
        ItemProfileWarehouse();
        ~ItemProfileWarehouse();

        static misc::NotNull<ItemProfileWarehouse *> Instance();
        static void Acquire();
        static void Release();

        void Setup();

        void Setup_StandardEquipment();
        void Setup_UniqueItems();
        void Setup_MiscItems();
        void Setup_NamedEquipment();
        void Setup_SetEquipment();
        void Setup_SummoningItems();

        bool IsSetup() const { return (profiles_.empty() == false); }

        const ItemProfileVec_t & GetNormalProfiles();
        const ItemProfileVec_t & GetReligiousProfiles();

        static Score_t Score(const stats::TraitSet &);

        static Score_t Score(
            const item::material::Enum PRI,
            const item::material::Enum SEC = item::material::Nothing);

        static Score_t Score(const item::weapon::axe_type::Enum);
        static Score_t Score(const item::weapon::bladedstaff_type::Enum);
        static Score_t Score(const item::weapon::club_type::Enum);
        static Score_t Score(const item::weapon::projectile_type::Enum);
        static Score_t Score(const item::weapon::sword_type::Enum);
        static Score_t Score(const item::weapon::whip_type::Enum);

        static Score_t ScoreKnife(const sfml_util::Size::Enum);
        static Score_t ScoreDagger(const sfml_util::Size::Enum);
        static Score_t ScoreStaff();
        static Score_t ScoreQuarterStaff();

        static Score_t Score(const item::armor::base_type::Enum);
        static Score_t Score(const item::armor::cover_type::Enum);
        static Score_t Score(const item::armor::helm_type::Enum);
        static Score_t Score(const item::armor::shield_type::Enum);

        static Score_t ScoreAventail(item::armor::base_type::Enum);
        static Score_t ScoreBracer(item::armor::base_type::Enum);
        static Score_t ScoreShirt(item::armor::base_type::Enum);
        static Score_t ScoreBoots(item::armor::base_type::Enum);
        static Score_t ScorePants(item::armor::base_type::Enum);
        static Score_t ScoreGauntlets(item::armor::base_type::Enum);

    private:
        void SetupFromThinProfile(
            const ItemProfile & THIN_PROFILE,
            const named_type::Enum NAMED_TYPE,
            const set_type::Enum SET_TYPE);

        void SetupFromThinProfile(
            const ItemProfile &,
            const named_type::Enum,
            const set_type::Enum,
            const element_type::Enum,
            const material::Enum,
            const material::Enum,
            const armor::base_type::Enum);

        void SetupFromThinProfile(
            const ItemProfile &,
            const named_type::Enum,
            const set_type::Enum,
            const ElementTypeVec_t &,
            const BaseMaterialVecPairVec_t &);

        const BaseMaterialVecPairVec_t GetMaterialsFromThinProfile(
            const ItemProfile & THIN_PROFILE, const named_type::Enum NAMED_TYPE);

        void SetupProfilesForMiscType(const item::misc_type::Enum);

        static const item::MaterialVecPair_t Materials(const item::weapon::axe_type::Enum);
        static const item::MaterialVecPair_t Materials(const item::weapon::bladedstaff_type::Enum);
        static const item::MaterialVecPair_t Materials(const item::weapon::club_type::Enum);
        static const item::MaterialVecPair_t Materials(const item::weapon::projectile_type::Enum);
        static const item::MaterialVecPair_t Materials(const item::weapon::sword_type::Enum);
        static const item::MaterialVecPair_t Materials(const item::weapon::whip_type::Enum);

        static const item::MaterialVecPair_t MaterialsKnife();
        static const item::MaterialVecPair_t MaterialsDagger();
        static const item::MaterialVecPair_t MaterialsStaff();
        static const item::MaterialVecPair_t MaterialsQuarterStaff();

        static const item::MaterialVecPair_t Materials(const item::armor::cover_type::Enum);
        static const item::MaterialVecPair_t Materials(const item::armor::helm_type::Enum);
        static const item::MaterialVecPair_t Materials(const item::armor::shield_type::Enum);

        static const item::MaterialVecPair_t MaterialsAventail(item::armor::base_type::Enum);
        static const item::MaterialVecPair_t MaterialsBracer(item::armor::base_type::Enum);
        static const item::MaterialVecPair_t MaterialsShirt(item::armor::base_type::Enum);
        static const item::MaterialVecPair_t MaterialsBoots(item::armor::base_type::Enum);
        static const item::MaterialVecPair_t MaterialsPants(item::armor::base_type::Enum);
        static const item::MaterialVecPair_t MaterialsGauntlets(item::armor::base_type::Enum);

        static const item::MaterialVec_t
            MaterialsPrimaryFromArmorBaseType(const item::armor::base_type::Enum);

        static const item::MaterialVecPair_t
            MaterialsFromBaseType(const item::armor::base_type::Enum);
        static const item::MaterialVecPair_t
            MaterialsFromBaseTypeNoCloth(const item::armor::base_type::Enum);

        static const item::ItemProfileVec_t ThinProfilesWeaponsSwords();
        static const item::ItemProfileVec_t ThinProfilesWeaponsProjectiles();
        static const item::ItemProfileVec_t ThinProfilesWeaponsAll();
        static const item::ItemProfileVec_t ThinProfilesArmor(const bool WILL_INCLUDE_COVERINGS);
        static const item::ItemProfileVec_t ThinProfiles(const item::named_type::Enum);
        static const item::ItemProfileVec_t ThinProfiles(const item::set_type::Enum);

        void AppendProfileToTheCorrectVector(const ItemProfile & PROFILE)
        {
            if (PROFILE.IsReligious())
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
        ItemProfileVec_t profiles_;
        ItemProfileVec_t religiousProfiles_;
    };
} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ITEMPROFILEWAREHOUSE_HPP_INCLUDED
