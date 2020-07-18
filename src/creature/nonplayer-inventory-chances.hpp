// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_NONPLAYER_INVENTORY_CHANCES_INCLUDED
#define HEROESPATH_CREATURE_NONPLAYER_INVENTORY_CHANCES_INCLUDED
//
// nonplayer-inventory-chances.hpp
//  Code responsible for creating non-player-characters items. (clothes/weapons/armor/jewelry/etc)
//
/*
#include "creature/complexity-type.hpp"
#include "creature/creature.hpp"
#include "creature/nonplayer-inventory-types.hpp"
#include "game/strong-types.hpp"
#include "item/armor-template.hpp"
#include "item/weapon-template.hpp"
#include "misc/not-null.hpp"
#include "misc/vector-map.hpp"

#include <cstddef> //for std::size_t
#include <memory>
#include <numeric>
#include <string>
#include <vector>

namespace heroespath
{
namespace creature
{

    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;

    namespace nonplayer
    {

        template <typename T>
        void NormalizeChanceMap(misc::VectorMap<T, float> & map)
        {
            if (map.Empty() == false)
            {
                const auto TOTAL { std::accumulate(
                    std::begin(map),
                    std::end(map),
                    0.0f,
                    [](const auto SUB_TOTAL, const auto & PAIR) {
                        return SUB_TOTAL + PAIR.second;
                    }) };

                const auto NORMALIZE_ADJ { (1.0f - TOTAL) / static_cast<float>(map.Size()) };

                for (auto & nextChancePair : map)
                {
                    nextChancePair.second += NORMALIZE_ADJ;
                }
            }
        }

        // A Subsystem class that is responsible for creating sets of items that will equip
        // non-player characters.
        class ChanceFactory
        {
            // a wrapper for all the info needed to create weapon chances
            struct WeaponSet
            {
                WeaponSet()
                    : count(0)
                    , // this default value intentionally invalid
                    chanceMap()
                {
                    chanceMap.Reserve(32);
                }

                std::size_t count;
                misc::VectorMap<item::WeaponTemplate, float> chanceMap;
            };

            using WeaponSetVec_t = std::vector<WeaponSet>;

            // responsible for wrapping all the armor chance information per role
            struct RoleArmorChance
            {
                explicit RoleArmorChance(
                    const item::ArmorTemplate & ARMOR_TEMPLATE,
                    const float CHANCE = 0.0f)
                    : type_wrapper(ARMOR_TEMPLATE)
                    , chance(CHANCE)
                {}

                item::ArmorTemplate type_wrapper;
                float chance;
            };

            using RoleArmorChanceVec_t = std::vector<RoleArmorChance>;

        public:
            ChanceFactory(const ChanceFactory &) = delete;
            ChanceFactory(ChanceFactory &&) = delete;
            ChanceFactory & operator=(const ChanceFactory &) = delete;
            ChanceFactory & operator=(ChanceFactory &&) = delete;

            ChanceFactory();
            ~ChanceFactory();

            static misc::NotNull<ChanceFactory *> Instance();
            static void Create();
            static void Destroy();
            void Initialize();

            const InventoryChances Make(const CreaturePtr_t CHARACTER_PTR) const;

        private:
            const InventoryChances
                Make(const Profile & PROFILE, const CreaturePtr_t CHARACTER_PTR) const;

            void LookupClothingMaterialChances(
                const Profile & PROFILE,
                const CreaturePtr_t CHARACTER_PTR,
                float & clothChance,
                float & leatherChance,
                float & silkChance) const;

            // returns a float < 0.0f if the value found was 'remaining'
            float GetFloatFromGameDataFile(
                const std::string & KEY, const float MIN, const float MAX) const;

            void Make_Coins(
                const Profile & PROFILE,
                Coin_t & coinsMin_OutParam,
                Coin_t & coinsMax_OutParam) const;

            const ClothingChances Make_ClothingChances(
                const Profile & PROFILE, const CreaturePtr_t CHARACTER_PTR) const;

            const WeaponChances Make_WeaponChances(
                const Profile & PROFILE, const CreaturePtr_t CHARACTER_PTR) const;

            const ArmorChances
                Make_ArmorChances(const Profile & PROFILE, const CreaturePtr_t CHARACTER_PTR) const;

            const ItemChanceMap_t Make_MiscItemChances(
                const Profile & PROFILE, const CreaturePtr_t CHARACTER_PTR) const;

            const ClothingChances Make_ClothingMaterialChances(
                const Profile & PROFILE, const CreaturePtr_t CHARACTER_PTR) const;

            void PopulateWeaponChances(
                WeaponChances &, const WeaponSet &, const Profile &, const CreaturePtr_t) const;

            void LookupPossibleArmorByRole(
                const Profile &, const CreaturePtr_t, ArmorChances &) const;

            void LookupPossibleWeaponsByRole(const role::Enum, WeaponSetVec_t &) const;

            void Make_ClothingMaterialChancesPrimary(
                const Profile &, const CreaturePtr_t, ItemChances &) const;

            void Make_MaterialChancesPrimary(
                const Profile & PROFILE,
                const CreaturePtr_t CHARACTER_PTR,
                const MaterialChanceMap_t & MATERIALS_TYPICAL,
                const Weight_t & ITEM_WEIGHT,
                MaterialChanceMap_t & materialsMap_OutParam) const;

            void Make_MaterialChancesSecondary(
                const Profile &, const CreaturePtr_t, MaterialChanceMap_t &) const;

            const MaterialChanceMap_t Make_MaterialChanceMap(
                const std::string & PREFIX,
                const std::string & POSTFIX,
                const item::MaterialVec_t & MATERIALS_VEC) const;

            const MaterialChanceMap_t Make_MaterialChanceMapCool() const;
            const MaterialChanceMap_t Make_MaterialChanceMapPrecious() const;
            const MaterialChanceMap_t Make_MaterialChanceMapMetal() const;

            void PopulateWeaponMaterials(
                const std::string & WEAPON_NAME,
                const MaterialChanceMap_t & TYPICAL_PRI_MATERIALS,
                const Profile & PROFILE,
                const CreaturePtr_t CHARACTER_PTR,
                MaterialChanceMap_t & materialsMapPri,
                MaterialChanceMap_t & materialsMapSec) const;

            void PopulatMaterials(
                const MaterialChanceMap_t & TYPICAL_PRI_MATERIALS,
                const Profile & PROFILE,
                const CreaturePtr_t CHARACTER_PTR,
                MaterialChanceMap_t & materialsMapPri,
                MaterialChanceMap_t & materialsMapSec,
                const Weight_t & WEIGHT,
                const item::Material::Enum FORCED_MAT_PRI = item::Material::Count) const;

            template <typename T>
            bool SetWeaponChances(
                const std::string & WEAPON_NAME,
                const T WEAPON_ENUM,
                const float ,//CHANCE_BASE,
                const Profile & PROFILE,
                const CreaturePtr_t CHARACTER_PTR,
                const MaterialChanceMap_t & MATERIALS_TYPICAL,
                misc::VectorMap<T, ItemChances> & weaponChanceMap) const
            {
                //if (IsWeaponPossibleConsideringComplexity(WEAPON_NAME, PROFILE.complexityType)
                //    == false)
                //{
                //    // TODO how to handle in a way that works without leaving
                //    // chances without materials?!
                //    // weaponChanceMap[WEAPON_ENUM].SetCountChanceSingleNoChance();
                //    // return false;
                //}
                








                const auto WEAPON_NUM_FLOAT { static_cast<float>(WEAPON_ENUM) };
                const auto WEAPON_COUNT_FLOAT { static_cast<float>(T::Count) };
                const auto ENUM_RATIO { WEAPON_NUM_FLOAT / WEAPON_COUNT_FLOAT };

                //...minus a small amount depending on how valuable/rare the
                // weapon is that is defined by its position in the enum
                auto chance { (ENUM_RATIO * CHANCE_WEAPON_ENUM_POS_ADJ_INDIVIDUAL_) };

                //...adjust based on creature rank, where higher rank means higher
                // chance of the more rare/valuable items
                // add ones to prevent division by zero
                const auto ENUM_RATIO_INVERSE { (WEAPON_COUNT_FLOAT + 1.0f)
                                                / (WEAPON_NUM_FLOAT + 1.0f) };

                const auto RANK_F { CHARACTER_PTR->Rank().GetAs<float>() };
                const auto RANK_GRANDMASTER_F { masterRankMax_ + 1.0f };
                const auto RANK_RATIO { RANK_F / RANK_GRANDMASTER_F };

                chance
                    += ((ENUM_RATIO_INVERSE * CHANCE_WEAPON_ENUM_POS_ADJ_INDIVIDUAL_) * RANK_RATIO);

                ChanceFactory::ForceMinMax(chance, CHANCE_MINIMUM_, 1.0f);
                weaponChanceMap[WEAPON_ENUM].SetCountChanceIncrementAndEquip(chance);

                PopulateWeaponMaterials(
                    WEAPON_NAME,
                    MATERIALS_TYPICAL,
                    PROFILE,
                    CHARACTER_PTR,
                    weaponChanceMap[WEAPON_ENUM].mat_map_pri,
                    weaponChanceMap[WEAPON_ENUM].mat_map_sec);

                return true;
            }

            void SetArmorChancesGeneral(
                ArmorItemChances & armorItemChances,
                const RoleArmorChance & ROLE_ARMOR_CHANCE,
                const Profile & PROFILE,
                const CreaturePtr_t CHARACTER_PTR,
                const bool WILL_MATERIALS_INCLUDED_WOOD) const;

            void SetArmorChancesSpecific(
                ItemChances & itemChances,
                const RoleArmorChance & ROLE_ARMOR_CHANCE,
                const Profile & PROFILE,
                const CreaturePtr_t CHARACTER_PTR,
                const bool WILL_MATERIALS_INCLUDED_WOOD,
                const item::Material::Enum FORCED_MAT_PRI = item::Material::Count) const;

            void RestrictMaterialsByComplexity(
                const complexity_type::Enum COMPLEXITY,
                float & chanceCool,
                MaterialChanceMap_t & chanceMapCool,
                float & chanceMetal,
                MaterialChanceMap_t & chanceMapMetal,
                float & chancePrecious,
                MaterialChanceMap_t & chanceMapPrecious) const;

            bool IsWeaponPossibleConsideringComplexity(
                const std::string & WEAPON_NAME, const complexity_type::Enum CREATURE_COMPLEXITY);

            const MaterialChanceMap_t MakeTypicalArmorMaterials(
                const Profile & PROFILE,
                const CreaturePtr_t CHARACTER_PTR,
                const bool INCLUDE_WOOD) const;

            void ForceMinMax(
                float & val_OutParam,
                const float MIN_VAL = CHANCE_MINIMUM_,
                const float MAX_VAL = CHANCE_MAXIMUM_) const
            {
                if (val_OutParam < MIN_VAL)
                {
                    val_OutParam = MIN_VAL;
                }
                else
                {
                    if ((MAX_VAL > 0.0f) && (val_OutParam > MAX_VAL))
                    {
                        val_OutParam = MAX_VAL;
                    }
                }
            }

            void ForceMin(float val_OutParam, const float MIN = CHANCE_MINIMUM_) const
            {
                ForceMinMax(val_OutParam, MIN, 0.0f);
            }

            void CacheRoleArmorChances();
            void CacheRoleWeaponChances();
            const WeaponSetVec_t GetRoleWeaponChances(const role::Enum ROLE);

        private:
            static const float CHANCE_MINIMUM_;
            static const float CHANCE_MAXIMUM_;
            static const float CHANCE_WEAPON_ENUM_POS_ADJ_SET_;
            static const float CHANCE_WEAPON_ENUM_POS_ADJ_INDIVIDUAL_;

            static std::unique_ptr<ChanceFactory> instanceUPtr_;

            // cache some floats from the GameDataFile that never change and are often used
            float masterRankMax_;
            float clothingChanceMin_;
            float clothingChanceMax_;
            float materialPrimaryChanceCool_;
            float materialPrimaryChanceMetal_;
            float materialPrimaryChancePrecious_;
            float collectorMaterialChanceIncreaseCool_;
            float collectorMaterialChanceIncreaseMetal_;
            float collectorMaterialChanceIncreasePrecious_;
            float collectorMaterialChancePerCool_;
            float collectorMaterialChancePerMetal_;
            float collectorMaterialChancePerPrecious_;
            float materialSecondaryChanceCool_;
            float materialSecondaryChanceMetal_;
            float materialSecondaryChancePrecious_;

            MaterialChanceMap_t materialChanceMapCool_;
            MaterialChanceMap_t materialChanceMapMetal_;
            MaterialChanceMap_t materialChanceMapPrecious_;

            misc::VectorMap<role::Enum, RoleArmorChanceVec_t> roleArmorChanceMap_;
            misc::VectorMap<role::Enum, WeaponSetVec_t> roleWeaponChanceMap_;
        };

    } // namespace nonplayer
} // namespace creature
} // namespace heroespath
*/
#endif // HEROESPATH_CREATURE_NONPLAYER_INVENTORY_CHANCES_INCLUDED
