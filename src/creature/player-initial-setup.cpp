// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// player-initial-setup.cpp
//
#include "player-initial-setup.hpp"

#include "creature/creature.hpp"
#include "creature/trait.hpp"
#include "item/item-factory.hpp"
#include "item/item.hpp"
#include "misc/config-file.hpp"
#include "misc/log-macros.hpp"
#include "misc/random.hpp"
#include "song/song-enum.hpp"
#include "spell/spell-enum.hpp"

#include <sstream>
#include <stdexcept>
#include <string>

namespace heroespath
{
namespace creature
{

    void PlayerInitialSetup::Setup(const creature::CreaturePtr_t CREATURE_PTR)
    {
        SetupInventory(CREATURE_PTR);
        SetupSpellsAndSongs(CREATURE_PTR);
        CREATURE_PTR->SetHeldWeaponsToBest();
        SetStartingHealth(CREATURE_PTR);
        SetStartingMana(CREATURE_PTR);
    }

    void PlayerInitialSetup::SetupInventory(const creature::CreaturePtr_t CREATURE_PTR)
    {
        const auto ROLE_ENUM = CREATURE_PTR->Role();
        const auto IS_PIXIE = CREATURE_PTR->IsPixie();

        if (ROLE_ENUM == creature::role::Knight)
        {
            item::ItemPtr_t weaponPtr { [&]() {
                switch (misc::Random(2))
                {
                    case 1:
                    {
                        return item::ItemFactory::Make(
                            item::Swords::Shortsword,
                            IS_PIXIE,
                            item::Material::Steel,
                            item::Material::Wood);
                    }
                    case 2:
                    {
                        return item::ItemFactory::Make(
                            item::Swords::Cutlass,
                            IS_PIXIE,
                            item::Material::Steel,
                            item::Material::Wood);
                    }
                    default:
                    {
                        return item::ItemFactory::Make(
                            item::Axes::Sickle,
                            IS_PIXIE,
                            item::Material::Steel,
                            item::Material::Wood);
                    }
                }
            }() };

            EnsureItemAddedAndEquipped(CREATURE_PTR, weaponPtr);
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainBoot(IS_PIXIE));
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainShirt(IS_PIXIE));
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainPant(IS_PIXIE));

            EnsureItemAddedAndEquipped(
                CREATURE_PTR,
                item::ItemFactory::Make(item::Helms::Great, IS_PIXIE, item::Material::Leather));
        }
        else if (ROLE_ENUM == creature::role::Beastmaster)
        {
            item::ItemPtr_t weaponPtr { [&]() {
                switch (misc::Random(2))
                {
                    case 1:
                    {
                        return item::ItemFactory::Make(
                            item::Swords::Shortsword,
                            IS_PIXIE,
                            item::Material::Steel,
                            item::Material::Wood);
                    }
                    case 2:
                    {
                        return item::ItemFactory::Make(
                            item::Swords::Cutlass,
                            IS_PIXIE,
                            item::Material::Steel,
                            item::Material::Wood);
                    }
                    default:
                    {
                        return item::ItemFactory::Make(
                            item::Axes::Sickle,
                            IS_PIXIE,
                            item::Material::Steel,
                            item::Material::Wood);
                    }
                }
            }() };

            EnsureItemAddedAndEquipped(CREATURE_PTR, weaponPtr);
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainBoot(IS_PIXIE));
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainShirt(IS_PIXIE));
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainPant(IS_PIXIE));
        }
        else if (ROLE_ENUM == creature::role::Archer)
        {
            EnsureItemAddedAndEquipped(
                CREATURE_PTR,
                item::ItemFactory::Make(
                    item::Projectiles::Shortbow, IS_PIXIE, item::Material::Wood));

            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainBoot(IS_PIXIE));
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainShirt(IS_PIXIE));
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainPant(IS_PIXIE));
        }
        else if (ROLE_ENUM == creature::role::Bard)
        {
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainBoot(IS_PIXIE));
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainShirt(IS_PIXIE));
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainPant(IS_PIXIE));
        }
        else if (ROLE_ENUM == creature::role::Thief)
        {
            EnsureItemAddedAndEquipped(
                CREATURE_PTR,
                item::ItemFactory::Make(
                    item::Knifes::Dagger, IS_PIXIE, item::Material::Steel, item::Material::Wood));

            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainBoot(IS_PIXIE));
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainShirt(IS_PIXIE));
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainPant(IS_PIXIE));

            // use to test achievements
            /*
            for (int i(0); i < 9; ++i)
            {
                CREATURE_PTR->GetAchievements().Increment(creature::AchievementType::LocksPicked);
            }
            */
        }
        else if (ROLE_ENUM == creature::role::Cleric)
        {
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainBoot(IS_PIXIE));
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainShirt(IS_PIXIE));
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainPant(IS_PIXIE));
        }
        else if (ROLE_ENUM == creature::role::Sorcerer)
        {
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainBoot(IS_PIXIE));
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainShirt(IS_PIXIE));
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainPant(IS_PIXIE));
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainWand(IS_PIXIE));
        }
    }

    void PlayerInitialSetup::SetupSpellsAndSongs(const creature::CreaturePtr_t CREATURE_PTR)
    {
        const auto ROLE_ENUM { CREATURE_PTR->Role() };

        if (ROLE_ENUM == creature::role::Cleric)
        {
            CREATURE_PTR->SpellAdd(spell::Spells::Awaken);
            CREATURE_PTR->SpellAdd(spell::Spells::Bandage);
            CREATURE_PTR->SpellAdd(spell::Spells::ClearMind);
            CREATURE_PTR->SpellAdd(spell::Spells::Lift);
            CREATURE_PTR->SpellAdd(spell::Spells::Sleep);
            CREATURE_PTR->SpellAdd(spell::Spells::Antidote);
        }
        else if (ROLE_ENUM == creature::role::Sorcerer)
        {
            CREATURE_PTR->SpellAdd(spell::Spells::Daze);
            CREATURE_PTR->SpellAdd(spell::Spells::Panic);
            CREATURE_PTR->SpellAdd(spell::Spells::Sleep);
            CREATURE_PTR->SpellAdd(spell::Spells::Awaken);
            CREATURE_PTR->SpellAdd(spell::Spells::Poison);
            CREATURE_PTR->SpellAdd(spell::Spells::Sparks);
            CREATURE_PTR->SpellAdd(spell::Spells::Trip);
        }
        else if (ROLE_ENUM == creature::role::Bard)
        {
            CREATURE_PTR->SongAdd(song::Songs::RallyDrum);
            CREATURE_PTR->SongAdd(song::Songs::Lullaby);
            CREATURE_PTR->SongAdd(song::Songs::PanicStrings);
            CREATURE_PTR->SongAdd(song::Songs::RousingRhythm);
            CREATURE_PTR->SongAdd(song::Songs::TripBeat);
            CREATURE_PTR->SongAdd(song::Songs::SpiritResonance);
        }
    }

    Health_t PlayerInitialSetup::GetStartingHealth(const creature::CreaturePtr_t CHARACTER_PTR)
    {
        const auto HEALTH_BASE { misc::ConfigFile::Instance()->ValueOrDefault<Health_t>(
            "player-race-health-initial-" + NAMEOF_ENUM_STR(CHARACTER_PTR->Race())) };

        return (
            HEALTH_BASE
            + misc::ConfigFile::Instance()->ValueOrDefault<Health_t>(
                "player-role-health-adjustment-initial-" + NAMEOF_ENUM_STR(CHARACTER_PTR->Role())));
    }

    void PlayerInitialSetup::SetStartingHealth(const creature::CreaturePtr_t CREATURE_PTR)
    {
        const auto STARTING_HEALTH { GetStartingHealth(CREATURE_PTR) };
        CREATURE_PTR->HealthNormalSet(STARTING_HEALTH);
        CREATURE_PTR->HealthCurrentSet(STARTING_HEALTH);
    }

    void PlayerInitialSetup::SetStartingMana(const creature::CreaturePtr_t CREATURE_PTR)
    {
        const auto ROLE_ENUM { CREATURE_PTR->Role() };

        if ((ROLE_ENUM == creature::role::Sorcerer) || (ROLE_ENUM == creature::role::Cleric))
        {
            const auto INITIAL_MANA { CREATURE_PTR->TraitNormal(Traits::Intelligence) / 2 };
            CREATURE_PTR->TraitNormalSet(Traits::Mana, INITIAL_MANA);
            CREATURE_PTR->TraitCurrentSet(Traits::Mana, INITIAL_MANA);
        }
        else if (ROLE_ENUM == creature::role::Bard)
        {
            const auto INITIAL_MANA { (CREATURE_PTR->TraitNormal(Traits::Intelligence)
                                       + CREATURE_PTR->TraitNormal(Traits::Charm))
                                      / 4 };

            CREATURE_PTR->TraitNormalSet(Traits::Mana, INITIAL_MANA);
            CREATURE_PTR->TraitCurrentSet(Traits::Mana, INITIAL_MANA);
        }
    }

    void PlayerInitialSetup::EnsureItemAddedAndEquipped(
        const creature::CreaturePtr_t CREATURE_PTR, const item::ItemPtr_t ITEM_PTR)
    {
        const auto ITEM_ADD_STR { CREATURE_PTR->ItemAdd(ITEM_PTR) };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (ITEM_ADD_STR == creature::Creature::ITEM_ACTION_SUCCESS_STR_),
            "player::PlayerInitialSetup::EnsureItemAddedAndEquipped() was unable to ADD "
            "item={"
                << ITEM_PTR->ToString() << "} to character={" << CREATURE_PTR->ToString()
                << "} for reason=" << ITEM_ADD_STR << ".");

        const auto ITEM_EQUIP_STR { CREATURE_PTR->ItemEquip(ITEM_PTR) };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (ITEM_EQUIP_STR == creature::Creature::ITEM_ACTION_SUCCESS_STR_),
            "player::PlayerInitialSetup::EnsureItemAddedAndEquipped() was unable to EQUIP "
            "item={"
                << ITEM_PTR->ToString() << "} to character={" << CREATURE_PTR->ToString()
                << "} for reason=" << ITEM_EQUIP_STR << ".");
    }

    const item::ItemPtr_t PlayerInitialSetup::MakePlainBoot(const bool IS_PIXIE)
    {
        return item::ItemFactory::Make(
            item::Armor::Boot,
            item::Forms::Plain,
            IS_PIXIE,
            item::Material::Leather,
            item::Material::Tin);
    }

    const item::ItemPtr_t PlayerInitialSetup::MakePlainShirt(const bool IS_PIXIE)
    {
        return item::ItemFactory::Make(
            item::Armor::Shirt,
            item::Forms::Plain,
            IS_PIXIE,
            item::Material::Leather,
            item::Material::Tin);
    }

    const item::ItemPtr_t PlayerInitialSetup::MakePlainPant(const bool IS_PIXIE)
    {
        return item::ItemFactory::Make(
            item::Armor::Pant,
            item::Forms::Plain,
            IS_PIXIE,
            item::Material::Leather,
            item::Material::Tin);
    }

    const item::ItemPtr_t PlayerInitialSetup::MakePlainWand(const bool IS_PIXIE)
    {
        return item::ItemFactory::Make(item::Misc::Wand, IS_PIXIE, item::Material::Wood);
    }

} // namespace creature
} // namespace heroespath
