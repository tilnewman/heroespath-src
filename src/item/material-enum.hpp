// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_MATERIAL_ENUM_HPP_INCLUDED
#define HEROESPATH_ITEM_MATERIAL_ENUM_HPP_INCLUDED
//
// material-enum.hpp
//
#include "creature/race-enum.hpp"
#include "game/strong-types.hpp"
#include "misc/enum-common.hpp"

#include <string_view>
#include <utility>
#include <vector>

namespace heroespath
{
namespace item
{

    struct Material : public EnumBaseCounting<EnumNameOfZeroIsNothing>
    {
        enum Enum : EnumUnderlying_t
        {
            // what Ghosts and Shades are made of
            Spirit = 0,

            Gas,
            Water,
            Blood,
            Acid,
            Dirt,
            Paper,
            Glass,
            Feather,
            Hair,
            DriedFlesh,
            Rope,

            // garment types
            Cloth,
            Silk,
            Hide, // used a the skin material for many creatures
            Leather, // similar to hide but more processed and strong
            Fur,

            Plant,
            Claw,
            Scales,
            Horn,
            Bone,
            Tooth,
            Wood,
            Stone,
            Obsidian,
            Tin, // 18 /  7.3 -modulus of rigidity / density
            Bronze, // 45 /  7.5
            Iron, // 41 /  7.8
            Steel, // 77 /  8.0
            Jade,
            Amethyst,
            Emerald,
            Pearl,
            Ruby,
            Lazuli,
            Silver, // 48 / 10.5
            Sapphire,
            Gold, // 27 / 19.3
            Platinum, // 82 / 21.5
            Diamond,
            Count
        };

        static constexpr std::string_view Name(const Enum MATERIAL) noexcept
        {
            if (MATERIAL == DriedFlesh)
            {
                return "Dried Flesh";
            }
            else
            {
                return NAMEOF_ENUM(MATERIAL);
            }
        }

        static inline Armor_t ArmorRatingBonus(const Enum MAT_PRI, const Enum MAT_SEC)
        {
            return ArmorRatingBonusPri(MAT_PRI) + ArmorRatingBonusSec(MAT_SEC);
        }

        static constexpr Coin_t PriceAdjPri(const Enum MAT_PRI) noexcept
        {
            switch (MAT_PRI)
            {
                case Spirit: return 0_coin;
                case Gas: return 0_coin;
                case Water: return 0_coin;
                case Blood: return 0_coin;
                case Acid: return 100_coin;
                case Dirt: return 0_coin;
                case Paper: return 0_coin;
                case Glass: return 10_coin;
                case Feather: return 0_coin;
                case Fur: return 8_coin;
                case Hair: return 0_coin;
                case DriedFlesh: return 4_coin;
                case Rope: return 0_coin;
                case Cloth: return 0_coin;
                case Silk: return 16_coin;
                case Hide: return 7_coin;
                case Leather: return 10_coin;
                case Plant: return 0_coin;
                case Claw: return 4_coin;
                case Scales: return 1000_coin;
                case Horn: return 6_coin;
                case Bone: return 12_coin;
                case Tooth: return 5_coin;
                case Wood: return 0_coin;
                case Stone: return 0_coin;
                case Obsidian: return 20_coin;
                case Tin: return 30_coin;
                case Bronze: return 32_coin;
                case Iron: return 36_coin;
                case Steel: return 40_coin;
                case Jade: return 100_coin;
                case Amethyst: return 200_coin;
                case Emerald: return 300_coin;
                case Pearl: return 500_coin;
                case Ruby: return 600_coin;
                case Lazuli: return 700_coin;
                case Silver: return 1000_coin;
                case Sapphire: return 2000_coin;
                case Gold: return 5000_coin;
                case Platinum: return 7500_coin;
                case Diamond: return 10000_coin;
                case Count:
                default: return 0_coin;
            }
        }

        static constexpr Coin_t PriceAdjSec(const Enum MATERIAL) noexcept
        {
            return PriceAdjPri(MATERIAL) / 10_coin;
        }

        static constexpr Coin_t PriceAdj(const Enum MAT_PRI, const Enum MAT_SEC) noexcept
        {
            return PriceAdjPri(MAT_PRI) + PriceAdjSec(MAT_SEC);
        }

        static constexpr float WeightMult(const Enum MAT_PRI, const Enum MAT_SEC) noexcept
        {
            return WeightMultPri(MAT_PRI) + WeightMultSec(MAT_SEC);
        }

        static constexpr float WeightMultPri(const Enum MAT_PRI) noexcept
        {
            switch (MAT_PRI)
            {
                case Spirit: return 0.0f;
                case Gas: return 0.001f;
                case Plant: return 0.9f;
                case Cloth: return 0.9f;
                case Silk: return 0.8f;
                case Leather: return 1.05f;
                case Rope: return 1.1f;
                case Bone: return 1.15f;
                case Paper: return 1.2f;
                case Claw: return 1.2f;
                case Horn: return 1.2f;
                case Tooth: return 1.2f;
                case Water: return 1.5f;
                case Acid: return 1.5f;
                case Blood: return 1.55f;
                case DriedFlesh: return 1.6f;
                case Hide: return 1.35f;
                case Feather: return 0.5f;
                case Fur: return 0.75f;
                case Hair: return 0.5f;
                case Scales: return 0.9f;
                case Glass: return 1.6f;
                case Wood: return 1.85f;
                case Dirt: return 1.9f;
                case Obsidian: return 2.2f;
                case Jade: return 2.1f;
                case Amethyst: return 2.1f;
                case Emerald: return 2.15f;
                case Pearl: return 1.2f;
                case Ruby: return 2.2f;
                case Lazuli: return 2.5f;
                case Sapphire: return 2.1f;
                case Diamond: return 2.6f;
                case Stone: return 3.0f;
                case Tin: return 4.0f; // 7.3 - density
                case Bronze: return 4.2f; // 7.5
                case Iron: return 4.6f; // 7.8
                case Steel: return 4.8f; // 8.0
                case Silver: return 7.3f; // 10.5
                case Gold: return 16.6f; // 19.3
                case Platinum: return 18.8f; // 21.5
                case Count:
                default: return 0.0f;
            }
        }

        static constexpr float WeightMultSec(const Enum MAT_SEC) noexcept
        {
            return WeightMultPri(MAT_SEC) / 10.0f;
        }

        static constexpr float EnchantmentBonusRatio(const Enum MATERIAL) noexcept
        {
            // clang-format off
            switch (MATERIAL)
            {
                 // this is the standard set that runs the full range of values
                case Glass:                    { return 0.1f;  }
                case DriedFlesh:               { return 0.5f;  }
                case Blood:                    { return 0.85f; }
                case Spirit:                   { return 1.25f; }//extra bonus for spirit is intentional

                // this is the "tribal" set that should range from about 5% to 50%
                case Leather:                  { return 0.05f;  }
                case Stone:                    { return 0.1f;   }
                case Fur:                      { return 0.136f; }
                case Horn:                     { return 0.172f; }
                case Hide:                     { return 0.23f;  }
                case Silk:                     { return 0.262f; }
                case Claw:                     { return 0.28f;  }
                case Tooth:                    { return 0.33f;  }
                case Scales:                   { return 0.38f;  }
                case Bone:                     { return 0.45f;  }
                case Obsidian:                 { return 0.5f;   }

                // the jewel set should start where tribal left off (50%) and reach 100% with diamond
                case Jade:                     { return 0.5f;  }
                case Amethyst:                 { return 0.55f; }
                case Emerald:                  { return 0.6f;  }
                case Lazuli:                   { return 0.68f; }
                case Pearl:                    { return 0.76f; }
                case Ruby:                     { return 0.84f; }
                case Sapphire:                 { return 0.92f; }
                case Diamond:                  { return 1.0f;  }

                // the metal set varies wildly, but should stop short of diamond's value
                case Tin:                      { return 0.05f;  }
                case Iron:                     { return 0.09f;  }
                case Steel:                    { return 0.11f;  }
                case Bronze:                   { return 0.333f; }
                case Silver:                   { return 0.666f; }
                case Gold:                     { return 0.8f;   }
                case Platinum:                 { return 0.85f;  }

                // all others are 0%
                case Feather:
                case Gas:
                case Water:
                case Dirt:
                case Acid:
                case Paper:
                case Hair:
                case Rope:
                case Cloth:
                case Plant:
                case Wood:  return 0.0f;

                case Count:
                default: return 0.0f;
            }
            // clang-format on
        }

        static constexpr int EnchantmentBonus(const Enum MAT_PRI, const Enum MAT_SEC) noexcept
        {
            // the enchantment factory has been setup to work with values that range from [0,20]
            const auto RANGE { 20.0f };

            // these two ratios should add up to 1.0f
            const auto PRIMARY_RATIO { 0.75f };
            const auto SECONDARY_RATIO { 0.25f };

            if (MAT_SEC == Count)
            {
                return static_cast<int>(EnchantmentBonusRatio(MAT_PRI) * RANGE);
            }
            else
            {
                return static_cast<int>(
                    ((EnchantmentBonusRatio(MAT_PRI) * PRIMARY_RATIO)
                     + (EnchantmentBonusRatio(MAT_SEC) * SECONDARY_RATIO))
                    * RANGE);
            }
        }

        // solid/liquid/gas/spirit are complete and mutually exclusive

        static constexpr bool IsGas(const Enum MATERIAL) noexcept { return (MATERIAL == Gas); }

        static constexpr bool IsSpirit(const Enum MATERIAL) noexcept
        {
            return (MATERIAL == Spirit);
        }

        static constexpr bool IsLiquid(const Enum MATERIAL) noexcept
        {
            return ((MATERIAL == Acid) || (MATERIAL == Blood) || (MATERIAL == Water));
        }

        static constexpr bool IsSolid(const Enum MATERIAL) noexcept
        {
            return !(
                IsLiquid(MATERIAL) || IsGas(MATERIAL) || IsSpirit(MATERIAL) || (MATERIAL >= Count));
        }

        static constexpr bool IsBendy(const Enum MATERIAL) noexcept
        {
            return (
                (MATERIAL == Leather) || (MATERIAL == Silk) || (MATERIAL == Dirt)
                || (MATERIAL == Plant) || (MATERIAL == DriedFlesh) || (MATERIAL == Hide)
                || (MATERIAL == Feather) || (MATERIAL == Fur) || (MATERIAL == Hair)
                || (MATERIAL == Paper) || (MATERIAL == Rope) || (MATERIAL == Cloth));
        }

        static constexpr bool IsRigid(const Enum MATERIAL) noexcept
        {
            return (IsSolid(MATERIAL) && !IsBendy(MATERIAL));
        }

        static constexpr bool IsBloody(const Enum PRI, const Enum SEC) noexcept
        {
            return ((PRI == Blood) || (SEC == Blood));
        }

        static constexpr float FireDamageRatio(const Enum PRI, const Enum SEC = Count) noexcept
        {
            float fireDamageRatio = [PRI]() {
                switch (PRI)
                {
                    case Wood: return 0.5f;
                    case Leather: return 0.6f;
                    case Silk: return 0.9f;
                    case Bone: return 0.2f;
                    case Water: return 0.01f;
                    case Dirt: return 0.01f;
                    case Acid: return 0.01f;
                    case Stone: return 0.001f;
                    case Plant: return 0.85f;
                    case DriedFlesh: return 0.75f;
                    case Hide: return 0.87f;
                    case Feather: return 1.0f;
                    case Fur: return 1.0f;
                    case Hair: return 2.0f;
                    case Scales: return 0.1f;
                    case Obsidian: return 0.0f;
                    case Jade: return 0.0f;
                    case Amethyst: return 0.0f;
                    case Emerald: return 0.0f;
                    case Pearl: return 0.01f;
                    case Ruby: return 0.0f;
                    case Lazuli: return 0.0f;
                    case Sapphire: return 0.0f;
                    case Diamond: return 0.0f;
                    case Glass: return 0.9f;
                    case Gas: return 0.0f;
                    case Blood: return 0.2f;
                    case Paper: return 0.99f;
                    case Rope: return 0.85f;
                    case Cloth: return 0.85f;
                    case Claw: return 0.1f;
                    case Horn: return 0.1f;
                    case Tooth: return 0.05f;
                    case Spirit: return 0.0f;
                    case Gold: return 0.01f;
                    case Iron: return 0.01f;
                    case Tin: return 0.01f;
                    case Bronze: return 0.01f;
                    case Silver: return 0.01f;
                    case Steel: return 0.01f;
                    case Platinum: return 0.01f;
                    case Count:
                    default: return 0.0f;
                }
            }();

            fireDamageRatio += [SEC]() {
                switch (SEC)
                {
                    case Wood: return 0.2f;
                    case Leather: return 0.1f;
                    case Silk: return 0.27f;
                    case Bone: return 0.05f;
                    case Water: return -0.75f;
                    case Dirt: return -0.6f;
                    case Acid: return -0.75f;
                    case Stone: return -0.5f;
                    case Plant: return 0.4f;
                    case DriedFlesh: return 0.1f;
                    case Hide: return 0.3f;
                    case Feather: return 0.2f;
                    case Fur: return 0.3f;
                    case Hair: return 0.5f;
                    case Scales: return -0.75f;
                    case Obsidian: return -0.5f;
                    case Jade: return -0.5f;
                    case Amethyst: return -0.5f;
                    case Emerald: return -0.5f;
                    case Pearl: return -0.5f;
                    case Ruby: return -0.5f;
                    case Lazuli: return -0.5f;
                    case Sapphire: return -0.5f;
                    case Diamond: return -0.5f;
                    case Glass: return 0.0f;
                    case Gas: return 0.0f;
                    case Blood: return -0.75f;
                    case Paper: return 0.5f;
                    case Rope: return 0.25f;
                    case Cloth: return 0.25f;
                    case Claw: return -0.1f;
                    case Horn: return -0.1f;
                    case Tooth: return -0.1f;
                    case Spirit: return 0.0f;
                    case Gold: return -0.5f;
                    case Iron: return -0.5f;
                    case Tin: return -0.5f;
                    case Bronze: return -0.5f;
                    case Silver: return -0.5f;
                    case Steel: return -0.5f;
                    case Platinum: return -0.5f;
                    case Count:
                    default:
                    {
                        return 0.0f;
                    }
                }
            }();

            if (fireDamageRatio < 0.0f)
            {
                fireDamageRatio = 0.0f;
            }

            if (fireDamageRatio > 1.0f)
            {
                fireDamageRatio = 1.0f;
            }

            return fireDamageRatio;
        }

        static constexpr bool IsMetal(const Enum MATERIAL) noexcept
        {
            return (
                (MATERIAL == Tin) || (MATERIAL == Gold) || (MATERIAL == Iron)
                || (MATERIAL == Bronze) || (MATERIAL == Silver) || (MATERIAL == Steel)
                || (MATERIAL == Platinum));
        }

        static constexpr bool IsFancyJewel(const Enum MATERIAL) noexcept
        {
            return (
                (MATERIAL == Amethyst) || (MATERIAL == Emerald) || (MATERIAL == Ruby)
                || (MATERIAL == Sapphire) || (MATERIAL == Diamond));
        }

        static constexpr bool IsJeweled(const Enum PRI, const Enum SEC) noexcept
        {
            return (IsFancyJewel(PRI) || IsFancyJewel(SEC));
        }

        static constexpr bool IsFancyMetal(const Enum MATERIAL) noexcept
        {
            return ((MATERIAL == Silver) || (MATERIAL == Gold));
        }

        static constexpr bool IsFancyOpaque(const Enum MATERIAL) noexcept
        {
            return (
                (MATERIAL == Obsidian) || (MATERIAL == Pearl) || (MATERIAL == Jade)
                || (MATERIAL == Lazuli));
        }

        static constexpr bool IsFancyTribal(const Enum MATERIAL) noexcept
        {
            return (
                (MATERIAL == Claw) || (MATERIAL == Bone) || (MATERIAL == Tooth)
                || (MATERIAL == Scales));
        }

        static constexpr bool IsFancy(const Enum MATERIAL) noexcept
        {
            return (
                IsFancyJewel(MATERIAL) || IsFancyMetal(MATERIAL) || IsFancyOpaque(MATERIAL)
                || IsFancyTribal(MATERIAL));
        }

        static constexpr bool IsFancyJewelOrJade(const Enum MATERIAL) noexcept
        {
            return (IsFancyJewel(MATERIAL) || (MATERIAL == Jade));
        }

        static constexpr bool RequiresAnPrefix(const Enum MATERIAL) noexcept
        {
            return (
                (MATERIAL == Obsidian) || (MATERIAL == Iron) || (MATERIAL == Amethyst)
                || (MATERIAL == Emerald));
        }

        static constexpr Score_t Score(const Enum PRI, const Enum SEC) noexcept
        {
            const auto BONUS_PRI { EnchantmentBonus(PRI, Count) };
            const auto BONUS_SEC { EnchantmentBonus(SEC, Count) };
            return Score_t::Make(50 + (BONUS_PRI * BONUS_PRI) + ((BONUS_SEC * BONUS_SEC) / 3));
        }

    private:
        static constexpr Armor_t ArmorRatingBonusPri(const Enum PRI) noexcept
        {
            switch (PRI)
            {
                case Spirit:
                case Gas:
                case Water:
                case Blood:
                case Acid:
                case Dirt:
                case Paper:
                case Glass:
                case Feather:
                case Hair:
                case Rope: return 0_armor;

                case Cloth: return 1_armor;
                case Silk: return 2_armor;
                case Hide: return 4_armor;
                case DriedFlesh: return 5_armor;
                case Leather: return 7_armor;
                case Fur: return 9_armor;
                case Plant: return 11_armor;

                case Claw:
                case Horn: return 12_armor;

                case Bone:
                case Tooth: return 13_armor;

                case Wood: return 15_armor;
                case Scales: return 18_armor;
                case Stone: return 20_armor;
                case Obsidian: return 21_armor;
                case Tin: return 25_armor;
                case Bronze: return 26_armor;
                case Iron: return 27_armor;
                case Steel: return 28_armor;
                case Jade: return 29_armor;
                case Amethyst: return 30_armor;
                case Emerald: return 31_armor;
                case Pearl: return 32_armor;
                case Ruby: return 33_armor;
                case Lazuli: return 34_armor;
                case Silver: return 35_armor;
                case Sapphire: return 36_armor;
                case Gold: return 40_armor;
                case Platinum: return 45_armor;
                case Diamond: return 50_armor;
                case Count:
                default: return 0_armor;
            }
        }

        static constexpr Armor_t ArmorRatingBonusSec(const Enum SEC) noexcept
        {
            return ArmorRatingBonusPri(SEC).ScaleCopy(0.5f);
        }
    };

    using MaterialVec_t = std::vector<Material::Enum>;
    using MaterialPair_t = std::pair<Material::Enum, Material::Enum>;
    using MaterialPairVec_t = std::vector<MaterialPair_t>;

    // Responsible for defining the relationship between an item's name and materials.
    struct MaterialNameStyle : public EnumBaseCounting<>
    {
        enum Enum : EnumUnderlying_t
        {
            Misc = 0,
            BodyPart,
            Decoration,
            Handle,
            Reinforced,
            ReinforcedWithBase,
            Tipped,
            Clasped,
            ClaspedWithBase,
            Count
        };

        static constexpr bool IsSecondaryMaterialOfNothingRequired(const Enum TYPE) noexcept
        {
            return ((TYPE == Decoration) || (TYPE == Reinforced) || (TYPE == ReinforcedWithBase));
        }
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_MATERIAL_ENUM_HPP_INCLUDED
