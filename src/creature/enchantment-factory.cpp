// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// enchantment-factory.cpp
//
#include "enchantment-factory.hpp"

#include "creature/enchantment-warehouse.hpp"
#include "creature/enchantment.hpp"
#include "item/item.hpp"
#include "log/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"

#include <numeric>

namespace heroespath
{
namespace creature
{

    std::unique_ptr<EnchantmentFactory> EnchantmentFactory::instanceUPtr_;

    EnchantmentFactory::EnchantmentFactory()
    {
        M_HP_LOG_DBG("Singleton Construction: EnchantmentFactory");
    }

    EnchantmentFactory::~EnchantmentFactory()
    {
        M_HP_LOG_DBG("Singleton Destruction: EnchantmentFactory");
    }

    misc::NotNull<EnchantmentFactory *> EnchantmentFactory::Instance()
    {
        if (!instanceUPtr_)
        {
            M_HP_LOG_ERR("Singleton Instance() before Acquire(): EnchantmentFactory");
            Acquire();
        }

        return instanceUPtr_.get();
    }

    void EnchantmentFactory::Acquire()
    {
        if (!instanceUPtr_)
        {
            instanceUPtr_ = std::make_unique<EnchantmentFactory>();
        }
        else
        {
            M_HP_LOG_ERR("Singleton Acquire() after Construction: EnchantmentFactory");
        }
    }

    void EnchantmentFactory::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (instanceUPtr_),
            "creature::EnchantmentFactory::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }

    const item::ItemPtr_t
        EnchantmentFactory::MakeStoreAttachReturn(const item::ItemPtr_t ITEM_PTR) const
    {
        if ((ITEM_PTR->IsArmor() || ITEM_PTR->IsWeapon())
            && (ITEM_PTR->ElementType() != item::element_type::None))
        {
            StoreAttachReturn(
                ITEM_PTR,
                NewFromElementType(
                    ITEM_PTR->ElementType(), ITEM_PTR->IsWeapon(), ITEM_PTR->MaterialPrimary()));
        }

        if ((ITEM_PTR->UniqueType() != item::unique_type::NotUnique)
            && (ITEM_PTR->UniqueType() != item::unique_type::Count))
        {
            for (auto const & NEXT_ENCHANTMENT_PTR :
                 NewFromUniqueType(ITEM_PTR->UniqueType(), ITEM_PTR->MaterialPrimary()))
            {
                StoreAttachReturn(ITEM_PTR, NEXT_ENCHANTMENT_PTR);
            }

            return ITEM_PTR;
        }

        if ((ITEM_PTR->NamedType() != item::named_type::NotNamed)
            && (ITEM_PTR->NamedType() != item::named_type::Count))
        {
            return StoreAttachReturn(
                ITEM_PTR,
                NewFromNamedType(
                    ITEM_PTR->NamedType(),
                    ITEM_PTR->MaterialPrimary(),
                    (ITEM_PTR->Category() & item::category::Armor),
                    (ITEM_PTR->Category() & item::category::Weapon)));
        }

        if (ITEM_PTR->SetType() != item::set_type::NotASet)
        {
            return StoreAttachReturn(ITEM_PTR, NewFromSetType(ITEM_PTR->SetType()));
        }

        if (ITEM_PTR->MiscType() != item::misc_type::NotMisc)
        {
            auto const ENCHANTMENT_PTR_OPT{ NewFromMiscType(
                ITEM_PTR->MiscType(), ITEM_PTR->MaterialPrimary(), ITEM_PTR->MaterialSecondary()) };

            if (ENCHANTMENT_PTR_OPT)
            {
                return StoreAttachReturn(ITEM_PTR, ENCHANTMENT_PTR_OPT.value());
            }
        }

        return ITEM_PTR;
    }

    const item::ItemPtr_t EnchantmentFactory::MakeStoreAttachReturn(
        const item::ItemPtr_t ITEM_PTR,
        const EnchantmentType::Enum TYPE,
        const stats::TraitSet & TRAIT_SET,
        const UseInfo & USE_INFO) const
    {
        return StoreAttachReturn(ITEM_PTR, new Enchantment(TYPE, TRAIT_SET, USE_INFO));
    }

    Score_t EnchantmentFactory::TreasureScore(
        const item::unique_type::Enum E, const item::material::Enum MATERIAL_PRIMARY) const
    {
        if ((E == item::unique_type::NotUnique) || (E == item::unique_type::Count))
        {
            return 0_score;
        }

        auto const ENCHANTMENT_VEC{ MakeFromUniqueType(E, MATERIAL_PRIMARY) };

        return std::accumulate(
            std::begin(ENCHANTMENT_VEC),
            std::end(ENCHANTMENT_VEC),
            0_score,
            [](auto const SUM, auto const & ENCHANTMENT) {
                return SUM + ENCHANTMENT.TreasureScore();
            });
    }

    Score_t EnchantmentFactory::TreasureScore(
        const item::misc_type::Enum E,
        const item::material::Enum MATERIAL_PRIMARY,
        const item::material::Enum MATERIAL_SECONDARY) const
    {
        if ((E == item::misc_type::NotMisc) || (E == item::misc_type::Count))
        {
            return 0_score;
        }

        return MakeFromMiscType(E, MATERIAL_PRIMARY, MATERIAL_SECONDARY).TreasureScore();
    }

    Score_t EnchantmentFactory::TreasureScore(const item::set_type::Enum E) const
    {
        if ((E == item::set_type::NotASet) || (E == item::set_type::Count))
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
        const bool IS_WEAPON,
        const bool IS_ARMOR) const
    {
        if ((NAMED_ENUM == item::named_type::NotNamed) || (NAMED_ENUM == item::named_type::Count))
        {
            return 0_score;
        }

        // the additional 450 is to raise the score of all named items
        return MakeFromNamedType(NAMED_ENUM, MATERIAL_PRIMARY, IS_WEAPON, IS_ARMOR).TreasureScore()
            + 450_score;
    }

    Score_t EnchantmentFactory::TreasureScore(
        const item::element_type::Enum E,
        const bool IS_WEAPON,
        const item::material::Enum MATERIAL_PRIMARY) const
    {
        if (E == item::element_type::None)
        {
            return 0_score;
        }

        // the additional 750 is to raise the score of all elemental items
        return MakeFromElementType(E, IS_WEAPON, MATERIAL_PRIMARY).TreasureScore() + 750_score;
    }

    const item::ItemPtr_t EnchantmentFactory::StoreAttachReturn(
        const item::ItemPtr_t ITEM_PTR, const EnchantmentPtr_t ENCHANTMENT_PTR) const
    {
        ITEM_PTR->EnchantmentAdd(EnchantmentWarehouse::Instance()->Store(ENCHANTMENT_PTR));
        return ITEM_PTR;
    }

    const EnchantmentPVec_t EnchantmentFactory::NewFromUniqueType(
        const item::unique_type::Enum UNIQUE_ENUM,
        const item::material::Enum MATERIAL_PRIMARY) const
    {
        EnchantmentPVec_t enchantmentPtrs;
        for (auto const & ENCHANTMENT : MakeFromUniqueType(UNIQUE_ENUM, MATERIAL_PRIMARY))
        {
            enchantmentPtrs.emplace_back(new Enchantment(ENCHANTMENT));
        }

        return enchantmentPtrs;
    }

    const std::vector<Enchantment> EnchantmentFactory::MakeFromUniqueType(
        const item::unique_type::Enum UNIQUE_ENUM,
        const item::material::Enum MATERIAL_PRIMARY) const
    {
        switch (UNIQUE_ENUM)
        {
            case item::unique_type::BasiliskTonge:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::MagicCast, 8),
                                      std::make_pair(stats::Traits::MagicEffect, 13),
                                      std::make_pair(stats::Traits::MagicResist, 13) })) };
            }
            case item::unique_type::BerserkersBeard:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Intelligence, -33),
                                      std::make_pair(stats::Traits::Charm, -16),
                                      std::make_pair(stats::Traits::DamageBonusMelee, 25),
                                      std::make_pair(stats::Traits::Encounter, 8),
                                      std::make_pair(stats::Traits::MagicResist, 18) })) };
            }
            case item::unique_type::BishopsHanky:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Luck, 8),
                                      std::make_pair(stats::Traits::Intelligence, 13),
                                      std::make_pair(stats::Traits::Charm, 18),
                                      std::make_pair(stats::Traits::MagicCast, 8),
                                      std::make_pair(stats::Traits::MagicEffect, 16),
                                      std::make_pair(stats::Traits::MagicResist, 16),
                                      std::make_pair(stats::Traits::BlessCast, 16),
                                      std::make_pair(stats::Traits::BlessEffect, 28),
                                      std::make_pair(stats::Traits::CurseResist, 22) })) };
            }
            case item::unique_type::BleedingTrophy:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem
                        | EnchantmentType::CurseWithoutItem),
                    stats::TraitSet(
                        { std::make_pair(stats::Traits::Luck, (MAT_BONUS * -3)),
                          std::make_pair(stats::Traits::CurseCast, (MAT_BONUS * 3)),
                          std::make_pair(stats::Traits::CurseEffect, 5 + (MAT_BONUS * 4)),
                          std::make_pair(stats::Traits::FindMagic, MAT_BONUS) })) };
            }
            case item::unique_type::BloodyDragonScale:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::AnimalResist, 50),
                                      std::make_pair(stats::Traits::FindCoinsAmount, 10),
                                      std::make_pair(stats::Traits::FindMagic, 10),
                                      std::make_pair(stats::Traits::Charm, -26) })) };
            }
            case item::unique_type::BottleOfBansheeScreams:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Charm, -20),
                                      std::make_pair(stats::Traits::Intelligence, -20),
                                      std::make_pair(stats::Traits::Encounter, -6) })) };
            }
            case item::unique_type::BraceletCrown:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet(
                        { std::make_pair(stats::Traits::Intelligence, (MAT_BONUS * 4)) })) };
            }
            case item::unique_type::BraceletFeather:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet(
                        { std::make_pair(stats::Traits::Accuracy, (MAT_BONUS * 4)) })) };
            }
            case item::unique_type::BraceletFist:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet(
                        { std::make_pair(stats::Traits::Strength, (MAT_BONUS * 4)) })) };
            }
            case item::unique_type::BraceletHourglass:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Speed, (MAT_BONUS * 4)) })) };
            }
            case item::unique_type::BraceletKey:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Luck, (MAT_BONUS * 4)) })) };
            }
            case item::unique_type::BraceletMask:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Charm, (MAT_BONUS * 4)) })) };
            }
            case item::unique_type::BroochCrown:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet(
                        { std::make_pair(stats::Traits::Intelligence, (MAT_BONUS * 5)) })) };
            }
            case item::unique_type::BroochFeather:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet(
                        { std::make_pair(stats::Traits::Accuracy, (MAT_BONUS * 5)) })) };
            }
            case item::unique_type::BroochFist:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet(
                        { std::make_pair(stats::Traits::Strength, (MAT_BONUS * 5)) })) };
            }
            case item::unique_type::BroochHourglass:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Speed, (MAT_BONUS * 5)) })) };
            }
            case item::unique_type::BroochKey:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Luck, (MAT_BONUS * 5)) })) };
            }
            case item::unique_type::BroochMask:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Charm, (MAT_BONUS * 5)) })) };
            }
            case item::unique_type::BurialShroud:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Charm, -28),
                                      std::make_pair(stats::Traits::Luck, -16),
                                      std::make_pair(stats::Traits::CurseCast, 18),
                                      std::make_pair(stats::Traits::CurseEffect, 50),
                                      std::make_pair(stats::Traits::CurseResist, 50),
                                      std::make_pair(stats::Traits::ShadowResist, -33),
                                      std::make_pair(stats::Traits::FindReligious, 18),
                                      std::make_pair(stats::Traits::BlessResist, 33) })) };
            }
            case item::unique_type::CharmCrown:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet(
                        { std::make_pair(stats::Traits::Intelligence, (MAT_BONUS * 3)) })) };
            }
            case item::unique_type::CharmFeather:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet(
                        { std::make_pair(stats::Traits::Accuracy, (MAT_BONUS * 3)) })) };
            }
            case item::unique_type::CharmFist:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet(
                        { std::make_pair(stats::Traits::Strength, (MAT_BONUS * 3)) })) };
            }
            case item::unique_type::CharmHourglass:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Speed, (MAT_BONUS * 3)) })) };
            }
            case item::unique_type::CharmKey:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Luck, (MAT_BONUS * 3)) })) };
            }
            case item::unique_type::CharmMask:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Charm, (MAT_BONUS * 3)) })) };
            }
            case item::unique_type::ChimeraBone:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::MagicResist, 18),
                                      std::make_pair(stats::Traits::Charm, -50),
                                      std::make_pair(stats::Traits::Intelligence, 13),
                                      std::make_pair(stats::Traits::FindMagic, 8) })) };
            }
            case item::unique_type::CobraTooth:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::DamageBonusMelee, 16),
                                      std::make_pair(stats::Traits::PoisonOnAll, 36) })) };
            }
            case item::unique_type::CommandersCape:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 18),
                                      std::make_pair(stats::Traits::ArmorRating, 18),
                                      std::make_pair(stats::Traits::CurseResist, 24) })) };
            }
            case item::unique_type::CopperTroll:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 24),
                                      std::make_pair(stats::Traits::Charm, -24),
                                      std::make_pair(stats::Traits::Encounter, -6) })) };
            }
            case item::unique_type::CrystalCat:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Speed, 24),
                                      std::make_pair(stats::Traits::Luck, 18),
                                      std::make_pair(stats::Traits::Encounter, -3) })) };
            }
            case item::unique_type::CrystalChimes:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::BoundToItem | EnchantmentType::WhenUsed),
                    stats::TraitSet(),
                    UseInfo(
                        -1,
                        static_cast<game::Phase::Enum>(
                            game::Phase::Combat | game::Phase::Exploring | game::Phase::Inventory)),
                    "Adds 15% Mana and removes the conditions: Dazed, Asleep Natural, and Daunted.",
                    2000_score,
                    Enchantment::UseEffectType::CrystalChimes) };
            }
            case item::unique_type::CyclopsEye:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet(
                        { std::make_pair(stats::Traits::HitCritical, 1 + (MAT_BONUS / 2)),
                          std::make_pair(stats::Traits::Surprise, (MAT_BONUS * -1)) })) };
            }
            case item::unique_type::DemonDiary:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::ShadowDamage, 13),
                                      std::make_pair(stats::Traits::ShadowResist, 33),
                                      std::make_pair(stats::Traits::CurseCast, 22),
                                      std::make_pair(stats::Traits::CurseEffect, 33),
                                      std::make_pair(stats::Traits::CurseResist, 33) })) };
            }
            case item::unique_type::DoveBloodVial:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::BoundToItem | EnchantmentType::WhenUsed),
                    stats::TraitSet(),
                    UseInfo(
                        10,
                        static_cast<game::Phase::Enum>(
                            game::Phase::Combat | game::Phase::Exploring | game::Phase::Inventory)),
                    "Removes conditions: Poison, Disease, Dazed, and Daunted.",
                    1900_score,
                    Enchantment::UseEffectType::DoveBloodVial) };
            }
            case item::unique_type::DragonToothWhistle:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::BoundToItem | EnchantmentType::WhenUsed),
                    stats::TraitSet(),
                    UseInfo(
                        -1,
                        static_cast<game::Phase::Enum>(
                            game::Phase::Combat | game::Phase::Inventory)),
                    "Panics non-dragon flying creatures.",
                    1500_score,
                    Enchantment::UseEffectType::DragonToothWhistle) };
            }
            case item::unique_type::DriedFrog:
            case item::unique_type::DriedGecko:
            case item::unique_type::DriedIguana:
            case item::unique_type::DriedLizard:
            case item::unique_type::DriedSalamander:
            case item::unique_type::DriedSkink:
            case item::unique_type::DriedToad:
            case item::unique_type::DriedTurtle:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::BoundToItem | EnchantmentType::WhenUsed),
                    stats::TraitSet(),
                    UseInfo(
                        1,
                        static_cast<game::Phase::Enum>(
                            game::Phase::Combat | game::Phase::Exploring | game::Phase::Inventory)),
                    "Drains 10% Mana, and removes the Poison condition.",
                    800_score,
                    Enchantment::UseEffectType::DriedEdible) };
            }
            case item::unique_type::DruidLeaf:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet(
                        { std::make_pair(stats::Traits::Charm, 10 + (MAT_BONUS * 3)),
                          std::make_pair(stats::Traits::Intelligence, 10 + (MAT_BONUS * 3)),
                          std::make_pair(stats::Traits::Mana, (MAT_BONUS * 2)),
                          std::make_pair(stats::Traits::FindReligious, 1 + (MAT_BONUS / 2)),
                          std::make_pair(stats::Traits::MagicCast, 1 + (MAT_BONUS / 2)),
                          std::make_pair(stats::Traits::MagicEffect, MAT_BONUS),
                          std::make_pair(stats::Traits::MagicResist, MAT_BONUS) })) };
            }
            case item::unique_type::EvilRabbitsFoot:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Luck, -66),
                                      std::make_pair(stats::Traits::Surprise, 6),
                                      std::make_pair(stats::Traits::Encounter, 6),
                                      std::make_pair(stats::Traits::FindMagic, -6),
                                      std::make_pair(stats::Traits::BlessCast, -66),
                                      std::make_pair(stats::Traits::BlessEffect, -66),
                                      std::make_pair(stats::Traits::BlessResist, -66),
                                      std::make_pair(stats::Traits::CurseCast, 66),
                                      std::make_pair(stats::Traits::CurseEffect, 666),
                                      std::make_pair(stats::Traits::CurseResist, 666),
                                      std::make_pair(stats::Traits::AnimalResist, -66) })) };
            }
            case item::unique_type::ExoticGoldenGong:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::BoundToItem | EnchantmentType::WhenUsed),
                    stats::TraitSet(),
                    UseInfo(
                        -1,
                        static_cast<game::Phase::Enum>(
                            game::Phase::Combat | game::Phase::Exploring | game::Phase::Inventory)),
                    "Adds 20% Mana and removes the conditions: Dazed, Asleep Natural, Daunted, and "
                    "Panicked.",
                    2400_score,
                    Enchantment::UseEffectType::GoldenGong) };
            }
            case item::unique_type::FanaticsFlag:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 18),
                                      std::make_pair(stats::Traits::Speed, 13),
                                      std::make_pair(stats::Traits::Charm, -28),
                                      std::make_pair(stats::Traits::Encounter, 8),
                                      std::make_pair(stats::Traits::Surprise, 6) })) };
            }
            case item::unique_type::FriarsChronicle:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem
                        | EnchantmentType::BlessWithoutItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::BlessCast, 33),
                                      std::make_pair(stats::Traits::ShadowResist, 16),
                                      std::make_pair(stats::Traits::Charm, 13),
                                      std::make_pair(stats::Traits::Mana, 13),
                                      std::make_pair(stats::Traits::FindReligious, 6) })) };
            }
            case item::unique_type::FuneralRecord:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem
                        | EnchantmentType::CurseWithoutItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::CurseCast, 33),
                                      std::make_pair(stats::Traits::ShadowResist, 13),
                                      std::make_pair(stats::Traits::Charm, -13),
                                      std::make_pair(stats::Traits::Intelligence, 13),
                                      std::make_pair(stats::Traits::FindReligious, 8),
                                      std::make_pair(stats::Traits::Mana, 13) })) };
            }
            case item::unique_type::GeneralsCape:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 24),
                                      std::make_pair(stats::Traits::ArmorRating, 24),
                                      std::make_pair(stats::Traits::CurseResist, 28) })) };
            }
            case item::unique_type::GhostSheet:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::ArmorRating, 13),
                                      std::make_pair(stats::Traits::MagicResist, 13),
                                      std::make_pair(stats::Traits::Speed, -13),
                                      std::make_pair(stats::Traits::Encounter, -6) })) };
            }
            case item::unique_type::GiantOwlEye:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Accuracy, 18),
                                      std::make_pair(stats::Traits::Speed, 13),
                                      std::make_pair(stats::Traits::HitCritical, 6),
                                      std::make_pair(stats::Traits::DamageBonusProj, 8),
                                      std::make_pair(stats::Traits::Surprise, -8) })) };
            }
            case item::unique_type::GriffinFeather:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem
                        | EnchantmentType::AllowsFlight),
                    stats::TraitSet({ std::make_pair(stats::Traits::Speed, 18),
                                      std::make_pair(stats::Traits::Strength, 13),
                                      std::make_pair(stats::Traits::DamageBonusProj, 13) })) };
            }
            case item::unique_type::HangmansNoose:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem
                        | EnchantmentType::AllowsFlight),
                    stats::TraitSet({ std::make_pair(stats::Traits::Charm, 18),
                                      std::make_pair(stats::Traits::Luck, 13),
                                      std::make_pair(stats::Traits::CurseCast, 22),
                                      std::make_pair(stats::Traits::CurseEffect, 33),
                                      std::make_pair(stats::Traits::CurseResist, 33),
                                      std::make_pair(stats::Traits::ShadowDamage, 18) })) };
            }
            case item::unique_type::HawkEye:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Accuracy, 16),
                                      std::make_pair(stats::Traits::Speed, 10),
                                      std::make_pair(stats::Traits::HitCritical, 6),
                                      std::make_pair(stats::Traits::DamageBonusProj, 10),
                                      std::make_pair(stats::Traits::Surprise, -6) })) };
            }
            case item::unique_type::HobgoblinNose:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Charm, -26),
                                      std::make_pair(stats::Traits::Luck, 13),
                                      std::make_pair(stats::Traits::Surprise, -10),
                                      std::make_pair(stats::Traits::MagicEffect, 13) })) };
            }
            case item::unique_type::HoboRing:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet(
                        { std::make_pair(stats::Traits::FindCoinsAmount, MAT_BONUS),
                          std::make_pair(stats::Traits::FindReligious, 1 + (MAT_BONUS / 4)),
                          std::make_pair(stats::Traits::FindMagic, 1 + (MAT_BONUS / 2)),
                          std::make_pair(stats::Traits::Charm, 24),
                          std::make_pair(stats::Traits::Mana, 22),
                          std::make_pair(stats::Traits::FindReligious, 16) })) };
            }
            case item::unique_type::HolyEpic:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem
                        | EnchantmentType::BlessWithoutItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::BlessCast, 66),
                                      std::make_pair(stats::Traits::BlessEffect, 80),
                                      std::make_pair(stats::Traits::ShadowResist, 28),
                                      std::make_pair(stats::Traits::Charm, 24),
                                      std::make_pair(stats::Traits::Mana, 22),
                                      std::make_pair(stats::Traits::FindReligious, 16) })) };
            }
            case item::unique_type::HornOfTheHorde:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 13),
                                      std::make_pair(stats::Traits::Encounter, -8),
                                      std::make_pair(stats::Traits::Surprise, -8) })) };
            }
            case item::unique_type::ImpTail:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::DamageBonusMelee, 20),
                                      std::make_pair(stats::Traits::Speed, 13) })) };
            }
            case item::unique_type::IslanderHeaddress:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Charm, 16),
                                      std::make_pair(stats::Traits::Mana, 13),
                                      std::make_pair(stats::Traits::MagicCast, 8),
                                      std::make_pair(stats::Traits::MagicResist, 18),
                                      std::make_pair(stats::Traits::CurseResist, 18),
                                      std::make_pair(stats::Traits::FindReligious, 13),
                                      std::make_pair(stats::Traits::Encounter, -8),
                                      std::make_pair(stats::Traits::ShadowResist, 20) })) };
            }
            case item::unique_type::JeweledAnkhNecklace:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet(
                        { std::make_pair(
                              stats::Traits::FindReligious,
                              static_cast<int>(static_cast<float>(MAT_BONUS) * 1.5f)),
                          std::make_pair(stats::Traits::FindMagic, MAT_BONUS),
                          std::make_pair(stats::Traits::MagicResist, 1 + (MAT_BONUS / 2)),
                          std::make_pair(stats::Traits::CurseCast, MAT_BONUS),
                          std::make_pair(stats::Traits::CurseEffect, 13 + (MAT_BONUS * 2)),
                          std::make_pair(stats::Traits::CurseResist, 13 + (MAT_BONUS * 2)) })) };
            }
            case item::unique_type::JeweledArmband:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet(
                        { std::make_pair(stats::Traits::ArmorRating, (MAT_BONUS * 2)) })) };
            }
            case item::unique_type::JeweledHandbag:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet(
                        { std::make_pair(stats::Traits::FindMagic, 1 + (MAT_BONUS / 2)),
                          std::make_pair(stats::Traits::FindCoinsAmount, 1 + (MAT_BONUS / 2)),
                          std::make_pair(stats::Traits::Luck, (MAT_BONUS * 2)) })) };
            }
            case item::unique_type::JeweledPrincessVeil:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet(
                        { std::make_pair(stats::Traits::Charm, 4 + MAT_BONUS),
                          std::make_pair(stats::Traits::FindMagic, 1 + (MAT_BONUS / 2)) })) };
            }
            case item::unique_type::KingsCape:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 30),
                                      std::make_pair(stats::Traits::ArmorRating, 30),
                                      std::make_pair(stats::Traits::CurseResist, 33) })) };
            }
            case item::unique_type::LastRitesScroll:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem
                        | EnchantmentType::CurseWithoutItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::CurseCast, 16),
                                      std::make_pair(stats::Traits::CurseEffect, 33),
                                      std::make_pair(stats::Traits::CurseResist, 33),
                                      std::make_pair(stats::Traits::ShadowResist, 28),
                                      std::make_pair(stats::Traits::Charm, -28),
                                      std::make_pair(stats::Traits::Intelligence, 28),
                                      std::make_pair(stats::Traits::FindReligious, 16),
                                      std::make_pair(stats::Traits::Mana, 26) })) };
            }
            case item::unique_type::MacabreManuscript:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem
                        | EnchantmentType::CurseWithoutItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::CurseCast, 33),
                                      std::make_pair(stats::Traits::ShadowResist, 24),
                                      std::make_pair(stats::Traits::Charm, -22),
                                      std::make_pair(stats::Traits::Intelligence, 22),
                                      std::make_pair(stats::Traits::FindReligious, 13),
                                      std::make_pair(stats::Traits::Mana, 22) })) };
            }
            case item::unique_type::MadRatJuju:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Charm, 16),
                                      std::make_pair(stats::Traits::Intelligence, 33),
                                      std::make_pair(stats::Traits::Speed, 33),
                                      std::make_pair(stats::Traits::AnimalResist, -18) })) };
            }
            case item::unique_type::MagicHorseshoe:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Luck, 18),
                                      std::make_pair(stats::Traits::FindReligious, 4),
                                      std::make_pair(stats::Traits::FindMagic, 8),
                                      std::make_pair(stats::Traits::FindCoinsAmount, 16) })) };
            }
            case item::unique_type::MagnifyingGlass:
            {
                return {};
            }
            case item::unique_type::ManaAmulet:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet(
                        { std::make_pair(stats::Traits::Mana, 18 + MAT_BONUS),
                          std::make_pair(stats::Traits::MagicEffect, 1 + (MAT_BONUS / 2)) })) };
            }
            case item::unique_type::MendicantRing:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Charm, -18),
                                      std::make_pair(stats::Traits::FindCoinsAmount, 16),
                                      std::make_pair(stats::Traits::FindMagic, 8),
                                      std::make_pair(stats::Traits::FindReligious, 18) })) };
            }
            case item::unique_type::MinotaurHide:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::AnimalResist, 33),
                                      std::make_pair(stats::Traits::Strength, 16),
                                      std::make_pair(stats::Traits::HitPower, 6),
                                      std::make_pair(stats::Traits::Charm, -28),
                                      std::make_pair(stats::Traits::Intelligence, -16) })) };
            }
            case item::unique_type::MonkRing:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::MagicCast, 6),
                                      std::make_pair(stats::Traits::MagicResist, 13),
                                      std::make_pair(stats::Traits::Intelligence, 18),
                                      std::make_pair(stats::Traits::Charm, 18),
                                      std::make_pair(stats::Traits::FindCoinsAmount, 13),
                                      std::make_pair(stats::Traits::CurseResist, 28) })) };
            }
            case item::unique_type::MortuaryOrnament:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::ShadowResist, 66),
                                      std::make_pair(stats::Traits::FindMagic, 13),
                                      std::make_pair(stats::Traits::FindReligious, 26) })) };
            }
            case item::unique_type::MournersMask:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Charm, -33),
                                      std::make_pair(stats::Traits::CurseCast, 16),
                                      std::make_pair(stats::Traits::CurseEffect, 33),
                                      std::make_pair(stats::Traits::CurseResist, 50),
                                      std::make_pair(stats::Traits::ShadowResist, 28) })) };
            }
            case item::unique_type::PantherPaw:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Speed, 18),
                                      std::make_pair(stats::Traits::Strength, 13),
                                      std::make_pair(stats::Traits::AnimalResist, 13) })) };
            }
            case item::unique_type::PinCrown:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Intelligence, MAT_BONUS) })) };
            }
            case item::unique_type::PinFeather:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Accuracy, MAT_BONUS) })) };
            }
            case item::unique_type::PinFist:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, MAT_BONUS) })) };
            }
            case item::unique_type::PinHourglass:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Speed, MAT_BONUS) })) };
            }
            case item::unique_type::PinKey:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Luck, MAT_BONUS) })) };
            }
            case item::unique_type::PinMask:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Charm, MAT_BONUS) })) };
            }
            case item::unique_type::PixieBell:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::BoundToItem | EnchantmentType::WhenUsed),
                    stats::TraitSet(),
                    UseInfo(
                        -1,
                        static_cast<game::Phase::Enum>(
                            game::Phase::Combat | game::Phase::Exploring | game::Phase::Inventory)),
                    "Adds 10% Mana and removes the conditions: Dazed, and Asleep Natural.",
                    1800_score,
                    Enchantment::UseEffectType::PixieBell) };
            }
            case item::unique_type::PriestRing:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::MagicCast, 8),
                                      std::make_pair(stats::Traits::MagicResist, 18),
                                      std::make_pair(stats::Traits::Charm, 18),
                                      std::make_pair(stats::Traits::BlessCast, 13),
                                      std::make_pair(stats::Traits::BlessEffect, 26) })) };
            }
            case item::unique_type::RascalMask:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Luck, 22),
                                      std::make_pair(stats::Traits::Charm, 18),
                                      std::make_pair(stats::Traits::FindMagic, 8),
                                      std::make_pair(stats::Traits::FindCoinsAmount, 16) })) };
            }
            case item::unique_type::RattlesnakeTail:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Speed, 16),
                                      std::make_pair(stats::Traits::Encounter, 8) })) };
            }
            case item::unique_type::RavenClaw:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 13),
                                      std::make_pair(stats::Traits::Accuracy, 13),
                                      std::make_pair(stats::Traits::DamageBonusProj, 18) })) };
            }
            case item::unique_type::RazorFingerclaw:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Charm, -13),
                                      std::make_pair(stats::Traits::DamageBonusFist, 500),
                                      std::make_pair(stats::Traits::DamageBonusMelee, 13) })) };
            }
            case item::unique_type::ReaperScythe:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::HealthGainMelee, 25) })) };
            }
            case item::unique_type::RegalCaptainsFlag:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 8),
                                      std::make_pair(stats::Traits::Charm, 8),
                                      std::make_pair(stats::Traits::Encounter, -6),
                                      std::make_pair(stats::Traits::Surprise, 8),
                                      std::make_pair(stats::Traits::MagicResist, 8) })) };
            }
            case item::unique_type::RequiemRegister:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem
                        | EnchantmentType::CurseWithoutItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::CurseCast, 33),
                                      std::make_pair(stats::Traits::ShadowResist, 18),
                                      std::make_pair(stats::Traits::Charm, -18),
                                      std::make_pair(stats::Traits::Intelligence, 18),
                                      std::make_pair(stats::Traits::FindReligious, 10),
                                      std::make_pair(stats::Traits::Mana, 18) })) };
            }
            case item::unique_type::RoyalScoutSpyglass:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Accuracy, 16),
                                      std::make_pair(stats::Traits::Surprise, -12) })) };
            }
            case item::unique_type::SaintCameoPin:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Accuracy, 16),
                                      std::make_pair(stats::Traits::Surprise, -12) })) };
            }
            case item::unique_type::SaintsJournal:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet(
                        { std::make_pair(stats::Traits::Charm, MAT_BONUS),
                          std::make_pair(stats::Traits::FindMagic, MAT_BONUS),
                          std::make_pair(stats::Traits::MagicEffect, 1 + (MAT_BONUS / 2)),
                          std::make_pair(stats::Traits::BlessCast, MAT_BONUS),
                          std::make_pair(stats::Traits::BlessEffect, 10 + (MAT_BONUS * 2)) })) };
            }
            case item::unique_type::SanguineRelic:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem
                        | EnchantmentType::CurseWithoutItem),
                    stats::TraitSet(
                        { std::make_pair(stats::Traits::CurseCast, (MAT_BONUS * 5)),
                          std::make_pair(stats::Traits::CurseEffect, 10 + (MAT_BONUS * 5)),
                          std::make_pair(stats::Traits::CurseResist, 10 + (MAT_BONUS * 5)),
                          std::make_pair(stats::Traits::FindReligious, MAT_BONUS) })) };
            }
            case item::unique_type::ScorpionStingerFingerclaw:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Charm, -18),
                                      std::make_pair(stats::Traits::DamageBonusFist, 500),
                                      std::make_pair(stats::Traits::DamageBonusMelee, 18),
                                      std::make_pair(stats::Traits::PoisonOnMelee, 18) })) };
            }
            case item::unique_type::ScoundrelSack:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::FindCoinsAmount, 18),
                                      std::make_pair(stats::Traits::FindMagic, 8),
                                      std::make_pair(stats::Traits::Luck, 18),
                                      std::make_pair(stats::Traits::Backstab, 6) })) };
            }
            case item::unique_type::SepultureDecoration:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::ShadowResist, 33),
                                      std::make_pair(stats::Traits::FindMagic, 8),
                                      std::make_pair(stats::Traits::FindReligious, 13) })) };
            }
            case item::unique_type::ShadeCloak:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::ArmorRating, 33),
                                      std::make_pair(stats::Traits::MagicResist, 26),
                                      std::make_pair(stats::Traits::Speed, -20),
                                      std::make_pair(stats::Traits::Encounter, -6) })) };
            }
            case item::unique_type::ShamanRainmaker:
            {
                return { Enchantment(
                             static_cast<EnchantmentType::Enum>(
                                 EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                             stats::TraitSet({ std::make_pair(stats::Traits::Charm, 8),
                                               std::make_pair(stats::Traits::Encounter, 8),
                                               std::make_pair(stats::Traits::FindMagic, 13),
                                               std::make_pair(stats::Traits::MagicCast, 8),
                                               std::make_pair(stats::Traits::MagicEffect, 18),
                                               std::make_pair(stats::Traits::BlessCast, 16),
                                               std::make_pair(stats::Traits::BlessEffect, 33) })),
                         Enchantment(
                             static_cast<EnchantmentType::Enum>(
                                 EnchantmentType::BoundToItem | EnchantmentType::WhenUsed),
                             stats::TraitSet(),
                             UseInfo(
                                 -1,
                                 static_cast<game::Phase::Enum>(
                                     game::Phase::Combat | game::Phase::Exploring
                                     | game::Phase::Inventory)),
                             "Adds 10% Mana.",
                             600_score,
                             Enchantment::UseEffectType::ShamanRainmaker) };
            }
            case item::unique_type::SharkToothNecklace:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 13),
                                      std::make_pair(stats::Traits::DamageBonusMelee, 18) })) };
            }
            case item::unique_type::SignetCrown:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet(
                        { std::make_pair(stats::Traits::Intelligence, (MAT_BONUS * 2)) })) };
            }
            case item::unique_type::SignetFeather:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet(
                        { std::make_pair(stats::Traits::Accuracy, (MAT_BONUS * 2)) })) };
            }
            case item::unique_type::SignetFist:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet(
                        { std::make_pair(stats::Traits::Strength, (MAT_BONUS * 2)) })) };
            }
            case item::unique_type::SignetHourglass:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Speed, (MAT_BONUS * 2)) })) };
            }
            case item::unique_type::SignetKey:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Luck, (MAT_BONUS * 2)) })) };
            }
            case item::unique_type::SignetMask:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Charm, (MAT_BONUS * 2)) })) };
            }
            case item::unique_type::SirenConch:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::MagicCast, 8),
                                      std::make_pair(stats::Traits::MagicEffect, 13),
                                      std::make_pair(stats::Traits::Charm, 13) })) };
            }
            case item::unique_type::SpecterChains:
            {
                return { Enchantment(
                             static_cast<EnchantmentType::Enum>(
                                 EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                             stats::TraitSet({ std::make_pair(stats::Traits::Strength, 18),
                                               std::make_pair(stats::Traits::Encounter, -6),
                                               std::make_pair(stats::Traits::Charm, 13) })),
                         Enchantment(
                             static_cast<EnchantmentType::Enum>(
                                 EnchantmentType::BoundToItem | EnchantmentType::WhenUsed),
                             stats::TraitSet(),
                             UseInfo(
                                 -1,
                                 static_cast<game::Phase::Enum>(
                                     game::Phase::Combat | game::Phase::Inventory)),
                             "Panics enemies.",
                             1300_score,
                             Enchantment::UseEffectType::SpecterChains) };
            }
            case item::unique_type::SpecterRobe:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::ArmorRating, 20),
                                      std::make_pair(stats::Traits::MagicResist, 18),
                                      std::make_pair(stats::Traits::Speed, -13),
                                      std::make_pair(stats::Traits::Encounter, -8) })) };
            }
            case item::unique_type::SprintersLegtie:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Speed, 22),
                                      std::make_pair(stats::Traits::Strength, -8) })) };
            }
            case item::unique_type::SwindlersBag:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::FindCoinsAmount, 22),
                                      std::make_pair(stats::Traits::FindMagic, 8),
                                      std::make_pair(stats::Traits::Luck, 22),
                                      std::make_pair(stats::Traits::HitCritical, 5) })) };
            }
            case item::unique_type::TribalFlag:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Charm, -16),
                                      std::make_pair(stats::Traits::Encounter, 6),
                                      std::make_pair(stats::Traits::Surprise, 8) })) };
            }
            case item::unique_type::TricksterPouch:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::FindCoinsAmount, 13),
                                      std::make_pair(stats::Traits::FindMagic, 8),
                                      std::make_pair(stats::Traits::Luck, 16),
                                      std::make_pair(stats::Traits::Charm, 18) })) };
            }
            case item::unique_type::TuningFork:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Charm, MAT_BONUS),
                                      std::make_pair(stats::Traits::MagicCast, MAT_BONUS) })) };
            }
            case item::unique_type::TurtleShell:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet(
                        { std::make_pair(stats::Traits::ArmorRating, (MAT_BONUS * 3)) })) };
            }
            case item::unique_type::VampiresToothNecklace:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Charm, 13),
                                      std::make_pair(stats::Traits::HealthGainAll, 25),
                                      std::make_pair(stats::Traits::CurseResist, 28) })) };
            }
            case item::unique_type::ViperFangFingerclaw:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Charm, -16),
                                      std::make_pair(stats::Traits::DamageBonusFist, 500),
                                      std::make_pair(stats::Traits::DamageBonusMelee, 16),
                                      std::make_pair(stats::Traits::PoisonOnMelee, 13) })) };
            }
            case item::unique_type::VultureGizzard:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::BoundToItem | EnchantmentType::WhenUsed),
                    stats::TraitSet(),
                    UseInfo(
                        3,
                        static_cast<game::Phase::Enum>(
                            game::Phase::Combat | game::Phase::Exploring | game::Phase::Inventory)),
                    "Curse Poison.",
                    1000_score,
                    Enchantment::UseEffectType::VultureGizzard) };
            }
            case item::unique_type::WarhorseMarionette:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 22),
                                      std::make_pair(stats::Traits::ArmorRating, 22),
                                      std::make_pair(stats::Traits::Speed, -13),
                                      std::make_pair(stats::Traits::HitPower, 13),
                                      std::make_pair(stats::Traits::Intelligence, -16) })) };
            }
            case item::unique_type::WarTrumpet:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::BoundToItem | EnchantmentType::WhenUsed),
                    stats::TraitSet(),
                    UseInfo(
                        -1,
                        static_cast<game::Phase::Enum>(
                            game::Phase::Combat | game::Phase::Inventory)),
                    "Panics or Daunts enemies.",
                    1100_score,
                    Enchantment::UseEffectType::WarTrumpet) };
            }
            case item::unique_type::WeaselTotem:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Speed, 28),
                                      std::make_pair(stats::Traits::Luck, 18),
                                      std::make_pair(stats::Traits::Encounter, -6) })) };
            }
            case item::unique_type::WolfenFur:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::AnimalResist, 28),
                                      std::make_pair(stats::Traits::Speed, 16),
                                      std::make_pair(stats::Traits::Charm, -16),
                                      std::make_pair(stats::Traits::Intelligence, -13) })) };
            }
            case item::unique_type::WraithTalisman:
            {
                return { Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenHeld | EnchantmentType::BoundToItem),
                    stats::TraitSet({
                        std::make_pair(stats::Traits::Charm, -16),
                        std::make_pair(stats::Traits::FindMagic, 8),
                        std::make_pair(stats::Traits::MagicCast, 8),
                        std::make_pair(stats::Traits::MagicEffect, 13),
                        std::make_pair(stats::Traits::MagicResist, 13),
                        std::make_pair(stats::Traits::CurseCast, 13),
                        std::make_pair(stats::Traits::CurseEffect, 33),
                        std::make_pair(stats::Traits::CurseResist, 33),
                    })) };
            }
            case item::unique_type::NotUnique:
            case item::unique_type::Count:
            default:
            {
                M_HP_LOG_ERR(
                    "creature::EnchantmentFactory::NewFromUniqueType"
                    << "(unique_type=" << item::unique_type::ToString(UNIQUE_ENUM)
                    << ") invalid unique_type.");

                return {};
            }
        }
    }

    const EnchantmentPtrOpt_t EnchantmentFactory::NewFromMiscType(
        const item::misc_type::Enum E,
        const item::material::Enum MATERIAL_PRIMARY,
        const item::material::Enum MATERIAL_SECONDARY) const
    {
        auto const ENCHANTMENT{ MakeFromMiscType(E, MATERIAL_PRIMARY, MATERIAL_SECONDARY) };

        if (ENCHANTMENT == Enchantment())
        {
            return boost::none;
        }
        else
        {
            return EnchantmentPtrOpt_t(new Enchantment(ENCHANTMENT));
        }
    }

    const Enchantment EnchantmentFactory::MakeFromMiscType(
        const item::misc_type::Enum E,
        const item::material::Enum MATERIAL_PRIMARY,
        const item::material::Enum MATERIAL_SECONDARY) const
    {
        if (E == item::misc_type::Spider_Eggs)
        {
            return Enchantment(
                EnchantmentType::WhenUsed, stats::TraitSet(), UseInfo(10, game::Phase::Combat));
        }
        else if (
            (item::misc_type::IsBlessed(E)) && (MATERIAL_SECONDARY != item::material::Count)
            && (MATERIAL_SECONDARY != item::material::Nothing))
        {
            auto const USE_COUNT{ item::material::Bonus(MATERIAL_SECONDARY) };
            return Enchantment(
                static_cast<EnchantmentType::Enum>(
                    EnchantmentType::BoundToItem | EnchantmentType::WhenHeld),
                stats::TraitSet({ std::make_pair(stats::Traits::Encounter, 3),
                                  std::make_pair(stats::Traits::BlessEffect, 3) }),
                UseInfo(
                    USE_COUNT,
                    static_cast<game::Phase::Enum>(
                        game::Phase::Combat | game::Phase::Inventory | game::Phase::Exploring)),
                "Used when casting or playing a Bless.",
                250_score,
                Enchantment::UseEffectType::Blessed);
        }
        else if (
            (item::misc_type::IsCursed(E)) && (MATERIAL_SECONDARY != item::material::Count)
            && (MATERIAL_SECONDARY != item::material::Nothing))
        {
            auto const USE_COUNT{ item::material::Bonus(MATERIAL_SECONDARY) };
            return Enchantment(
                static_cast<EnchantmentType::Enum>(
                    EnchantmentType::BoundToItem | EnchantmentType::WhenHeld),
                stats::TraitSet({ std::make_pair(stats::Traits::Luck, -3),
                                  std::make_pair(stats::Traits::CurseEffect, 3) }),
                UseInfo(
                    USE_COUNT,
                    static_cast<game::Phase::Enum>(
                        game::Phase::Combat | game::Phase::Inventory | game::Phase::Exploring)),
                "Used when casting or playing a Curse.",
                250_score,
                Enchantment::UseEffectType::Cursed);
        }
        else if (
            (E == item::misc_type::LockPicks) && (MATERIAL_PRIMARY != item::material::Count)
            && (MATERIAL_PRIMARY != item::material::Nothing))
        {
            auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

            return Enchantment(
                static_cast<EnchantmentType::Enum>(
                    EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                stats::TraitSet({ std::make_pair(stats::Traits::Luck, 12 + MAT_BONUS),
                                  std::make_pair(stats::Traits::Speed, 11 + (MAT_BONUS / 2)),
                                  std::make_pair(stats::Traits::Backstab, 1 + (MAT_BONUS / 2)) }));
        }
        else if (
            (E == item::misc_type::Wand) && (MATERIAL_PRIMARY != item::material::Count)
            && (MATERIAL_PRIMARY != item::material::Nothing))
        {
            auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

            return Enchantment(
                static_cast<EnchantmentType::Enum>(
                    EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                stats::TraitSet({ std::make_pair(stats::Traits::Mana, 11 + (MAT_BONUS / 2)) }));
        }
        else if (
            (E == item::misc_type::Shard) && (MATERIAL_PRIMARY != item::material::Count)
            && (MATERIAL_PRIMARY != item::material::Nothing))
        {
            auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

            return Enchantment(
                static_cast<EnchantmentType::Enum>(
                    EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                stats::TraitSet({ std::make_pair(stats::Traits::Mana, 11 + (MAT_BONUS / 2)),
                                  std::make_pair(stats::Traits::MagicEffect, (MAT_BONUS / 2)) }));
        }
        else if (
            (E == item::misc_type::Staff) && (MATERIAL_PRIMARY != item::material::Count)
            && (MATERIAL_PRIMARY != item::material::Nothing))
        {
            auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

            return Enchantment(
                static_cast<EnchantmentType::Enum>(
                    EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                stats::TraitSet({ std::make_pair(stats::Traits::Mana, 11 + (MAT_BONUS / 2)),
                                  std::make_pair(stats::Traits::MagicEffect, 11 + (MAT_BONUS / 2)),
                                  std::make_pair(stats::Traits::MagicCast, 5 + (MAT_BONUS / 4)) }));
        }
        else if (
            (E == item::misc_type::Orb) && (MATERIAL_PRIMARY != item::material::Count)
            && (MATERIAL_PRIMARY != item::material::Nothing))
        {
            auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

            return Enchantment(
                static_cast<EnchantmentType::Enum>(
                    EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                stats::TraitSet({ std::make_pair(stats::Traits::Mana, 12 + MAT_BONUS),
                                  std::make_pair(stats::Traits::MagicEffect, MAT_BONUS),
                                  std::make_pair(stats::Traits::MagicCast, 1 + (MAT_BONUS / 2)) }));
        }
        else if (
            (E == item::misc_type::Scepter) && (MATERIAL_PRIMARY != item::material::Count)
            && (MATERIAL_PRIMARY != item::material::Nothing))
        {
            auto const MAT_BONUS{ item::material::Bonus(MATERIAL_PRIMARY) };

            return Enchantment(
                static_cast<EnchantmentType::Enum>(
                    EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                stats::TraitSet({ std::make_pair(stats::Traits::Mana, 14 + (MAT_BONUS * 2)),
                                  std::make_pair(stats::Traits::MagicEffect, (MAT_BONUS * 2)),
                                  std::make_pair(stats::Traits::MagicCast, MAT_BONUS) }));
        }
        else if (E == item::misc_type::Petrified_Snake)
        {
            return Enchantment(
                static_cast<EnchantmentType::Enum>(
                    EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                stats::TraitSet({ std::make_pair(stats::Traits::Mana, 10),
                                  std::make_pair(stats::Traits::MagicEffect, 8),
                                  std::make_pair(stats::Traits::MagicResist, 16),
                                  std::make_pair(stats::Traits::PoisonOnAll, 33) }));
        }
        else if (E == item::misc_type::Mummy_Hand)
        {
            return Enchantment(
                static_cast<EnchantmentType::Enum>(
                    EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                stats::TraitSet({ std::make_pair(stats::Traits::Mana, 20),
                                  std::make_pair(stats::Traits::MagicEffect, 16),
                                  std::make_pair(stats::Traits::MagicResist, 16),
                                  std::make_pair(stats::Traits::PanicOnSpell, 33),
                                  std::make_pair(stats::Traits::CurseEffect, 33),
                                  std::make_pair(stats::Traits::CurseResist, 33) }));
        }
        else if (E == item::misc_type::Icicle)
        {
            return Enchantment(
                static_cast<EnchantmentType::Enum>(
                    EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                stats::TraitSet({ std::make_pair(stats::Traits::Mana, 18),
                                  std::make_pair(stats::Traits::MagicEffect, 18),
                                  std::make_pair(stats::Traits::MagicCast, 13),
                                  std::make_pair(stats::Traits::FrostDamage, 75),
                                  std::make_pair(stats::Traits::FrostResist, 75) }));
        }
        else if (E == item::misc_type::Finger)
        {
            return Enchantment(
                static_cast<EnchantmentType::Enum>(
                    EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                stats::TraitSet({ std::make_pair(stats::Traits::Mana, 24),
                                  std::make_pair(stats::Traits::MagicEffect, 13),
                                  std::make_pair(stats::Traits::MagicCast, 18),
                                  std::make_pair(stats::Traits::FireResist, 50),
                                  std::make_pair(stats::Traits::FrostResist, 50),
                                  std::make_pair(stats::Traits::HonorResist, 50),
                                  std::make_pair(stats::Traits::ShadowResist, 50) }));
        }
        else if (E == item::misc_type::Unicorn_Horn)
        {
            return Enchantment(
                static_cast<EnchantmentType::Enum>(
                    EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem
                    | EnchantmentType::BlessWithoutItem),
                stats::TraitSet({ std::make_pair(stats::Traits::Charm, 16),
                                  std::make_pair(stats::Traits::Luck, 13),
                                  std::make_pair(stats::Traits::FindMagic, 26),
                                  std::make_pair(stats::Traits::Mana, 28),
                                  std::make_pair(stats::Traits::MagicEffect, 28),
                                  std::make_pair(stats::Traits::MagicCast, 14),
                                  std::make_pair(stats::Traits::BlessCast, 80),
                                  std::make_pair(stats::Traits::BlessEffect, 80),
                                  std::make_pair(stats::Traits::ShadowResist, 28) }));
        }
        else if (E == item::misc_type::Devil_Horn)
        {
            return Enchantment(
                static_cast<EnchantmentType::Enum>(
                    EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem
                    | EnchantmentType::CurseWithoutItem),
                stats::TraitSet({ std::make_pair(stats::Traits::Charm, -13),
                                  std::make_pair(stats::Traits::Speed, 13),
                                  std::make_pair(stats::Traits::Intelligence, 16),
                                  std::make_pair(stats::Traits::FindMagic, 26),
                                  std::make_pair(stats::Traits::Mana, 28),
                                  std::make_pair(stats::Traits::MagicEffect, 28),
                                  std::make_pair(stats::Traits::MagicCast, 14),
                                  std::make_pair(stats::Traits::CurseCast, 80),
                                  std::make_pair(stats::Traits::CurseEffect, 80),
                                  std::make_pair(stats::Traits::ShadowDamage, 28) }));
        }
        else if (E == item::misc_type::Recorder)
        {
            return Enchantment(
                static_cast<EnchantmentType::Enum>(
                    EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem
                    | EnchantmentType::BlessWithoutItem),
                stats::TraitSet({ std::make_pair(stats::Traits::Mana, 24),
                                  std::make_pair(stats::Traits::Charm, 13),
                                  std::make_pair(stats::Traits::Intelligence, 13),
                                  std::make_pair(stats::Traits::MagicCast, 16),
                                  std::make_pair(stats::Traits::MagicEffect, 16),
                                  std::make_pair(stats::Traits::BlessCast, 25),
                                  std::make_pair(stats::Traits::BlessEffect, 25),
                                  std::make_pair(stats::Traits::CurseResist, 25),
                                  std::make_pair(stats::Traits::ShadowResist, 18) }));
        }
        else if (E == item::misc_type::Viol)
        {
            return Enchantment(
                static_cast<EnchantmentType::Enum>(
                    EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem
                    | EnchantmentType::CurseWithoutItem),
                stats::TraitSet({ std::make_pair(stats::Traits::Mana, 24),
                                  std::make_pair(stats::Traits::Charm, 13),
                                  std::make_pair(stats::Traits::Intelligence, 13),
                                  std::make_pair(stats::Traits::MagicCast, 16),
                                  std::make_pair(stats::Traits::MagicEffect, 16),
                                  std::make_pair(stats::Traits::CurseCast, 25),
                                  std::make_pair(stats::Traits::CurseEffect, 25),
                                  std::make_pair(stats::Traits::ShadowDamage, 18) }));
        }
        else if (E == item::misc_type::Pipe_And_Tabor)
        {
            return Enchantment(
                static_cast<EnchantmentType::Enum>(
                    EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem
                    | EnchantmentType::BlessWithoutItem),
                stats::TraitSet({ std::make_pair(stats::Traits::Mana, 28),
                                  std::make_pair(stats::Traits::Charm, 18),
                                  std::make_pair(stats::Traits::Intelligence, 18),
                                  std::make_pair(stats::Traits::MagicCast, 33),
                                  std::make_pair(stats::Traits::MagicEffect, 33),
                                  std::make_pair(stats::Traits::BlessCast, 50),
                                  std::make_pair(stats::Traits::BlessEffect, 50) }));
        }
        else if (E == item::misc_type::Hurdy_Gurdy)
        {
            return Enchantment(
                static_cast<EnchantmentType::Enum>(
                    EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem
                    | EnchantmentType::CurseWithoutItem),
                stats::TraitSet({ std::make_pair(stats::Traits::Mana, 28),
                                  std::make_pair(stats::Traits::Charm, 18),
                                  std::make_pair(stats::Traits::Intelligence, 18),
                                  std::make_pair(stats::Traits::MagicCast, 33),
                                  std::make_pair(stats::Traits::MagicEffect, 33),
                                  std::make_pair(stats::Traits::CurseCast, 50),
                                  std::make_pair(stats::Traits::CurseEffect, 50) }));
        }
        else if (E == item::misc_type::Lyre)
        {
            return Enchantment(
                static_cast<EnchantmentType::Enum>(
                    EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                stats::TraitSet({ std::make_pair(stats::Traits::Mana, 33),
                                  std::make_pair(stats::Traits::Charm, 20),
                                  std::make_pair(stats::Traits::Intelligence, 20),
                                  std::make_pair(stats::Traits::MagicCast, 50),
                                  std::make_pair(stats::Traits::MagicEffect, 50),
                                  std::make_pair(stats::Traits::FindMagic, 18),
                                  std::make_pair(stats::Traits::FindCoinsAmount, 18) }));
        }

        return Enchantment();
    }

    const EnchantmentPtr_t EnchantmentFactory::NewFromSetType(const item::set_type::Enum E) const
    {
        return new Enchantment(MakeFromSetType(E));
    }

    const Enchantment EnchantmentFactory::MakeFromSetType(const item::set_type::Enum E) const
    {
        switch (E)
        {
            case item::set_type::TheAssassins:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Luck, 4),
                                      std::make_pair(stats::Traits::Speed, 4),
                                      std::make_pair(stats::Traits::Backstab, 2),
                                      std::make_pair(stats::Traits::FindMagic, 2) }));
            }
            case item::set_type::TheNeverwinter:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Luck, 8),
                                      std::make_pair(stats::Traits::Speed, 6),
                                      std::make_pair(stats::Traits::Backstab, 4),
                                      std::make_pair(stats::Traits::FindMagic, 3) }));
            }
            case item::set_type::TheTickler:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Luck, 13),
                                      std::make_pair(stats::Traits::Speed, 8),
                                      std::make_pair(stats::Traits::Backstab, 6),
                                      std::make_pair(stats::Traits::FindMagic, 4) }));
            }
            case item::set_type::TheMagus:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Intelligence, 4),
                                      std::make_pair(stats::Traits::Mana, 4),
                                      std::make_pair(stats::Traits::MagicCast, 2),
                                      std::make_pair(stats::Traits::MagicEffect, 2),
                                      std::make_pair(stats::Traits::MagicResist, 2),
                                      std::make_pair(stats::Traits::ShadowDamage, 2) }));
            }
            case item::set_type::TheNecromancers:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Intelligence, 8),
                                      std::make_pair(stats::Traits::Mana, 6),
                                      std::make_pair(stats::Traits::MagicCast, 4),
                                      std::make_pair(stats::Traits::MagicEffect, 4),
                                      std::make_pair(stats::Traits::MagicResist, 4),
                                      std::make_pair(stats::Traits::ShadowDamage, 4) }));
            }
            case item::set_type::TheWarlocks:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Intelligence, 16),
                                      std::make_pair(stats::Traits::Mana, 8),
                                      std::make_pair(stats::Traits::MagicCast, 8),
                                      std::make_pair(stats::Traits::MagicEffect, 8),
                                      std::make_pair(stats::Traits::MagicResist, 8),
                                      std::make_pair(stats::Traits::ShadowDamage, 8) }));
            }
            case item::set_type::TheLichKings:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem
                        | EnchantmentType::CurseWithoutItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Intelligence, 20),
                                      std::make_pair(stats::Traits::Speed, 8),
                                      std::make_pair(stats::Traits::Mana, 10),
                                      std::make_pair(stats::Traits::MagicCast, 13),
                                      std::make_pair(stats::Traits::MagicEffect, 13),
                                      std::make_pair(stats::Traits::MagicResist, 13),
                                      std::make_pair(stats::Traits::FireDamage, 13),
                                      std::make_pair(stats::Traits::FireResist, 13),
                                      std::make_pair(stats::Traits::ShadowDamage, 13),
                                      std::make_pair(stats::Traits::ShadowDamage, 13),
                                      std::make_pair(stats::Traits::HonorResist, 13),
                                      std::make_pair(stats::Traits::CurseCast, 13),
                                      std::make_pair(stats::Traits::CurseEffect, 13),
                                      std::make_pair(stats::Traits::CurseResist, 13),
                                      std::make_pair(stats::Traits::FindMagic, 10),
                                      std::make_pair(stats::Traits::FindReligious, 5) }));
            }
            case item::set_type::TheSages:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Charm, 4),
                                      std::make_pair(stats::Traits::Mana, 4),
                                      std::make_pair(stats::Traits::MagicCast, 2),
                                      std::make_pair(stats::Traits::MagicEffect, 2),
                                      std::make_pair(stats::Traits::MagicResist, 2),
                                      std::make_pair(stats::Traits::ShadowResist, 2) }));
            }
            case item::set_type::TheShamans:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Charm, 8),
                                      std::make_pair(stats::Traits::Mana, 6),
                                      std::make_pair(stats::Traits::MagicCast, 4),
                                      std::make_pair(stats::Traits::MagicEffect, 4),
                                      std::make_pair(stats::Traits::MagicResist, 4),
                                      std::make_pair(stats::Traits::ShadowResist, 4) }));
            }
            case item::set_type::TheOracles:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Charm, 16),
                                      std::make_pair(stats::Traits::Mana, 8),
                                      std::make_pair(stats::Traits::MagicCast, 8),
                                      std::make_pair(stats::Traits::MagicEffect, 8),
                                      std::make_pair(stats::Traits::MagicResist, 8),
                                      std::make_pair(stats::Traits::ShadowResist, 8) }));
            }
            case item::set_type::TheAngelic:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem
                        | EnchantmentType::BlessWithoutItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Charm, 20),
                                      std::make_pair(stats::Traits::Speed, 8),
                                      std::make_pair(stats::Traits::Mana, 10),
                                      std::make_pair(stats::Traits::MagicCast, 13),
                                      std::make_pair(stats::Traits::MagicEffect, 13),
                                      std::make_pair(stats::Traits::MagicResist, 13),
                                      std::make_pair(stats::Traits::BlessCast, 13),
                                      std::make_pair(stats::Traits::BlessEffect, 13),
                                      std::make_pair(stats::Traits::CurseResist, 13),
                                      std::make_pair(stats::Traits::AnimalResist, 13),
                                      std::make_pair(stats::Traits::FindReligious, 10),
                                      std::make_pair(stats::Traits::FindMagic, 5),
                                      std::make_pair(stats::Traits::FireResist, 13),
                                      std::make_pair(stats::Traits::FrostResist, 13),
                                      std::make_pair(stats::Traits::HonorResist, 13),
                                      std::make_pair(stats::Traits::ShadowResist, 13) }));
            }
            case item::set_type::TheBalladeers:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Charm, 4),
                                      std::make_pair(stats::Traits::Intelligence, 4),
                                      std::make_pair(stats::Traits::Mana, 4),
                                      std::make_pair(stats::Traits::MagicCast, 2),
                                      std::make_pair(stats::Traits::MagicEffect, 2),
                                      std::make_pair(stats::Traits::MagicResist, 2) }));
            }
            case item::set_type::TheTroubadours:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Charm, 8),
                                      std::make_pair(stats::Traits::Intelligence, 8),
                                      std::make_pair(stats::Traits::Mana, 6),
                                      std::make_pair(stats::Traits::MagicCast, 5),
                                      std::make_pair(stats::Traits::MagicEffect, 5),
                                      std::make_pair(stats::Traits::MagicResist, 5) }));
            }
            case item::set_type::TheMuses:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Charm, 13),
                                      std::make_pair(stats::Traits::Intelligence, 13),
                                      std::make_pair(stats::Traits::Mana, 8),
                                      std::make_pair(stats::Traits::MagicCast, 10),
                                      std::make_pair(stats::Traits::MagicEffect, 10),
                                      std::make_pair(stats::Traits::MagicResist, 10) }));
            }
            case item::set_type::TheCavaliers:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 2),
                                      std::make_pair(stats::Traits::Speed, 2),
                                      std::make_pair(stats::Traits::HitPower, 1) }));
            }
            case item::set_type::TheChampions:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 4),
                                      std::make_pair(stats::Traits::Speed, 3),
                                      std::make_pair(stats::Traits::HitPower, 1),
                                      std::make_pair(stats::Traits::HitCritical, 1),
                                      std::make_pair(stats::Traits::MagicResist, 1) }));
            }
            case item::set_type::ThePaladins:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 6),
                                      std::make_pair(stats::Traits::Speed, 4),
                                      std::make_pair(stats::Traits::HitPower, 2),
                                      std::make_pair(stats::Traits::HitCritical, 1),
                                      std::make_pair(stats::Traits::MagicResist, 2) }));
            }
            case item::set_type::TheBerserkers:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 8),
                                      std::make_pair(stats::Traits::Speed, 5),
                                      std::make_pair(stats::Traits::HitPower, 2),
                                      std::make_pair(stats::Traits::HitCritical, 2),
                                      std::make_pair(stats::Traits::MagicResist, 3) }));
            }
            case item::set_type::TheRosewood:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 10),
                                      std::make_pair(stats::Traits::Speed, 6),
                                      std::make_pair(stats::Traits::HitPower, 3),
                                      std::make_pair(stats::Traits::HitCritical, 2),
                                      std::make_pair(stats::Traits::MagicResist, 4) }));
            }
            case item::set_type::TheDragonslayers:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 13),
                                      std::make_pair(stats::Traits::Speed, 8),
                                      std::make_pair(stats::Traits::HitPower, 3),
                                      std::make_pair(stats::Traits::HitCritical, 3),
                                      std::make_pair(stats::Traits::MagicResist, 5) }));
            }
            case item::set_type::TheEventideRider:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 10),
                                      std::make_pair(stats::Traits::Speed, 5),
                                      std::make_pair(stats::Traits::HitPower, 4),
                                      std::make_pair(stats::Traits::HitCritical, 2),
                                      std::make_pair(stats::Traits::MagicResist, 3),
                                      std::make_pair(stats::Traits::CurseResist, 10),
                                      std::make_pair(stats::Traits::CurseOnDamage, 4),
                                      std::make_pair(stats::Traits::ShadowDamage, 2) }));
            }
            case item::set_type::TheHunters:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Accuracy, 8),
                                      std::make_pair(stats::Traits::Strength, 4),
                                      std::make_pair(stats::Traits::Speed, 4),
                                      std::make_pair(stats::Traits::MagicResist, 1),
                                      std::make_pair(stats::Traits::HitCritical, 1) }));
            }
            case item::set_type::TheSureShot:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Accuracy, 10),
                                      std::make_pair(stats::Traits::Strength, 6),
                                      std::make_pair(stats::Traits::Speed, 6),
                                      std::make_pair(stats::Traits::MagicResist, 2),
                                      std::make_pair(stats::Traits::HitCritical, 2) }));
            }
            case item::set_type::TheMarksmans:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Accuracy, 13),
                                      std::make_pair(stats::Traits::Strength, 10),
                                      std::make_pair(stats::Traits::Speed, 10),
                                      std::make_pair(stats::Traits::MagicResist, 3),
                                      std::make_pair(stats::Traits::HitCritical, 3) }));
            }
            case item::set_type::TheDeadeye:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Accuracy, 16),
                                      std::make_pair(stats::Traits::Strength, 13),
                                      std::make_pair(stats::Traits::Speed, 13),
                                      std::make_pair(stats::Traits::MagicResist, 4),
                                      std::make_pair(stats::Traits::HitCritical, 4) }));
            }
            case item::set_type::TheDuskRanger:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Accuracy, 12),
                                      std::make_pair(stats::Traits::Strength, 6),
                                      std::make_pair(stats::Traits::Speed, 6),
                                      std::make_pair(stats::Traits::MagicResist, 1),
                                      std::make_pair(stats::Traits::HitCritical, 5),
                                      std::make_pair(stats::Traits::CurseResist, 8),
                                      std::make_pair(stats::Traits::CurseOnDamage, 8),
                                      std::make_pair(stats::Traits::ShadowDamage, 3) }));
            }
            case item::set_type::TheVenomBow:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Accuracy, 12),
                                      std::make_pair(stats::Traits::Strength, 6),
                                      std::make_pair(stats::Traits::Speed, 6),
                                      std::make_pair(stats::Traits::MagicResist, 1),
                                      std::make_pair(stats::Traits::HitCritical, 3),
                                      std::make_pair(stats::Traits::PoisonOnAll, 8) }));
            }
            case item::set_type::TheCritterChannelers:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 2),
                                      std::make_pair(stats::Traits::Accuracy, 1),
                                      std::make_pair(stats::Traits::Speed, 1),
                                      std::make_pair(stats::Traits::Intelligence, 2),
                                      std::make_pair(stats::Traits::AnimalResist, 5),
                                      std::make_pair(stats::Traits::MagicResist, 1) }));
            }
            case item::set_type::TheMammalianHead:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 3),
                                      std::make_pair(stats::Traits::Accuracy, 2),
                                      std::make_pair(stats::Traits::Speed, 2),
                                      std::make_pair(stats::Traits::Intelligence, 3),
                                      std::make_pair(stats::Traits::AnimalResist, 6),
                                      std::make_pair(stats::Traits::MagicResist, 1) }));
            }
            case item::set_type::TheSavageTaskmasters:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 4),
                                      std::make_pair(stats::Traits::Accuracy, 3),
                                      std::make_pair(stats::Traits::Speed, 3),
                                      std::make_pair(stats::Traits::Intelligence, 4),
                                      std::make_pair(stats::Traits::AnimalResist, 7),
                                      std::make_pair(stats::Traits::MagicResist, 2) }));
            }
            case item::set_type::TheMonsterOverseers:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 5),
                                      std::make_pair(stats::Traits::Accuracy, 4),
                                      std::make_pair(stats::Traits::Speed, 4),
                                      std::make_pair(stats::Traits::Intelligence, 5),
                                      std::make_pair(stats::Traits::AnimalResist, 8),
                                      std::make_pair(stats::Traits::MagicResist, 3) }));
            }
            case item::set_type::TheBeastRulers:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 6),
                                      std::make_pair(stats::Traits::Accuracy, 5),
                                      std::make_pair(stats::Traits::Speed, 5),
                                      std::make_pair(stats::Traits::Intelligence, 6),
                                      std::make_pair(stats::Traits::AnimalResist, 9),
                                      std::make_pair(stats::Traits::MagicResist, 4) }));
            }
            case item::set_type::NotASet:
            case item::set_type::Count:
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
        return new Enchantment(MakeFromSetCompleteType(E));
    }

    const Enchantment
        EnchantmentFactory::MakeFromSetCompleteType(const item::set_type::Enum E) const
    {
        switch (E)
        {
            case item::set_type::TheAssassins:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Luck, 20),
                                      std::make_pair(stats::Traits::Speed, 20),
                                      std::make_pair(stats::Traits::Backstab, 5),
                                      std::make_pair(stats::Traits::FindMagic, 10),
                                      std::make_pair(stats::Traits::FindCoinsAmount, 13) }));
            }
            case item::set_type::TheNeverwinter:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Luck, 33),
                                      std::make_pair(stats::Traits::Speed, 33),
                                      std::make_pair(stats::Traits::Backstab, 10),
                                      std::make_pair(stats::Traits::FindMagic, 13),
                                      std::make_pair(stats::Traits::FindCoinsAmount, 18) }));
            }
            case item::set_type::TheTickler:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Luck, 50),
                                      std::make_pair(stats::Traits::Speed, 50),
                                      std::make_pair(stats::Traits::Backstab, 15),
                                      std::make_pair(stats::Traits::FindMagic, 18),
                                      std::make_pair(stats::Traits::FindCoinsAmount, 24) }));
            }
            case item::set_type::TheMagus:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Intelligence, 20),
                                      std::make_pair(stats::Traits::MagicCast, 8),
                                      std::make_pair(stats::Traits::MagicEffect, 8),
                                      std::make_pair(stats::Traits::MagicResist, 8),
                                      std::make_pair(stats::Traits::ShadowDamage, 10),
                                      std::make_pair(stats::Traits::Mana, 18) }));
            }
            case item::set_type::TheNecromancers:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Intelligence, 33),
                                      std::make_pair(stats::Traits::MagicCast, 16),
                                      std::make_pair(stats::Traits::MagicEffect, 16),
                                      std::make_pair(stats::Traits::MagicResist, 16),
                                      std::make_pair(stats::Traits::ShadowDamage, 20),
                                      std::make_pair(stats::Traits::Mana, 24) }));
            }
            case item::set_type::TheWarlocks:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Intelligence, 50),
                                      std::make_pair(stats::Traits::MagicCast, 33),
                                      std::make_pair(stats::Traits::MagicEffect, 33),
                                      std::make_pair(stats::Traits::MagicResist, 33),
                                      std::make_pair(stats::Traits::ShadowDamage, 16),
                                      std::make_pair(stats::Traits::Mana, 16),
                                      std::make_pair(stats::Traits::CurseCast, 28),
                                      std::make_pair(stats::Traits::CurseEffect, 28) }));
            }
            case item::set_type::TheLichKings:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem
                        | EnchantmentType::CurseWithoutItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Intelligence, 50),
                                      std::make_pair(stats::Traits::Speed, 20),
                                      std::make_pair(stats::Traits::Mana, 33),
                                      std::make_pair(stats::Traits::MagicCast, 20),
                                      std::make_pair(stats::Traits::MagicEffect, 20),
                                      std::make_pair(stats::Traits::MagicResist, 20),
                                      std::make_pair(stats::Traits::FireDamage, 22),
                                      std::make_pair(stats::Traits::FireResist, 22),
                                      std::make_pair(stats::Traits::ShadowDamage, 22),
                                      std::make_pair(stats::Traits::ShadowDamage, 22),
                                      std::make_pair(stats::Traits::HonorResist, 22),
                                      std::make_pair(stats::Traits::CurseCast, 20),
                                      std::make_pair(stats::Traits::CurseEffect, 20),
                                      std::make_pair(stats::Traits::CurseResist, 20),
                                      std::make_pair(stats::Traits::FindMagic, 20),
                                      std::make_pair(stats::Traits::FindReligious, 10) }));
            }
            case item::set_type::TheSages:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Charm, 20),
                                      std::make_pair(stats::Traits::MagicCast, 8),
                                      std::make_pair(stats::Traits::MagicEffect, 8),
                                      std::make_pair(stats::Traits::MagicResist, 8),
                                      std::make_pair(stats::Traits::BlessCast, 8),
                                      std::make_pair(stats::Traits::BlessEffect, 8),
                                      std::make_pair(stats::Traits::CurseResist, 8),
                                      std::make_pair(stats::Traits::Mana, 18),
                                      std::make_pair(stats::Traits::ShadowResist, 10) }));
            }
            case item::set_type::TheShamans:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Charm, 33),
                                      std::make_pair(stats::Traits::MagicCast, 16),
                                      std::make_pair(stats::Traits::MagicEffect, 16),
                                      std::make_pair(stats::Traits::MagicResist, 16),
                                      std::make_pair(stats::Traits::BlessCast, 13),
                                      std::make_pair(stats::Traits::BlessEffect, 13),
                                      std::make_pair(stats::Traits::CurseResist, 16),
                                      std::make_pair(stats::Traits::Mana, 24),
                                      std::make_pair(stats::Traits::ShadowResist, 20) }));
            }
            case item::set_type::TheOracles:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Charm, 50),
                                      std::make_pair(stats::Traits::MagicCast, 33),
                                      std::make_pair(stats::Traits::MagicEffect, 33),
                                      std::make_pair(stats::Traits::MagicResist, 33),
                                      std::make_pair(stats::Traits::BlessCast, 18),
                                      std::make_pair(stats::Traits::BlessEffect, 18),
                                      std::make_pair(stats::Traits::CurseResist, 33),
                                      std::make_pair(stats::Traits::Mana, 28),
                                      std::make_pair(stats::Traits::ShadowResist, 30) }));
            }
            case item::set_type::TheAngelic:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem
                        | EnchantmentType::BlessWithoutItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Charm, 50),
                                      std::make_pair(stats::Traits::Speed, 20),
                                      std::make_pair(stats::Traits::Mana, 33),
                                      std::make_pair(stats::Traits::MagicCast, 20),
                                      std::make_pair(stats::Traits::MagicEffect, 20),
                                      std::make_pair(stats::Traits::MagicResist, 20),
                                      std::make_pair(stats::Traits::BlessCast, 22),
                                      std::make_pair(stats::Traits::BlessEffect, 22),
                                      std::make_pair(stats::Traits::CurseResist, 22),
                                      std::make_pair(stats::Traits::AnimalResist, 12),
                                      std::make_pair(stats::Traits::FindReligious, 20),
                                      std::make_pair(stats::Traits::FindMagic, 20),
                                      std::make_pair(stats::Traits::FireResist, 20),
                                      std::make_pair(stats::Traits::FrostResist, 20),
                                      std::make_pair(stats::Traits::HonorResist, 20),
                                      std::make_pair(stats::Traits::ShadowResist, 20) }));
            }
            case item::set_type::TheBalladeers:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Charm, 20),
                                      std::make_pair(stats::Traits::Intelligence, 20),
                                      std::make_pair(stats::Traits::MagicCast, 8),
                                      std::make_pair(stats::Traits::MagicEffect, 8),
                                      std::make_pair(stats::Traits::MagicResist, 8),
                                      std::make_pair(stats::Traits::Mana, 18) }));
            }
            case item::set_type::TheTroubadours:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Charm, 33),
                                      std::make_pair(stats::Traits::Intelligence, 33),
                                      std::make_pair(stats::Traits::MagicCast, 16),
                                      std::make_pair(stats::Traits::MagicEffect, 16),
                                      std::make_pair(stats::Traits::MagicResist, 16),
                                      std::make_pair(stats::Traits::Mana, 24) }));
            }
            case item::set_type::TheMuses:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Charm, 50),
                                      std::make_pair(stats::Traits::Intelligence, 50),
                                      std::make_pair(stats::Traits::MagicCast, 28),
                                      std::make_pair(stats::Traits::MagicEffect, 28),
                                      std::make_pair(stats::Traits::MagicResist, 28),
                                      std::make_pair(stats::Traits::Mana, 28) }));
            }
            case item::set_type::TheCavaliers:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 20),
                                      std::make_pair(stats::Traits::Speed, 13),
                                      std::make_pair(stats::Traits::MagicResist, 6),
                                      std::make_pair(stats::Traits::CurseResist, 10),
                                      std::make_pair(stats::Traits::HitCritical, 2),
                                      std::make_pair(stats::Traits::HitPower, 5) }));
            }
            case item::set_type::TheChampions:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 33),
                                      std::make_pair(stats::Traits::Speed, 18),
                                      std::make_pair(stats::Traits::MagicResist, 8),
                                      std::make_pair(stats::Traits::CurseResist, 20),
                                      std::make_pair(stats::Traits::HitCritical, 3),
                                      std::make_pair(stats::Traits::HitPower, 6) }));
            }
            case item::set_type::ThePaladins:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 50),
                                      std::make_pair(stats::Traits::Speed, 26),
                                      std::make_pair(stats::Traits::MagicResist, 10),
                                      std::make_pair(stats::Traits::CurseResist, 30),
                                      std::make_pair(stats::Traits::HitCritical, 4),
                                      std::make_pair(stats::Traits::HitPower, 7) }));
            }
            case item::set_type::TheBerserkers:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 70),
                                      std::make_pair(stats::Traits::Speed, 33),
                                      std::make_pair(stats::Traits::MagicResist, 12),
                                      std::make_pair(stats::Traits::CurseResist, 40),
                                      std::make_pair(stats::Traits::HitCritical, 5),
                                      std::make_pair(stats::Traits::HitPower, 8) }));
            }
            case item::set_type::TheRosewood:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 100),
                                      std::make_pair(stats::Traits::Speed, 50),
                                      std::make_pair(stats::Traits::MagicResist, 14),
                                      std::make_pair(stats::Traits::CurseResist, 50),
                                      std::make_pair(stats::Traits::HitCritical, 6),
                                      std::make_pair(stats::Traits::HitPower, 9) }));
            }
            case item::set_type::TheDragonslayers:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 133),
                                      std::make_pair(stats::Traits::Speed, 70),
                                      std::make_pair(stats::Traits::MagicResist, 16),
                                      std::make_pair(stats::Traits::CurseResist, 60),
                                      std::make_pair(stats::Traits::HitCritical, 7),
                                      std::make_pair(stats::Traits::HitPower, 10) }));
            }
            case item::set_type::TheEventideRider:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 133),
                                      std::make_pair(stats::Traits::Speed, 70),
                                      std::make_pair(stats::Traits::MagicResist, 16),
                                      std::make_pair(stats::Traits::CurseResist, 20),
                                      std::make_pair(stats::Traits::HitCritical, 7),
                                      std::make_pair(stats::Traits::HitPower, 10),
                                      std::make_pair(stats::Traits::CurseOnDamage, 26),
                                      std::make_pair(stats::Traits::ShadowDamage, 14) }));
            }
            case item::set_type::TheHunters:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Accuracy, 20),
                                      std::make_pair(stats::Traits::Strength, 15),
                                      std::make_pair(stats::Traits::Speed, 15),
                                      std::make_pair(stats::Traits::MagicResist, 2),
                                      std::make_pair(stats::Traits::HitCritical, 4),
                                      std::make_pair(stats::Traits::HitPower, 2) }));
            }
            case item::set_type::TheSureShot:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Accuracy, 33),
                                      std::make_pair(stats::Traits::Strength, 28),
                                      std::make_pair(stats::Traits::Speed, 28),
                                      std::make_pair(stats::Traits::MagicResist, 4),
                                      std::make_pair(stats::Traits::HitCritical, 6),
                                      std::make_pair(stats::Traits::HitPower, 4) }));
            }
            case item::set_type::TheMarksmans:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Accuracy, 50),
                                      std::make_pair(stats::Traits::Strength, 36),
                                      std::make_pair(stats::Traits::Speed, 36),
                                      std::make_pair(stats::Traits::MagicResist, 8),
                                      std::make_pair(stats::Traits::HitCritical, 8),
                                      std::make_pair(stats::Traits::HitPower, 6) }));
            }
            case item::set_type::TheDeadeye:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Accuracy, 70),
                                      std::make_pair(stats::Traits::Strength, 50),
                                      std::make_pair(stats::Traits::Speed, 50),
                                      std::make_pair(stats::Traits::MagicResist, 16),
                                      std::make_pair(stats::Traits::HitCritical, 13),
                                      std::make_pair(stats::Traits::HitPower, 8) }));
            }
            case item::set_type::TheDuskRanger:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Accuracy, 50),
                                      std::make_pair(stats::Traits::Strength, 50),
                                      std::make_pair(stats::Traits::Speed, 50),
                                      std::make_pair(stats::Traits::MagicResist, 8),
                                      std::make_pair(stats::Traits::HitCritical, 20),
                                      std::make_pair(stats::Traits::CurseResist, 50),
                                      std::make_pair(stats::Traits::CurseOnDamage, 33),
                                      std::make_pair(stats::Traits::ShadowDamage, 18) }));
            }
            case item::set_type::TheVenomBow:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Accuracy, 50),
                                      std::make_pair(stats::Traits::Strength, 50),
                                      std::make_pair(stats::Traits::Speed, 50),
                                      std::make_pair(stats::Traits::MagicResist, 8),
                                      std::make_pair(stats::Traits::HitCritical, 20),
                                      std::make_pair(stats::Traits::PoisonOnAll, 33) }));
            }
            case item::set_type::TheCritterChannelers:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 10),
                                      std::make_pair(stats::Traits::Accuracy, 8),
                                      std::make_pair(stats::Traits::Speed, 8),
                                      std::make_pair(stats::Traits::Intelligence, 8),
                                      std::make_pair(stats::Traits::AnimalResist, 8),
                                      std::make_pair(stats::Traits::MagicResist, 2) }));
            }
            case item::set_type::TheMammalianHead:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 15),
                                      std::make_pair(stats::Traits::Accuracy, 10),
                                      std::make_pair(stats::Traits::Speed, 10),
                                      std::make_pair(stats::Traits::Intelligence, 10),
                                      std::make_pair(stats::Traits::AnimalResist, 13),
                                      std::make_pair(stats::Traits::MagicResist, 2) }));
            }
            case item::set_type::TheSavageTaskmasters:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 20),
                                      std::make_pair(stats::Traits::Accuracy, 13),
                                      std::make_pair(stats::Traits::Speed, 13),
                                      std::make_pair(stats::Traits::Intelligence, 13),
                                      std::make_pair(stats::Traits::AnimalResist, 18),
                                      std::make_pair(stats::Traits::MagicResist, 4) }));
            }
            case item::set_type::TheMonsterOverseers:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 25),
                                      std::make_pair(stats::Traits::Accuracy, 18),
                                      std::make_pair(stats::Traits::Speed, 18),
                                      std::make_pair(stats::Traits::Intelligence, 18),
                                      std::make_pair(stats::Traits::AnimalResist, 24),
                                      std::make_pair(stats::Traits::MagicResist, 8) }));
            }
            case item::set_type::TheBeastRulers:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 30),
                                      std::make_pair(stats::Traits::Accuracy, 24),
                                      std::make_pair(stats::Traits::Speed, 24),
                                      std::make_pair(stats::Traits::Intelligence, 24),
                                      std::make_pair(stats::Traits::AnimalResist, 28),
                                      std::make_pair(stats::Traits::MagicResist, 16) }));
            }
            case item::set_type::NotASet:
            case item::set_type::Count:
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
        const item::material::Enum MATERIAL_PRIMARY) const
    {
        return new Enchantment(MakeFromElementType(E, IS_WEAPON, MATERIAL_PRIMARY));
    }

    const Enchantment EnchantmentFactory::MakeFromElementType(
        const item::element_type::Enum E,
        const bool IS_WEAPON,
        const item::material::Enum MATERIAL_PRIMARY) const
    {
        using namespace item;

        if (E == element_type::None)
        {
            std::ostringstream ss;
            ss << "creature::EnchantmentFactory::NewFromElementType(element_type=" << E
               << ", is_weapon=" << std::boolalpha << IS_WEAPON << ", mat_pri=" << MATERIAL_PRIMARY
               << ") was given an element_type of NONE.";

            throw std::runtime_error(ss.str());
        }

        stats::TraitSet traits;

        if ((MATERIAL_PRIMARY == material::Nothing) || (MATERIAL_PRIMARY == material::Count))
        {
            if (IS_WEAPON)
            {
                auto const DAMAGE{ 50 };

                if (E & element_type::Fire)
                {
                    traits.Get(stats::Traits::FireDamage).CurrentSet(DAMAGE);
                }

                if (E & element_type::Frost)
                {
                    traits.Get(stats::Traits::FrostDamage).CurrentSet(DAMAGE);
                }

                if (E & element_type::Honor)
                {
                    traits.Get(stats::Traits::HonorDamage).CurrentSet(DAMAGE);
                }

                if (E & element_type::Shadow)
                {
                    traits.Get(stats::Traits::ShadowDamage).CurrentSet(DAMAGE);
                }
            }
            else
            {
                auto const RESISTANCE{ 13 };

                if (E & element_type::Fire)
                {
                    traits.Get(stats::Traits::FireResist).CurrentSet(RESISTANCE);
                }

                if (E & element_type::Frost)
                {
                    traits.Get(stats::Traits::FrostResist).CurrentSet(RESISTANCE);
                }

                if (E & element_type::Honor)
                {
                    traits.Get(stats::Traits::HonorResist).CurrentSet(RESISTANCE);
                }

                if (E & element_type::Shadow)
                {
                    traits.Get(stats::Traits::ShadowResist).CurrentSet(RESISTANCE);
                }
            }
        }
        else
        {
            auto const MAT_BONUS{ material::Bonus(MATERIAL_PRIMARY) };

            if (IS_WEAPON)
            {
                auto const DAMAGE_BASE{ 10 };
                auto const DAMAGE_MULT{ 5 };
                auto const DAMAGE{ DAMAGE_BASE + (MAT_BONUS * DAMAGE_MULT) };

                if (E & element_type::Fire)
                {
                    traits.Get(stats::Traits::FireDamage).CurrentSet(DAMAGE);
                }

                if (E & element_type::Frost)
                {
                    traits.Get(stats::Traits::FrostDamage).CurrentSet(DAMAGE);
                }

                if (E & element_type::Honor)
                {
                    traits.Get(stats::Traits::HonorDamage).CurrentSet(DAMAGE);
                }

                if (E & element_type::Shadow)
                {
                    traits.Get(stats::Traits::ShadowDamage).CurrentSet(DAMAGE);
                }
            }
            else
            {
                auto const RES_BASE{ 4.0f };
                auto const RES_MULT{ 1.0f / 1.5f };

                auto const RESISTANCE{ static_cast<int>(
                    RES_BASE + (static_cast<float>(MAT_BONUS) * RES_MULT)) };

                if (E & element_type::Fire)
                {
                    traits.Get(stats::Traits::FireResist).CurrentSet(RESISTANCE);
                }

                if (E & element_type::Frost)
                {
                    traits.Get(stats::Traits::FrostResist).CurrentSet(RESISTANCE);
                }

                if (E & element_type::Honor)
                {
                    traits.Get(stats::Traits::HonorResist).CurrentSet(RESISTANCE);
                }

                if (E & element_type::Shadow)
                {
                    traits.Get(stats::Traits::ShadowResist).CurrentSet(RESISTANCE);
                }
            }
        }

        return Enchantment(EnchantmentType::WhenEquipped, traits);
    }

    const EnchantmentPtr_t EnchantmentFactory::NewFromNamedType(
        const item::named_type::Enum NAMED_ENUM,
        const item::material::Enum MATERIAL_ENUM,
        const bool IS_WEAPON,
        const bool IS_ARMOR) const
    {
        return new Enchantment(MakeFromNamedType(NAMED_ENUM, MATERIAL_ENUM, IS_WEAPON, IS_ARMOR));
    }

    const Enchantment EnchantmentFactory::MakeFromNamedType(
        const item::named_type::Enum NAMED_ENUM,
        const item::material::Enum MATERIAL_ENUM,
        const bool IS_WEAPON,
        const bool IS_ARMOR) const
    {
        switch (NAMED_ENUM)
        {
            case item::named_type::Wicked:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 13),
                                      std::make_pair(stats::Traits::Luck, -18),
                                      std::make_pair(stats::Traits::Charm, -13),
                                      std::make_pair(stats::Traits::Intelligence, -13),
                                      std::make_pair(stats::Traits::CurseOnDamage, 3),
                                      std::make_pair(stats::Traits::HitCritical, 2),
                                      std::make_pair(stats::Traits::CurseResist, -13),
                                      std::make_pair(stats::Traits::Encounter, 1),
                                      std::make_pair(stats::Traits::Surprise, 1) }));
            }
            case item::named_type::Fiendish:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 16),
                                      std::make_pair(stats::Traits::Luck, -20),
                                      std::make_pair(stats::Traits::Charm, -18),
                                      std::make_pair(stats::Traits::Intelligence, -18),
                                      std::make_pair(stats::Traits::CurseOnDamage, 6),
                                      std::make_pair(stats::Traits::HitCritical, 4),
                                      std::make_pair(stats::Traits::CurseResist, -18),
                                      std::make_pair(stats::Traits::Encounter, 2),
                                      std::make_pair(stats::Traits::Surprise, 2) }));
            }
            case item::named_type::Infernal:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 18),
                                      std::make_pair(stats::Traits::Luck, -22),
                                      std::make_pair(stats::Traits::Charm, -24),
                                      std::make_pair(stats::Traits::Intelligence, -24),
                                      std::make_pair(stats::Traits::CurseOnDamage, 8),
                                      std::make_pair(stats::Traits::HitCritical, 6),
                                      std::make_pair(stats::Traits::CurseResist, -26),
                                      std::make_pair(stats::Traits::Encounter, 3),
                                      std::make_pair(stats::Traits::Surprise, 3) }));
            }
            case item::named_type::Raging:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 26),
                                      std::make_pair(stats::Traits::Luck, -24),
                                      std::make_pair(stats::Traits::Charm, -28),
                                      std::make_pair(stats::Traits::Intelligence, -28),
                                      std::make_pair(stats::Traits::CurseOnDamage, 10),
                                      std::make_pair(stats::Traits::HitCritical, 8),
                                      std::make_pair(stats::Traits::CurseResist, -33),
                                      std::make_pair(stats::Traits::Encounter, 4),
                                      std::make_pair(stats::Traits::Surprise, 4) }));
            }
            case item::named_type::Diabolic:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 33),
                                      std::make_pair(stats::Traits::Luck, -26),
                                      std::make_pair(stats::Traits::Charm, -33),
                                      std::make_pair(stats::Traits::Intelligence, -33),
                                      std::make_pair(stats::Traits::CurseOnDamage, 13),
                                      std::make_pair(stats::Traits::HitCritical, 10),
                                      std::make_pair(stats::Traits::CurseResist, -100),
                                      std::make_pair(stats::Traits::Encounter, 5),
                                      std::make_pair(stats::Traits::Surprise, 5) }));
            }
            case item::named_type::Dancing:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 18),
                                      std::make_pair(stats::Traits::Speed, 18),
                                      std::make_pair(stats::Traits::CurseEffect, 50),
                                      std::make_pair(stats::Traits::HitCritical, 8),
                                      std::make_pair(stats::Traits::ArmorRating, 16),
                                      std::make_pair(stats::Traits::Accuracy, 16) }));
            }
            case item::named_type::Marauder:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 16),
                                      std::make_pair(stats::Traits::Accuracy, 13),
                                      std::make_pair(stats::Traits::HitCritical, 5) }));
            }
            case item::named_type::Honest:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Accuracy, 1),
                                      std::make_pair(stats::Traits::ArmorRating, 1),
                                      std::make_pair(stats::Traits::BlessEffect, 4),
                                      std::make_pair(stats::Traits::ShadowResist, 8),
                                      std::make_pair(stats::Traits::HonorResist, 8) }));
            }
            case item::named_type::Noble:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Accuracy, 2),
                                      std::make_pair(stats::Traits::ArmorRating, 2),
                                      std::make_pair(stats::Traits::BlessEffect, 6),
                                      std::make_pair(stats::Traits::Surprise, -2),
                                      std::make_pair(stats::Traits::ShadowResist, 13),
                                      std::make_pair(stats::Traits::HonorResist, 13) }));
            }
            case item::named_type::Daring:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Accuracy, 3),
                                      std::make_pair(stats::Traits::ArmorRating, 3),
                                      std::make_pair(stats::Traits::BlessEffect, 8),
                                      std::make_pair(stats::Traits::Surprise, -2),
                                      std::make_pair(stats::Traits::Charm, 2),
                                      std::make_pair(stats::Traits::Intelligence, 2),
                                      std::make_pair(stats::Traits::ShadowResist, 18),
                                      std::make_pair(stats::Traits::HonorResist, 18) }));
            }
            case item::named_type::Elite:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Accuracy, 4),
                                      std::make_pair(stats::Traits::ArmorRating, 4),
                                      std::make_pair(stats::Traits::BlessEffect, 13),
                                      std::make_pair(stats::Traits::Surprise, -2),
                                      std::make_pair(stats::Traits::Charm, 3),
                                      std::make_pair(stats::Traits::Intelligence, 3),
                                      std::make_pair(stats::Traits::CurseResist, 8),
                                      std::make_pair(stats::Traits::ShadowResist, 24),
                                      std::make_pair(stats::Traits::HonorResist, 24) }));
            }
            case item::named_type::Valiant:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Accuracy, 5),
                                      std::make_pair(stats::Traits::ArmorRating, 5),
                                      std::make_pair(stats::Traits::BlessEffect, 16),
                                      std::make_pair(stats::Traits::Surprise, -2),
                                      std::make_pair(stats::Traits::Charm, 4),
                                      std::make_pair(stats::Traits::Intelligence, 4),
                                      std::make_pair(stats::Traits::CurseResist, 10),
                                      std::make_pair(stats::Traits::ShadowResist, 28),
                                      std::make_pair(stats::Traits::HonorResist, 28) }));
            }
            case item::named_type::Heros:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Accuracy, 6),
                                      std::make_pair(stats::Traits::ArmorRating, 6),
                                      std::make_pair(stats::Traits::BlessEffect, 18),
                                      std::make_pair(stats::Traits::Surprise, -2),
                                      std::make_pair(stats::Traits::Charm, 5),
                                      std::make_pair(stats::Traits::Intelligence, 5),
                                      std::make_pair(stats::Traits::CurseResist, 13),
                                      std::make_pair(stats::Traits::MagicResist, 4),
                                      std::make_pair(stats::Traits::ShadowResist, 33),
                                      std::make_pair(stats::Traits::HonorResist, 33) }));
            }
            case item::named_type::Army:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 3),
                                      std::make_pair(stats::Traits::Speed, 3),
                                      std::make_pair(stats::Traits::ArmorRating, 3) }));
            }
            case item::named_type::Gladiator:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 8),
                                      std::make_pair(stats::Traits::Speed, 6) }));
            }
            case item::named_type::Soldiers:
            {
                if (IS_WEAPON)
                {
                    return Enchantment(
                        static_cast<EnchantmentType::Enum>(
                            EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                        stats::TraitSet({ std::make_pair(stats::Traits::Strength, 18),
                                          std::make_pair(stats::Traits::Speed, 13),
                                          std::make_pair(stats::Traits::Charm, 8),
                                          std::make_pair(stats::Traits::HonorDamage, 22) }));
                }

                if (IS_ARMOR)
                {
                    return Enchantment(
                        static_cast<EnchantmentType::Enum>(
                            EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                        stats::TraitSet({ std::make_pair(stats::Traits::ArmorRating, 4) }));
                }

                std::ostringstream ss;
                ss << "creature::EnchantmentFactory::NewFromNamedType(named_type=Soldiers, "
                      "is_weapon=false, is_armor=false, mat_pri="
                   << MATERIAL_ENUM
                   << ") failed because for that named_type must be weapon or armor.";

                throw std::runtime_error(ss.str());
            }
            case item::named_type::Wardens:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 5),
                                      std::make_pair(stats::Traits::ArmorRating, 5),
                                      std::make_pair(stats::Traits::Surprise, -3) }));
            }
            case item::named_type::Princes:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 8),
                                      std::make_pair(stats::Traits::ArmorRating, 8) }));
            }
            case item::named_type::Ranger:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 6),
                                      std::make_pair(stats::Traits::Accuracy, 8),
                                      std::make_pair(stats::Traits::ArmorRating, 4) }));
            }
            case item::named_type::Samurai:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 10),
                                      std::make_pair(stats::Traits::Accuracy, 10),
                                      std::make_pair(stats::Traits::HitCritical, 2) }));
            }
            case item::named_type::Thors:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Strength, 12),
                                      std::make_pair(stats::Traits::ArmorRating, 12),
                                      std::make_pair(stats::Traits::MagicResist, 4),
                                      std::make_pair(stats::Traits::HitPower, 2) }));
            }
            case item::named_type::Nile:
            {
                if (IS_WEAPON)
                {
                    return Enchantment(
                        static_cast<EnchantmentType::Enum>(
                            EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                        stats::TraitSet({ std::make_pair(stats::Traits::Strength, 12),
                                          std::make_pair(stats::Traits::Accuracy, 12),
                                          std::make_pair(stats::Traits::ArmorRating, 8),
                                          std::make_pair(stats::Traits::CurseResist, 16),
                                          std::make_pair(stats::Traits::HitCritical, 1),
                                          std::make_pair(stats::Traits::PoisonOnMelee, 20) }));
                }

                if (IS_ARMOR)
                {
                    return Enchantment(
                        static_cast<EnchantmentType::Enum>(
                            EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                        stats::TraitSet({ std::make_pair(stats::Traits::Strength, 12),
                                          std::make_pair(stats::Traits::Accuracy, 12),
                                          std::make_pair(stats::Traits::ArmorRating, 8),
                                          std::make_pair(stats::Traits::CurseResist, 16),
                                          std::make_pair(stats::Traits::HitCritical, 1) }));
                }

                std::ostringstream ss;
                ss << "creature::EnchantmentFactory::NewFromNamedType(named_type=Nile, "
                      "is_weapon=false, is_armor=false, mat_pri="
                   << MATERIAL_ENUM
                   << ") failed because for that named_type must be weapon or armor.";

                throw std::runtime_error(ss.str());
            }
            case item::named_type::Searing:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::FireDamage, 16) }));
            }
            case item::named_type::Burning:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::FireDamage, 33) }));
            }
            case item::named_type::Fiery:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::FireDamage, 66) }));
            }
            case item::named_type::Charred:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::FireResist, 8) }));
            }
            case item::named_type::Icy:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::FrostDamage, 16) }));
            }
            case item::named_type::Winter:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::FrostDamage, 33) }));
            }
            case item::named_type::Frigid:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::FrostDamage, 66) }));
            }
            case item::named_type::Chill:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::FrostResist, 8) }));
            }
            case item::named_type::Frozen:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::FrostResist, 13) }));
            }
            case item::named_type::Arctic:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::FrostResist, 18) }));
            }
            case item::named_type::Proud:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::HonorDamage, 16) }));
            }
            case item::named_type::Glory:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::HonorDamage, 33) }));
            }
            case item::named_type::Pure:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::HonorDamage, 66) }));
            }
            case item::named_type::Light:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::HonorResist, 8) }));
            }
            case item::named_type::Dawn:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::HonorResist, 13) }));
            }
            case item::named_type::Sun:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::HonorResist, 18) }));
            }
            case item::named_type::Gloom:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::ShadowDamage, 16) }));
            }
            case item::named_type::Twilight:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::ShadowDamage, 33) }));
            }
            case item::named_type::Dusk:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::ShadowDamage, 66) }));
            }
            case item::named_type::Sorrow:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::ShadowResist, 20) }));
            }
            case item::named_type::Woe:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::ShadowResist, 20) }));
            }
            case item::named_type::Misery:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::ShadowResist, 30) }));
            }
            case item::named_type::Moon:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Charm, -18),
                                      std::make_pair(stats::Traits::ArmorRating, 18),
                                      std::make_pair(stats::Traits::ShadowResist, 33),
                                      std::make_pair(stats::Traits::CurseResist, 33) }));
            }
            case item::named_type::Dark:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::ShadowDamage, 66),
                                      std::make_pair(stats::Traits::CurseOnDamage, 8),
                                      std::make_pair(stats::Traits::HitCritical, 4),
                                      std::make_pair(stats::Traits::Strength, 18) }));
            }
            case item::named_type::Betrayer:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::ShadowDamage, 80),
                                      std::make_pair(stats::Traits::CurseOnDamage, 18),
                                      std::make_pair(stats::Traits::HitCritical, 8),
                                      std::make_pair(stats::Traits::Strength, 33),
                                      std::make_pair(stats::Traits::Luck, -33),
                                      std::make_pair(stats::Traits::CurseResist, 33) }));
            }
            case item::named_type::Imposters:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Luck, 10),
                                      std::make_pair(stats::Traits::Backstab, 3),
                                      std::make_pair(stats::Traits::FindCoinsAmount, 5),
                                      std::make_pair(stats::Traits::FindMagic, 5),
                                      std::make_pair(stats::Traits::Surprise, -5) }));
            }
            case item::named_type::Pickpocket:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Luck, 13),
                                      std::make_pair(stats::Traits::Backstab, 4),
                                      std::make_pair(stats::Traits::FindCoinsAmount, 8),
                                      std::make_pair(stats::Traits::FindMagic, 6),
                                      std::make_pair(stats::Traits::Surprise, -6) }));
            }
            case item::named_type::Burglar:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Luck, 18),
                                      std::make_pair(stats::Traits::Backstab, 5),
                                      std::make_pair(stats::Traits::FindCoinsAmount, 10),
                                      std::make_pair(stats::Traits::FindMagic, 8),
                                      std::make_pair(stats::Traits::Surprise, -8) }));
            }
            case item::named_type::Mountebank:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Luck, 21),
                                      std::make_pair(stats::Traits::Backstab, 6),
                                      std::make_pair(stats::Traits::FindCoinsAmount, 8),
                                      std::make_pair(stats::Traits::FindMagic, 4),
                                      std::make_pair(stats::Traits::Charm, 18) }));
            }
            case item::named_type::Charlatans:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Luck, 13),
                                      std::make_pair(stats::Traits::Backstab, 3),
                                      std::make_pair(stats::Traits::FindCoinsAmount, 6),
                                      std::make_pair(stats::Traits::FindMagic, 3),
                                      std::make_pair(stats::Traits::Charm, 24) }));
            }
            case item::named_type::Focus:
            {
                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Accuracy, 18),
                                      std::make_pair(stats::Traits::Strength, 13),
                                      std::make_pair(stats::Traits::DamageBonusProj, 16) }));
            }
            case item::named_type::Robbers:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_ENUM) };

                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Luck, (MAT_BONUS / 2)) }));
            }
            case item::named_type::Thugs:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_ENUM) };

                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Luck, 10 + (MAT_BONUS / 2)) }));
            }
            case item::named_type::Knaves:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_ENUM) };

                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Luck, 20 + (MAT_BONUS / 2)) }));
            }
            case item::named_type::Muggers:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_ENUM) };

                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet({ std::make_pair(stats::Traits::Backstab, (MAT_BONUS / 2)) }));
            }
            case item::named_type::Thief:
            {
                auto const MATERIAL_BASED_BONUS{ 5 + (item::material::Bonus(MATERIAL_ENUM) / 2) };

                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet(
                        { std::make_pair(stats::Traits::Luck, MATERIAL_BASED_BONUS),
                          std::make_pair(stats::Traits::Backstab, MATERIAL_BASED_BONUS) }));
            }
            case item::named_type::Pirate:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_ENUM) };

                return Enchantment(
                    static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped | EnchantmentType::BoundToItem),
                    stats::TraitSet(
                        { std::make_pair(stats::Traits::Backstab, 8 + (MAT_BONUS / 2)),
                          std::make_pair(stats::Traits::FindCoinsAmount, 8 + (MAT_BONUS / 2)),
                          std::make_pair(stats::Traits::Luck, (MAT_BONUS / 2)),
                          std::make_pair(stats::Traits::FindMagic, (MAT_BONUS / 2)) }));
            }
            case item::named_type::NotNamed:
            case item::named_type::Count:
            default:
            {
                std::ostringstream ss;
                ss << "creature::EnchantmentFactory::NewFromNamedType(named_type=" << NAMED_ENUM
                   << ", is_weapon=" << std::boolalpha << IS_WEAPON
                   << ", is_armor=" << std::boolalpha << IS_ARMOR << "mat_pri=" << MATERIAL_ENUM
                   << ") failed to find a matching named_type.";

                throw std::runtime_error(ss.str());
            }
        }
    }

} // namespace creature
} // namespace heroespath
