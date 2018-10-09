// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// enchantment-factory.cpp
//
#include "enchantment-factory.hpp"

#include "creature/enchantment-warehouse.hpp"
#include "creature/enchantment.hpp"
#include "item/item.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/log-macros.hpp"

#include <exception>
#include <numeric>
#include <sstream>

namespace heroespath
{
namespace creature
{

    const EnchantmentPVec_t EnchantmentFactory::MakeAndStore(
        const item::TypeWrapper & TYPE_WRAPPER,
        const item::material::Enum MATERIAL_PRIMARY,
        const item::material::Enum MATERIAL_SECONDARY,
        const bool IS_WEAPON,
        const bool IS_ARMOR) const
    {
        if (TYPE_WRAPPER.set != item::set_type::Not)
        {
            // if part of a set, then only the set enchantments will apply
            return { NewFromSetType(TYPE_WRAPPER.set) };
        }
        else
        {
            EnchantmentPVec_t enchantmentsPVec;

            if (TYPE_WRAPPER.element != item::element_type::None)
            {
                enchantmentsPVec.emplace_back(NewFromElementType(
                    TYPE_WRAPPER.element, IS_WEAPON, MATERIAL_PRIMARY, MATERIAL_SECONDARY));
            }

            if ((TYPE_WRAPPER.name != item::named_type::Not)
                && (TYPE_WRAPPER.name != item::named_type::Count))
            {
                enchantmentsPVec.emplace_back(NewFromNamedType(
                    TYPE_WRAPPER.name, MATERIAL_PRIMARY, MATERIAL_SECONDARY, IS_ARMOR, IS_WEAPON));
            }

            if (TYPE_WRAPPER.misc != item::misc_type::Not)
            {
                for (const auto & ENCHANTMENT_PTR :
                     NewFromMiscType(TYPE_WRAPPER.misc, MATERIAL_PRIMARY, MATERIAL_SECONDARY))
                {
                    enchantmentsPVec.emplace_back(ENCHANTMENT_PTR);
                }
            }

            return enchantmentsPVec;
        }
    }

    Score_t EnchantmentFactory::TreasureScore(
        const item::misc_type::Enum MISC_TYPE,
        const item::material::Enum MATERIAL_PRIMARY,
        const item::material::Enum MATERIAL_SECONDARY) const
    {
        using namespace item;

        Score_t score { 0_score };

        if ((MISC_TYPE == misc_type::Not) || (MISC_TYPE == misc_type::Count))
        {
            return score;
        }

        // below are score bonuses needed because enchantments don't provide enough

        if (misc_type::AllowsCasting(MISC_TYPE))
        {
            score += 150_score;
        }

        if ((misc_type::IsBlessed(MISC_TYPE)) || (misc_type::IsCursed(MISC_TYPE)))
        {
            score += 150_score;
        }

        if (misc_type::IsQuestItem(MISC_TYPE))
        {
            score += 250_score;
        }

        if (misc_type::IsSummoning(MISC_TYPE))
        {
            score += 100_score;
        }

        if (misc_type::IsUseable(MISC_TYPE))
        {
            score += 150_score;
        }

        if (MISC_TYPE == misc_type::SpiderEggs)
        {
            score += 500_score;
        }
        else if (MISC_TYPE == misc_type::Ring)
        {
            score += 20_score;
        }
        else if (MISC_TYPE == misc_type::DrumLute)
        {
            score += 100_score;
        }
        else if (MISC_TYPE == misc_type::LockPicks)
        {
            score += 200_score;
        }

        for (const auto & ENCHANTMENT :
             MakeFromMiscType(MISC_TYPE, MATERIAL_PRIMARY, MATERIAL_SECONDARY))
        {
            score += ENCHANTMENT.TreasureScore();
        }

        return score;
    }

    Score_t EnchantmentFactory::TreasureScore(const item::set_type::Enum E) const
    {
        if ((E == item::set_type::Not) || (E == item::set_type::Count))
        {
            return 0_score;
        }

        // the additional 100 is to raise the score of all set items
        return MakeFromSetType(E).TreasureScore() + MakeFromSetCompleteType(E).TreasureScore()
            + 100_score;
    }

    Score_t EnchantmentFactory::TreasureScore(
        const item::named_type::Enum NAMED_ENUM,
        const item::material::Enum MATERIAL_PRIMARY,
        const item::material::Enum MATERIAL_SECONDARY,
        const bool IS_WEAPON,
        const bool IS_ARMOR) const
    {
        if ((NAMED_ENUM == item::named_type::Not) || (NAMED_ENUM == item::named_type::Count))
        {
            return 0_score;
        }

        // the additional 450 is to raise the score of all named items
        return MakeFromNamedType(
                   NAMED_ENUM, MATERIAL_PRIMARY, MATERIAL_SECONDARY, IS_WEAPON, IS_ARMOR)
                   .TreasureScore()
            + 450_score;
    }

    Score_t EnchantmentFactory::TreasureScore(
        const item::element_type::Enum ELEMENT_TYPE,
        const bool IS_WEAPON,
        const item::material::Enum MATERIAL_PRIMARY,
        const item::material::Enum MATERIAL_SECONDARY) const
    {
        if (ELEMENT_TYPE == item::element_type::None)
        {
            return 0_score;
        }

        // the additional 750 is to raise the score of all elemental items
        return MakeFromElementType(ELEMENT_TYPE, IS_WEAPON, MATERIAL_PRIMARY, MATERIAL_SECONDARY)
                   .TreasureScore()
            + (750_score * Score_t::Make(item::element_type::CountBitsSet(ELEMENT_TYPE)));
    }

    const EnchantmentPtr_t EnchantmentFactory::Make(
        const EnchantmentType::Enum TYPE,
        const TraitSet & TRAIT_SET,
        const UseInfo & USE_INFO) const
    {
        return Make(Enchantment(TYPE, TRAIT_SET, USE_INFO));
    }

    const EnchantmentPtr_t EnchantmentFactory::Make(const Enchantment & ENCHANTMENT) const
    {
        return EnchantmentWarehouse::Access().Store(std::make_unique<Enchantment>(ENCHANTMENT));
    }

    const EnchantmentPVec_t EnchantmentFactory::NewFromMiscType(
        const item::misc_type::Enum MISC_TYPE,
        const item::material::Enum MATERIAL_PRIMARY,
        const item::material::Enum MATERIAL_SECONDARY) const
    {
        EnchantmentPVec_t enchantmentPtrs;

        for (const auto & ENCHANTMENT :
             MakeFromMiscType(MISC_TYPE, MATERIAL_PRIMARY, MATERIAL_SECONDARY))
        {
            enchantmentPtrs.emplace_back(Make(ENCHANTMENT));
        }

        return enchantmentPtrs;
    }

    const EnchantmentVec_t EnchantmentFactory::MakeFromMiscType(
        const item::misc_type::Enum MISC_TYPE,
        const item::material::Enum MATERIAL_PRIMARY,
        const item::material::Enum MATERIAL_SECONDARY) const
    {
        using namespace item;

        M_HP_ASSERT_OR_LOG_AND_THROW(
            ((MATERIAL_PRIMARY != material::Count) && (MATERIAL_PRIMARY != material::Nothing)),
            "creature::EnchantmentFactory::MakeFromMiscType(misc_type="
                << misc_type::ToString(MISC_TYPE) << ") given an invalid primary material="
                << ((MATERIAL_PRIMARY == material::Count) ? "Count" : "Nothing"));

        if ((MATERIAL_SECONDARY != material::Count) && (MATERIAL_SECONDARY != material::Nothing))
        {
            // intentionally use only the secondary material to get the USE_COUNT
            const auto USE_COUNT { material::EnchantmentBonus(
                MATERIAL_SECONDARY, MATERIAL_SECONDARY) };

            if (misc_type::IsBlessed(MISC_TYPE))
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::BoundToItem | EnchantmentType::WhenHeld),
                    TraitSet({ std::make_pair(Traits::Encounter, 3),
                               std::make_pair(Traits::BlessEffect, 3) }),
                    UseInfo(
                        USE_COUNT,
                        static_cast<game::Phase::Enum>(
                            game::Phase::Combat | game::Phase::Inventory | game::Phase::Exploring)),
                    "Used when casting or playing a Bless.",
                    250_score,
                    Enchantment::UseEffectType::Blessed) };
            }
            else if (misc_type::IsCursed(MISC_TYPE))
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::BoundToItem | EnchantmentType::WhenHeld),
                    TraitSet({ std::make_pair(Traits::Luck, -3),
                               std::make_pair(Traits::CurseEffect, 3) }),
                    UseInfo(
                        USE_COUNT,
                        static_cast<game::Phase::Enum>(
                            game::Phase::Combat | game::Phase::Inventory | game::Phase::Exploring)),
                    "Used when casting or playing a Curse.",
                    250_score,
                    Enchantment::UseEffectType::Cursed) };
            }
        }

        const auto MAT_BONUS { material::EnchantmentBonus(MATERIAL_PRIMARY, MATERIAL_SECONDARY) };

        switch (MISC_TYPE)
        {
                // quest items (see below for AngelBraid and LitchHand)
            case misc_type::Crumhorn:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Mana, 33),
                               std::make_pair(Traits::Mana, 3),
                               std::make_pair(Traits::Charm, 13),
                               std::make_pair(Traits::Intelligence, 13),
                               std::make_pair(Traits::MagicCast, 18),
                               std::make_pair(Traits::CurseCast, 18),
                               std::make_pair(Traits::CurseEffect, 18) })) };
            }
            case misc_type::DevilHorn:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem
                        | EnchantmentType::CurseWithoutItem),
                    TraitSet({ std::make_pair(Traits::Charm, -13),
                               std::make_pair(Traits::Speed, 13),
                               std::make_pair(Traits::Intelligence, 16),
                               std::make_pair(Traits::FindMagic, 26),
                               std::make_pair(Traits::Mana, 28),
                               std::make_pair(Traits::MagicEffect, 28),
                               std::make_pair(Traits::MagicCast, 14),
                               std::make_pair(Traits::CurseCast, 80),
                               std::make_pair(Traits::CurseEffect, 80),
                               std::make_pair(Traits::ShadowDamage, 28) })) };
            }
            case misc_type::GolemFinger:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Mana, 24),
                               std::make_pair(Traits::MagicEffect, 13),
                               std::make_pair(Traits::MagicCast, 18),
                               std::make_pair(Traits::FireResist, 50),
                               std::make_pair(Traits::FrostResist, 50),
                               std::make_pair(Traits::HonorResist, 50),
                               std::make_pair(Traits::ShadowResist, 50) })) };
            }
            case misc_type::HurdyGurdy:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem
                        | EnchantmentType::CurseWithoutItem),
                    TraitSet({ std::make_pair(Traits::Mana, 28),
                               std::make_pair(Traits::Charm, 18),
                               std::make_pair(Traits::Intelligence, 18),
                               std::make_pair(Traits::MagicCast, 33),
                               std::make_pair(Traits::MagicEffect, 33),
                               std::make_pair(Traits::CurseCast, 50),
                               std::make_pair(Traits::CurseEffect, 50) })) };
            }
            case misc_type::Icicle:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Mana, 18),
                               std::make_pair(Traits::MagicEffect, 18),
                               std::make_pair(Traits::MagicCast, 13),
                               std::make_pair(Traits::FrostDamage, 75),
                               std::make_pair(Traits::FrostResist, 75) })) };
            }
            case misc_type::Lyre:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Mana, 33),
                               std::make_pair(Traits::Charm, 20),
                               std::make_pair(Traits::Intelligence, 20),
                               std::make_pair(Traits::MagicCast, 50),
                               std::make_pair(Traits::MagicEffect, 50),
                               std::make_pair(Traits::FindMagic, 18),
                               std::make_pair(Traits::FindCoinsAmount, 18) })) };
            }
            case misc_type::MummyHand:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Mana, 20),
                               std::make_pair(Traits::MagicEffect, 16),
                               std::make_pair(Traits::MagicResist, 16),
                               std::make_pair(Traits::PanicOnSpell, 33),
                               std::make_pair(Traits::CurseEffect, 33),
                               std::make_pair(Traits::CurseResist, 33) })) };
            }
            case misc_type::PetrifiedSnake:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Mana, 10),
                               std::make_pair(Traits::MagicEffect, 8),
                               std::make_pair(Traits::MagicResist, 16),
                               std::make_pair(Traits::PoisonOnAll, 33) })) };
            }
            case misc_type::PipeAndTabor:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem
                        | EnchantmentType::BlessWithoutItem),
                    TraitSet({ std::make_pair(Traits::Mana, 28),
                               std::make_pair(Traits::Charm, 18),
                               std::make_pair(Traits::Intelligence, 18),
                               std::make_pair(Traits::MagicCast, 33),
                               std::make_pair(Traits::MagicEffect, 33),
                               std::make_pair(Traits::BlessCast, 50),
                               std::make_pair(Traits::BlessEffect, 50) })) };
            }
            case misc_type::Recorder:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem
                        | EnchantmentType::BlessWithoutItem),
                    TraitSet({ std::make_pair(Traits::Mana, 24),
                               std::make_pair(Traits::Charm, 13),
                               std::make_pair(Traits::Intelligence, 13),
                               std::make_pair(Traits::MagicCast, 16),
                               std::make_pair(Traits::MagicEffect, 16),
                               std::make_pair(Traits::BlessCast, 25),
                               std::make_pair(Traits::BlessEffect, 25),
                               std::make_pair(Traits::CurseResist, 25),
                               std::make_pair(Traits::ShadowResist, 18) })) };
            }
            case misc_type::UnicornHorn:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem
                        | EnchantmentType::BlessWithoutItem),
                    TraitSet({ std::make_pair(Traits::Charm, 16),
                               std::make_pair(Traits::Luck, 13),
                               std::make_pair(Traits::FindMagic, 26),
                               std::make_pair(Traits::Mana, 28),
                               std::make_pair(Traits::MagicEffect, 28),
                               std::make_pair(Traits::MagicCast, 14),
                               std::make_pair(Traits::BlessCast, 80),
                               std::make_pair(Traits::BlessEffect, 80),
                               std::make_pair(Traits::ShadowResist, 28) })) };
            }
            case misc_type::Viol:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem
                        | EnchantmentType::CurseWithoutItem),
                    TraitSet({ std::make_pair(Traits::Mana, 24),
                               std::make_pair(Traits::Charm, 13),
                               std::make_pair(Traits::Intelligence, 13),
                               std::make_pair(Traits::MagicCast, 16),
                               std::make_pair(Traits::MagicEffect, 16),
                               std::make_pair(Traits::CurseCast, 25),
                               std::make_pair(Traits::CurseEffect, 25),
                               std::make_pair(Traits::ShadowDamage, 18) })) };
            }

            // unique items
            case misc_type::BasiliskTonge:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::MagicCast, 8),
                               std::make_pair(Traits::MagicEffect, 13),
                               std::make_pair(Traits::MagicResist, 13) })) };
            }
            case misc_type::BerserkersBeard:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Intelligence, -33),
                               std::make_pair(Traits::Charm, -16),
                               std::make_pair(Traits::DamageBonusMelee, 25),
                               std::make_pair(Traits::Encounter, 8),
                               std::make_pair(Traits::MagicResist, 18) })) };
            }
            case misc_type::BishopsHanky:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Luck, 8),
                               std::make_pair(Traits::Intelligence, 13),
                               std::make_pair(Traits::Charm, 18),
                               std::make_pair(Traits::MagicCast, 8),
                               std::make_pair(Traits::MagicEffect, 16),
                               std::make_pair(Traits::MagicResist, 16),
                               std::make_pair(Traits::BlessCast, 16),
                               std::make_pair(Traits::BlessEffect, 28),
                               std::make_pair(Traits::CurseResist, 22) })) };
            }
            case misc_type::BleedingTrophy:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem
                        | EnchantmentType::CurseWithoutItem),
                    TraitSet({ std::make_pair(Traits::Luck, (MAT_BONUS * -3)),
                               std::make_pair(Traits::CurseCast, (MAT_BONUS * 3)),
                               std::make_pair(Traits::CurseEffect, 5 + (MAT_BONUS * 4)),
                               std::make_pair(Traits::FindMagic, MAT_BONUS) })) };
            }
            case misc_type::BloodyDragonScale:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::AnimalResist, 50),
                               std::make_pair(Traits::FindCoinsAmount, 10),
                               std::make_pair(Traits::FindMagic, 10),
                               std::make_pair(Traits::Charm, -26) })) };
            }
            case misc_type::BottleOfBansheeScreams:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, -20),
                               std::make_pair(Traits::Intelligence, -20),
                               std::make_pair(Traits::Encounter, -6) })) };
            }
            case misc_type::BronzeTroll:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 24),
                               std::make_pair(Traits::Charm, -24),
                               std::make_pair(Traits::Encounter, -6) })) };
            }
            case misc_type::Brooch_Crown:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Intelligence, (MAT_BONUS * 3)) })) };
            }
            case misc_type::Brooch_Feather:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Accuracy, (MAT_BONUS * 3)) })) };
            }
            case misc_type::Brooch_Fist:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, (MAT_BONUS * 3)) })) };
            }
            case misc_type::Brooch_Hourglass:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Speed, (MAT_BONUS * 3)) })) };
            }
            case misc_type::Brooch_Key:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Luck, (MAT_BONUS * 3)) })) };
            }
            case misc_type::Brooch_Mask:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, (MAT_BONUS * 3)) })) };
            }
            case misc_type::BurialShroud:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, -28),
                               std::make_pair(Traits::Luck, -16),
                               std::make_pair(Traits::CurseCast, 18),
                               std::make_pair(Traits::CurseEffect, 50),
                               std::make_pair(Traits::CurseResist, 50),
                               std::make_pair(Traits::ShadowResist, -33),
                               std::make_pair(Traits::FindReligious, 18),
                               std::make_pair(Traits::BlessResist, 33) })) };
            }
            case misc_type::CapeCommanders:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 18),
                               std::make_pair(Traits::ArmorRating, 18),
                               std::make_pair(Traits::CurseResist, 24) })) };
            }

            case misc_type::CapeGenerals:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 24),
                               std::make_pair(Traits::ArmorRating, 24),
                               std::make_pair(Traits::CurseResist, 28) })) };
            }
            case misc_type::CapeKings:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 30),
                               std::make_pair(Traits::ArmorRating, 30),
                               std::make_pair(Traits::CurseResist, 33) })) };
            }
            case misc_type::Charm_Crown:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Intelligence, (MAT_BONUS * 5)) })) };
            }
            case misc_type::Charm_Feather:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Accuracy, (MAT_BONUS * 5)) })) };
            }
            case misc_type::Charm_Fist:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, (MAT_BONUS * 5)) })) };
            }
            case misc_type::Charm_Hourglass:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Speed, (MAT_BONUS * 5)) })) };
            }
            case misc_type::Charm_Key:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Luck, (MAT_BONUS * 5)) })) };
            }
            case misc_type::Charm_Mask:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, (MAT_BONUS * 5)) })) };
            }
            case misc_type::ChimeraBone:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::MagicResist, 18),
                               std::make_pair(Traits::Charm, -50),
                               std::make_pair(Traits::Intelligence, 13),
                               std::make_pair(Traits::FindMagic, 8) })) };
            }
            case misc_type::CobraTooth:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::DamageBonusMelee, 16),
                               std::make_pair(Traits::PoisonOnAll, 36) })) };
            }
            case misc_type::CrystalChimes:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::BoundToItem | EnchantmentType::WhenUsed),
                    TraitSet({ std::make_pair(Traits::Mana, (10 + MAT_BONUS)) }),
                    UseInfo(
                        -1,
                        static_cast<game::Phase::Enum>(
                            game::Phase::Combat | game::Phase::Exploring | game::Phase::Inventory)),
                    "", // set by the Enchantment object
                    1600_score + Score_t::Make(50 * MAT_BONUS),
                    Enchantment::UseEffectType::CrystalChimes) };
            }
            case misc_type::DemonDiary:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::ShadowDamage, 13),
                               std::make_pair(Traits::ShadowResist, 33),
                               std::make_pair(Traits::CurseCast, 22),
                               std::make_pair(Traits::CurseEffect, 33),
                               std::make_pair(Traits::CurseResist, 33) })) };
            }
            case misc_type::DoveBloodVial:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::BoundToItem | EnchantmentType::WhenUsed),
                    TraitSet(),
                    UseInfo(
                        10,
                        static_cast<game::Phase::Enum>(
                            game::Phase::Combat | game::Phase::Exploring | game::Phase::Inventory)),
                    "Removes conditions: Poison, Disease, Dazed, and Daunted.",
                    1900_score,
                    Enchantment::UseEffectType::DoveBloodVial) };
            }
            case misc_type::DragonToothWhistle:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::BoundToItem | EnchantmentType::WhenUsed),
                    TraitSet(),
                    UseInfo(
                        -1,
                        static_cast<game::Phase::Enum>(
                            game::Phase::Combat | game::Phase::Inventory)),
                    "Panics non-dragon flying creatures.",
                    1500_score,
                    Enchantment::UseEffectType::DragonToothWhistle) };
            }
            case misc_type::DriedFrog:
            case misc_type::DriedGecko:
            case misc_type::DriedIguana:
            case misc_type::DriedLizard:
            case misc_type::DriedSalamander:
            case misc_type::DriedSkink:
            case misc_type::DriedToad:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::BoundToItem | EnchantmentType::WhenUsed),
                    TraitSet(),
                    UseInfo(
                        1,
                        static_cast<game::Phase::Enum>(
                            game::Phase::Combat | game::Phase::Exploring | game::Phase::Inventory)),
                    "Drains 10% Mana, and removes the Poison condition.",
                    800_score,
                    Enchantment::UseEffectType::DriedEdible) };
            }
            case misc_type::DruidLeaf:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, 10 + (MAT_BONUS * 3)),
                               std::make_pair(Traits::Intelligence, 10 + (MAT_BONUS * 3)),
                               std::make_pair(Traits::Mana, (MAT_BONUS * 2)),
                               std::make_pair(Traits::FindReligious, 1 + (MAT_BONUS / 2)),
                               std::make_pair(Traits::MagicCast, 1 + (MAT_BONUS / 2)),
                               std::make_pair(Traits::MagicEffect, MAT_BONUS),
                               std::make_pair(Traits::MagicResist, MAT_BONUS) })) };
            }
            case misc_type::EvilRabbitsFoot:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Luck, -66),
                               std::make_pair(Traits::Surprise, 6),
                               std::make_pair(Traits::Encounter, 6),
                               std::make_pair(Traits::FindMagic, -6),
                               std::make_pair(Traits::BlessCast, -66),
                               std::make_pair(Traits::BlessEffect, -66),
                               std::make_pair(Traits::BlessResist, -66),
                               std::make_pair(Traits::CurseCast, 66),
                               std::make_pair(Traits::CurseEffect, 666),
                               std::make_pair(Traits::CurseResist, 666),
                               std::make_pair(Traits::AnimalResist, -66) })) };
            }
            case misc_type::ExoticGoldenGong:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::BoundToItem | EnchantmentType::WhenUsed),
                    TraitSet(),
                    UseInfo(
                        -1,
                        static_cast<game::Phase::Enum>(
                            game::Phase::Combat | game::Phase::Exploring | game::Phase::Inventory)),
                    "Adds 20% Mana and removes the conditions: Dazed, Asleep Natural, Daunted, "
                    "and "
                    "Panicked.",
                    2400_score,
                    Enchantment::UseEffectType::GoldenGong) };
            }
            case misc_type::EyeCyclops:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::HitCritical, 1 + (MAT_BONUS / 2)),
                               std::make_pair(Traits::Surprise, (MAT_BONUS * -1)) })) };
            }
            case misc_type::EyeGiantOwl:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Accuracy, 18),
                               std::make_pair(Traits::Speed, 13),
                               std::make_pair(Traits::HitCritical, 6),
                               std::make_pair(Traits::DamageBonusProj, 8),
                               std::make_pair(Traits::Surprise, -8) })) };
            }
            case misc_type::EyeHawk:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Accuracy, 16),
                               std::make_pair(Traits::Speed, 10),
                               std::make_pair(Traits::HitCritical, 6),
                               std::make_pair(Traits::DamageBonusProj, 10),
                               std::make_pair(Traits::Surprise, -6) })) };
            }
            case misc_type::FlagFanatics:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 18),
                               std::make_pair(Traits::Speed, 13),
                               std::make_pair(Traits::Charm, -28),
                               std::make_pair(Traits::Encounter, 8),
                               std::make_pair(Traits::Surprise, 6) })) };
            }
            case misc_type::FlagRegalCaptains:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 8),
                               std::make_pair(Traits::Charm, 8),
                               std::make_pair(Traits::Encounter, -6),
                               std::make_pair(Traits::Surprise, 8),
                               std::make_pair(Traits::MagicResist, 8) })) };
            }
            case misc_type::FlagTribal:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, -16),
                               std::make_pair(Traits::Encounter, 6),
                               std::make_pair(Traits::Surprise, 8) })) };
            }
            case misc_type::FriarsChronicle:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem
                        | EnchantmentType::BlessWithoutItem),
                    TraitSet({ std::make_pair(Traits::BlessCast, 33),
                               std::make_pair(Traits::ShadowResist, 16),
                               std::make_pair(Traits::Charm, 13),
                               std::make_pair(Traits::Mana, 13),
                               std::make_pair(Traits::FindReligious, 6) })) };
            }
            case misc_type::FuneralRecord:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem
                        | EnchantmentType::CurseWithoutItem),
                    TraitSet({ std::make_pair(Traits::CurseCast, 33),
                               std::make_pair(Traits::ShadowResist, 13),
                               std::make_pair(Traits::Charm, -13),
                               std::make_pair(Traits::Intelligence, 13),
                               std::make_pair(Traits::FindReligious, 8),
                               std::make_pair(Traits::Mana, 13) })) };
            }
            case misc_type::GhostSheet:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::ArmorRating, 13),
                               std::make_pair(Traits::MagicResist, 13),
                               std::make_pair(Traits::Speed, -13),
                               std::make_pair(Traits::Encounter, -6) })) };
            }
            case misc_type::GlassCat:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Speed, 24),
                               std::make_pair(Traits::Luck, 18),
                               std::make_pair(Traits::Encounter, -3) })) };
            }
            case misc_type::GriffinFeather:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem
                        | EnchantmentType::AllowsFlight),
                    TraitSet({ std::make_pair(Traits::Speed, 18),
                               std::make_pair(Traits::Strength, 13),
                               std::make_pair(Traits::DamageBonusProj, 13) })) };
            }
            case misc_type::HangmansNoose:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem
                        | EnchantmentType::AllowsFlight),
                    TraitSet({ std::make_pair(Traits::Charm, 18),
                               std::make_pair(Traits::Luck, 13),
                               std::make_pair(Traits::CurseCast, 22),
                               std::make_pair(Traits::CurseEffect, 33),
                               std::make_pair(Traits::CurseResist, 33),
                               std::make_pair(Traits::ShadowDamage, 18) })) };
            }

            case misc_type::HobgoblinNose:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, -26),
                               std::make_pair(Traits::Luck, 13),
                               std::make_pair(Traits::Surprise, -10),
                               std::make_pair(Traits::MagicEffect, 13) })) };
            }
            case misc_type::HolyEpic:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem
                        | EnchantmentType::BlessWithoutItem),
                    TraitSet({ std::make_pair(Traits::BlessCast, 66),
                               std::make_pair(Traits::BlessEffect, 80),
                               std::make_pair(Traits::ShadowResist, 28),
                               std::make_pair(Traits::Charm, 24),
                               std::make_pair(Traits::Mana, 22),
                               std::make_pair(Traits::FindReligious, 16) })) };
            }
            case misc_type::HornOfTheHorde:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 13),
                               std::make_pair(Traits::Encounter, -8),
                               std::make_pair(Traits::Surprise, -8) })) };
            }
            case misc_type::ImpTail:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::DamageBonusMelee, 20),
                               std::make_pair(Traits::Speed, 13) })) };
            }
            case misc_type::IslanderHeaddress:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, 16),
                               std::make_pair(Traits::Mana, 13),
                               std::make_pair(Traits::MagicCast, 8),
                               std::make_pair(Traits::MagicResist, 18),
                               std::make_pair(Traits::CurseResist, 18),
                               std::make_pair(Traits::FindReligious, 13),
                               std::make_pair(Traits::Encounter, -8),
                               std::make_pair(Traits::ShadowResist, 20) })) };
            }
            case misc_type::JeweledArmband:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::ArmorRating, (MAT_BONUS * 2)) })) };
            }
            case misc_type::JeweledHandbag:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::FindMagic, 1 + (MAT_BONUS / 2)),
                               std::make_pair(Traits::FindCoinsAmount, 1 + (MAT_BONUS / 2)),
                               std::make_pair(Traits::Luck, (MAT_BONUS * 2)) })) };
            }
            case misc_type::JeweledPrincessVeil:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, 4 + MAT_BONUS),
                               std::make_pair(Traits::FindMagic, 1 + (MAT_BONUS / 2)) })) };
            }

            case misc_type::LastRitesScroll:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem
                        | EnchantmentType::CurseWithoutItem),
                    TraitSet({ std::make_pair(Traits::CurseCast, 16),
                               std::make_pair(Traits::CurseEffect, 33),
                               std::make_pair(Traits::CurseResist, 33),
                               std::make_pair(Traits::ShadowResist, 28),
                               std::make_pair(Traits::Charm, -28),
                               std::make_pair(Traits::Intelligence, 28),
                               std::make_pair(Traits::FindReligious, 16),
                               std::make_pair(Traits::Mana, 26) })) };
            }
            case misc_type::MacabreManuscript:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem
                        | EnchantmentType::CurseWithoutItem),
                    TraitSet({ std::make_pair(Traits::CurseCast, 33),
                               std::make_pair(Traits::ShadowResist, 24),
                               std::make_pair(Traits::Charm, -22),
                               std::make_pair(Traits::Intelligence, 22),
                               std::make_pair(Traits::FindReligious, 13),
                               std::make_pair(Traits::Mana, 22) })) };
            }
            case misc_type::MadRatJuju:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, 16),
                               std::make_pair(Traits::Intelligence, 33),
                               std::make_pair(Traits::Speed, 33),
                               std::make_pair(Traits::AnimalResist, -18) })) };
            }
            case misc_type::MagicHorseshoe:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Luck, 18),
                               std::make_pair(Traits::FindReligious, 4),
                               std::make_pair(Traits::FindMagic, 8),
                               std::make_pair(Traits::FindCoinsAmount, 16) })) };
            }
            case misc_type::MagnifyingGlass:
            {
                // TODO
                return {};
            }
            case misc_type::ManaAmulet:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Mana, 18 + MAT_BONUS),
                               std::make_pair(Traits::MagicEffect, 1 + (MAT_BONUS / 2)) })) };
            }
            case misc_type::MaskMourners:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, -33),
                               std::make_pair(Traits::CurseCast, 16),
                               std::make_pair(Traits::CurseEffect, 33),
                               std::make_pair(Traits::CurseResist, 50),
                               std::make_pair(Traits::ShadowResist, 28) })) };
            }
            case misc_type::MaskRascal:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Luck, 22),
                               std::make_pair(Traits::Charm, 18),
                               std::make_pair(Traits::FindMagic, 8),
                               std::make_pair(Traits::FindCoinsAmount, 16) })) };
            }
            case misc_type::MinotaurHide:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::AnimalResist, 33),
                               std::make_pair(Traits::Strength, 16),
                               std::make_pair(Traits::HitPower, 6),
                               std::make_pair(Traits::Charm, -28),
                               std::make_pair(Traits::Intelligence, -16) })) };
            }
            case misc_type::MortuaryOrnament:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::ShadowResist, 66),
                               std::make_pair(Traits::FindMagic, 13),
                               std::make_pair(Traits::FindReligious, 26) })) };
            }
            case misc_type::NecklaceJeweledAnkh:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(
                                   Traits::FindReligious,
                                   static_cast<int>(static_cast<float>(MAT_BONUS) * 1.5f)),
                               std::make_pair(Traits::FindMagic, MAT_BONUS),
                               std::make_pair(Traits::MagicResist, 1 + (MAT_BONUS / 2)),
                               std::make_pair(Traits::CurseCast, MAT_BONUS),
                               std::make_pair(Traits::CurseEffect, 13 + (MAT_BONUS * 2)),
                               std::make_pair(Traits::CurseResist, 13 + (MAT_BONUS * 2)) })) };
            }
            case misc_type::NecklaceSharkTooth:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 13),
                               std::make_pair(Traits::DamageBonusMelee, 18) })) };
            }
            case misc_type::NecklaceVampiresTooth:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, 13),
                               std::make_pair(Traits::HealthGainAll, 25),
                               std::make_pair(Traits::CurseResist, 28) })) };
            }
            case misc_type::PantherPaw:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Speed, 18),
                               std::make_pair(Traits::Strength, 13),
                               std::make_pair(Traits::AnimalResist, 13) })) };
            }
            case misc_type::Pin_Crown:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Intelligence, MAT_BONUS) })) };
            }
            case misc_type::Pin_Feather:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Accuracy, MAT_BONUS) })) };
            }
            case misc_type::Pin_Fist:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, MAT_BONUS) })) };
            }
            case misc_type::Pin_Hourglass:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Speed, MAT_BONUS) })) };
            }
            case misc_type::Pin_Key:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Luck, MAT_BONUS) })) };
            }
            case misc_type::Pin_Mask:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, MAT_BONUS) })) };
            }
            case misc_type::PixieBell:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::BoundToItem | EnchantmentType::WhenUsed),
                    TraitSet(),
                    UseInfo(
                        -1,
                        static_cast<game::Phase::Enum>(
                            game::Phase::Combat | game::Phase::Exploring | game::Phase::Inventory)),
                    "Adds 10% Mana and removes the conditions: Dazed, and Asleep Natural.",
                    1800_score,
                    Enchantment::UseEffectType::PixieBell) };
            }
            case misc_type::RattlesnakeTail:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Speed, 16),
                               std::make_pair(Traits::Encounter, 8) })) };
            }
            case misc_type::RavenClaw:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 13),
                               std::make_pair(Traits::Accuracy, 13),
                               std::make_pair(Traits::DamageBonusProj, 18) })) };
            }
            case misc_type::ReaperScythe:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::HealthGainMelee, 25) })) };
            }

            case misc_type::RequiemRegister:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem
                        | EnchantmentType::CurseWithoutItem),
                    TraitSet({ std::make_pair(Traits::CurseCast, 33),
                               std::make_pair(Traits::ShadowResist, 18),
                               std::make_pair(Traits::Charm, -18),
                               std::make_pair(Traits::Intelligence, 18),
                               std::make_pair(Traits::FindReligious, 10),
                               std::make_pair(Traits::Mana, 18) })) };
            }
            case misc_type::RingHobo:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::FindCoinsAmount, MAT_BONUS),
                               std::make_pair(Traits::FindReligious, 1 + (MAT_BONUS / 4)),
                               std::make_pair(Traits::FindMagic, 1 + (MAT_BONUS / 2)),
                               std::make_pair(Traits::Charm, 24),
                               std::make_pair(Traits::Mana, 22),
                               std::make_pair(Traits::FindReligious, 16) })) };
            }
            case misc_type::RingMendicant:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, -18),
                               std::make_pair(Traits::FindCoinsAmount, 16),
                               std::make_pair(Traits::FindMagic, 8),
                               std::make_pair(Traits::FindReligious, 18) })) };
            }
            case misc_type::RingMonk:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::MagicCast, 6),
                               std::make_pair(Traits::MagicResist, 13),
                               std::make_pair(Traits::Intelligence, 18),
                               std::make_pair(Traits::Charm, 18),
                               std::make_pair(Traits::FindCoinsAmount, 13),
                               std::make_pair(Traits::CurseResist, 28) })) };
            }
            case misc_type::RingPriest:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::MagicCast, 8),
                               std::make_pair(Traits::MagicResist, 18),
                               std::make_pair(Traits::Charm, 18),
                               std::make_pair(Traits::BlessCast, 13),
                               std::make_pair(Traits::BlessEffect, 26) })) };
            }
            case misc_type::RoyalScoutSpyglass:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Accuracy, 16),
                               std::make_pair(Traits::Surprise, -12) })) };
            }
            case misc_type::SaintCameoPin:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Accuracy, 16),
                               std::make_pair(Traits::Surprise, -12) })) };
            }
            case misc_type::SaintsJournal:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, MAT_BONUS),
                               std::make_pair(Traits::FindMagic, MAT_BONUS),
                               std::make_pair(Traits::MagicEffect, 1 + (MAT_BONUS / 2)),
                               std::make_pair(Traits::BlessCast, MAT_BONUS),
                               std::make_pair(Traits::BlessEffect, 10 + (MAT_BONUS * 2)) })) };
            }
            case misc_type::SanguineRelic:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem
                        | EnchantmentType::CurseWithoutItem),
                    TraitSet({ std::make_pair(Traits::CurseCast, (MAT_BONUS * 5)),
                               std::make_pair(Traits::CurseEffect, 10 + (MAT_BONUS * 5)),
                               std::make_pair(Traits::CurseResist, 10 + (MAT_BONUS * 5)),
                               std::make_pair(Traits::FindReligious, MAT_BONUS) })) };
            }
            case misc_type::ScoundrelSack:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::FindCoinsAmount, 18),
                               std::make_pair(Traits::FindMagic, 8),
                               std::make_pair(Traits::Luck, 18),
                               std::make_pair(Traits::Backstab, 6) })) };
            }
            case misc_type::SepultureDecoration:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::ShadowResist, 33),
                               std::make_pair(Traits::FindMagic, 8),
                               std::make_pair(Traits::FindReligious, 13) })) };
            }
            case misc_type::ShadeCloak:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::ArmorRating, 33),
                               std::make_pair(Traits::MagicResist, 26),
                               std::make_pair(Traits::Speed, -20),
                               std::make_pair(Traits::Encounter, -6) })) };
            }
            case misc_type::ShamanRainmaker:
            {
                return { Enchantment(
                             static_cast<EnchantmentType::Enum>(
                                 EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                             TraitSet({ std::make_pair(Traits::Charm, 8),
                                        std::make_pair(Traits::Encounter, 8),
                                        std::make_pair(Traits::FindMagic, 13),
                                        std::make_pair(Traits::MagicCast, 8),
                                        std::make_pair(Traits::MagicEffect, 18),
                                        std::make_pair(Traits::BlessCast, 16),
                                        std::make_pair(Traits::BlessEffect, 33) })),
                         Enchantment(
                             static_cast<EnchantmentType::Enum>(
                                 EnchantmentType::BoundToItem | EnchantmentType::WhenUsed),
                             TraitSet(),
                             UseInfo(
                                 -1,
                                 static_cast<game::Phase::Enum>(
                                     game::Phase::Combat | game::Phase::Exploring
                                     | game::Phase::Inventory)),
                             "Adds 10% Mana.",
                             600_score,
                             Enchantment::UseEffectType::ShamanRainmaker) };
            }
            case misc_type::SirenConch:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::MagicCast, 8),
                               std::make_pair(Traits::MagicEffect, 13),
                               std::make_pair(Traits::Charm, 13) })) };
            }
            case misc_type::SpecterChains:
            {
                return { Enchantment(
                             static_cast<EnchantmentType::Enum>(
                                 EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                             TraitSet({ std::make_pair(Traits::Strength, 18),
                                        std::make_pair(Traits::Encounter, -6),
                                        std::make_pair(Traits::Charm, 13) })),
                         Enchantment(
                             static_cast<EnchantmentType::Enum>(
                                 EnchantmentType::BoundToItem | EnchantmentType::WhenUsed),
                             TraitSet(),
                             UseInfo(
                                 -1,
                                 static_cast<game::Phase::Enum>(
                                     game::Phase::Combat | game::Phase::Inventory)),
                             "Panics enemies.",
                             1300_score,
                             Enchantment::UseEffectType::SpecterChains) };
            }
            case misc_type::SpecterRobe:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::ArmorRating, 20),
                               std::make_pair(Traits::MagicResist, 18),
                               std::make_pair(Traits::Speed, -13),
                               std::make_pair(Traits::Encounter, -8) })) };
            }
            case misc_type::SpiderEggs:
            {
                return { Enchantment(
                    EnchantmentType::WhenUsed, TraitSet(), UseInfo(10, game::Phase::Combat)) };
            }
            case misc_type::SprintersLegtie:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Speed, 22),
                               std::make_pair(Traits::Strength, -8) })) };
            }
            case misc_type::SwindlersBag:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::FindCoinsAmount, 22),
                               std::make_pair(Traits::FindMagic, 8),
                               std::make_pair(Traits::Luck, 22),
                               std::make_pair(Traits::HitCritical, 5) })) };
            }
            case misc_type::TricksterPouch:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::FindCoinsAmount, 13),
                               std::make_pair(Traits::FindMagic, 8),
                               std::make_pair(Traits::Luck, 16),
                               std::make_pair(Traits::Charm, 18) })) };
            }
            case misc_type::TuningFork:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, MAT_BONUS),
                               std::make_pair(Traits::MagicCast, MAT_BONUS) })) };
            }
            case misc_type::TurtleShell:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::ArmorRating, (MAT_BONUS * 3)) })) };
            }
            case misc_type::VultureGizzard:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::BoundToItem | EnchantmentType::WhenUsed),
                    TraitSet(),
                    UseInfo(
                        3,
                        static_cast<game::Phase::Enum>(
                            game::Phase::Combat | game::Phase::Exploring | game::Phase::Inventory)),
                    "Curse Poison.",
                    1000_score,
                    Enchantment::UseEffectType::VultureGizzard) };
            }
            case misc_type::WarhorseMarionette:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 22),
                               std::make_pair(Traits::ArmorRating, 22),
                               std::make_pair(Traits::Speed, -13),
                               std::make_pair(Traits::HitPower, 13),
                               std::make_pair(Traits::Intelligence, -16) })) };
            }
            case misc_type::WarTrumpet:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::BoundToItem | EnchantmentType::WhenUsed),
                    TraitSet(),
                    UseInfo(
                        -1,
                        static_cast<game::Phase::Enum>(
                            game::Phase::Combat | game::Phase::Inventory)),
                    "Panics or Daunts enemies.",
                    1100_score,
                    Enchantment::UseEffectType::WarTrumpet) };
            }
            case misc_type::WeaselTotem:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Speed, 28),
                               std::make_pair(Traits::Luck, 18),
                               std::make_pair(Traits::Encounter, -6) })) };
            }
            case misc_type::WolfenFur:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::AnimalResist, 28),
                               std::make_pair(Traits::Speed, 16),
                               std::make_pair(Traits::Charm, -16),
                               std::make_pair(Traits::Intelligence, -13) })) };
            }
            case misc_type::WraithTalisman:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({
                        std::make_pair(Traits::Charm, -16),
                        std::make_pair(Traits::FindMagic, 8),
                        std::make_pair(Traits::MagicCast, 8),
                        std::make_pair(Traits::MagicEffect, 13),
                        std::make_pair(Traits::MagicResist, 13),
                        std::make_pair(Traits::CurseCast, 13),
                        std::make_pair(Traits::CurseEffect, 33),
                        std::make_pair(Traits::CurseResist, 33),
                    })) };
            }
            case misc_type::Scepter:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Mana, 14 + (MAT_BONUS * 2)),
                               std::make_pair(Traits::MagicEffect, (MAT_BONUS * 2)),
                               std::make_pair(Traits::MagicCast, MAT_BONUS) })) };
            }

            // standalone items
            case misc_type::Egg:
            case misc_type::Embryo:
            {
                return { Enchantment(
                    EnchantmentType::WhenUsed, TraitSet(), UseInfo(1, game::Phase::Combat)) };
            }
            case misc_type::LockPicks:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Luck, 12 + MAT_BONUS),
                               std::make_pair(Traits::Speed, 11 + (MAT_BONUS / 2)),
                               std::make_pair(Traits::Backstab, 1 + (MAT_BONUS / 2)) })) };
            }
            case misc_type::Orb:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Mana, 12 + MAT_BONUS),
                               std::make_pair(Traits::MagicEffect, MAT_BONUS),
                               std::make_pair(Traits::MagicCast, 1 + (MAT_BONUS / 2)) })) };
            }
            case misc_type::Seeds:
            {
                return { Enchantment(
                    EnchantmentType::WhenUsed, TraitSet(), UseInfo(10, game::Phase::Combat)) };
            }
            case misc_type::Shard:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Mana, 11 + (MAT_BONUS / 2)),
                               std::make_pair(Traits::MagicEffect, (MAT_BONUS / 2)) })) };
            }
            case misc_type::SummoningStatue:
            {
                return { Enchantment(
                    EnchantmentType::WhenUsed,
                    TraitSet(),
                    UseInfo(MAT_BONUS, game::Phase::Combat)) };
            }

            // see blessed and cursed above
            case misc_type::Bust:
            case misc_type::DollBlessed:
            case misc_type::DollCursed:
            case misc_type::DriedHead:
            case misc_type::FigurineBlessed:
            case misc_type::FigurineCursed:
            case misc_type::PuppetBlessed:
            case misc_type::PuppetCursed:

            // no enchantments for these plain items
            case misc_type::Doll:
            case misc_type::DrumLute:
            case misc_type::Goblet:
            case misc_type::Key:
            case misc_type::Mirror:
            case misc_type::Pendant:
            case misc_type::Ring:
            case misc_type::Staff:
            case misc_type::Wand:

            // only a set_type so the set_type takes care of the enchantment
            case misc_type::AngelBraid:
            case misc_type::LitchHand:

            case misc_type::Not:
            case misc_type::Count:
            default: { return {};
            }
        }
    }

    const EnchantmentPtr_t EnchantmentFactory::NewFromSetType(const item::set_type::Enum E) const
    {
        return Make(MakeFromSetType(E));
    }

    const Enchantment EnchantmentFactory::MakeFromSetType(const item::set_type::Enum E) const
    {
        using namespace item;

        switch (E)
        {
            case set_type::TheAssassins:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Luck, 4),
                               std::make_pair(Traits::Speed, 4),
                               std::make_pair(Traits::Backstab, 2),
                               std::make_pair(Traits::FindMagic, 2) }));
            }
            case set_type::TheNeverwinter:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Luck, 8),
                               std::make_pair(Traits::Speed, 6),
                               std::make_pair(Traits::Backstab, 4),
                               std::make_pair(Traits::FindMagic, 3) }));
            }
            case set_type::TheTickler:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Luck, 13),
                               std::make_pair(Traits::Speed, 8),
                               std::make_pair(Traits::Backstab, 6),
                               std::make_pair(Traits::FindMagic, 4) }));
            }
            case set_type::TheMagus:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Intelligence, 4),
                               std::make_pair(Traits::Mana, 4),
                               std::make_pair(Traits::MagicCast, 2),
                               std::make_pair(Traits::MagicEffect, 2),
                               std::make_pair(Traits::MagicResist, 2),
                               std::make_pair(Traits::ShadowDamage, 2) }));
            }
            case set_type::TheNecromancers:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Intelligence, 8),
                               std::make_pair(Traits::Mana, 6),
                               std::make_pair(Traits::MagicCast, 4),
                               std::make_pair(Traits::MagicEffect, 4),
                               std::make_pair(Traits::MagicResist, 4),
                               std::make_pair(Traits::ShadowDamage, 4) }));
            }
            case set_type::TheWarlocks:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Intelligence, 16),
                               std::make_pair(Traits::Mana, 8),
                               std::make_pair(Traits::MagicCast, 8),
                               std::make_pair(Traits::MagicEffect, 8),
                               std::make_pair(Traits::MagicResist, 8),
                               std::make_pair(Traits::ShadowDamage, 8) }));
            }
            case set_type::TheLichKings:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem
                        | EnchantmentType::CurseWithoutItem),
                    TraitSet({ std::make_pair(Traits::Intelligence, 20),
                               std::make_pair(Traits::Speed, 8),
                               std::make_pair(Traits::Mana, 10),
                               std::make_pair(Traits::MagicCast, 13),
                               std::make_pair(Traits::MagicEffect, 13),
                               std::make_pair(Traits::MagicResist, 13),
                               std::make_pair(Traits::FireDamage, 13),
                               std::make_pair(Traits::FireResist, 13),
                               std::make_pair(Traits::ShadowDamage, 13),
                               std::make_pair(Traits::ShadowDamage, 13),
                               std::make_pair(Traits::HonorResist, 13),
                               std::make_pair(Traits::CurseCast, 13),
                               std::make_pair(Traits::CurseEffect, 13),
                               std::make_pair(Traits::CurseResist, 13),
                               std::make_pair(Traits::FindMagic, 10),
                               std::make_pair(Traits::FindReligious, 5) }));
            }
            case set_type::TheSages:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, 4),
                               std::make_pair(Traits::Mana, 4),
                               std::make_pair(Traits::MagicCast, 2),
                               std::make_pair(Traits::MagicEffect, 2),
                               std::make_pair(Traits::MagicResist, 2),
                               std::make_pair(Traits::ShadowResist, 2) }));
            }
            case set_type::TheShamans:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, 8),
                               std::make_pair(Traits::Mana, 6),
                               std::make_pair(Traits::MagicCast, 4),
                               std::make_pair(Traits::MagicEffect, 4),
                               std::make_pair(Traits::MagicResist, 4),
                               std::make_pair(Traits::ShadowResist, 4) }));
            }
            case set_type::TheOracles:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, 16),
                               std::make_pair(Traits::Mana, 8),
                               std::make_pair(Traits::MagicCast, 8),
                               std::make_pair(Traits::MagicEffect, 8),
                               std::make_pair(Traits::MagicResist, 8),
                               std::make_pair(Traits::ShadowResist, 8) }));
            }
            case set_type::TheAngelic:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem
                        | EnchantmentType::BlessWithoutItem),
                    TraitSet({ std::make_pair(Traits::Charm, 20),
                               std::make_pair(Traits::Speed, 8),
                               std::make_pair(Traits::Mana, 10),
                               std::make_pair(Traits::MagicCast, 13),
                               std::make_pair(Traits::MagicEffect, 13),
                               std::make_pair(Traits::MagicResist, 13),
                               std::make_pair(Traits::BlessCast, 13),
                               std::make_pair(Traits::BlessEffect, 13),
                               std::make_pair(Traits::CurseResist, 13),
                               std::make_pair(Traits::AnimalResist, 13),
                               std::make_pair(Traits::FindReligious, 10),
                               std::make_pair(Traits::FindMagic, 5),
                               std::make_pair(Traits::FireResist, 13),
                               std::make_pair(Traits::FrostResist, 13),
                               std::make_pair(Traits::HonorResist, 13),
                               std::make_pair(Traits::ShadowResist, 13) }));
            }
            case set_type::TheBalladeers:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, 4),
                               std::make_pair(Traits::Intelligence, 4),
                               std::make_pair(Traits::Mana, 4),
                               std::make_pair(Traits::MagicCast, 2),
                               std::make_pair(Traits::MagicEffect, 2),
                               std::make_pair(Traits::MagicResist, 2) }));
            }
            case set_type::TheTroubadours:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, 8),
                               std::make_pair(Traits::Intelligence, 8),
                               std::make_pair(Traits::Mana, 6),
                               std::make_pair(Traits::MagicCast, 5),
                               std::make_pair(Traits::MagicEffect, 5),
                               std::make_pair(Traits::MagicResist, 5) }));
            }
            case set_type::TheMuses:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, 13),
                               std::make_pair(Traits::Intelligence, 13),
                               std::make_pair(Traits::Mana, 8),
                               std::make_pair(Traits::MagicCast, 10),
                               std::make_pair(Traits::MagicEffect, 10),
                               std::make_pair(Traits::MagicResist, 10) }));
            }
            case set_type::TheCavaliers:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 2),
                               std::make_pair(Traits::Speed, 2),
                               std::make_pair(Traits::HitPower, 1) }));
            }
            case set_type::TheChampions:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 4),
                               std::make_pair(Traits::Speed, 3),
                               std::make_pair(Traits::HitPower, 1),
                               std::make_pair(Traits::HitCritical, 1),
                               std::make_pair(Traits::MagicResist, 1) }));
            }
            case set_type::ThePaladins:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 6),
                               std::make_pair(Traits::Speed, 4),
                               std::make_pair(Traits::HitPower, 2),
                               std::make_pair(Traits::HitCritical, 1),
                               std::make_pair(Traits::MagicResist, 2) }));
            }
            case set_type::TheBerserkers:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 8),
                               std::make_pair(Traits::Speed, 5),
                               std::make_pair(Traits::HitPower, 2),
                               std::make_pair(Traits::HitCritical, 2),
                               std::make_pair(Traits::MagicResist, 3) }));
            }
            case set_type::TheRosewood:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 10),
                               std::make_pair(Traits::Speed, 6),
                               std::make_pair(Traits::HitPower, 3),
                               std::make_pair(Traits::HitCritical, 2),
                               std::make_pair(Traits::MagicResist, 4) }));
            }
            case set_type::TheDragonslayers:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 13),
                               std::make_pair(Traits::Speed, 8),
                               std::make_pair(Traits::HitPower, 3),
                               std::make_pair(Traits::HitCritical, 3),
                               std::make_pair(Traits::MagicResist, 5) }));
            }
            case set_type::TheEventideRider:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 10),
                               std::make_pair(Traits::Speed, 5),
                               std::make_pair(Traits::HitPower, 4),
                               std::make_pair(Traits::HitCritical, 2),
                               std::make_pair(Traits::MagicResist, 3),
                               std::make_pair(Traits::CurseResist, 10),
                               std::make_pair(Traits::CurseOnDamage, 4),
                               std::make_pair(Traits::ShadowDamage, 2) }));
            }
            case set_type::TheHunters:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Accuracy, 8),
                               std::make_pair(Traits::Strength, 4),
                               std::make_pair(Traits::Speed, 4),
                               std::make_pair(Traits::MagicResist, 1),
                               std::make_pair(Traits::HitCritical, 1) }));
            }
            case set_type::TheSureShot:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Accuracy, 10),
                               std::make_pair(Traits::Strength, 6),
                               std::make_pair(Traits::Speed, 6),
                               std::make_pair(Traits::MagicResist, 2),
                               std::make_pair(Traits::HitCritical, 2) }));
            }
            case set_type::TheMarksmans:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Accuracy, 13),
                               std::make_pair(Traits::Strength, 10),
                               std::make_pair(Traits::Speed, 10),
                               std::make_pair(Traits::MagicResist, 3),
                               std::make_pair(Traits::HitCritical, 3) }));
            }
            case set_type::TheDeadeye:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Accuracy, 16),
                               std::make_pair(Traits::Strength, 13),
                               std::make_pair(Traits::Speed, 13),
                               std::make_pair(Traits::MagicResist, 4),
                               std::make_pair(Traits::HitCritical, 4) }));
            }
            case set_type::TheDuskRanger:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Accuracy, 12),
                               std::make_pair(Traits::Strength, 6),
                               std::make_pair(Traits::Speed, 6),
                               std::make_pair(Traits::MagicResist, 1),
                               std::make_pair(Traits::HitCritical, 5),
                               std::make_pair(Traits::CurseResist, 8),
                               std::make_pair(Traits::CurseOnDamage, 8),
                               std::make_pair(Traits::ShadowDamage, 3) }));
            }
            case set_type::TheVenomBow:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Accuracy, 12),
                               std::make_pair(Traits::Strength, 6),
                               std::make_pair(Traits::Speed, 6),
                               std::make_pair(Traits::MagicResist, 1),
                               std::make_pair(Traits::HitCritical, 3),
                               std::make_pair(Traits::PoisonOnAll, 8) }));
            }
            case set_type::TheCritterChannelers:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 2),
                               std::make_pair(Traits::Accuracy, 1),
                               std::make_pair(Traits::Speed, 1),
                               std::make_pair(Traits::Intelligence, 2),
                               std::make_pair(Traits::AnimalResist, 5),
                               std::make_pair(Traits::MagicResist, 1) }));
            }
            case set_type::TheMammalianHead:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 3),
                               std::make_pair(Traits::Accuracy, 2),
                               std::make_pair(Traits::Speed, 2),
                               std::make_pair(Traits::Intelligence, 3),
                               std::make_pair(Traits::AnimalResist, 6),
                               std::make_pair(Traits::MagicResist, 1) }));
            }
            case set_type::TheSavageTaskmasters:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 4),
                               std::make_pair(Traits::Accuracy, 3),
                               std::make_pair(Traits::Speed, 3),
                               std::make_pair(Traits::Intelligence, 4),
                               std::make_pair(Traits::AnimalResist, 7),
                               std::make_pair(Traits::MagicResist, 2) }));
            }
            case set_type::TheMonsterOverseers:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 5),
                               std::make_pair(Traits::Accuracy, 4),
                               std::make_pair(Traits::Speed, 4),
                               std::make_pair(Traits::Intelligence, 5),
                               std::make_pair(Traits::AnimalResist, 8),
                               std::make_pair(Traits::MagicResist, 3) }));
            }
            case set_type::TheBeastRulers:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 6),
                               std::make_pair(Traits::Accuracy, 5),
                               std::make_pair(Traits::Speed, 5),
                               std::make_pair(Traits::Intelligence, 6),
                               std::make_pair(Traits::AnimalResist, 9),
                               std::make_pair(Traits::MagicResist, 4) }));
            }
            case set_type::Not:
            case set_type::Count:
            default:
            {
                std::ostringstream ss;
                ss << "creature::EnchantmentFactory::NewFromSetType(set_type=" << E
                   << ") was unable to create with these arguments.";

                throw std::runtime_error(ss.str());
            }
        }
    }

    const EnchantmentPtr_t
        EnchantmentFactory::NewFromSetCompleteType(const item::set_type::Enum E) const
    {
        return Make(MakeFromSetCompleteType(E));
    }

    const Enchantment
        EnchantmentFactory::MakeFromSetCompleteType(const item::set_type::Enum E) const
    {
        using namespace item;

        switch (E)
        {
            case set_type::TheAssassins:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Luck, 20),
                               std::make_pair(Traits::Speed, 20),
                               std::make_pair(Traits::Backstab, 5),
                               std::make_pair(Traits::FindMagic, 10),
                               std::make_pair(Traits::FindCoinsAmount, 13) }));
            }
            case set_type::TheNeverwinter:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Luck, 33),
                               std::make_pair(Traits::Speed, 33),
                               std::make_pair(Traits::Backstab, 10),
                               std::make_pair(Traits::FindMagic, 13),
                               std::make_pair(Traits::FindCoinsAmount, 18) }));
            }
            case set_type::TheTickler:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Luck, 50),
                               std::make_pair(Traits::Speed, 50),
                               std::make_pair(Traits::Backstab, 15),
                               std::make_pair(Traits::FindMagic, 18),
                               std::make_pair(Traits::FindCoinsAmount, 24) }));
            }
            case set_type::TheMagus:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Intelligence, 20),
                               std::make_pair(Traits::MagicCast, 8),
                               std::make_pair(Traits::MagicEffect, 8),
                               std::make_pair(Traits::MagicResist, 8),
                               std::make_pair(Traits::ShadowDamage, 10),
                               std::make_pair(Traits::Mana, 18) }));
            }
            case set_type::TheNecromancers:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Intelligence, 33),
                               std::make_pair(Traits::MagicCast, 16),
                               std::make_pair(Traits::MagicEffect, 16),
                               std::make_pair(Traits::MagicResist, 16),
                               std::make_pair(Traits::ShadowDamage, 20),
                               std::make_pair(Traits::Mana, 24) }));
            }
            case set_type::TheWarlocks:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Intelligence, 50),
                               std::make_pair(Traits::MagicCast, 33),
                               std::make_pair(Traits::MagicEffect, 33),
                               std::make_pair(Traits::MagicResist, 33),
                               std::make_pair(Traits::ShadowDamage, 16),
                               std::make_pair(Traits::Mana, 16),
                               std::make_pair(Traits::CurseCast, 28),
                               std::make_pair(Traits::CurseEffect, 28) }));
            }
            case set_type::TheLichKings:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem
                        | EnchantmentType::CurseWithoutItem),
                    TraitSet({ std::make_pair(Traits::Intelligence, 50),
                               std::make_pair(Traits::Speed, 20),
                               std::make_pair(Traits::Mana, 33),
                               std::make_pair(Traits::MagicCast, 20),
                               std::make_pair(Traits::MagicEffect, 20),
                               std::make_pair(Traits::MagicResist, 20),
                               std::make_pair(Traits::FireDamage, 22),
                               std::make_pair(Traits::FireResist, 22),
                               std::make_pair(Traits::ShadowDamage, 22),
                               std::make_pair(Traits::ShadowDamage, 22),
                               std::make_pair(Traits::HonorResist, 22),
                               std::make_pair(Traits::CurseCast, 20),
                               std::make_pair(Traits::CurseEffect, 20),
                               std::make_pair(Traits::CurseResist, 20),
                               std::make_pair(Traits::FindMagic, 20),
                               std::make_pair(Traits::FindReligious, 10) }));
            }
            case set_type::TheSages:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, 20),
                               std::make_pair(Traits::MagicCast, 8),
                               std::make_pair(Traits::MagicEffect, 8),
                               std::make_pair(Traits::MagicResist, 8),
                               std::make_pair(Traits::BlessCast, 8),
                               std::make_pair(Traits::BlessEffect, 8),
                               std::make_pair(Traits::CurseResist, 8),
                               std::make_pair(Traits::Mana, 18),
                               std::make_pair(Traits::ShadowResist, 10) }));
            }
            case set_type::TheShamans:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, 33),
                               std::make_pair(Traits::MagicCast, 16),
                               std::make_pair(Traits::MagicEffect, 16),
                               std::make_pair(Traits::MagicResist, 16),
                               std::make_pair(Traits::BlessCast, 13),
                               std::make_pair(Traits::BlessEffect, 13),
                               std::make_pair(Traits::CurseResist, 16),
                               std::make_pair(Traits::Mana, 24),
                               std::make_pair(Traits::ShadowResist, 20) }));
            }
            case set_type::TheOracles:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, 50),
                               std::make_pair(Traits::MagicCast, 33),
                               std::make_pair(Traits::MagicEffect, 33),
                               std::make_pair(Traits::MagicResist, 33),
                               std::make_pair(Traits::BlessCast, 18),
                               std::make_pair(Traits::BlessEffect, 18),
                               std::make_pair(Traits::CurseResist, 33),
                               std::make_pair(Traits::Mana, 28),
                               std::make_pair(Traits::ShadowResist, 30) }));
            }
            case set_type::TheAngelic:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem
                        | EnchantmentType::BlessWithoutItem),
                    TraitSet({ std::make_pair(Traits::Charm, 50),
                               std::make_pair(Traits::Speed, 20),
                               std::make_pair(Traits::Mana, 33),
                               std::make_pair(Traits::MagicCast, 20),
                               std::make_pair(Traits::MagicEffect, 20),
                               std::make_pair(Traits::MagicResist, 20),
                               std::make_pair(Traits::BlessCast, 22),
                               std::make_pair(Traits::BlessEffect, 22),
                               std::make_pair(Traits::CurseResist, 22),
                               std::make_pair(Traits::AnimalResist, 12),
                               std::make_pair(Traits::FindReligious, 20),
                               std::make_pair(Traits::FindMagic, 20),
                               std::make_pair(Traits::FireResist, 20),
                               std::make_pair(Traits::FrostResist, 20),
                               std::make_pair(Traits::HonorResist, 20),
                               std::make_pair(Traits::ShadowResist, 20) }));
            }
            case set_type::TheBalladeers:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, 20),
                               std::make_pair(Traits::Intelligence, 20),
                               std::make_pair(Traits::MagicCast, 8),
                               std::make_pair(Traits::MagicEffect, 8),
                               std::make_pair(Traits::MagicResist, 8),
                               std::make_pair(Traits::Mana, 18) }));
            }
            case set_type::TheTroubadours:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, 33),
                               std::make_pair(Traits::Intelligence, 33),
                               std::make_pair(Traits::MagicCast, 16),
                               std::make_pair(Traits::MagicEffect, 16),
                               std::make_pair(Traits::MagicResist, 16),
                               std::make_pair(Traits::Mana, 24) }));
            }
            case set_type::TheMuses:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, 50),
                               std::make_pair(Traits::Intelligence, 50),
                               std::make_pair(Traits::MagicCast, 28),
                               std::make_pair(Traits::MagicEffect, 28),
                               std::make_pair(Traits::MagicResist, 28),
                               std::make_pair(Traits::Mana, 28) }));
            }
            case set_type::TheCavaliers:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 20),
                               std::make_pair(Traits::Speed, 13),
                               std::make_pair(Traits::MagicResist, 6),
                               std::make_pair(Traits::CurseResist, 10),
                               std::make_pair(Traits::HitCritical, 2),
                               std::make_pair(Traits::HitPower, 5) }));
            }
            case set_type::TheChampions:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 33),
                               std::make_pair(Traits::Speed, 18),
                               std::make_pair(Traits::MagicResist, 8),
                               std::make_pair(Traits::CurseResist, 20),
                               std::make_pair(Traits::HitCritical, 3),
                               std::make_pair(Traits::HitPower, 6) }));
            }
            case set_type::ThePaladins:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 50),
                               std::make_pair(Traits::Speed, 26),
                               std::make_pair(Traits::MagicResist, 10),
                               std::make_pair(Traits::CurseResist, 30),
                               std::make_pair(Traits::HitCritical, 4),
                               std::make_pair(Traits::HitPower, 7) }));
            }
            case set_type::TheBerserkers:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 70),
                               std::make_pair(Traits::Speed, 33),
                               std::make_pair(Traits::MagicResist, 12),
                               std::make_pair(Traits::CurseResist, 40),
                               std::make_pair(Traits::HitCritical, 5),
                               std::make_pair(Traits::HitPower, 8) }));
            }
            case set_type::TheRosewood:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 100),
                               std::make_pair(Traits::Speed, 50),
                               std::make_pair(Traits::MagicResist, 14),
                               std::make_pair(Traits::CurseResist, 50),
                               std::make_pair(Traits::HitCritical, 6),
                               std::make_pair(Traits::HitPower, 9) }));
            }
            case set_type::TheDragonslayers:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 133),
                               std::make_pair(Traits::Speed, 70),
                               std::make_pair(Traits::MagicResist, 16),
                               std::make_pair(Traits::CurseResist, 60),
                               std::make_pair(Traits::HitCritical, 7),
                               std::make_pair(Traits::HitPower, 10) }));
            }
            case set_type::TheEventideRider:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 133),
                               std::make_pair(Traits::Speed, 70),
                               std::make_pair(Traits::MagicResist, 16),
                               std::make_pair(Traits::CurseResist, 20),
                               std::make_pair(Traits::HitCritical, 7),
                               std::make_pair(Traits::HitPower, 10),
                               std::make_pair(Traits::CurseOnDamage, 26),
                               std::make_pair(Traits::ShadowDamage, 14) }));
            }
            case set_type::TheHunters:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Accuracy, 20),
                               std::make_pair(Traits::Strength, 15),
                               std::make_pair(Traits::Speed, 15),
                               std::make_pair(Traits::MagicResist, 2),
                               std::make_pair(Traits::HitCritical, 4),
                               std::make_pair(Traits::HitPower, 2) }));
            }
            case set_type::TheSureShot:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Accuracy, 33),
                               std::make_pair(Traits::Strength, 28),
                               std::make_pair(Traits::Speed, 28),
                               std::make_pair(Traits::MagicResist, 4),
                               std::make_pair(Traits::HitCritical, 6),
                               std::make_pair(Traits::HitPower, 4) }));
            }
            case set_type::TheMarksmans:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Accuracy, 50),
                               std::make_pair(Traits::Strength, 36),
                               std::make_pair(Traits::Speed, 36),
                               std::make_pair(Traits::MagicResist, 8),
                               std::make_pair(Traits::HitCritical, 8),
                               std::make_pair(Traits::HitPower, 6) }));
            }
            case set_type::TheDeadeye:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Accuracy, 70),
                               std::make_pair(Traits::Strength, 50),
                               std::make_pair(Traits::Speed, 50),
                               std::make_pair(Traits::MagicResist, 16),
                               std::make_pair(Traits::HitCritical, 13),
                               std::make_pair(Traits::HitPower, 8) }));
            }
            case set_type::TheDuskRanger:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Accuracy, 50),
                               std::make_pair(Traits::Strength, 50),
                               std::make_pair(Traits::Speed, 50),
                               std::make_pair(Traits::MagicResist, 8),
                               std::make_pair(Traits::HitCritical, 20),
                               std::make_pair(Traits::CurseResist, 50),
                               std::make_pair(Traits::CurseOnDamage, 33),
                               std::make_pair(Traits::ShadowDamage, 18) }));
            }
            case set_type::TheVenomBow:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Accuracy, 50),
                               std::make_pair(Traits::Strength, 50),
                               std::make_pair(Traits::Speed, 50),
                               std::make_pair(Traits::MagicResist, 8),
                               std::make_pair(Traits::HitCritical, 20),
                               std::make_pair(Traits::PoisonOnAll, 33) }));
            }
            case set_type::TheCritterChannelers:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 10),
                               std::make_pair(Traits::Accuracy, 8),
                               std::make_pair(Traits::Speed, 8),
                               std::make_pair(Traits::Intelligence, 8),
                               std::make_pair(Traits::AnimalResist, 8),
                               std::make_pair(Traits::MagicResist, 2) }));
            }
            case set_type::TheMammalianHead:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 15),
                               std::make_pair(Traits::Accuracy, 10),
                               std::make_pair(Traits::Speed, 10),
                               std::make_pair(Traits::Intelligence, 10),
                               std::make_pair(Traits::AnimalResist, 13),
                               std::make_pair(Traits::MagicResist, 2) }));
            }
            case set_type::TheSavageTaskmasters:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 20),
                               std::make_pair(Traits::Accuracy, 13),
                               std::make_pair(Traits::Speed, 13),
                               std::make_pair(Traits::Intelligence, 13),
                               std::make_pair(Traits::AnimalResist, 18),
                               std::make_pair(Traits::MagicResist, 4) }));
            }
            case set_type::TheMonsterOverseers:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 25),
                               std::make_pair(Traits::Accuracy, 18),
                               std::make_pair(Traits::Speed, 18),
                               std::make_pair(Traits::Intelligence, 18),
                               std::make_pair(Traits::AnimalResist, 24),
                               std::make_pair(Traits::MagicResist, 8) }));
            }
            case set_type::TheBeastRulers:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 30),
                               std::make_pair(Traits::Accuracy, 24),
                               std::make_pair(Traits::Speed, 24),
                               std::make_pair(Traits::Intelligence, 24),
                               std::make_pair(Traits::AnimalResist, 28),
                               std::make_pair(Traits::MagicResist, 16) }));
            }
            case set_type::Not:
            case set_type::Count:
            default:
            {
                std::ostringstream ss;
                ss << "creature::EnchantmentFactory::NewFromSetCompleteType(set_type=" << E
                   << ") was unable to create with these arguments.";

                throw std::runtime_error(ss.str());
            }
        }
    }

    const EnchantmentPtr_t EnchantmentFactory::NewFromElementType(
        const item::element_type::Enum E,
        const bool IS_WEAPON,
        const item::material::Enum MATERIAL_PRIMARY,
        const item::material::Enum MATERIAL_SECONDARY) const
    {
        return Make(MakeFromElementType(E, IS_WEAPON, MATERIAL_PRIMARY, MATERIAL_SECONDARY));
    }

    const Enchantment EnchantmentFactory::MakeFromElementType(
        const item::element_type::Enum E,
        const bool IS_WEAPON,
        const item::material::Enum MATERIAL_PRIMARY,
        const item::material::Enum MATERIAL_SECONDARY) const
    {
        using namespace item;

        if (E == element_type::None)
        {
            std::ostringstream ss;
            ss << "creature::EnchantmentFactory::NewFromElementType(element_type=" << E
               << ", is_weapon=" << std::boolalpha << IS_WEAPON << ", mat_pri="
               << ((MATERIAL_PRIMARY == material::Count) ? "Count"
                                                         : material::ToString(MATERIAL_PRIMARY))
               << ", mat_sec="
               << ((MATERIAL_SECONDARY == material::Count) ? "Count"
                                                           : material::ToString(MATERIAL_SECONDARY))
               << ") was given an element_type of NONE.";

            throw std::runtime_error(ss.str());
        }

        TraitSet traits;

        if ((MATERIAL_PRIMARY == material::Nothing) || (MATERIAL_PRIMARY == material::Count))
        {
            if (IS_WEAPON)
            {
                const auto DAMAGE { 50 };

                if (E & element_type::Fire)
                {
                    traits.Get(Traits::FireDamage).CurrentSet(DAMAGE);
                }

                if (E & element_type::Frost)
                {
                    traits.Get(Traits::FrostDamage).CurrentSet(DAMAGE);
                }

                if (E & element_type::Honor)
                {
                    traits.Get(Traits::HonorDamage).CurrentSet(DAMAGE);
                }

                if (E & element_type::Shadow)
                {
                    traits.Get(Traits::ShadowDamage).CurrentSet(DAMAGE);
                }
            }
            else
            {
                // if not IS_WEAPON then the item is assumed to be armor or wearable misc

                const auto RESISTANCE { 13 };

                if (E & element_type::Fire)
                {
                    traits.Get(Traits::FireResist).CurrentSet(RESISTANCE);
                }

                if (E & element_type::Frost)
                {
                    traits.Get(Traits::FrostResist).CurrentSet(RESISTANCE);
                }

                if (E & element_type::Honor)
                {
                    traits.Get(Traits::HonorResist).CurrentSet(RESISTANCE);
                }

                if (E & element_type::Shadow)
                {
                    traits.Get(Traits::ShadowResist).CurrentSet(RESISTANCE);
                }
            }
        }
        else
        {
            const auto MAT_BONUS { material::EnchantmentBonus(
                MATERIAL_PRIMARY, MATERIAL_SECONDARY) };

            if (IS_WEAPON)
            {
                const auto DAMAGE_BASE { 10 };
                const auto DAMAGE_MULT { 5 };
                const auto DAMAGE { DAMAGE_BASE + (MAT_BONUS * DAMAGE_MULT) };

                if (E & element_type::Fire)
                {
                    traits.Get(Traits::FireDamage).CurrentSet(DAMAGE);
                }

                if (E & element_type::Frost)
                {
                    traits.Get(Traits::FrostDamage).CurrentSet(DAMAGE);
                }

                if (E & element_type::Honor)
                {
                    traits.Get(Traits::HonorDamage).CurrentSet(DAMAGE);
                }

                if (E & element_type::Shadow)
                {
                    traits.Get(Traits::ShadowDamage).CurrentSet(DAMAGE);
                }
            }
            else
            {
                const auto RES_BASE { 4.0f };
                const auto RES_MULT { 1.0f / 1.5f };

                const auto RESISTANCE { static_cast<int>(
                    RES_BASE + (static_cast<float>(MAT_BONUS) * RES_MULT)) };

                if (E & element_type::Fire)
                {
                    traits.Get(Traits::FireResist).CurrentSet(RESISTANCE);
                }

                if (E & element_type::Frost)
                {
                    traits.Get(Traits::FrostResist).CurrentSet(RESISTANCE);
                }

                if (E & element_type::Honor)
                {
                    traits.Get(Traits::HonorResist).CurrentSet(RESISTANCE);
                }

                if (E & element_type::Shadow)
                {
                    traits.Get(Traits::ShadowResist).CurrentSet(RESISTANCE);
                }
            }
        }

        return Enchantment(EnchantmentType::WhenEquipped, traits);
    }

    const EnchantmentPtr_t EnchantmentFactory::NewFromNamedType(
        const item::named_type::Enum NAMED_ENUM,
        const item::material::Enum MATERIAL_PRIMARY,
        const item::material::Enum MATERIAL_SECONDARY,
        const bool IS_WEAPON,
        const bool IS_ARMOR) const
    {
        return Make(MakeFromNamedType(
            NAMED_ENUM, MATERIAL_PRIMARY, MATERIAL_SECONDARY, IS_WEAPON, IS_ARMOR));
    }

    const Enchantment EnchantmentFactory::MakeFromNamedType(
        const item::named_type::Enum NAMED_ENUM,
        const item::material::Enum MATERIAL_PRIMARY,
        const item::material::Enum MATERIAL_SECONDARY,
        const bool IS_WEAPON,
        const bool IS_ARMOR) const
    {
        using namespace item;

        switch (NAMED_ENUM)
        {
            case named_type::Wicked:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 13),
                               std::make_pair(Traits::Luck, -18),
                               std::make_pair(Traits::Charm, -13),
                               std::make_pair(Traits::Intelligence, -13),
                               std::make_pair(Traits::CurseOnDamage, 3),
                               std::make_pair(Traits::HitCritical, 2),
                               std::make_pair(Traits::CurseResist, -13),
                               std::make_pair(Traits::Encounter, 1),
                               std::make_pair(Traits::Surprise, 1) }));
            }
            case named_type::Fiendish:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 16),
                               std::make_pair(Traits::Luck, -20),
                               std::make_pair(Traits::Charm, -18),
                               std::make_pair(Traits::Intelligence, -18),
                               std::make_pair(Traits::CurseOnDamage, 6),
                               std::make_pair(Traits::HitCritical, 4),
                               std::make_pair(Traits::CurseResist, -18),
                               std::make_pair(Traits::Encounter, 2),
                               std::make_pair(Traits::Surprise, 2) }));
            }
            case named_type::Infernal:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 18),
                               std::make_pair(Traits::Luck, -22),
                               std::make_pair(Traits::Charm, -24),
                               std::make_pair(Traits::Intelligence, -24),
                               std::make_pair(Traits::CurseOnDamage, 8),
                               std::make_pair(Traits::HitCritical, 6),
                               std::make_pair(Traits::CurseResist, -26),
                               std::make_pair(Traits::Encounter, 3),
                               std::make_pair(Traits::Surprise, 3) }));
            }
            case named_type::Raging:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 26),
                               std::make_pair(Traits::Luck, -24),
                               std::make_pair(Traits::Charm, -28),
                               std::make_pair(Traits::Intelligence, -28),
                               std::make_pair(Traits::CurseOnDamage, 10),
                               std::make_pair(Traits::HitCritical, 8),
                               std::make_pair(Traits::CurseResist, -33),
                               std::make_pair(Traits::Encounter, 4),
                               std::make_pair(Traits::Surprise, 4) }));
            }
            case named_type::Diabolic:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 33),
                               std::make_pair(Traits::Luck, -26),
                               std::make_pair(Traits::Charm, -33),
                               std::make_pair(Traits::Intelligence, -33),
                               std::make_pair(Traits::CurseOnDamage, 13),
                               std::make_pair(Traits::HitCritical, 10),
                               std::make_pair(Traits::CurseResist, -100),
                               std::make_pair(Traits::Encounter, 5),
                               std::make_pair(Traits::Surprise, 5) }));
            }
            case named_type::Dancing:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 18),
                               std::make_pair(Traits::Speed, 18),
                               std::make_pair(Traits::CurseEffect, 50),
                               std::make_pair(Traits::HitCritical, 8),
                               std::make_pair(Traits::ArmorRating, 16),
                               std::make_pair(Traits::Accuracy, 16) }));
            }
            case named_type::Marauder:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 16),
                               std::make_pair(Traits::Accuracy, 13),
                               std::make_pair(Traits::HitCritical, 5) }));
            }
            case named_type::Honest:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Accuracy, 1),
                               std::make_pair(Traits::ArmorRating, 1),
                               std::make_pair(Traits::BlessEffect, 4),
                               std::make_pair(Traits::ShadowResist, 8),
                               std::make_pair(Traits::HonorResist, 8) }));
            }
            case named_type::Noble:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Accuracy, 2),
                               std::make_pair(Traits::ArmorRating, 2),
                               std::make_pair(Traits::BlessEffect, 6),
                               std::make_pair(Traits::Surprise, -2),
                               std::make_pair(Traits::ShadowResist, 13),
                               std::make_pair(Traits::HonorResist, 13) }));
            }
            case named_type::Daring:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Accuracy, 3),
                               std::make_pair(Traits::ArmorRating, 3),
                               std::make_pair(Traits::BlessEffect, 8),
                               std::make_pair(Traits::Surprise, -2),
                               std::make_pair(Traits::Charm, 2),
                               std::make_pair(Traits::Intelligence, 2),
                               std::make_pair(Traits::ShadowResist, 18),
                               std::make_pair(Traits::HonorResist, 18) }));
            }
            case named_type::Elite:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Accuracy, 4),
                               std::make_pair(Traits::ArmorRating, 4),
                               std::make_pair(Traits::BlessEffect, 13),
                               std::make_pair(Traits::Surprise, -2),
                               std::make_pair(Traits::Charm, 3),
                               std::make_pair(Traits::Intelligence, 3),
                               std::make_pair(Traits::CurseResist, 8),
                               std::make_pair(Traits::ShadowResist, 24),
                               std::make_pair(Traits::HonorResist, 24) }));
            }
            case named_type::Valiant:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Accuracy, 5),
                               std::make_pair(Traits::ArmorRating, 5),
                               std::make_pair(Traits::BlessEffect, 16),
                               std::make_pair(Traits::Surprise, -2),
                               std::make_pair(Traits::Charm, 4),
                               std::make_pair(Traits::Intelligence, 4),
                               std::make_pair(Traits::CurseResist, 10),
                               std::make_pair(Traits::ShadowResist, 28),
                               std::make_pair(Traits::HonorResist, 28) }));
            }
            case named_type::Heros:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Accuracy, 6),
                               std::make_pair(Traits::ArmorRating, 6),
                               std::make_pair(Traits::BlessEffect, 18),
                               std::make_pair(Traits::Surprise, -2),
                               std::make_pair(Traits::Charm, 5),
                               std::make_pair(Traits::Intelligence, 5),
                               std::make_pair(Traits::CurseResist, 13),
                               std::make_pair(Traits::MagicResist, 4),
                               std::make_pair(Traits::ShadowResist, 33),
                               std::make_pair(Traits::HonorResist, 33) }));
            }
            case named_type::Army:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 3),
                               std::make_pair(Traits::Speed, 3),
                               std::make_pair(Traits::ArmorRating, 3) }));
            }
            case named_type::Gladiator:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet(
                        { std::make_pair(Traits::Strength, 8), std::make_pair(Traits::Speed, 6) }));
            }
            case named_type::Soldiers:
            {
                if (IS_WEAPON)
                {
                    return Enchantment(
                        static_cast<EnchantmentType::Enum>(
                            EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                        TraitSet({ std::make_pair(Traits::Strength, 18),
                                   std::make_pair(Traits::Speed, 13),
                                   std::make_pair(Traits::Charm, 8),
                                   std::make_pair(Traits::HonorDamage, 22) }));
                }

                if (IS_ARMOR)
                {
                    return Enchantment(
                        static_cast<EnchantmentType::Enum>(
                            EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                        TraitSet({ std::make_pair(Traits::ArmorRating, 4) }));
                }

                std::ostringstream ss;
                ss << "creature::EnchantmentFactory::NewFromNamedType(named_type=Soldiers, "
                      "is_weapon=false, is_armor=false, mat_pri = "
                   << material::ToString(MATERIAL_PRIMARY)
                   << ", mat_sec=" << material::ToString(MATERIAL_SECONDARY)
                   << ") failed because for that named_type must be weapon or armor.";

                throw std::runtime_error(ss.str());
            }
            case named_type::Wardens:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 5),
                               std::make_pair(Traits::ArmorRating, 5),
                               std::make_pair(Traits::Surprise, -3) }));
            }
            case named_type::Princes:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 8),
                               std::make_pair(Traits::ArmorRating, 8) }));
            }
            case named_type::Ranger:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 6),
                               std::make_pair(Traits::Accuracy, 8),
                               std::make_pair(Traits::ArmorRating, 4) }));
            }
            case named_type::Samurai:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 10),
                               std::make_pair(Traits::Accuracy, 10),
                               std::make_pair(Traits::HitCritical, 2) }));
            }
            case named_type::Thors:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 12),
                               std::make_pair(Traits::ArmorRating, 12),
                               std::make_pair(Traits::MagicResist, 4),
                               std::make_pair(Traits::HitPower, 2) }));
            }
            case named_type::Nile:
            {
                if (IS_WEAPON)
                {
                    return Enchantment(
                        static_cast<EnchantmentType::Enum>(
                            EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                        TraitSet({ std::make_pair(Traits::Strength, 12),
                                   std::make_pair(Traits::Accuracy, 12),
                                   std::make_pair(Traits::ArmorRating, 8),
                                   std::make_pair(Traits::CurseResist, 16),
                                   std::make_pair(Traits::HitCritical, 1),
                                   std::make_pair(Traits::PoisonOnMelee, 20) }));
                }

                if (IS_ARMOR)
                {
                    return Enchantment(
                        static_cast<EnchantmentType::Enum>(
                            EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                        TraitSet({ std::make_pair(Traits::Strength, 12),
                                   std::make_pair(Traits::Accuracy, 12),
                                   std::make_pair(Traits::ArmorRating, 8),
                                   std::make_pair(Traits::CurseResist, 16),
                                   std::make_pair(Traits::HitCritical, 1) }));
                }

                std::ostringstream ss;
                ss << "creature::EnchantmentFactory::NewFromNamedType(named_type=Nile, "
                      "is_weapon=false, is_armor=false, mat_pri = "
                   << material::ToString(MATERIAL_PRIMARY)
                   << ", mat_sec=" << material::ToString(MATERIAL_SECONDARY)
                   << ") failed because for that named_type must be weapon or armor.";

                throw std::runtime_error(ss.str());
            }
            case named_type::Searing:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::FireDamage, 16) }));
            }
            case named_type::Burning:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::FireDamage, 33) }));
            }
            case named_type::Fiery:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::FireDamage, 66) }));
            }
            case named_type::Charred:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::FireResist, 8) }));
            }
            case named_type::Icy:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::FrostDamage, 16) }));
            }
            case named_type::Winter:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::FrostDamage, 33) }));
            }
            case named_type::Frigid:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::FrostDamage, 66) }));
            }
            case named_type::Chill:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::FrostResist, 8) }));
            }
            case named_type::Frozen:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::FrostResist, 13) }));
            }
            case named_type::Arctic:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::FrostResist, 18) }));
            }
            case named_type::Proud:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::HonorDamage, 16) }));
            }
            case named_type::Glory:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::HonorDamage, 33) }));
            }
            case named_type::Pure:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::HonorDamage, 66) }));
            }
            case named_type::Light:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::HonorResist, 8) }));
            }
            case named_type::Dawn:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::HonorResist, 13) }));
            }
            case named_type::Sun:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::HonorResist, 18) }));
            }
            case named_type::Gloom:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::ShadowDamage, 16) }));
            }
            case named_type::Twilight:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::ShadowDamage, 33) }));
            }
            case named_type::Dusk:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::ShadowDamage, 66) }));
            }
            case named_type::Sorrow:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::ShadowResist, 20) }));
            }
            case named_type::Woe:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::ShadowResist, 20) }));
            }
            case named_type::Misery:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::ShadowResist, 30) }));
            }
            case named_type::Moon:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, -18),
                               std::make_pair(Traits::ArmorRating, 18),
                               std::make_pair(Traits::ShadowResist, 33),
                               std::make_pair(Traits::CurseResist, 33) }));
            }
            case named_type::Dark:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::ShadowDamage, 66),
                               std::make_pair(Traits::CurseOnDamage, 8),
                               std::make_pair(Traits::HitCritical, 4),
                               std::make_pair(Traits::Strength, 18) }));
            }
            case named_type::Betrayer:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::ShadowDamage, 80),
                               std::make_pair(Traits::CurseOnDamage, 18),
                               std::make_pair(Traits::HitCritical, 8),
                               std::make_pair(Traits::Strength, 33),
                               std::make_pair(Traits::Luck, -33),
                               std::make_pair(Traits::CurseResist, 33) }));
            }
            case named_type::Imposters:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Luck, 10),
                               std::make_pair(Traits::Backstab, 3),
                               std::make_pair(Traits::FindCoinsAmount, 5),
                               std::make_pair(Traits::FindMagic, 5),
                               std::make_pair(Traits::Surprise, -5) }));
            }
            case named_type::Pickpocket:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Luck, 13),
                               std::make_pair(Traits::Backstab, 4),
                               std::make_pair(Traits::FindCoinsAmount, 8),
                               std::make_pair(Traits::FindMagic, 6),
                               std::make_pair(Traits::Surprise, -6) }));
            }
            case named_type::Burglar:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Luck, 18),
                               std::make_pair(Traits::Backstab, 5),
                               std::make_pair(Traits::FindCoinsAmount, 10),
                               std::make_pair(Traits::FindMagic, 8),
                               std::make_pair(Traits::Surprise, -8) }));
            }
            case named_type::Mountebank:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Luck, 21),
                               std::make_pair(Traits::Backstab, 6),
                               std::make_pair(Traits::FindCoinsAmount, 8),
                               std::make_pair(Traits::FindMagic, 4),
                               std::make_pair(Traits::Charm, 18) }));
            }
            case named_type::Charlatans:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Luck, 13),
                               std::make_pair(Traits::Backstab, 3),
                               std::make_pair(Traits::FindCoinsAmount, 6),
                               std::make_pair(Traits::FindMagic, 3),
                               std::make_pair(Traits::Charm, 24) }));
            }
            case named_type::Focus:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Accuracy, 18),
                               std::make_pair(Traits::Strength, 13),
                               std::make_pair(Traits::DamageBonusProj, 16) }));
            }
            case named_type::Robbers:
            {
                const auto MAT_BONUS { material::EnchantmentBonus(
                    MATERIAL_PRIMARY, MATERIAL_SECONDARY) };

                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Luck, (MAT_BONUS / 2)) }));
            }
            case named_type::Thugs:
            {
                const auto MAT_BONUS { material::EnchantmentBonus(
                    MATERIAL_PRIMARY, MATERIAL_SECONDARY) };

                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Luck, 10 + (MAT_BONUS / 2)) }));
            }
            case named_type::Knaves:
            {
                const auto MAT_BONUS { material::EnchantmentBonus(
                    MATERIAL_PRIMARY, MATERIAL_SECONDARY) };

                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Luck, 20 + (MAT_BONUS / 2)) }));
            }
            case named_type::Muggers:
            {
                const auto MAT_BONUS { material::EnchantmentBonus(
                    MATERIAL_PRIMARY, MATERIAL_SECONDARY) };

                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Backstab, (MAT_BONUS / 2)) }));
            }
            case named_type::Thief:
            {
                const auto MATERIAL_BASED_BONUS {
                    5 + (material::EnchantmentBonus(MATERIAL_PRIMARY, MATERIAL_SECONDARY) / 2)
                };

                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Luck, MATERIAL_BASED_BONUS),
                               std::make_pair(Traits::Backstab, MATERIAL_BASED_BONUS) }));
            }
            case named_type::Pirate:
            {
                const auto MAT_BONUS { material::EnchantmentBonus(
                    MATERIAL_PRIMARY, MATERIAL_SECONDARY) };

                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Backstab, 8 + (MAT_BONUS / 2)),
                               std::make_pair(Traits::FindCoinsAmount, 8 + (MAT_BONUS / 2)),
                               std::make_pair(Traits::Luck, (MAT_BONUS / 2)),
                               std::make_pair(Traits::FindMagic, (MAT_BONUS / 2)) }));
            }
            case named_type::Not:
            case named_type::Count:
            default:
            {
                std::ostringstream ss;
                ss << "creature::EnchantmentFactory::NewFromNamedType(named_type=" << NAMED_ENUM
                   << ", is_weapon=" << std::boolalpha << IS_WEAPON << ", is_armor=" << IS_ARMOR
                   << ", mat_pri=" << material::ToString(MATERIAL_PRIMARY)
                   << ", mat_sec=" << material::ToString(MATERIAL_SECONDARY)
                   << ") failed to find a matching named_type.";

                throw std::runtime_error(ss.str());
            }
        }
    }

} // namespace creature
} // namespace heroespath
