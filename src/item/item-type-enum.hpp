// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_ITEMTYPEENUM_HPP_INCLUDED
#define HEROESPATH_ITEM_ITEMTYPEENUM_HPP_INCLUDED
//
// item-type-enum.hpp
//  Enumerations for all of an Item's various types.
//
#include "creature/race-enum.hpp"
#include "creature/role-enum.hpp"
#include "creature/trait.hpp"
#include "game/strong-types.hpp"
#include "misc/boost-string-includes.hpp"
#include "misc/enum-common.hpp"

#include <string>
#include <utility>
#include <vector>

namespace heroespath
{
namespace item
{

    struct category : public EnumBaseBitField<>
    {
        enum Enum : EnumUnderlying_t
        {
            None = 0,

            // useless, can't do anything or be used, all magic/enchantments fail
            Broken = 1 << 0,

            Useable = 1 << 1,
            BodyPart = 1 << 2,
            Equipable = 1 << 3,
            Wearable = 1 << 4,
            OneHanded = 1 << 5,
            TwoHanded = 1 << 6,
            ConsumedOnUse = 1 << 7,
            ShowsEnemyInfo = 1 << 8,
            Last = ShowsEnemyInfo
        };

        static const std::string Name(const Enum, const EnumStringHow & HOW = EnumStringHow());
    };

    struct material : public EnumBaseCounting<EnumFirstValue::Nothing>
    {
        enum Enum : EnumUnderlying_t
        {
            // Use Nothing as default instead of Error so that an Item's
            // materialSec_ can be Nothing.
            Nothing = 0,

            // what Ghosts and Shades are made of
            Spirit,

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

        static constexpr std::string_view Name(const item::material::Enum MATERIAL) noexcept
        {
            if (MATERIAL == material::DriedFlesh)
            {
                return "Dried Flesh";
            }
            else
            {
                return NAMEOF_ENUM(MATERIAL);
            }
        }

        // TODO move to MaterialFactory
        static constexpr std::pair<material::Enum, material::Enum>
            SkinMaterial(const creature::race::Enum RACE) noexcept
        {
            // keep in sync with ItemImagePaths::GetSkinImageFilename()

            if ((RACE == creature::race::Wolfen) || (RACE == creature::race::Troll)
                || (RACE == creature::race::Boar) || (RACE == creature::race::LionBoar)
                || (RACE == creature::race::Ramonaut) || (RACE == creature::race::Wereboar))
            {
                return std::make_pair(Hide, Nothing);
            }
            else if (
                (RACE == creature::race::ThreeHeadedHound) || (RACE == creature::race::Minotaur)
                || (RACE == creature::race::Lion) || (RACE == creature::race::Werebear)
                || (RACE == creature::race::Werewolf))
            {
                return std::make_pair(Hide, Fur);
            }
            else if (
                (RACE == creature::race::Dragon) || (RACE == creature::race::Hydra)
                || (RACE == creature::race::LizardWalker) || (RACE == creature::race::Naga)
                || (RACE == creature::race::Serpent) || (RACE == creature::race::Cobra)
                || (RACE == creature::race::Wyvern))
            {
                return std::make_pair(Scales, Nothing);
            }
            else if (RACE == creature::race::Plant)
            {
                return std::make_pair(Plant, Nothing);
            }
            else
            {
                return std::make_pair(Nothing, Nothing);
            }
        }

        static constexpr Armor_t ArmorRatingBonusPri(const material::Enum MATERIAL_PRI) noexcept
        {
            switch (MATERIAL_PRI)
            {
                case Nothing:
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

        static Armor_t ArmorRatingBonusSec(const Enum MATERIAL_SEC);

        static inline Armor_t
            ArmorRatingBonus(const material::Enum MATERIAL_PRI, const material::Enum MATERIAL_SEC)
        {
            return ArmorRatingBonusPri(MATERIAL_PRI) + ArmorRatingBonusSec(MATERIAL_SEC);
        }

        static constexpr Coin_t PriceAdjPri(const material::Enum MATERIAL_PRI) noexcept
        {
            switch (MATERIAL_PRI)
            {
                case Nothing: return 0_coin;
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

        static constexpr Coin_t
            PriceAdj(const material::Enum MATERIAL_PRI, const material::Enum MATERIAL_SEC) noexcept
        {
            return PriceAdjPri(MATERIAL_PRI) + PriceAdjSec(MATERIAL_SEC);
        }

        static constexpr float WeightMult(
            const material::Enum MATERIAL_PRI, const material::Enum MATERIAL_SEC) noexcept
        {
            return WeightMultPri(MATERIAL_PRI) + WeightMultSec(MATERIAL_SEC);
        }

        static constexpr float WeightMultPri(const material::Enum MATERIAL_PRI) noexcept
        {
            switch (MATERIAL_PRI)
            {
                case Nothing: return 0.0f;
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

        static constexpr float WeightMultSec(const material::Enum MATERIAL_SEC) noexcept
        {
            return WeightMultPri(MATERIAL_SEC) / 10.0f;
        }

        static constexpr float EnchantmentBonusRatio(const material::Enum MATERIAL) noexcept
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
                case Nothing:
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

        static constexpr int
            EnchantmentBonus(const Enum MATERIAL_PRIMARY, const Enum MATERIAL_SECONDARY) noexcept
        {
            // the enchantment factory has been setup to work with values that range from [0,20]
            const auto RANGE { 20.0f };

            // these two ratios should add up to 1.0f
            const auto PRIMARY_RATIO { 0.75f };
            const auto SECONDARY_RATIO { 0.25f };

            if (MATERIAL_SECONDARY == Nothing)
            {
                return static_cast<int>(EnchantmentBonusRatio(MATERIAL_PRIMARY) * RANGE);
            }
            else
            {
                return static_cast<int>(
                    ((EnchantmentBonusRatio(MATERIAL_PRIMARY) * PRIMARY_RATIO)
                     + (EnchantmentBonusRatio(MATERIAL_SECONDARY) * SECONDARY_RATIO))
                    * RANGE);
            }
        }

        // solid/liquid/gas/spirit are complete and mutually exclusive

        static constexpr bool IsLiquid(const Enum MATERIAL) noexcept
        {
            return ((MATERIAL == Acid) || (MATERIAL == Blood) || (MATERIAL == Water));
        }

        static constexpr bool IsGas(const Enum MATERIAL) noexcept { return (MATERIAL == Gas); }

        static constexpr bool IsSpirit(const Enum MATERIAL) noexcept
        {
            return (MATERIAL == Spirit);
        }

        static constexpr bool IsSolid(const Enum MATERIAL) noexcept
        {
            return !(
                (MATERIAL == Nothing) || IsLiquid(MATERIAL) || IsGas(MATERIAL)
                || IsSpirit(MATERIAL));
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

        static constexpr float
            FireDamageRatio(const material::Enum PRI, const material::Enum SEC = Nothing) noexcept
        {
            float fireDamageRatio = [PRI]() {
                switch (PRI)
                {
                    case Nothing: return 0.0f;
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
                    case material::Nothing: return 0.0f;
                    case material::Wood: return 0.2f;
                    case material::Leather: return 0.1f;
                    case material::Silk: return 0.27f;
                    case material::Bone: return 0.05f;
                    case material::Water: return -0.75f;
                    case material::Dirt: return -0.6f;
                    case material::Acid: return -0.75f;
                    case material::Stone: return -0.5f;
                    case material::Plant: return 0.4f;
                    case material::DriedFlesh: return 0.1f;
                    case material::Hide: return 0.3f;
                    case material::Feather: return 0.2f;
                    case material::Fur: return 0.3f;
                    case material::Hair: return 0.5f;
                    case material::Scales: return -0.75f;
                    case material::Obsidian: return -0.5f;
                    case material::Jade: return -0.5f;
                    case material::Amethyst: return -0.5f;
                    case material::Emerald: return -0.5f;
                    case material::Pearl: return -0.5f;
                    case material::Ruby: return -0.5f;
                    case material::Lazuli: return -0.5f;
                    case material::Sapphire: return -0.5f;
                    case material::Diamond: return -0.5f;
                    case material::Glass: return 0.0f;
                    case material::Gas: return 0.0f;
                    case material::Blood: return -0.75f;
                    case material::Paper: return 0.5f;
                    case material::Rope: return 0.25f;
                    case material::Cloth: return 0.25f;
                    case material::Claw: return -0.1f;
                    case material::Horn: return -0.1f;
                    case material::Tooth: return -0.1f;
                    case material::Spirit: return 0.0f;
                    case material::Gold: return -0.5f;
                    case material::Iron: return -0.5f;
                    case material::Tin: return -0.5f;
                    case material::Bronze: return -0.5f;
                    case material::Silver: return -0.5f;
                    case material::Steel: return -0.5f;
                    case material::Platinum: return -0.5f;
                    case material::Count:
                    default: { return 0.0f;
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

        static constexpr bool IsGarmentType(const Enum MATERIAL) noexcept
        {
            return (
                (MATERIAL == Fur) || (MATERIAL == Cloth) || (MATERIAL == Silk) || (MATERIAL == Hide)
                || (MATERIAL == Leather));
        }

        static constexpr bool RequiresAnPrefix(const Enum MATERIAL) noexcept
        {
            return (
                (MATERIAL == Obsidian) || (MATERIAL == Iron) || (MATERIAL == Amethyst)
                || (MATERIAL == Emerald));
        }
    };

    using MaterialVec_t = std::vector<material::Enum>;
    using MaterialPair_t = std::pair<material::Enum, material::Enum>;
    using MaterialPairVec_t = std::vector<MaterialPair_t>;

    struct element_type : public EnumBaseBitField<>
    {
        enum Enum : EnumUnderlying_t
        {
            None = 0,
            Fire = 1 << 0,
            Frost = 1 << 1,
            Honor = 1 << 2,
            Shadow = 1 << 3,
            Last = Shadow
        };

        static constexpr bool IsValid(const Enum ENUM) noexcept
        {
            return !(((ENUM & Fire) && (ENUM & Frost)) || ((ENUM & Honor) && (ENUM & Shadow)));
        }

        static const std::string Name(
            const Enum, const EnumStringHow & HOW = EnumStringHow(), const bool INCLUDE_OF = true);

        template <typename T>
        static const std::vector<element_type::Enum>
            ValidCombinations(const T VALUE, const bool WILL_INCLUDE_NOTHING = false)
        {
            const auto ELEMENT_TYPE { static_cast<element_type::Enum>(VALUE) };

            std::vector<Enum> enums;

            if (WILL_INCLUDE_NOTHING)
            {
                enums.emplace_back(None);
            }

            if (ELEMENT_TYPE == element_type::None)
            {
                return enums;
            }

            if (ELEMENT_TYPE & Fire)
            {
                enums.emplace_back(Fire);
            }

            if (ELEMENT_TYPE & Frost)
            {
                enums.emplace_back(Frost);
            }

            if (ELEMENT_TYPE & Honor)
            {
                enums.emplace_back(Honor);
            }

            if (ELEMENT_TYPE & Shadow)
            {
                enums.emplace_back(Shadow);
            }

            if ((ELEMENT_TYPE & Fire) && (ELEMENT_TYPE & Honor))
            {
                enums.emplace_back(static_cast<Enum>(Fire | Honor));
            }

            if ((ELEMENT_TYPE & Fire) && (ELEMENT_TYPE & Shadow))
            {
                enums.emplace_back(static_cast<Enum>(Fire | Shadow));
            }

            if ((ELEMENT_TYPE & Frost) && (ELEMENT_TYPE & Honor))
            {
                enums.emplace_back(static_cast<Enum>(Frost | Honor));
            }

            if ((ELEMENT_TYPE & Frost) && (ELEMENT_TYPE & Shadow))
            {
                enums.emplace_back(static_cast<Enum>(Frost | Shadow));
            }

            return enums;
        }

        static const std::vector<element_type::Enum> ValidCombinations()
        {
            return ValidCombinations(
                element_type::Fire | element_type::Frost | element_type::Honor
                | element_type::Shadow);
        }
    };

    using ElementEnumVec_t = std::vector<element_type::Enum>;

    struct misc_type : public EnumBaseCounting<EnumFirstValue::Not>
    {
        enum Enum : EnumUnderlying_t
        {
            Not = 0,

            // quest items
            // all musical instruments except for the DrumLute are quest items
            AngelBraid,
            Crumhorn,
            DevilHorn,
            GolemFinger,
            HurdyGurdy,
            Icicle,
            LichHand,
            Lyre,
            MummyHand,
            PetrifiedSnake,
            PipeAndTabor,
            Recorder,
            UnicornHorn,
            Viol,

            // unique items
            BasiliskTongue,
            BerserkersBeard,
            BishopsHanky,
            BleedingTrophy,
            BloodyDragonScale,
            BottleOfBansheeScreams,
            BronzeTroll,
            Brooch_Crown,
            Brooch_Feather,
            Brooch_Fist,
            Brooch_Hourglass,
            Brooch_Key,
            Brooch_Mask,
            BurialShroud,
            CapeCommanders,
            CapeGenerals,
            CapeKings,
            Charm_Crown,
            Charm_Feather,
            Charm_Fist,
            Charm_Hourglass,
            Charm_Key,
            Charm_Mask,
            ChimeraBone,
            CobraTooth,
            CrystalChimes,
            DemonDiary,
            DoveBloodVial,
            DragonToothWhistle,
            DriedFrog,
            DriedGecko,
            DriedIguana,
            DriedLizard,
            DriedSalamander,
            DriedSkink,
            DriedToad,
            DruidLeaf,
            EvilRabbitsFoot,
            ExoticGoldenGong,
            EyeCyclops,
            EyeGiantOwl,
            EyeHawk,
            FlagFanatics,
            FlagRegalCaptains,
            FlagTribal,
            FriarsChronicle,
            FuneralRecord,
            GhostSheet,
            GlassCat,
            GriffinFeather,
            HangmansNoose,
            HobgoblinNose,
            HolyEpic,
            HornOfTheHorde,
            ImpTail,
            IslanderHeaddress,
            JeweledArmband,
            JeweledHandbag,
            JeweledPrincessVeil,
            LastRitesScroll,
            MacabreManuscript,
            MadRatJuju,
            MagicHorseshoe,
            MagnifyingGlass,
            ManaAmulet,
            MaskMourners,
            MaskRascal,
            MinotaurHide,
            MortuaryOrnament,
            NecklaceJeweledAnkh,
            NecklaceSharkTooth,
            NecklaceVampiresTooth,
            PantherPaw,
            Pin_Crown,
            Pin_Feather,
            Pin_Fist,
            Pin_Hourglass,
            Pin_Key,
            Pin_Mask,
            PixieBell,
            RattlesnakeTail,
            RavenClaw,
            ReaperScythe,
            RequiemRegister,
            RingHobo,
            RingMendicant,
            RingMonk,
            RingPriest,
            RoyalScoutSpyglass,
            SaintCameoPin,
            SaintsJournal,
            SanguineRelic,
            ScoundrelSack,
            SepultureDecoration,
            ShadeCloak,
            ShamanRainmaker,
            SirenConch,
            SpecterChains,
            SpecterRobe,
            SpiderEggs,
            SprintersLegtie,
            SwindlersBag,
            TricksterPouch,
            TuningFork,
            TurtleShell,
            VultureGizzard,
            WarhorseMarionette,
            WarTrumpet,
            WeaselTotem,
            WolfenFur,
            WraithTalisman,

            // standalone items
            Bust,
            Doll,
            DollBlessed,
            DollCursed,
            DriedHead,
            DrumLute, // the only musical instrument that is not a quest item
            Egg,
            Embryo,
            FigurineBlessed,
            FigurineCursed,
            Goblet,
            Key,
            LockPicks,
            Mirror,
            Orb,
            Pendant,
            PuppetBlessed,
            PuppetCursed,
            Ring,
            Scepter,
            Seeds,
            Shard,
            Staff,
            SummoningStatue,
            Wand,

            Count
        };

        static constexpr std::string_view Name(const misc_type::Enum ENUM) noexcept
        {
            switch (ENUM)
            {
                case Not: return "Not";
                case AngelBraid: return "Angel Braid";
                case Crumhorn: return "Crumhorn";
                case DevilHorn: return "Devil Horn";
                case GolemFinger: return "Golem Finger";
                case HurdyGurdy: return "Hannibal's Hurdy Gurdy";
                case Icicle: return "Icicle";
                case LichHand: return "Lich Hand";
                case Lyre: return "Satyr's Lyre";
                case MummyHand: return "Mummy Hand";
                case PetrifiedSnake: return "Petrified Snake";
                case PipeAndTabor: return "Puritan's Pipe And Tabor";
                case Recorder: return "Reverend's Recorder";
                case UnicornHorn: return "Unicorn Horn";
                case Viol: return "Villain's Viol";
                case BasiliskTongue: return "Basilisk Tongue";
                case BerserkersBeard: return "Berserker's Beard";
                case BishopsHanky: return "Bishop's Hanky";
                case BleedingTrophy: return "Bleeding Trophy";
                case BloodyDragonScale: return "Bloody Dragon Scale";
                case BottleOfBansheeScreams: return "Bottle Of Banshee Screams";
                case BronzeTroll: return "Bronze Troll";
                case Brooch_Crown: return "Crown Brooch";
                case Brooch_Feather: return "Feather Brooch";
                case Brooch_Fist: return "Fist Brooch";
                case Brooch_Hourglass: return "Hourglass Brooch";
                case Brooch_Key: return "Key Brooch";
                case Brooch_Mask: return "Mask Brooch";
                case BurialShroud: return "Burial Shroud";
                case CapeCommanders: return "Commander's Cape";
                case CapeGenerals: return "General's Cape";
                case CapeKings: return "King's Cape";
                case Charm_Crown: return "Crown Charm";
                case Charm_Feather: return "Feather Charm";
                case Charm_Fist: return "Fist Charm";
                case Charm_Hourglass: return "Hourglass Charm";
                case Charm_Key: return "Key Charm";
                case Charm_Mask: return "Mask Charm";
                case ChimeraBone: return "Chimera Bone";
                case CobraTooth: return "Cobra Tooth";
                case CrystalChimes: return "Crystal CrystalChimes";
                case DemonDiary: return "Demon's Diary";
                case DoveBloodVial: return "Dove Blood Vial";
                case DragonToothWhistle: return "Dragon Tooth Whistle";
                case DriedFrog: return "Dried Frog";
                case DriedGecko: return "Dried Gecko";
                case DriedIguana: return "Dried Iguana";
                case DriedLizard: return "Dried Lizard";
                case DriedSalamander: return "Dried Salamander";
                case DriedSkink: return "Dried Skink";
                case DriedToad: return "Dried Toad";
                case DruidLeaf: return "Druid Leaf";
                case EvilRabbitsFoot: return "Evil Rabbits Foot";
                case ExoticGoldenGong: return "Exotic Golden Gong";
                case EyeCyclops: return "Cyclop's Eye";
                case EyeGiantOwl: return "Giant Owl Eye";
                case EyeHawk: return "Hawk Eye";
                case FlagFanatics: return "Fanatic's Flag";
                case FlagRegalCaptains: return "Regal Captain's Flag";
                case FlagTribal: return "Tribal Flag";
                case FriarsChronicle: return "Friar's Chronicle";
                case FuneralRecord: return "Funeral Record";
                case GhostSheet: return "Ghost Sheet";
                case GlassCat: return "Glass Cat";
                case GriffinFeather: return "Griffin Feather";
                case HangmansNoose: return "Hangman's Noose";
                case HobgoblinNose: return "Hobgoblin Nose";
                case HolyEpic: return "Holy Epic";
                case HornOfTheHorde: return "Horn of The Horde";
                case ImpTail: return "Imp Tail";
                case IslanderHeaddress: return "Islander Headdress";
                case JeweledArmband: return "Armband";
                case JeweledHandbag: return "Handbag";
                case JeweledPrincessVeil: return "Princess Veil";
                case LastRitesScroll: return "Last Rites Scroll";
                case MacabreManuscript: return "Macabre Manuscript";
                case MadRatJuju: return "Mad Rat Juju";
                case MagicHorseshoe: return "Magic Horseshoe";
                case MagnifyingGlass: return "Magnifying Glass";
                case ManaAmulet: return "Mana Amulet";
                case MaskMourners: return "Mourner's Mask";
                case MaskRascal: return "Rascal Mask";
                case MinotaurHide: return "Minotaur Hide";
                case MortuaryOrnament: return "Mortuary Ornament";
                case NecklaceJeweledAnkh: return "Ankh Necklace";
                case NecklaceSharkTooth: return "Shark Tooth Necklace";
                case NecklaceVampiresTooth: return "Vampire's Tooth Necklace";
                case PantherPaw: return "Panther Paw";
                case Pin_Crown: return "Crown Pin";
                case Pin_Feather: return "Feather Pin";
                case Pin_Fist: return "Fist Pin";
                case Pin_Hourglass: return "Hourglass Pin";
                case Pin_Key: return "Key Pin";
                case Pin_Mask: return "Mask Pin";
                case PixieBell: return "Pixie Bell";
                case RattlesnakeTail: return "Rattlesnake Tail";
                case RavenClaw: return "Raven Claw";
                case ReaperScythe: return "Reaper Scythe";
                case RequiemRegister: return "Requiem Register";
                case RingHobo: return "Hobo Ring";
                case RingMendicant: return "Mendicant Ring";
                case RingMonk: return "Monk Ring";
                case RingPriest: return "PriestRing";
                case RoyalScoutSpyglass: return "Royal Scout Spyglass";
                case SaintCameoPin: return "Saint Cameo Pin";
                case SaintsJournal: return "Saint's Journal";
                case SanguineRelic: return "Sanguine Relic";
                case ScoundrelSack: return "Scoundrel Sack";
                case SepultureDecoration: return "Sepulture Decoration";
                case ShadeCloak: return "Shade Cloak";
                case ShamanRainmaker: return "Shaman Rainmaker";
                case SirenConch: return "Siren Conch";
                case SpecterChains: return "Specter Chains";
                case SpecterRobe: return "Specter Robe";
                case SpiderEggs: return "Spider Eggs";
                case SprintersLegtie: return "Sprinter's Legtie";
                case SwindlersBag: return "Swindler's Bag";
                case TricksterPouch: return "Trickster Pouch";
                case TuningFork: return "Tuning Fork";
                case TurtleShell: return "Turtle Shell";
                case VultureGizzard: return "Vulture Gizzard";
                case WarhorseMarionette: return "Warhorse Marionette";
                case WarTrumpet: return "War Trumpet";
                case WeaselTotem: return "Weasel Totem";
                case WolfenFur: return "Wolfen Fur";
                case WraithTalisman: return "Wraith Talisman";
                case Bust: return "Bust";
                case Doll: return "Doll";
                case DollBlessed: return "Doll";
                case DollCursed: return "Doll";
                case DriedHead: return "Dried Head";
                case DrumLute: return "Drumlute";
                case Egg: return "Egg";
                case Embryo: return "Embryo";
                case FigurineBlessed: return "Figurine";
                case FigurineCursed: return "Figurine";
                case Goblet: return "Goblet";
                case Key: return "Key";
                case LockPicks: return "Lockpicks";
                case Mirror: return "Mirror";
                case Orb: return "Orb";
                case Pendant: return "Pendant";
                case PuppetBlessed: return "Puppet";
                case PuppetCursed: return "Puppet";
                case Ring: return "Ring";
                case Scepter: return "Scepter";
                case Seeds: return "Seeds";
                case Shard: return "Shard";
                case Staff: return "Staff";
                case SummoningStatue: return "Summoning Statue";
                case Wand: return "Wand";
                case Count: return "Count";
                default: return "item::misc_type::Name(ENUM=out_of_range)";
            }
        }

        // these three functions are complete and mutually exclusive

        static constexpr bool IsQuestItem(const Enum ENUM) noexcept
        {
            return (
                (ENUM == AngelBraid) || (ENUM == Crumhorn) || (ENUM == DevilHorn)
                || (ENUM == GolemFinger) || (ENUM == HurdyGurdy) || (ENUM == Icicle)
                || (ENUM == LichHand) || (ENUM == Lyre) || (ENUM == MummyHand)
                || (ENUM == PetrifiedSnake) || (ENUM == PipeAndTabor) || (ENUM == Recorder)
                || (ENUM == UnicornHorn) || (ENUM == Viol));
        }

        static constexpr bool IsStandalone(const Enum ENUM) noexcept
        {
            return (
                (ENUM == Bust) || (ENUM == Doll) || (ENUM == DollBlessed) || (ENUM == DollCursed)
                || (ENUM == DriedHead) || (ENUM == DrumLute) || (ENUM == Egg) || (ENUM == Embryo)
                || (ENUM == FigurineBlessed) || (ENUM == FigurineCursed) || (ENUM == Goblet)
                || (ENUM == Key) || (ENUM == LockPicks) || (ENUM == Mirror) || (ENUM == Orb)
                || (ENUM == Pendant) || (ENUM == PuppetBlessed) || (ENUM == PuppetCursed)
                || (ENUM == Ring) || (ENUM == Scepter) || (ENUM == Seeds) || (ENUM == Shard)
                || (ENUM == Staff) || (ENUM == SummoningStatue) || (ENUM == Wand));
        }

        static constexpr bool IsUnique(const Enum ENUM) noexcept
        {
            return (
                (ENUM != Not) && (ENUM != Count) && (IsQuestItem(ENUM) == false)
                && (IsStandalone(ENUM) == false));
        }

        static constexpr bool IsMusicalInstrument(const Enum ENUM) noexcept
        {
            return (
                (ENUM == DrumLute) || (ENUM == Crumhorn) || (ENUM == HurdyGurdy) || (ENUM == Lyre)
                || (ENUM == PipeAndTabor) || (ENUM == Recorder) || (ENUM == Viol));
        }

        static constexpr bool HasPixieVersion(const Enum ENUM) noexcept
        {
            return (
                (ENUM == JeweledArmband) || (ENUM == CapeCommanders) || (ENUM == CapeKings)
                || (ENUM == CapeGenerals) || (ENUM == ShadeCloak) || (ENUM == GhostSheet)
                || (ENUM == SpecterRobe) || (ENUM == BurialShroud) || IsMusicalInstrument(ENUM)
                || (ENUM == Staff));
        }

        static constexpr bool HasOnlyPixieVersion(const Enum) { return false; }

        static constexpr bool MustBePixieVersionForPixiesToEquip(const Enum ENUM) noexcept
        {
            return (
                IsMusicalInstrument(ENUM) || (ENUM == JeweledArmband) || (ENUM == CapeCommanders)
                || (ENUM == CapeKings) || (ENUM == CapeGenerals) || (ENUM == ShadeCloak)
                || (ENUM == GhostSheet) || (ENUM == SpecterRobe) || (ENUM == BurialShroud)
                || (ENUM == Staff) ||

                // these items must be pixie for pixies to equip, but there are no pixie versions of
                // these items so pixies can never equip them.
                (ENUM == ExoticGoldenGong) || (ENUM == IslanderHeaddress)
                || (ENUM == HornOfTheHorde) || (ENUM == WarTrumpet)
                || (ENUM == WarhorseMarionette));
        }

        static constexpr float ReligiousRatio(const Enum ENUM) noexcept
        {
            if ((ENUM == SpecterChains) || (ENUM == DoveBloodVial))
            {
                return 0.1f;
            }
            else if ((ENUM == SpecterRobe) || (ENUM == ManaAmulet))
            {
                return 0.2f;
            }
            else if (
                (ENUM == ShadeCloak) || (ENUM == SanguineRelic) || (ENUM == EvilRabbitsFoot)
                || (ENUM == BleedingTrophy) || (ENUM == WraithTalisman))
            {
                return 0.5f;
            }
            else if (ENUM == Bust)
            {
                return 0.6f;
            }
            else if (ENUM == SepultureDecoration)
            {
                return 0.65f;
            }
            else if (
                (ENUM == RingMendicant) || (ENUM == DriedHead) || (ENUM == DemonDiary)
                || (ENUM == MacabreManuscript))
            {
                return 0.75f;
            }
            else if (
                (ENUM == BurialShroud) || (ENUM == MortuaryOrnament) || (ENUM == GhostSheet)
                || (ENUM == FuneralRecord) || (ENUM == FriarsChronicle))
            {
                return 0.8f;
            }
            else if (
                (ENUM == SaintsJournal) || (ENUM == SaintCameoPin) || (ENUM == RequiemRegister)
                || (ENUM == RingMonk) || (ENUM == LastRitesScroll) || (ENUM == HolyEpic)
                || (ENUM == DruidLeaf) || (ENUM == ShamanRainmaker) || (ENUM == FigurineBlessed)
                || (ENUM == FigurineCursed) || (ENUM == DollBlessed) || (ENUM == DollCursed))
            {
                return 0.9f;
            }
            else if ((ENUM == RingPriest) || (ENUM == BishopsHanky))
            {
                return 0.99f;
            }
            else
            {
                return 0.0f;
            }
        }

        static constexpr bool HasNonFleshEyes(const Enum ENUM) noexcept
        {
            return (
                (ENUM == Bust) || (ENUM == Doll) || (ENUM == DollBlessed) || (ENUM == DollCursed)
                || (ENUM == FigurineBlessed) || (ENUM == FigurineCursed) || (ENUM == PuppetBlessed)
                || (ENUM == PuppetCursed) || (ENUM == MadRatJuju) || (ENUM == SummoningStatue)
                || (ENUM == WraithTalisman) || (ENUM == BronzeTroll) || (ENUM == WeaselTotem)
                || (ENUM == DriedHead)); // yeah it's flesh but what the hell having special
                                         // eyes is cool (oh and it's not edible so it's okay)
        }

        static constexpr bool IsBlessed(const Enum ENUM) noexcept
        {
            return (
                (ENUM == DollBlessed) || (ENUM == FigurineBlessed) || (ENUM == PuppetBlessed)
                || (ENUM == Bust));
        }

        static constexpr bool IsCursed(const Enum ENUM) noexcept
        {
            return (
                (ENUM == DollCursed) || (ENUM == FigurineCursed) || (ENUM == PuppetCursed)
                || (ENUM == DriedHead));
        }

        static constexpr Weight_t Weight(const misc_type::Enum ENUM) noexcept
        {
            // clang-format off
            switch (ENUM)
            {
                case DruidLeaf:             { return 1_weight;   }
                case GriffinFeather:        { return 2_weight;   }
                case CobraTooth:            { return 3_weight;   }
                case AngelBraid:            { return 4_weight;   }
                case BerserkersBeard:       { return 5_weight;   }
                case RattlesnakeTail:       { return 6_weight;   }
                case RavenClaw:             { return 6_weight;   }
                case EyeHawk:               { return 7_weight;   }
                case EyeGiantOwl:           { return 8_weight;   }
                case EyeCyclops:            { return 9_weight;   }
                case BishopsHanky:          { return 9_weight;   }
                case EvilRabbitsFoot:       { return 9_weight;   }
                case HobgoblinNose:         { return 10_weight;  }
                case PantherPaw:            { return 10_weight;  }
                case DragonToothWhistle:    { return 10_weight;  }
                case Ring:                  { return 10_weight;  }
                case RingHobo:              { return 11_weight;  }
                case RingMendicant:         { return 11_weight;  }
                case RingMonk:              { return 11_weight;  }
                case RingPriest:            { return 11_weight;  }
                case Pin_Crown:             { return 11_weight;  }
                case Pin_Feather:           { return 11_weight;  }
                case Pin_Fist:              { return 11_weight;  }
                case Pin_Hourglass:         { return 11_weight;  }
                case Pin_Key:               { return 11_weight;  }
                case Pin_Mask:              { return 11_weight;  }
                case SaintCameoPin:         { return 11_weight;  }
                case NecklaceJeweledAnkh:   { return 12_weight;  }
                case NecklaceSharkTooth:    { return 12_weight;  }
                case NecklaceVampiresTooth: { return 12_weight;  }
                case Brooch_Crown:          { return 15_weight;  }
                case Brooch_Feather:        { return 15_weight;  }
                case Brooch_Fist:           { return 15_weight;  }
                case Brooch_Hourglass:      { return 15_weight;  }
                case Brooch_Key:            { return 15_weight;  }
                case Brooch_Mask:           { return 15_weight;  }
                case Charm_Crown:           { return 17_weight;  }
                case Charm_Feather:         { return 17_weight;  }
                case Charm_Fist:            { return 17_weight;  }
                case Charm_Hourglass:       { return 17_weight;  }
                case Charm_Key:             { return 17_weight;  }
                case Charm_Mask:            { return 17_weight;  }
                case Seeds:                 { return 18_weight;  }
                case Wand:                  { return 20_weight;  }
                case MummyHand:             { return 25_weight;  }
                case LichHand:              { return 25_weight;  }
                case Icicle:                { return 26_weight;  }
                case UnicornHorn:           { return 27_weight;  }
                case DevilHorn:             { return 28_weight;  }
                case Scepter:               { return 29_weight;  }//intentionally set low
                case FlagFanatics:          { return 30_weight;  }
                case FlagTribal:            { return 30_weight;  }
                case FlagRegalCaptains:     { return 30_weight;  }
                case MadRatJuju:            { return 31_weight;  }
                case Pendant:               { return 31_weight;  }
                case ManaAmulet:            { return 33_weight;  }
                case WeaselTotem:           { return 33_weight;  }
                case SprintersLegtie:       { return 33_weight;  }
                case HangmansNoose:         { return 34_weight;  }
                case JeweledPrincessVeil:   { return 35_weight;  }
                case Doll:                  { return 35_weight;  }
                case DollCursed:            { return 35_weight;  }
                case PuppetCursed:          { return 35_weight;  }
                case DollBlessed:           { return 35_weight;  }
                case FigurineBlessed:       { return 35_weight;  }
                case PuppetBlessed:         { return 35_weight;  }
                case FigurineCursed:        { return 35_weight;  }
                case Recorder:              { return 36_weight;  }
                case SirenConch:            { return 36_weight;  }
                case TurtleShell:           { return 36_weight;  }
                case Embryo:                { return 36_weight;  }
                case ChimeraBone:           { return 37_weight;  }
                case DriedIguana:           { return 38_weight;  }
                case DriedSkink:            { return 38_weight;  }
                case DriedGecko:            { return 38_weight;  }
                case DriedLizard:           { return 38_weight;  }
                case DriedFrog:             { return 38_weight;  }
                case DriedSalamander:       { return 38_weight;  }
                case DriedToad:             { return 38_weight;  }
                case PetrifiedSnake:        { return 38_weight;  }
                case BasiliskTongue:        { return 38_weight;  }
                case VultureGizzard:        { return 38_weight;  }
                case ImpTail:               { return 39_weight;  }
                case BloodyDragonScale:     { return 39_weight;  }
                case DriedHead:             { return 40_weight;  }
                case MaskRascal:            { return 40_weight;  }
                case MaskMourners:          { return 40_weight;  }
                case TuningFork:            { return 50_weight;  }
                case Key:                   { return 50_weight;  }
                case LockPicks:             { return 55_weight;  }
                case PixieBell:             { return 55_weight;  }
                case ScoundrelSack:         { return 60_weight;  }
                case SwindlersBag:          { return 60_weight;  }
                case TricksterPouch:        { return 60_weight;  }
                case JeweledHandbag:        { return 60_weight;  }
                case DoveBloodVial:         { return 62_weight;  }
                case BottleOfBansheeScreams:{ return 62_weight;  }
                case Mirror:                { return 63_weight;  }
                case MagnifyingGlass:       { return 64_weight;  }
                case JeweledArmband:        { return 65_weight;  }
                case LastRitesScroll:       { return 65_weight;  }
                case ShamanRainmaker:       { return 65_weight;  }
                case CapeCommanders:        { return 66_weight;  }
                case CapeGenerals:          { return 66_weight;  }
                case CapeKings:             { return 66_weight;  }
                case WolfenFur:             { return 70_weight;  }
                case MinotaurHide:          { return 70_weight;  }
                case WraithTalisman:        { return 75_weight;  }
                case SepultureDecoration:   { return 75_weight;  }
                case MortuaryOrnament:      { return 75_weight;  }
                case BleedingTrophy:        { return 75_weight;  }
                case SanguineRelic:         { return 75_weight;  }
                case GlassCat:              { return 75_weight;  }
                case Goblet:                { return 78_weight;  }
                case GhostSheet:            { return 80_weight;  }
                case ShadeCloak:            { return 80_weight;  }
                case SpecterRobe:           { return 80_weight;  }
                case BurialShroud:          { return 80_weight;  }
                case FuneralRecord:         { return 120_weight; }
                case MacabreManuscript:     { return 120_weight; }
                case RequiemRegister:       { return 120_weight; }
                case DemonDiary:            { return 120_weight; }
                case SaintsJournal:         { return 120_weight; }
                case FriarsChronicle:       { return 120_weight; }
                case HolyEpic:              { return 120_weight; }
                case MagicHorseshoe:        { return 130_weight; }
                case GolemFinger:           { return 140_weight; }
                case Shard:                 { return 140_weight; }
                case Crumhorn:              { return 140_weight; }
                case Staff:                 { return 150_weight; }
                case SummoningStatue:       { return 160_weight; }
                case BronzeTroll:           { return 160_weight; }
                case RoyalScoutSpyglass:    { return 160_weight; }
                case Orb:                   { return 160_weight; }
                case Bust:                  { return 165_weight; }
                case Egg:                   { return 200_weight; }
                case SpiderEggs:            { return 200_weight; }
                case Lyre:                  { return 205_weight; }
                case CrystalChimes:         { return 205_weight; }
                case IslanderHeaddress:     { return 210_weight; }
                case PipeAndTabor:          { return 220_weight; }
                case Viol:                  { return 230_weight; }
                case DrumLute:              { return 250_weight; }
                case ReaperScythe:          { return 260_weight; }
                case HurdyGurdy:            { return 280_weight; }
                case WarTrumpet:            { return 300_weight; }
                case HornOfTheHorde:        { return 300_weight; }
                case WarhorseMarionette:    { return 320_weight; }
                case SpecterChains:         { return 400_weight; }
                case ExoticGoldenGong:      { return 450_weight; }
                case Not:
                case Count:
                default: return 0_weight;
            }
            // clang-format on
        }

        static constexpr bool IsUseable(const Enum ENUM) noexcept
        {
            return (
                IsSummoning(ENUM) || (ENUM == CrystalChimes) || (ENUM == DoveBloodVial)
                || (ENUM == DragonToothWhistle) || (ENUM == DriedFrog) || (ENUM == DriedGecko)
                || (ENUM == DriedIguana) || (ENUM == DriedLizard) || (ENUM == DriedSalamander)
                || (ENUM == DriedSkink) || (ENUM == DriedToad) || (ENUM == ExoticGoldenGong)
                || (ENUM == MagnifyingGlass) || (ENUM == PixieBell) || (ENUM == ShamanRainmaker)
                || (ENUM == SpecterChains) || (ENUM == VultureGizzard) || (ENUM == WarTrumpet)
                || (ENUM == WraithTalisman) || (ENUM == Key) || (ENUM == BottleOfBansheeScreams));
        }

        static constexpr bool IsSummoning(const Enum ENUM) noexcept
        {
            return (
                (ENUM == Egg) || (ENUM == Embryo) || (ENUM == Seeds) || (ENUM == SummoningStatue)
                || ((ENUM == SpiderEggs)));
        }

        static constexpr category::Enum EquipCategory(const Enum ENUM) noexcept
        {
            switch (ENUM)
            {
                case BerserkersBeard:
                case Brooch_Crown:
                case Brooch_Feather:
                case Brooch_Fist:
                case Brooch_Hourglass:
                case Brooch_Key:
                case Brooch_Mask:
                case BurialShroud:
                case CapeCommanders:
                case CapeGenerals:
                case CapeKings:
                case Charm_Crown:
                case Charm_Feather:
                case Charm_Fist:
                case Charm_Hourglass:
                case Charm_Key:
                case Charm_Mask:
                case DruidLeaf:
                case GhostSheet:
                case HangmansNoose:
                case IslanderHeaddress:
                case JeweledArmband:
                case JeweledPrincessVeil:
                case ManaAmulet:
                case MaskMourners:
                case MaskRascal:
                case NecklaceJeweledAnkh:
                case NecklaceSharkTooth:
                case NecklaceVampiresTooth:
                case Pin_Crown:
                case Pin_Feather:
                case Pin_Fist:
                case Pin_Hourglass:
                case Pin_Key:
                case Pin_Mask:
                case RingHobo:
                case RingMendicant:
                case RingMonk:
                case RingPriest:
                case SaintCameoPin:
                case ShadeCloak:
                case SpecterRobe:
                case SprintersLegtie:
                case Pendant:
                case Ring:
                {
                    return static_cast<category::Enum>(category::Equipable | category::Wearable);
                }

                case AngelBraid:
                case DevilHorn:
                case GolemFinger:
                case Icicle:
                case LichHand:
                case MummyHand:
                case PetrifiedSnake:
                case UnicornHorn:
                case Orb:
                case Scepter:
                case Wand:
                case Shard:
                {
                    return static_cast<category::Enum>(category::Equipable | category::OneHanded);
                }

                case Crumhorn:
                case HurdyGurdy:
                case Lyre:
                case PipeAndTabor:
                case Recorder:
                case Viol:
                case DrumLute:
                case ReaperScythe:
                case Staff:
                {
                    return static_cast<category::Enum>(category::Equipable | category::TwoHanded);
                }

                case BasiliskTongue:
                case BishopsHanky:
                case BleedingTrophy:
                case BloodyDragonScale:
                case BottleOfBansheeScreams:
                case BronzeTroll:
                case ChimeraBone:
                case CobraTooth:
                case CrystalChimes:
                case DemonDiary:
                case DoveBloodVial:
                case DragonToothWhistle:
                case DriedFrog:
                case DriedGecko:
                case DriedIguana:
                case DriedLizard:
                case DriedSalamander:
                case DriedSkink:
                case DriedToad:
                case EvilRabbitsFoot:
                case ExoticGoldenGong:
                case EyeCyclops:
                case EyeGiantOwl:
                case EyeHawk:
                case FlagFanatics:
                case FlagRegalCaptains:
                case FlagTribal:
                case FriarsChronicle:
                case FuneralRecord:
                case GlassCat:
                case GriffinFeather:
                case MadRatJuju:
                case HobgoblinNose:
                case HolyEpic:
                case HornOfTheHorde:
                case ImpTail:
                case JeweledHandbag:
                case LastRitesScroll:
                case MacabreManuscript:
                case MagicHorseshoe:
                case MagnifyingGlass:
                case MinotaurHide:
                case MortuaryOrnament:
                case PantherPaw:
                case PixieBell:
                case RattlesnakeTail:
                case RavenClaw:
                case RequiemRegister:
                case RoyalScoutSpyglass:
                case SaintsJournal:
                case SanguineRelic:
                case ScoundrelSack:
                case SepultureDecoration:
                case ShamanRainmaker:
                case SirenConch:
                case SpecterChains:
                case Seeds:
                case SwindlersBag:
                case SpiderEggs:
                case TricksterPouch:
                case TuningFork:
                case TurtleShell:
                case VultureGizzard:
                case WarhorseMarionette:
                case WarTrumpet:
                case WeaselTotem:
                case WolfenFur:
                case WraithTalisman:
                case Bust:
                case Doll:
                case DollBlessed:
                case DollCursed:
                case DriedHead:
                case Egg:
                case Embryo:
                case FigurineBlessed:
                case FigurineCursed:
                case Goblet:
                case Key:
                case LockPicks:
                case Mirror:
                case PuppetBlessed:
                case PuppetCursed:
                case SummoningStatue:
                case Not: return category::None;

                case Count:
                default: return category::None;
            }
        }

        static constexpr bool AllowsCasting(const Enum ENUM) noexcept
        {
            return (
                (ENUM == AngelBraid) || (ENUM == DevilHorn) || (ENUM == GolemFinger)
                || (ENUM == Icicle) || (ENUM == LichHand) || (ENUM == MummyHand)
                || (ENUM == PetrifiedSnake) || (ENUM == UnicornHorn) || (ENUM == Orb)
                || (ENUM == Scepter) || (ENUM == Wand) || (ENUM == Shard) || (ENUM == Staff));
        }

        static constexpr bool IsWeapon(const Enum ENUM) noexcept
        {
            return (ENUM == ReaperScythe) || (ENUM == Staff);
        }

        static constexpr bool IsArmor(const Enum ENUM) noexcept
        {
            return (
                (ENUM == BurialShroud) || (ENUM == CapeCommanders) || (ENUM == CapeGenerals)
                || (ENUM == CapeKings) || (ENUM == GhostSheet) || (ENUM == ShadeCloak)
                || (ENUM == SpecterRobe));
        }

        static const ElementEnumVec_t ElementTypes(const Enum, const bool WILL_INCLUDE_NONE);
    };

    struct set_type : public EnumBaseCounting<EnumFirstValue::Not>
    {
        enum Enum : EnumUnderlying_t
        {
            Not = 0,
            TheAssassins,
            TheNeverwinter,
            TheTickler,
            TheMagus,
            TheNecromancers,
            TheWarlocks,
            TheLichKings,
            TheSages,
            TheShamans,
            TheOracles,
            TheAngelic,
            TheBalladeers,
            TheTroubadours,
            TheMuses,
            TheCavaliers,
            TheChampions,
            ThePaladins,
            TheBerserkers,
            TheRosewood,
            TheDragonslayers,
            TheEventideRider,
            TheHunters,
            TheSureShot,
            TheMarksmans,
            TheDeadeye,
            TheDuskRanger,
            TheVenomBow,
            TheCritterChannelers,
            TheMammalianHead,
            TheSavageTaskmasters,
            TheMonsterOverseers,
            TheBeastRulers,
            Count
        };

        static constexpr std::string_view Name(const set_type::Enum SET_TYPE) noexcept
        {
            switch (SET_TYPE)
            {
                case Not: return "Not";
                case TheAssassins: return "The Assassin's";
                case TheNeverwinter: return "The Neverwinter";
                case TheTickler: return "The Tickler's";
                case TheMagus: return "The Magus'";
                case TheNecromancers: return "The Necromancer's";
                case TheWarlocks: return "The Warlock's";
                case TheLichKings: return "The Lich King's";
                case TheSages: return "The Sage's";
                case TheShamans: return "The Shaman's";
                case TheOracles: return "The Oracle's";
                case TheAngelic: return "The Angelic";
                case TheBalladeers: return "The Balladeer's";
                case TheTroubadours: return "The Troubadour's";
                case TheMuses: return "The Muses'";
                case TheCavaliers: return "The Cavalier's";
                case TheChampions: return "The Champion's";
                case ThePaladins: return "The Paladin's";
                case TheBerserkers: return "The Berserker's";
                case TheRosewood: return "The Rosewood";
                case TheDragonslayers: return "The Dragonslayer's";
                case TheEventideRider: return "The Eventide Rider's";
                case TheHunters: return "The Hunter's";
                case TheSureShot: return "The Sure Shot's";
                case TheMarksmans: return "The Marksman's";
                case TheDeadeye: return "The Deadeye's";
                case TheDuskRanger: return "The Dusk Ranger's";
                case TheVenomBow: return "The Venom Bow";
                case TheCritterChannelers: return "The Critter Channeler's";
                case TheMammalianHead: return "The Mammalian Head's";
                case TheSavageTaskmasters: return "The Savage Taskmaster's";
                case TheMonsterOverseers: return "The Monster Overseer's";
                case TheBeastRulers: return "The Beast Ruler's";
                case Count: return "Count";
                default: return "item::set_type::Name(ENUM=out_of_range)";
            }
        }

        static constexpr creature::role::Enum Role(const set_type::Enum SET_TYPE) noexcept
        {
            switch (SET_TYPE)
            {
                case Not: return creature::role::Count;

                case TheAssassins:
                case TheTickler:
                case TheNeverwinter: return creature::role::Thief;

                case TheMagus:
                case TheNecromancers:
                case TheWarlocks:
                case TheLichKings: return creature::role::Sorcerer;

                case TheSages:
                case TheShamans:
                case TheOracles:
                case TheAngelic: return creature::role::Cleric;

                case TheBalladeers:
                case TheTroubadours:
                case TheMuses: return creature::role::Bard;

                case TheCavaliers:
                case TheChampions:
                case ThePaladins:
                case TheBerserkers:
                case TheRosewood:
                case TheDragonslayers:
                case TheEventideRider: return creature::role::Knight;

                case TheHunters:
                case TheSureShot:
                case TheMarksmans:
                case TheDeadeye:
                case TheDuskRanger:
                case TheVenomBow: return creature::role::Archer;

                case TheCritterChannelers:
                case TheMammalianHead:
                case TheSavageTaskmasters:
                case TheMonsterOverseers:
                case TheBeastRulers: return creature::role::Beastmaster;

                case Count:
                default: return creature::role::Count;
            }
        }
    };

    struct named_type : public EnumBaseCounting<EnumFirstValue::Not>
    {
        enum Enum : EnumUnderlying_t
        {
            Not = 0,
            Arctic,
            Army,
            Betrayer,
            Burglar,
            Burning,
            Charlatans,
            Charred,
            Chill,
            Dancing,
            Daring,
            Dark,
            Dawn,
            Diabolic,
            Dusk,
            Elite,
            Fiendish,
            Fiery,
            Focus,
            Frigid,
            Frozen,
            Gladiator,
            Gloom,
            Glory,
            Heroes,
            Honest,
            Icy,
            Imposters,
            Infernal,
            Knaves,
            Light,
            Marauder,
            Misery,
            Moon,
            Mountebank,
            Muggers,
            Nile,
            Noble,
            Pickpocket,
            Pirate,
            Princes,
            Proud,
            Pure,
            Raging,
            Ranger,
            Robbers,
            Samurai,
            Searing,
            Soldiers,
            Sorrow,
            Sun,
            Thief,
            Thors,
            Thugs,
            Twilight,
            Valiant,
            Wardens,
            Wicked,
            Winter,
            Woe,
            Count
        };

        static constexpr std::string_view Name(const Enum NAMED_TYPE) noexcept
        {
            switch (NAMED_TYPE)
            {
                case Charlatans: return "Charlatan's";
                case Heroes: return "Hero's";
                case Imposters: return "Imposter's";
                case Knaves: return "Knave's";
                case Muggers: return "Mugger's";
                case Princes: return "Prince's";
                case Robbers: return "Robber's";
                case Soldiers: return "Soldier's";
                case Thors: return "Thor's";
                case Thugs: return "Thug's";
                case Wardens: return "Warden's";
                case Not: return "Not";
                case Arctic: return "Arctic";
                case Army: return "Army";
                case Betrayer: return "Betrayer";
                case Burglar: return "Burglar";
                case Burning: return "Burning";
                case Charred: return "Charred";
                case Chill: return "Chill";
                case Dancing: return "Dancing";
                case Daring: return "Daring";
                case Dark: return "Dark";
                case Dawn: return "Dawn";
                case Diabolic: return "Diabolic";
                case Dusk: return "Dusk";
                case Elite: return "Elite";
                case Fiendish: return "Fiendish";
                case Fiery: return "Fiery";
                case Focus: return "Focus";
                case Frigid: return "Frigid";
                case Frozen: return "Frozen";
                case Gladiator: return "Gladiator";
                case Gloom: return "Gloom";
                case Glory: return "Glory";
                case Honest: return "Honest";
                case Icy: return "Icy";
                case Infernal: return "Infernal";
                case Light: return "Light";
                case Marauder: return "Marauder";
                case Misery: return "Misery";
                case Moon: return "Moon";
                case Mountebank: return "Mountebank";
                case Nile: return "Nile";
                case Noble: return "Noble";
                case Pickpocket: return "Pickpocket";
                case Pirate: return "Pirate";
                case Proud: return "Proud";
                case Pure: return "Pure";
                case Searing: return "Searing";
                case Raging: return "Raging";
                case Ranger: return "Ranger";
                case Samurai: return "Samurai";
                case Sorrow: return "Sorrow";
                case Sun: return "Sun";
                case Thief: return "Thief";
                case Twilight: return "Twilight";
                case Valiant: return "Valiant";
                case Wicked: return "Wicked";
                case Winter: return "Winter";
                case Woe: return "Woe";
                case Count: return "Count";
                default: return "item::named_type::Name(ENUM=out_of_range)";
            }
        }

        static const ElementEnumVec_t ElementTypes(const Enum, const bool WILL_INCLUDE_NONE);
    };

    struct weapon_type : public EnumBaseBitField<SeparatorAlwaysSlash>
    {
        enum Enum : EnumUnderlying_t
        {
            None = 0,

            // all weapons are either Melee or Projectile
            Melee = 1 << 0,
            Projectile = 1 << 1,

            BodyPart = 1 << 2,

            // allows combat verbs slice/cut/slash/etc
            Bladed = 1 << 3,

            // allows combat verbs stab/etc
            Pointed = 1 << 4,

            Sword = 1 << 5,
            Axe = 1 << 6,
            Whip = 1 << 7,
            Knife = 1 << 8,
            Club = 1 << 9,
            Staff = 1 << 10,
            BladedStaff = 1 << 11,

            Last = BladedStaff
        };

        static const std::string
            Name(const Enum WEAPON_TYPE, const EnumStringHow & HOW = EnumStringHow());
    };

    struct armor_type : public EnumBaseCounting<EnumFirstValue::Not>
    {
        enum Enum : EnumUnderlying_t
        {
            Not,
            Shield,
            Helm,
            Gauntlets,
            Pants,
            Boots,

            // this type includes all torso coverings, i.e. plate/mail/scale armor
            Shirt,

            Bracers,
            Aventail,

            // either vest, robe, or cloak
            Covering,

            // only used by creatures with skin that is significant to their armor rating, such
            // as dragons with scales but not for human flesh
            Skin,

            Count
        };

        static constexpr bool DoesRequireBaseType(const armor_type::Enum ARMOR_TYPE) noexcept
        {
            switch (ARMOR_TYPE)
            {
                case Gauntlets:
                case Pants:
                case Boots:
                case Shirt:
                case Bracers:
                case Aventail: return true;

                case Not:
                case Shield:
                case Helm:
                case Covering:
                case Skin:
                case Count:
                default: return false;
            }
        }
    };

    struct body_part : public EnumBaseCounting<EnumFirstValue::Valid>
    {
        enum Enum : EnumUnderlying_t
        {
            Fists = 0,
            Claws,
            Tentacles,
            Bite,
            Breath,
            Skin,
            Count
        };
    };

    // Responsible for defining the relationship between an item's name and materials.
    struct name_material_type : public EnumBaseCounting<EnumFirstValue::Valid>
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

#endif // HEROESPATH_ITEM_ITEMTYPEENUM_HPP_INCLUDED
