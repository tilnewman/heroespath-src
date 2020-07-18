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
#include "misc/enum-util.hpp"
#include "misc/log-macros.hpp"

#include <numeric>
#include <sstream>
#include <stdexcept>

namespace heroespath
{
namespace creature
{

    const EnchantmentPVec_t EnchantmentFactory::MakeAndStore(const item::Item & ITEM)
    {
        if (ITEM.SetType() < item::Set::Count)
        {
            // if part of a set, then only the set enchantments will apply
            return { NewFromSetType(ITEM.SetType()) };
        }
        else
        {
            EnchantmentPVec_t enchantmentsPVec;

            if (ITEM.ElementType() != item::Element::None)
            {
                enchantmentsPVec.emplace_back(NewFromElementType(
                    ITEM.ElementType(),
                    ITEM.IsWeapon(),
                    ITEM.MaterialPrimary(),
                    ITEM.MaterialSecondary()));
            }

            if (ITEM.NamedType() < item::Named::Count)
            {
                enchantmentsPVec.emplace_back(NewFromNamedType(
                    ITEM.NamedType(),
                    ITEM.MaterialPrimary(),
                    ITEM.MaterialSecondary(),
                    ITEM.IsWeapon(),
                    ITEM.IsArmor()));
            }

            if (ITEM.MiscType() < item::Misc::Count)
            {
                for (const auto & ENCHANTMENT_PTR : NewFromMiscType(
                         ITEM.MiscType(), ITEM.MaterialPrimary(), ITEM.MaterialSecondary()))
                {
                    enchantmentsPVec.emplace_back(ENCHANTMENT_PTR);
                }
            }

            return enchantmentsPVec;
        }
    }

    // TODO this one appears to be broken and not doing anything
    // Score_t EnchantmentFactory::Score(
    //    const item::Misc::Enum MISC_TYPE,
    //    const item::Material::Enum MAT_PRI,
    //    const item::Material::Enum MAT_SEC)
    //{
    //    Score_t score { 0_score };
    //
    //    if (MISC_TYPE < item::Misc::Count)
    //    {
    //        return score;
    //    }
    //
    //    // below are score bonuses needed because enchantments don't provide enough
    //
    //    if (item::Misc::AllowsCasting(MISC_TYPE))
    //    {
    //        score += 150_score;
    //    }
    //
    //    if ((item::Misc::IsBlessed(MISC_TYPE)) || (item::Misc::IsCursed(MISC_TYPE)))
    //    {
    //        score += 150_score;
    //    }
    //
    //    if (item::Misc::IsQuest(MISC_TYPE))
    //    {
    //        score += 250_score;
    //    }
    //
    //    if (item::Misc::IsSummoning(MISC_TYPE))
    //    {
    //        score += 100_score;
    //    }
    //
    //    if (item::Misc::IsUseable(MISC_TYPE))
    //    {
    //        score += 150_score;
    //    }
    //
    //    if (MISC_TYPE == item::Misc::SpiderEggs)
    //    {
    //        score += 500_score;
    //    }
    //    else if (MISC_TYPE == item::Misc::Ring)
    //    {
    //        score += 20_score;
    //    }
    //    else if (MISC_TYPE == item::Misc::DrumLute)
    //    {
    //        score += 100_score;
    //    }
    //    else if (MISC_TYPE == item::Misc::LockPicks)
    //    {
    //        score += 200_score;
    //    }
    //
    //    for (const auto & ENCHANTMENT :
    //         MakeFromMiscType(MISC_TYPE, MAT_PRI, MAT_SEC))
    //    {
    //        score += ENCHANTMENT.Score();
    //    }
    //
    //    return score;
    //}

    Score_t EnchantmentFactory::Score(const item::Set::Enum ENUM)
    {
        Score_t result = 0_score;

        if (ENUM < item::Set::Count)
        {
            result += MakeFromSetType(ENUM).Score();
            result += MakeFromSetCompleteType(ENUM).Score();

            // this is to raise the score of all set items to help balance the game
            result += 100_score;
        }

        return result;
    }

    Score_t EnchantmentFactory::Score(
        const item::Named::Enum NAMED_ENUM,
        const item::Material::Enum MAT_PRI,
        const item::Material::Enum MAT_SEC,
        const bool IS_WEAPON,
        const bool IS_ARMOR)
    {
        Score_t result = 0_score;

        if (NAMED_ENUM < item::Named::Count)
        {
            result += MakeFromNamedType(NAMED_ENUM, MAT_PRI, MAT_SEC, IS_WEAPON, IS_ARMOR).Score();

            // this is to raise the score of all named items to help balance the game
            result += 450_score;
        }

        return result;
    }

    Score_t EnchantmentFactory::Score(
        const item::Element::Enum ELEMENT_TYPE,
        const bool IS_WEAPON,
        const item::Material::Enum MAT_PRI,
        const item::Material::Enum MAT_SEC)
    {
        Score_t result = 0_score;

        if (ELEMENT_TYPE < item::Element::None)
        {
            result += MakeFromElementType(ELEMENT_TYPE, IS_WEAPON, MAT_PRI, MAT_SEC).Score();

            // this 750* is to raise the score of all items with elemental enchantments
            result += (750_score * Score_t::Make(misc::CountBits(ELEMENT_TYPE)));
        }

        return result;
    }

    const EnchantmentPtr_t EnchantmentFactory::Make(
        const EnchantmentType::Enum TYPE, const TraitSet & TRAIT_SET, const UseInfo & USE_INFO)
    {
        return Make(Enchantment(TYPE, TRAIT_SET, USE_INFO));
    }

    const EnchantmentPtr_t EnchantmentFactory::Make(const Enchantment & ENCHANTMENT)
    {
        return EnchantmentWarehouse::Access().Store(std::make_unique<Enchantment>(ENCHANTMENT));
    }

    const EnchantmentPVec_t EnchantmentFactory::NewFromMiscType(
        const item::Misc::Enum MISC_TYPE,
        const item::Material::Enum MAT_PRI,
        const item::Material::Enum MAT_SEC)
    {
        EnchantmentPVec_t enchantmentPtrs;

        for (const auto & ENCHANTMENT : MakeFromMiscType(MISC_TYPE, MAT_PRI, MAT_SEC))
        {
            enchantmentPtrs.emplace_back(Make(ENCHANTMENT));
        }

        return enchantmentPtrs;
    }

    const EnchantmentVec_t EnchantmentFactory::MakeFromMiscType(
        const item::Misc::Enum MISC_TYPE,
        const item::Material::Enum MAT_PRI,
        const item::Material::Enum MAT_SEC)
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (MAT_PRI < item::Material::Count),
            "creature::EnchantmentFactory::MakeFromMiscType(Misc="
                << NAMEOF_ENUM(MISC_TYPE)
                << ") given an invalid primary material=" << NAMEOF_ENUM(MAT_PRI));

        if (MAT_SEC < item::Material::Count)
        {
            // intentionally use only the secondary material to get the USE_COUNT
            const auto USE_COUNT { item::Material::EnchantmentBonus(MAT_SEC, MAT_SEC) };

            if (item::Misc::IsBlessed(MISC_TYPE))
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
            else if (item::Misc::IsCursed(MISC_TYPE))
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

        const auto MAT_BONUS { item::Material::EnchantmentBonus(MAT_PRI, MAT_SEC) };

        switch (MISC_TYPE)
        {
                // quest items (see below for AngelBraid and LichHand)
            case item::Misc::Crumhorn:
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
            case item::Misc::DevilHorn:
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
            case item::Misc::GolemFinger:
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
            case item::Misc::HurdyGurdy:
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
            case item::Misc::Icicle:
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
            case item::Misc::Lyre:
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
            case item::Misc::MummyHand:
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
            case item::Misc::PetrifiedSnake:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Mana, 10),
                               std::make_pair(Traits::MagicEffect, 8),
                               std::make_pair(Traits::MagicResist, 16),
                               std::make_pair(Traits::PoisonOnAll, 33) })) };
            }
            case item::Misc::PipeAndTabor:
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
            case item::Misc::Recorder:
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
            case item::Misc::UnicornHorn:
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
            case item::Misc::Viol:
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
            case item::Misc::BasiliskTongue:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::MagicCast, 8),
                               std::make_pair(Traits::MagicEffect, 13),
                               std::make_pair(Traits::MagicResist, 13) })) };
            }
            case item::Misc::BerserkersBeard:
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
            case item::Misc::BishopsHanky:
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
            case item::Misc::BleedingTrophy:
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
            case item::Misc::BloodyDragonScale:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::AnimalResist, 50),
                               std::make_pair(Traits::FindCoinsAmount, 10),
                               std::make_pair(Traits::FindMagic, 10),
                               std::make_pair(Traits::Charm, -26) })) };
            }
            case item::Misc::BottleOfBansheeScreams:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, -20),
                               std::make_pair(Traits::Intelligence, -20),
                               std::make_pair(Traits::Encounter, -6) })) };
            }
            case item::Misc::BronzeTroll:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 24),
                               std::make_pair(Traits::Charm, -24),
                               std::make_pair(Traits::Encounter, -6) })) };
            }
            case item::Misc::BroochCrown:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Intelligence, (MAT_BONUS * 3)) })) };
            }
            case item::Misc::BroochFeather:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Accuracy, (MAT_BONUS * 3)) })) };
            }
            case item::Misc::BroochFist:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, (MAT_BONUS * 3)) })) };
            }
            case item::Misc::BroochHourglass:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Speed, (MAT_BONUS * 3)) })) };
            }
            case item::Misc::BroochKey:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Luck, (MAT_BONUS * 3)) })) };
            }
            case item::Misc::BroochMask:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, (MAT_BONUS * 3)) })) };
            }
            case item::Misc::BurialShroud:
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
            case item::Misc::CapeCommanders:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 18),
                               std::make_pair(Traits::ArmorRating, 18),
                               std::make_pair(Traits::CurseResist, 24) })) };
            }

            case item::Misc::CapeGenerals:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 24),
                               std::make_pair(Traits::ArmorRating, 24),
                               std::make_pair(Traits::CurseResist, 28) })) };
            }
            case item::Misc::CapeKings:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 30),
                               std::make_pair(Traits::ArmorRating, 30),
                               std::make_pair(Traits::CurseResist, 33) })) };
            }
            case item::Misc::CharmCrown:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Intelligence, (MAT_BONUS * 5)) })) };
            }
            case item::Misc::CharmFeather:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Accuracy, (MAT_BONUS * 5)) })) };
            }
            case item::Misc::CharmFist:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, (MAT_BONUS * 5)) })) };
            }
            case item::Misc::CharmHourglass:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Speed, (MAT_BONUS * 5)) })) };
            }
            case item::Misc::CharmKey:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Luck, (MAT_BONUS * 5)) })) };
            }
            case item::Misc::CharmMask:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, (MAT_BONUS * 5)) })) };
            }
            case item::Misc::ChimeraBone:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::MagicResist, 18),
                               std::make_pair(Traits::Charm, -50),
                               std::make_pair(Traits::Intelligence, 13),
                               std::make_pair(Traits::FindMagic, 8) })) };
            }
            case item::Misc::CobraTooth:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::DamageBonusMelee, 16),
                               std::make_pair(Traits::PoisonOnAll, 36) })) };
            }
            case item::Misc::CrystalChimes:
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
            case item::Misc::DemonDiary:
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
            case item::Misc::DoveBloodVial:
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
            case item::Misc::DragonToothWhistle:
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
            case item::Misc::DriedFrog:
            case item::Misc::DriedGecko:
            case item::Misc::DriedIguana:
            case item::Misc::DriedLizard:
            case item::Misc::DriedSalamander:
            case item::Misc::DriedSkink:
            case item::Misc::DriedToad:
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
            case item::Misc::DruidLeaf:
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
            case item::Misc::EvilRabbitsFoot:
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
            case item::Misc::ExoticGoldenGong:
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
            case item::Misc::EyeCyclops:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::HitCritical, 1 + (MAT_BONUS / 2)),
                               std::make_pair(Traits::Surprise, (MAT_BONUS * -1)) })) };
            }
            case item::Misc::EyeGiantOwl:
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
            case item::Misc::EyeHawk:
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
            case item::Misc::FlagFanatics:
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
            case item::Misc::FlagRegalCaptains:
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
            case item::Misc::FlagTribal:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, -16),
                               std::make_pair(Traits::Encounter, 6),
                               std::make_pair(Traits::Surprise, 8) })) };
            }
            case item::Misc::FriarsChronicle:
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
            case item::Misc::FuneralRecord:
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
            case item::Misc::GhostSheet:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::ArmorRating, 13),
                               std::make_pair(Traits::MagicResist, 13),
                               std::make_pair(Traits::Speed, -13),
                               std::make_pair(Traits::Encounter, -6) })) };
            }
            case item::Misc::GlassCat:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Speed, 24),
                               std::make_pair(Traits::Luck, 18),
                               std::make_pair(Traits::Encounter, -3) })) };
            }
            case item::Misc::GriffinFeather:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem
                        | EnchantmentType::AllowsFlight),
                    TraitSet({ std::make_pair(Traits::Speed, 18),
                               std::make_pair(Traits::Strength, 13),
                               std::make_pair(Traits::DamageBonusProj, 13) })) };
            }
            case item::Misc::HangmansNoose:
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

            case item::Misc::HobgoblinNose:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, -26),
                               std::make_pair(Traits::Luck, 13),
                               std::make_pair(Traits::Surprise, -10),
                               std::make_pair(Traits::MagicEffect, 13) })) };
            }
            case item::Misc::HolyEpic:
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
            case item::Misc::HornOfTheHorde:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 13),
                               std::make_pair(Traits::Encounter, -8),
                               std::make_pair(Traits::Surprise, -8) })) };
            }
            case item::Misc::ImpTail:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::DamageBonusMelee, 20),
                               std::make_pair(Traits::Speed, 13) })) };
            }
            case item::Misc::IslanderHeaddress:
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
            case item::Misc::JeweledArmband:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::ArmorRating, (MAT_BONUS * 2)) })) };
            }
            case item::Misc::JeweledHandbag:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::FindMagic, 1 + (MAT_BONUS / 2)),
                               std::make_pair(Traits::FindCoinsAmount, 1 + (MAT_BONUS / 2)),
                               std::make_pair(Traits::Luck, (MAT_BONUS * 2)) })) };
            }
            case item::Misc::JeweledPrincessVeil:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, 4 + MAT_BONUS),
                               std::make_pair(Traits::FindMagic, 1 + (MAT_BONUS / 2)) })) };
            }

            case item::Misc::LastRitesScroll:
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
            case item::Misc::MacabreManuscript:
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
            case item::Misc::MadRatJuju:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, 16),
                               std::make_pair(Traits::Intelligence, 33),
                               std::make_pair(Traits::Speed, 33),
                               std::make_pair(Traits::AnimalResist, -18) })) };
            }
            case item::Misc::MagicHorseshoe:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Luck, 18),
                               std::make_pair(Traits::FindReligious, 4),
                               std::make_pair(Traits::FindMagic, 8),
                               std::make_pair(Traits::FindCoinsAmount, 16) })) };
            }
            case item::Misc::MagnifyingGlass:
            {
                // TODO
                return {};
            }
            case item::Misc::ManaAmulet:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Mana, 18 + MAT_BONUS),
                               std::make_pair(Traits::MagicEffect, 1 + (MAT_BONUS / 2)) })) };
            }
            case item::Misc::MaskMourners:
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
            case item::Misc::MaskRascal:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Luck, 22),
                               std::make_pair(Traits::Charm, 18),
                               std::make_pair(Traits::FindMagic, 8),
                               std::make_pair(Traits::FindCoinsAmount, 16) })) };
            }
            case item::Misc::MinotaurHide:
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
            case item::Misc::MortuaryOrnament:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::ShadowResist, 66),
                               std::make_pair(Traits::FindMagic, 13),
                               std::make_pair(Traits::FindReligious, 26) })) };
            }
            case item::Misc::NecklaceJeweledAnkh:
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
            case item::Misc::NecklaceSharkTooth:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 13),
                               std::make_pair(Traits::DamageBonusMelee, 18) })) };
            }
            case item::Misc::NecklaceVampiresTooth:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, 13),
                               std::make_pair(Traits::HealthGainAll, 25),
                               std::make_pair(Traits::CurseResist, 28) })) };
            }
            case item::Misc::PantherPaw:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Speed, 18),
                               std::make_pair(Traits::Strength, 13),
                               std::make_pair(Traits::AnimalResist, 13) })) };
            }
            case item::Misc::PinCrown:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Intelligence, MAT_BONUS) })) };
            }
            case item::Misc::PinFeather:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Accuracy, MAT_BONUS) })) };
            }
            case item::Misc::PinFist:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, MAT_BONUS) })) };
            }
            case item::Misc::PinHourglass:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Speed, MAT_BONUS) })) };
            }
            case item::Misc::PinKey:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Luck, MAT_BONUS) })) };
            }
            case item::Misc::PinMask:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, MAT_BONUS) })) };
            }
            case item::Misc::PixieBell:
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
            case item::Misc::RattlesnakeTail:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Speed, 16),
                               std::make_pair(Traits::Encounter, 8) })) };
            }
            case item::Misc::RavenClaw:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 13),
                               std::make_pair(Traits::Accuracy, 13),
                               std::make_pair(Traits::DamageBonusProj, 18) })) };
            }
            case item::Misc::ReaperScythe:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::HealthGainMelee, 25) })) };
            }

            case item::Misc::RequiemRegister:
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
            case item::Misc::RingHobo:
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
            case item::Misc::RingMendicant:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, -18),
                               std::make_pair(Traits::FindCoinsAmount, 16),
                               std::make_pair(Traits::FindMagic, 8),
                               std::make_pair(Traits::FindReligious, 18) })) };
            }
            case item::Misc::RingMonk:
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
            case item::Misc::RingPriest:
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
            case item::Misc::RoyalScoutSpyglass:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Accuracy, 16),
                               std::make_pair(Traits::Surprise, -12) })) };
            }
            case item::Misc::SaintCameoPin:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Accuracy, 16),
                               std::make_pair(Traits::Surprise, -12) })) };
            }
            case item::Misc::SaintsJournal:
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
            case item::Misc::SanguineRelic:
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
            case item::Misc::ScoundrelSack:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::FindCoinsAmount, 18),
                               std::make_pair(Traits::FindMagic, 8),
                               std::make_pair(Traits::Luck, 18),
                               std::make_pair(Traits::Backstab, 6) })) };
            }
            case item::Misc::SepultureDecoration:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::ShadowResist, 33),
                               std::make_pair(Traits::FindMagic, 8),
                               std::make_pair(Traits::FindReligious, 13) })) };
            }
            case item::Misc::ShadeCloak:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::ArmorRating, 33),
                               std::make_pair(Traits::MagicResist, 26),
                               std::make_pair(Traits::Speed, -20),
                               std::make_pair(Traits::Encounter, -6) })) };
            }
            case item::Misc::ShamanRainmaker:
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
            case item::Misc::SirenConch:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::MagicCast, 8),
                               std::make_pair(Traits::MagicEffect, 13),
                               std::make_pair(Traits::Charm, 13) })) };
            }
            case item::Misc::SpecterChains:
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
            case item::Misc::SpecterRobe:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::ArmorRating, 20),
                               std::make_pair(Traits::MagicResist, 18),
                               std::make_pair(Traits::Speed, -13),
                               std::make_pair(Traits::Encounter, -8) })) };
            }
            case item::Misc::SpiderEggs:
            {
                return { Enchantment(
                    EnchantmentType::WhenUsed, TraitSet(), UseInfo(10, game::Phase::Combat)) };
            }
            case item::Misc::SprintersLegtie:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Speed, 22),
                               std::make_pair(Traits::Strength, -8) })) };
            }
            case item::Misc::SwindlersBag:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::FindCoinsAmount, 22),
                               std::make_pair(Traits::FindMagic, 8),
                               std::make_pair(Traits::Luck, 22),
                               std::make_pair(Traits::HitCritical, 5) })) };
            }
            case item::Misc::TricksterPouch:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::FindCoinsAmount, 13),
                               std::make_pair(Traits::FindMagic, 8),
                               std::make_pair(Traits::Luck, 16),
                               std::make_pair(Traits::Charm, 18) })) };
            }
            case item::Misc::TuningFork:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, MAT_BONUS),
                               std::make_pair(Traits::MagicCast, MAT_BONUS) })) };
            }
            case item::Misc::TurtleShell:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::ArmorRating, (MAT_BONUS * 3)) })) };
            }
            case item::Misc::VultureGizzard:
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
            case item::Misc::WarhorseMarionette:
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
            case item::Misc::WarTrumpet:
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
            case item::Misc::WeaselTotem:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Speed, 28),
                               std::make_pair(Traits::Luck, 18),
                               std::make_pair(Traits::Encounter, -6) })) };
            }
            case item::Misc::WolfenFur:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::AnimalResist, 28),
                               std::make_pair(Traits::Speed, 16),
                               std::make_pair(Traits::Charm, -16),
                               std::make_pair(Traits::Intelligence, -13) })) };
            }
            case item::Misc::WraithTalisman:
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
            case item::Misc::Scepter:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Mana, 14 + (MAT_BONUS * 2)),
                               std::make_pair(Traits::MagicEffect, (MAT_BONUS * 2)),
                               std::make_pair(Traits::MagicCast, MAT_BONUS) })) };
            }

            // standalone items
            case item::Misc::Egg:
            case item::Misc::Embryo:
            {
                return { Enchantment(
                    EnchantmentType::WhenUsed, TraitSet(), UseInfo(1, game::Phase::Combat)) };
            }
            case item::Misc::LockPicks:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Luck, 12 + MAT_BONUS),
                               std::make_pair(Traits::Speed, 11 + (MAT_BONUS / 2)),
                               std::make_pair(Traits::Backstab, 1 + (MAT_BONUS / 2)) })) };
            }
            case item::Misc::Orb:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Mana, 12 + MAT_BONUS),
                               std::make_pair(Traits::MagicEffect, MAT_BONUS),
                               std::make_pair(Traits::MagicCast, 1 + (MAT_BONUS / 2)) })) };
            }
            case item::Misc::Seeds:
            {
                return { Enchantment(
                    EnchantmentType::WhenUsed, TraitSet(), UseInfo(10, game::Phase::Combat)) };
            }
            case item::Misc::Shard:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Mana, 11 + (MAT_BONUS / 2)),
                               std::make_pair(Traits::MagicEffect, (MAT_BONUS / 2)) })) };
            }
            case item::Misc::SummoningStatue:
            {
                return { Enchantment(
                    EnchantmentType::WhenUsed,
                    TraitSet(),
                    UseInfo(MAT_BONUS, game::Phase::Combat)) };
            }

            // see blessed and cursed above
            case item::Misc::Bust:
            case item::Misc::DollBlessed:
            case item::Misc::DollCursed:
            case item::Misc::DriedHead:
            case item::Misc::FigurineBlessed:
            case item::Misc::FigurineCursed:
            case item::Misc::PuppetBlessed:
            case item::Misc::PuppetCursed:

            // no enchantments for these plain items
            case item::Misc::Doll:
            case item::Misc::DrumLute:
            case item::Misc::Goblet:
            case item::Misc::Key:
            case item::Misc::Mirror:
            case item::Misc::Pendant:
            case item::Misc::Ring:
            case item::Misc::Wand:

            // only a Set so the Set takes care of the enchantment
            case item::Misc::AngelBraid:
            case item::Misc::LichHand:

            case item::Misc::Count:
            default:
            {
                return {};
            }
        }
    }

    const EnchantmentPtr_t EnchantmentFactory::NewFromSetType(const item::Set::Enum ENUM)
    {
        return Make(MakeFromSetType(ENUM));
    }

    const Enchantment EnchantmentFactory::MakeFromSetType(const item::Set::Enum ENUM)
    {
        switch (ENUM)
        {
            case item::Set::Assassins:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Luck, 4),
                               std::make_pair(Traits::Speed, 4),
                               std::make_pair(Traits::Backstab, 2),
                               std::make_pair(Traits::FindMagic, 2) }));
            }
            case item::Set::Neverwinter:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Luck, 8),
                               std::make_pair(Traits::Speed, 6),
                               std::make_pair(Traits::Backstab, 4),
                               std::make_pair(Traits::FindMagic, 3) }));
            }
            case item::Set::Tickler:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Luck, 13),
                               std::make_pair(Traits::Speed, 8),
                               std::make_pair(Traits::Backstab, 6),
                               std::make_pair(Traits::FindMagic, 4) }));
            }
            case item::Set::Magus:
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
            case item::Set::Necromancers:
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
            case item::Set::Warlocks:
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
            case item::Set::LichKings:
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
            case item::Set::Sages:
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
            case item::Set::Shamans:
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
            case item::Set::Oracles:
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
            case item::Set::Angelic:
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
            case item::Set::Balladeers:
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
            case item::Set::Troubadours:
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
            case item::Set::Muses:
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
            case item::Set::Cavaliers:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 2),
                               std::make_pair(Traits::Speed, 2),
                               std::make_pair(Traits::HitPower, 1) }));
            }
            case item::Set::Champions:
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
            case item::Set::Paladins:
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
            case item::Set::Berserkers:
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
            case item::Set::Rosewood:
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
            case item::Set::Dragonslayers:
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
            case item::Set::EventideRider:
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
            case item::Set::Hunters:
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
            case item::Set::SureShot:
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
            case item::Set::Marksmans:
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
            case item::Set::Deadeye:
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
            case item::Set::DuskRanger:
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
            case item::Set::VenomBow:
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
            case item::Set::CritterChannelers:
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
            case item::Set::MammalianHead:
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
            case item::Set::SavageTaskmasters:
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
            case item::Set::MonsterOverseers:
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
            case item::Set::BeastRulers:
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
            case item::Set::Count:
            default:
            {
                std::ostringstream ss;
                ss << "creature::EnchantmentFactory::NewFromSetType(Set=" << NAMEOF_ENUM(ENUM)
                   << ") was unable to create with these arguments.";

                throw std::runtime_error(ss.str());
            }
        }
    }

    const EnchantmentPtr_t EnchantmentFactory::NewFromSetCompleteType(const item::Set::Enum ENUM)
    {
        return Make(MakeFromSetCompleteType(ENUM));
    }

    const Enchantment EnchantmentFactory::MakeFromSetCompleteType(const item::Set::Enum ENUM)
    {
        switch (ENUM)
        {
            case item::Set::Assassins:
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
            case item::Set::Neverwinter:
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
            case item::Set::Tickler:
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
            case item::Set::Magus:
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
            case item::Set::Necromancers:
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
            case item::Set::Warlocks:
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
            case item::Set::LichKings:
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
            case item::Set::Sages:
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
            case item::Set::Shamans:
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
            case item::Set::Oracles:
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
            case item::Set::Angelic:
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
            case item::Set::Balladeers:
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
            case item::Set::Troubadours:
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
            case item::Set::Muses:
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
            case item::Set::Cavaliers:
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
            case item::Set::Champions:
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
            case item::Set::Paladins:
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
            case item::Set::Berserkers:
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
            case item::Set::Rosewood:
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
            case item::Set::Dragonslayers:
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
            case item::Set::EventideRider:
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
            case item::Set::Hunters:
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
            case item::Set::SureShot:
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
            case item::Set::Marksmans:
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
            case item::Set::Deadeye:
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
            case item::Set::DuskRanger:
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
            case item::Set::VenomBow:
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
            case item::Set::CritterChannelers:
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
            case item::Set::MammalianHead:
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
            case item::Set::SavageTaskmasters:
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
            case item::Set::MonsterOverseers:
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
            case item::Set::BeastRulers:
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
            case item::Set::Count:
            default:
            {
                std::ostringstream ss;
                ss << "creature::EnchantmentFactory::NewFromSetCompleteType(Set="
                   << NAMEOF_ENUM(ENUM) << ") was unable to create with these arguments.";

                throw std::runtime_error(ss.str());
            }
        }
    }

    const EnchantmentPtr_t EnchantmentFactory::NewFromElementType(
        const item::Element::Enum ENUM,
        const bool IS_WEAPON,
        const item::Material::Enum MAT_PRI,
        const item::Material::Enum MAT_SEC)
    {
        return Make(MakeFromElementType(ENUM, IS_WEAPON, MAT_PRI, MAT_SEC));
    }

    const Enchantment EnchantmentFactory::MakeFromElementType(
        const item::Element::Enum ENUM,
        const bool IS_WEAPON,
        const item::Material::Enum MAT_PRI,
        const item::Material::Enum MAT_SEC)
    {
        if (ENUM == item::Element::None)
        {
            std::ostringstream ss;
            ss << "creature::EnchantmentFactory::NewFromElementType(Element=" << NAMEOF_ENUM(ENUM)
               << ", is_weapon=" << std::boolalpha << IS_WEAPON
               << ", mat_pri=" << NAMEOF_ENUM(MAT_PRI) << ", mat_sec=" << NAMEOF_ENUM(MAT_SEC)
               << ") was given an Element of NONE.";

            throw std::runtime_error(ss.str());
        }

        TraitSet traits;

        if (MAT_PRI >= item::Material::Count)
        {
            if (IS_WEAPON)
            {
                const auto DAMAGE { 50 };

                if (ENUM & item::Element::Fire)
                {
                    traits.Get(Traits::FireDamage).CurrentSet(DAMAGE);
                }

                if (ENUM & item::Element::Frost)
                {
                    traits.Get(Traits::FrostDamage).CurrentSet(DAMAGE);
                }

                if (ENUM & item::Element::Honor)
                {
                    traits.Get(Traits::HonorDamage).CurrentSet(DAMAGE);
                }

                if (ENUM & item::Element::Shadow)
                {
                    traits.Get(Traits::ShadowDamage).CurrentSet(DAMAGE);
                }
            }
            else
            {
                // if not IS_WEAPON then the item is assumed to be armor or wearable misc

                const auto RESISTANCE { 13 };

                if (ENUM & item::Element::Fire)
                {
                    traits.Get(Traits::FireResist).CurrentSet(RESISTANCE);
                }

                if (ENUM & item::Element::Frost)
                {
                    traits.Get(Traits::FrostResist).CurrentSet(RESISTANCE);
                }

                if (ENUM & item::Element::Honor)
                {
                    traits.Get(Traits::HonorResist).CurrentSet(RESISTANCE);
                }

                if (ENUM & item::Element::Shadow)
                {
                    traits.Get(Traits::ShadowResist).CurrentSet(RESISTANCE);
                }
            }
        }
        else
        {
            const auto MAT_BONUS { item::Material::EnchantmentBonus(MAT_PRI, MAT_SEC) };

            if (IS_WEAPON)
            {
                const auto DAMAGE_BASE { 10 };
                const auto DAMAGE_MULT { 5 };
                const auto DAMAGE { DAMAGE_BASE + (MAT_BONUS * DAMAGE_MULT) };

                if (ENUM & item::Element::Fire)
                {
                    traits.Get(Traits::FireDamage).CurrentSet(DAMAGE);
                }

                if (ENUM & item::Element::Frost)
                {
                    traits.Get(Traits::FrostDamage).CurrentSet(DAMAGE);
                }

                if (ENUM & item::Element::Honor)
                {
                    traits.Get(Traits::HonorDamage).CurrentSet(DAMAGE);
                }

                if (ENUM & item::Element::Shadow)
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

                if (ENUM & item::Element::Fire)
                {
                    traits.Get(Traits::FireResist).CurrentSet(RESISTANCE);
                }

                if (ENUM & item::Element::Frost)
                {
                    traits.Get(Traits::FrostResist).CurrentSet(RESISTANCE);
                }

                if (ENUM & item::Element::Honor)
                {
                    traits.Get(Traits::HonorResist).CurrentSet(RESISTANCE);
                }

                if (ENUM & item::Element::Shadow)
                {
                    traits.Get(Traits::ShadowResist).CurrentSet(RESISTANCE);
                }
            }
        }

        return Enchantment(EnchantmentType::WhenEquipped, traits);
    }

    const EnchantmentPtr_t EnchantmentFactory::NewFromNamedType(
        const item::Named::Enum NAMED_ENUM,
        const item::Material::Enum MAT_PRI,
        const item::Material::Enum MAT_SEC,
        const bool IS_WEAPON,
        const bool IS_ARMOR)
    {
        return Make(MakeFromNamedType(NAMED_ENUM, MAT_PRI, MAT_SEC, IS_WEAPON, IS_ARMOR));
    }

    const Enchantment EnchantmentFactory::MakeFromNamedType(
        const item::Named::Enum NAMED_ENUM,
        const item::Material::Enum MAT_PRI,
        const item::Material::Enum MAT_SEC,
        const bool IS_WEAPON,
        const bool IS_ARMOR)
    {
        switch (NAMED_ENUM)
        {
            case item::Named::Wicked:
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
            case item::Named::Fiendish:
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
            case item::Named::Infernal:
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
            case item::Named::Raging:
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
            case item::Named::Diabolic:
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
            case item::Named::Dancing:
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
            case item::Named::Marauder:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 16),
                               std::make_pair(Traits::Accuracy, 13),
                               std::make_pair(Traits::HitCritical, 5) }));
            }
            case item::Named::Honest:
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
            case item::Named::Noble:
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
            case item::Named::Daring:
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
            case item::Named::Elite:
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
            case item::Named::Valiant:
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
            case item::Named::Heroes:
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
            case item::Named::Army:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 3),
                               std::make_pair(Traits::Speed, 3),
                               std::make_pair(Traits::ArmorRating, 3) }));
            }
            case item::Named::Gladiator:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet(
                        { std::make_pair(Traits::Strength, 8), std::make_pair(Traits::Speed, 6) }));
            }
            case item::Named::Soldiers:
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
                ss << "creature::EnchantmentFactory::NewFromNamedType(Named=Soldiers, "
                      "is_weapon=false, is_armor=false, mat_pri = "
                   << NAMEOF_ENUM(MAT_PRI) << ", mat_sec=" << NAMEOF_ENUM(MAT_SEC)
                   << ") failed because for that Named must be weapon or armor.";

                throw std::runtime_error(ss.str());
            }
            case item::Named::Wardens:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 5),
                               std::make_pair(Traits::ArmorRating, 5),
                               std::make_pair(Traits::Surprise, -3) }));
            }
            case item::Named::Princes:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 8),
                               std::make_pair(Traits::ArmorRating, 8) }));
            }
            case item::Named::Ranger:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 6),
                               std::make_pair(Traits::Accuracy, 8),
                               std::make_pair(Traits::ArmorRating, 4) }));
            }
            case item::Named::Samurai:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 10),
                               std::make_pair(Traits::Accuracy, 10),
                               std::make_pair(Traits::HitCritical, 2) }));
            }
            case item::Named::Thors:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Strength, 12),
                               std::make_pair(Traits::ArmorRating, 12),
                               std::make_pair(Traits::MagicResist, 4),
                               std::make_pair(Traits::HitPower, 2) }));
            }
            case item::Named::Nile:
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
                ss << "creature::EnchantmentFactory::NewFromNamedType(Named=Nile, "
                      "is_weapon=false, is_armor=false, mat_pri = "
                   << NAMEOF_ENUM(MAT_PRI) << ", mat_sec=" << NAMEOF_ENUM(MAT_SEC)
                   << ") failed because for that Named must be weapon or armor.";

                throw std::runtime_error(ss.str());
            }
            case item::Named::Searing:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::FireDamage, 16) }));
            }
            case item::Named::Burning:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::FireDamage, 33) }));
            }
            case item::Named::Fiery:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::FireDamage, 66) }));
            }
            case item::Named::Charred:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::FireResist, 8) }));
            }
            case item::Named::Icy:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::FrostDamage, 16) }));
            }
            case item::Named::Winter:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::FrostDamage, 33) }));
            }
            case item::Named::Frigid:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::FrostDamage, 66) }));
            }
            case item::Named::Chill:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::FrostResist, 8) }));
            }
            case item::Named::Frozen:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::FrostResist, 13) }));
            }
            case item::Named::Arctic:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::FrostResist, 18) }));
            }
            case item::Named::Proud:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::HonorDamage, 16) }));
            }
            case item::Named::Glory:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::HonorDamage, 33) }));
            }
            case item::Named::Pure:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::HonorDamage, 66) }));
            }
            case item::Named::Light:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::HonorResist, 8) }));
            }
            case item::Named::Dawn:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::HonorResist, 13) }));
            }
            case item::Named::Sun:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::HonorResist, 18) }));
            }
            case item::Named::Gloom:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::ShadowDamage, 16) }));
            }
            case item::Named::Twilight:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::ShadowDamage, 33) }));
            }
            case item::Named::Dusk:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::ShadowDamage, 66) }));
            }
            case item::Named::Sorrow:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::ShadowResist, 20) }));
            }
            case item::Named::Woe:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::ShadowResist, 20) }));
            }
            case item::Named::Misery:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::ShadowResist, 30) }));
            }
            case item::Named::Moon:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Charm, -18),
                               std::make_pair(Traits::ArmorRating, 18),
                               std::make_pair(Traits::ShadowResist, 33),
                               std::make_pair(Traits::CurseResist, 33) }));
            }
            case item::Named::Dark:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::ShadowDamage, 66),
                               std::make_pair(Traits::CurseOnDamage, 8),
                               std::make_pair(Traits::HitCritical, 4),
                               std::make_pair(Traits::Strength, 18) }));
            }
            case item::Named::Betrayer:
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
            case item::Named::Imposters:
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
            case item::Named::Pickpocket:
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
            case item::Named::Burglar:
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
            case item::Named::Mountebank:
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
            case item::Named::Charlatans:
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
            case item::Named::Focus:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Accuracy, 18),
                               std::make_pair(Traits::Strength, 13),
                               std::make_pair(Traits::DamageBonusProj, 16) }));
            }
            case item::Named::Robbers:
            {
                const auto MAT_BONUS { item::Material::EnchantmentBonus(MAT_PRI, MAT_SEC) };

                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Luck, (MAT_BONUS / 2)) }));
            }
            case item::Named::Thugs:
            {
                const auto MAT_BONUS { item::Material::EnchantmentBonus(MAT_PRI, MAT_SEC) };

                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Luck, 10 + (MAT_BONUS / 2)) }));
            }
            case item::Named::Knaves:
            {
                const auto MAT_BONUS { item::Material::EnchantmentBonus(MAT_PRI, MAT_SEC) };

                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Luck, 20 + (MAT_BONUS / 2)) }));
            }
            case item::Named::Muggers:
            {
                const auto MAT_BONUS { item::Material::EnchantmentBonus(MAT_PRI, MAT_SEC) };

                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Backstab, (MAT_BONUS / 2)) }));
            }
            case item::Named::Thief:
            {
                const auto MATERIAL_BASED_BONUS {
                    5 + (item::Material::EnchantmentBonus(MAT_PRI, MAT_SEC) / 2)
                };

                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Luck, MATERIAL_BASED_BONUS),
                               std::make_pair(Traits::Backstab, MATERIAL_BASED_BONUS) }));
            }
            case item::Named::Pirate:
            {
                const auto MAT_BONUS { item::Material::EnchantmentBonus(MAT_PRI, MAT_SEC) };

                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    TraitSet({ std::make_pair(Traits::Backstab, 8 + (MAT_BONUS / 2)),
                               std::make_pair(Traits::FindCoinsAmount, 8 + (MAT_BONUS / 2)),
                               std::make_pair(Traits::Luck, (MAT_BONUS / 2)),
                               std::make_pair(Traits::FindMagic, (MAT_BONUS / 2)) }));
            }
            case item::Named::Count:
            default:
            {
                std::ostringstream ss;
                ss << "creature::EnchantmentFactory::NewFromNamedType(Named=" << NAMED_ENUM
                   << ", is_weapon=" << std::boolalpha << IS_WEAPON << ", is_armor=" << IS_ARMOR
                   << ", mat_pri=" << NAMEOF_ENUM(MAT_PRI) << ", mat_sec=" << NAMEOF_ENUM(MAT_SEC)
                   << ") failed to find a matching Named.";

                throw std::runtime_error(ss.str());
            }
        }
    }

} // namespace creature
} // namespace heroespath
