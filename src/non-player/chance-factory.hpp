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
#ifndef HEROESPATH_NONPLAYER_CHANCEFACTORY_INCLUDED
#define HEROESPATH_NONPLAYER_CHANCEFACTORY_INCLUDED
//
// inventory-factory.hpp
//  Code responsible for creating non-player-characters items. (clothes/weapons/armor/jewelry/etc)
//
#include "creature/creature.hpp"
#include "creature/race-enum.hpp"
#include "creature/role-enum.hpp"
#include "game/game-data-file.hpp"
#include "item/weapon-info.hpp"
#include "misc/not-null.hpp"
#include "misc/types.hpp"
#include "misc/vector-map.hpp"
#include "non-player/ownership-chance-types.hpp"
#include "non-player/ownership-profile.hpp"

#include <cstddef> //for std::size_t
#include <memory>
#include <numeric>
#include <string>
#include <vector>

namespace heroespath
{
namespace non_player
{
    namespace ownership
    {

        template <typename T>
        void NormalizeChanceMap(misc::VectorMap<T, float> & map)
        {
            if (map.Empty() == false)
            {
                auto const TOTAL{ std::accumulate(
                    std::begin(map),
                    std::end(map),
                    0.0f,
                    [](auto const SUB_TOTAL, auto const & PAIR) {
                        return SUB_TOTAL + PAIR.second;
                    }) };

                auto const NORMALIZE_ADJ{ (1.0f - TOTAL) / static_cast<float>(map.Size()) };

                for (auto & nextChancePair : map)
                {
                    nextChancePair.second += NORMALIZE_ADJ;
                }
            }
        }

        // A singleton class that is responsible for creating sets of items that will equip
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
                {}

                std::size_t count;
                misc::VectorMap<item::weapon::WeaponInfo, float> chanceMap;
            };

            using WeaponSetVec_t = std::vector<WeaponSet>;

            // responsible for wrapping all the armor chance information per role
            struct RoleArmorChance
            {
                RoleArmorChance(
                    const std::string & NAME_SHORT = "",
                    const std::string & NAME_COMPLETE = "",
                    const float CHANCE = 0.0f,
                    item::armor::base_type::Enum BASE_TYPE = item::armor::base_type::Count)
                    : name_short(NAME_SHORT)
                    , name_complete(NAME_COMPLETE)
                    , chance(CHANCE)
                    , base_type(BASE_TYPE)
                {}

                std::string name_short;
                std::string name_complete;
                float chance;
                item::armor::base_type::Enum base_type;
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
            static void Acquire();
            static void Release();
            static void Initialize();

            static const chance::InventoryChances Make(const creature::CreaturePtr_t CHARACTER_PTR);

        private:
            static const chance::InventoryChances
                Make(const Profile & PROFILE, const creature::CreaturePtr_t CHARACTER_PTR);

            static void LookupClothingMaterialChances(
                const Profile & PROFILE,
                const creature::CreaturePtr_t CHARACTER_PTR,
                float & clothChance,
                float & softleatherChance,
                float & hardleatherChance);

            // returns a float < 0.0f if the value found was 'remaining'
            static float
                GetFloatFromGameDataFile(const std::string & KEY, const float MIN, const float MAX);

            static void Make_Coins(
                const Profile & PROFILE, Coin_t & coinsMin_OutParam, Coin_t & coinsMax_OutParam);

            static const chance::ClothingChances Make_ClothingChances(
                const Profile & PROFILE, const creature::CreaturePtr_t CHARACTER_PTR);

            static const chance::WeaponChances Make_WeaponChances(
                const Profile & PROFILE, const creature::CreaturePtr_t CHARACTER_PTR);

            static const chance::ArmorChances Make_ArmorChances(
                const Profile & PROFILE, const creature::CreaturePtr_t CHARACTER_PTR);

            static const chance::ItemChanceMap_t Make_MiscItemChances(
                const Profile & PROFILE, const creature::CreaturePtr_t CHARACTER_PTR);

            static const chance::ClothingChances Make_ClothingMaterialChances(
                const Profile & PROFILE, const creature::CreaturePtr_t CHARACTER_PTR);

            static void PopulateWeaponChances(
                chance::WeaponChances &,
                const WeaponSet &,
                const Profile &,
                const creature::CreaturePtr_t);

            static void LookupPossibleArmorByRole(
                const Profile &, const creature::CreaturePtr_t, chance::ArmorChances &);

            static void LookupPossibleWeaponsByRole(const creature::role::Enum, WeaponSetVec_t &);

            static void Make_ClothingMaterialChancesPrimary(
                const Profile &, const creature::CreaturePtr_t, chance::ItemChances &);

            static void Make_MaterialChancesPrimary(
                const Profile & PROFILE,
                const creature::CreaturePtr_t CHARACTER_PTR,
                const chance::MaterialChanceMap_t & MATERIALS_TYPICAL,
                const Weight_t & ITEM_WEIGHT,
                chance::MaterialChanceMap_t & materialsMap_OutParam);

            static void Make_MaterialChancesSecondary(
                const Profile &, const creature::CreaturePtr_t, chance::MaterialChanceMap_t &);

            static const chance::MaterialChanceMap_t Make_MaterialChanceMap(
                const std::string & PREFIX,
                const std::string & POSTFIX,
                const chance::MaterialVec_t & MATERIALS_VEC);

            static const chance::MaterialChanceMap_t Make_MaterialChanceMapCool();
            static const chance::MaterialChanceMap_t Make_MaterialChanceMapPrecious();
            static const chance::MaterialChanceMap_t Make_MaterialChanceMapMetal();

            static void Make_KnifeOrDaggerSizeChances(
                const creature::rank_class::Enum, chance::SizeChanceMap_t &);

            static void PopulateWeaponMaterials(
                const std::string & WEAPON_NAME,
                const chance::MaterialChanceMap_t & TYPICAL_PRI_MATERIALS,
                const Profile & PROFILE,
                const creature::CreaturePtr_t CHARACTER_PTR,
                chance::MaterialChanceMap_t & materialsMapPri,
                chance::MaterialChanceMap_t & materialsMapSec);

            static void PopulatMaterials(
                const chance::MaterialChanceMap_t & TYPICAL_PRI_MATERIALS,
                const Profile & PROFILE,
                const creature::CreaturePtr_t CHARACTER_PTR,
                chance::MaterialChanceMap_t & materialsMapPri,
                chance::MaterialChanceMap_t & materialsMapSec,
                const Weight_t & WEIGHT);

            template <typename T>
            static bool SetWeaponChances(
                const std::string & WEAPON_NAME,
                const T WEAPON_ENUM,
                const float /*CHANCE_BASE*/,
                const Profile & PROFILE,
                const creature::CreaturePtr_t CHARACTER_PTR,
                const chance::MaterialChanceMap_t & MATERIALS_TYPICAL,
                misc::VectorMap<T, chance::ItemChances> & weaponChanceMap)
            {
                if (IsWeaponPossibleConsideringComplexity(WEAPON_NAME, PROFILE.complexityType)
                    == false)
                {
                    // TODO how to handle in a way that works without leaving
                    // chances without materials?!
                    // weaponChanceMap[WEAPON_ENUM].SetCountChanceSingleNoChance();
                    // return false;
                }

                auto const WEAPON_NUM_FLOAT{ static_cast<float>(WEAPON_ENUM) };
                auto const WEAPON_COUNT_FLOAT{ static_cast<float>(T::Count) };
                auto const ENUM_RATIO{ WEAPON_NUM_FLOAT / WEAPON_COUNT_FLOAT };

                //...minus a small amount depending on how valuable/rare the
                // weapon is that is defined by its position in the enum
                auto chance{ (ENUM_RATIO * CHANCE_WEAPON_ENUM_POS_ADJ_INDIVIDUAL_) };

                //...adjust based on creature rank, where higher rank means higher
                // chance of the more rare/valuable items
                // add ones to prevent division by zero
                auto const ENUM_RATIO_INVERSE{ (WEAPON_COUNT_FLOAT + 1.0f)
                                               / (WEAPON_NUM_FLOAT + 1.0f) };

                auto const RANK_F{ CHARACTER_PTR->Rank().As<float>() };

                auto const RANK_GRANDMASTER_F{ masterRankMax_ + 1.0f };

                auto const RANK_RATIO{ RANK_F / RANK_GRANDMASTER_F };

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

            static void SetArmorChancesGeneral(
                chance::ArmorItemChances & armorItemChances,
                const std::string & COMPLETE_NAME,
                const item::armor::base_type::Enum TYPE,
                const float CHANCE,
                const Profile & PROFILE,
                const creature::CreaturePtr_t CHARACTER_PTR,
                const bool WILL_MATERIALS_INCLUDED_WOOD);

            static void SetArmorChancesSpecific(
                chance::ItemChances & itemChances,
                const std::string & COMPLETE_NAME,
                const float CHANCE,
                const Profile & PROFILE,
                const creature::CreaturePtr_t CHARACTER_PTR,
                const bool WILL_MATERIALS_INCLUDED_WOOD);

            static void RestrictMaterialsByComplexity(
                const complexity_type::Enum COMPLEXITY,
                float & chanceCool,
                chance::MaterialChanceMap_t & chanceMapCool,
                float & chanceMetal,
                chance::MaterialChanceMap_t & chanceMapMetal,
                float & chancePrecious,
                chance::MaterialChanceMap_t & chanceMapPrecious);

            static bool IsWeaponPossibleConsideringComplexity(
                const std::string & WEAPON_NAME, const complexity_type::Enum CREATURE_COMPLEXITY);

            static const chance::MaterialChanceMap_t MakeTypicalArmorMaterials(
                const Profile & PROFILE,
                const creature::CreaturePtr_t CHARACTER_PTR,
                const bool INCLUDE_WOOD);

            static void ForceMinMax(
                float & val_OutParam,
                const float MIN_VAL = CHANCE_MINIMUM_,
                const float MAX_VAL = CHANCE_MAXIMUM_)
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

            static void ForceMin(float val_OutParam, const float MIN = CHANCE_MINIMUM_)
            {
                ForceMinMax(val_OutParam, MIN, 0.0f);
            }

            static void CacheRoleArmorChances();

            static void CacheGameDataFileFloats();

        private:
            static const float CHANCE_MINIMUM_;
            static const float CHANCE_MAXIMUM_;
            static const float CHANCE_WEAPON_ENUM_POS_ADJ_SET_;
            static const float CHANCE_WEAPON_ENUM_POS_ADJ_INDIVIDUAL_;

            static std::unique_ptr<ChanceFactory> instanceUPtr_;

            static chance::MaterialChanceMap_t materialChanceMapCool_;
            static chance::MaterialChanceMap_t materialChanceMapMetal_;
            static chance::MaterialChanceMap_t materialChanceMapPrecious_;

            static misc::VectorMap<creature::role::Enum, RoleArmorChanceVec_t> roleArmorChanceMap_;

            // cache some floats from the GameDataFile that never change and are often used
            static float masterRankMax_;
            static float clothingChanceMin_;
            static float clothingChanceMax_;
            static float materialPrimaryChanceCool_;
            static float materialPrimaryChanceMetal_;
            static float materialPrimaryChancePrecious_;
            static float collectorMaterialChanceIncreaseCool_;
            static float collectorMaterialChanceIncreaseMetal_;
            static float collectorMaterialChanceIncreasePrecious_;
            static float collectorMaterialChancePerCool_;
            static float collectorMaterialChancePerMetal_;
            static float collectorMaterialChancePerPrecious_;
            static float materialSecondaryChanceCool_;
            static float materialSecondaryChanceMetal_;
            static float materialSecondaryChancePrecious_;
        };

    } // namespace ownership
} // namespace non_player
} // namespace heroespath

#endif // HEROESPATH_NONPLAYER_CHANCEFACTORY_INCLUDED
