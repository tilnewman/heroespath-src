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

    void PlayerInitialSetup::Setup(const creature::CreaturePtr_t CREATURE_PTR) const
    {
        SetupInventory(CREATURE_PTR);
        SetupSpellsAndSongs(CREATURE_PTR);
        CREATURE_PTR->SetHeldWeaponsToBest();
        SetStartingHealth(CREATURE_PTR);
        SetStartingMana(CREATURE_PTR);
    }

    void PlayerInitialSetup::SetupInventory(const creature::CreaturePtr_t CREATURE_PTR) const
    {
        EquipBodyParts(CREATURE_PTR);

        const auto ROLE_ENUM { CREATURE_PTR->Role() };

        if (ROLE_ENUM == creature::role::Knight)
        {
            item::ItemPtr_t weaponPtr { [&]() {
                item::ItemProfile profile;
                switch (misc::Random(2))
                {
                    case 1:
                    {
                        profile.SetSword(
                            item::weapon::sword_type::Shortsword,
                            item::material::Steel,
                            item::material::Wood);

                        return itemFactory_.Make(profile);
                    }
                    case 2:
                    {
                        profile.SetSword(
                            item::weapon::sword_type::Cutlass,
                            item::material::Steel,
                            item::material::Wood);

                        return itemFactory_.Make(profile);
                    }
                    default:
                    {
                        profile.SetAxe(
                            item::weapon::axe_type::Sickle,
                            item::material::Steel,
                            item::material::Wood);
                        return itemFactory_.Make(profile);
                    }
                }
            }() };

            EnsureItemAddedAndEquipped(CREATURE_PTR, weaponPtr);
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainBoots(CREATURE_PTR->IsPixie()));
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainShirt(CREATURE_PTR->IsPixie()));
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainPants(CREATURE_PTR->IsPixie()));

            {
                item::ItemProfile helmProfile;

                helmProfile.SetHelm(
                    item::armor::helm_type::Great,
                    item::material::Leather,
                    item::material::Nothing);

                EnsureItemAddedAndEquipped(CREATURE_PTR, itemFactory_.Make(helmProfile));
            }

            return;
        }
        else if (ROLE_ENUM == creature::role::Beastmaster)
        {
            item::ItemPtr_t weaponPtr { [&]() {
                item::ItemProfile profile;
                switch (misc::Random(2))
                {
                    case 1:
                    {
                        profile.SetSword(
                            item::weapon::sword_type::Shortsword,
                            item::material::Steel,
                            item::material::Wood);

                        return itemFactory_.Make(profile);
                    }
                    case 2:
                    {
                        profile.SetSword(
                            item::weapon::sword_type::Cutlass,
                            item::material::Steel,
                            item::material::Wood);

                        return itemFactory_.Make(profile);
                    }
                    default:
                    {
                        profile.SetAxe(
                            item::weapon::axe_type::Sickle,
                            item::material::Steel,
                            item::material::Wood);
                        return itemFactory_.Make(profile);
                    }
                }
            }() };

            EnsureItemAddedAndEquipped(CREATURE_PTR, weaponPtr);
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainBoots(CREATURE_PTR->IsPixie()));
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainShirt(CREATURE_PTR->IsPixie()));
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainPants(CREATURE_PTR->IsPixie()));

            {
                item::ItemProfile wandProfile;
                wandProfile.SetMisc(
                    item::misc_type::Wand, CREATURE_PTR->IsPixie(), item::material::Wood);
                EnsureItemAddedAndEquipped(CREATURE_PTR, itemFactory_.Make(wandProfile));
            }

            return;
        }
        else if (ROLE_ENUM == creature::role::Archer)
        {
            {
                item::ItemProfile bowProfile;
                bowProfile.SetProjectile(
                    item::weapon::projectile_type::Shortbow, item::material::Wood);
                EnsureItemAddedAndEquipped(CREATURE_PTR, itemFactory_.Make(bowProfile));
            }

            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainBoots(CREATURE_PTR->IsPixie()));
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainShirt(CREATURE_PTR->IsPixie()));
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainPants(CREATURE_PTR->IsPixie()));
            return;
        }
        else if (ROLE_ENUM == creature::role::Bard)
        {
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainBoots(CREATURE_PTR->IsPixie()));
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainShirt(CREATURE_PTR->IsPixie()));
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainPants(CREATURE_PTR->IsPixie()));
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainWand(CREATURE_PTR->IsPixie()));
            return;
        }
        else if (ROLE_ENUM == creature::role::Thief)
        {
            {
                item::ItemProfile daggerProfile;

                daggerProfile.SetKnife(
                    item::material::Steel,
                    item::material::Wood,
                    item::named_type::Not,
                    item::set_type::Not,
                    item::element_type::None,
                    CREATURE_PTR->IsPixie());

                EnsureItemAddedAndEquipped(CREATURE_PTR, itemFactory_.Make(daggerProfile));
            }

            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainBoots(CREATURE_PTR->IsPixie()));
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainShirt(CREATURE_PTR->IsPixie()));
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainPants(CREATURE_PTR->IsPixie()));

            // use to test achievements
            /*
            for (int i(0); i < 9; ++i)
            {
                CREATURE_PTR->GetAchievements().Increment(creature::AchievementType::LocksPicked);
            }
            */

            return;
        }
        else if (ROLE_ENUM == creature::role::Cleric)
        {
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainBoots(CREATURE_PTR->IsPixie()));
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainShirt(CREATURE_PTR->IsPixie()));
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainPants(CREATURE_PTR->IsPixie()));
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainWand(CREATURE_PTR->IsPixie()));
            return;
        }
        else if (ROLE_ENUM == creature::role::Sorcerer)
        {
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainBoots(CREATURE_PTR->IsPixie()));
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainShirt(CREATURE_PTR->IsPixie()));
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainPants(CREATURE_PTR->IsPixie()));
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainWand(CREATURE_PTR->IsPixie()));
            return;
        }
        else if (
            (ROLE_ENUM == creature::role::Wolfen) || (ROLE_ENUM == creature::role::Sylavin)
            || (ROLE_ENUM == creature::role::Firebrand))
        {
            EnsureItemAddedAndEquipped(
                CREATURE_PTR, itemFactory_.Make(item::body_part::Skin, CREATURE_PTR));
            return;
        }

        std::ostringstream ss;
        ss << "player::PlayerInitialSetup::SetupInventory(\"" << CREATURE_PTR->Name()
           << "\", race=" << CREATURE_PTR->RaceNameForced() << ", role=" << CREATURE_PTR->RoleName()
           << ")  failed to assign any items.";

        throw std::runtime_error(ss.str());
    }

    void PlayerInitialSetup::SetupSpellsAndSongs(const creature::CreaturePtr_t CREATURE_PTR) const
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

    void PlayerInitialSetup::EquipBodyParts(const creature::CreaturePtr_t CREATURE_PTR) const
    {
        const auto BODY { CREATURE_PTR->Body() };

        if (BODY.HasBreath()
            && ((CREATURE_PTR->Role() == creature::role::Sylavin)
                || (CREATURE_PTR->Role() == creature::role::Firebrand)))
        {
            EnsureItemAddedAndEquipped(
                CREATURE_PTR, itemFactory_.Make(item::body_part::Breath, CREATURE_PTR));
        }

        if (BODY.HasClaws())
        {
            EnsureItemAddedAndEquipped(
                CREATURE_PTR, itemFactory_.Make(item::body_part::Claws, CREATURE_PTR));
        }

        if (BODY.HasBite())
        {
            EnsureItemAddedAndEquipped(
                CREATURE_PTR, itemFactory_.Make(item::body_part::Bite, CREATURE_PTR));
        }

        if ((BODY.IsHumanoid()) && (CREATURE_PTR->IsPixie() == false))
        {
            EnsureItemAddedAndEquipped(
                CREATURE_PTR, itemFactory_.Make(item::body_part::Fists, CREATURE_PTR));
        }
    }

    Health_t
        PlayerInitialSetup::GetStartingHealth(const creature::CreaturePtr_t CHARACTER_PTR) const
    {
        const auto HEALTH_BASE { misc::ConfigFile::Instance()->ValueOrDefault<Health_t>(
            "player-race-health-initial-" + NAMEOF_ENUM_STR(CHARACTER_PTR->Race())) };

        return (
            HEALTH_BASE
            + misc::ConfigFile::Instance()->ValueOrDefault<Health_t>(
                  "player-role-health-adjustment-initial-"
                  + NAMEOF_ENUM_STR(CHARACTER_PTR->Role())));
    }

    void PlayerInitialSetup::SetStartingHealth(const creature::CreaturePtr_t CREATURE_PTR) const
    {
        const auto STARTING_HEALTH { GetStartingHealth(CREATURE_PTR) };
        CREATURE_PTR->HealthNormalSet(STARTING_HEALTH);
        CREATURE_PTR->HealthCurrentSet(STARTING_HEALTH);
    }

    void PlayerInitialSetup::SetStartingMana(const creature::CreaturePtr_t CREATURE_PTR) const
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
        const creature::CreaturePtr_t CREATURE_PTR, const item::ItemPtr_t ITEM_PTR) const
    {
        const auto ITEM_ADD_STR { CREATURE_PTR->ItemAdd(ITEM_PTR) };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (ITEM_ADD_STR == creature::Creature::ITEM_ACTION_SUCCESS_STR_),
            "player::PlayerInitialSetup::EnsureItemAddedAndEquipped() was unable to ADD item={"
                << ITEM_PTR->ToString() << "} to character={" << CREATURE_PTR->ToString()
                << "} for reason=" << ITEM_ADD_STR << ".");

        const auto ITEM_EQUIP_STR { CREATURE_PTR->ItemEquip(ITEM_PTR) };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (ITEM_EQUIP_STR == creature::Creature::ITEM_ACTION_SUCCESS_STR_),
            "player::PlayerInitialSetup::EnsureItemAddedAndEquipped() was unable to EQUIP item={"
                << ITEM_PTR->ToString() << "} to character={" << CREATURE_PTR->ToString()
                << "} for reason=" << ITEM_EQUIP_STR << ".");
    }

    const item::ItemPtr_t PlayerInitialSetup::MakePlainBoots(const bool IS_PIXIE) const
    {
        item::ItemProfile profile;

        profile.SetBoots(
            item::armor::base_type::Plain,
            item::material::Leather,
            item::material::Tin,
            item::named_type::Not,
            item::set_type::Not,
            item::element_type::None,
            IS_PIXIE);

        return itemFactory_.Make(profile);
    }

    const item::ItemPtr_t PlayerInitialSetup::MakePlainShirt(const bool IS_PIXIE) const
    {
        item::ItemProfile profile;

        profile.SetShirt(
            item::armor::base_type::Plain,
            item::material::Leather,
            item::material::Tin,
            item::named_type::Not,
            item::set_type::Not,
            item::element_type::None,
            IS_PIXIE);

        return itemFactory_.Make(profile);
    }

    const item::ItemPtr_t PlayerInitialSetup::MakePlainPants(const bool IS_PIXIE) const
    {
        item::ItemProfile profile;

        profile.SetPants(
            item::armor::base_type::Plain,
            item::material::Leather,
            item::material::Tin,
            item::named_type::Not,
            item::set_type::Not,
            item::element_type::None,
            IS_PIXIE);

        return itemFactory_.Make(profile);
    }

    const item::ItemPtr_t PlayerInitialSetup::MakePlainWand(const bool IS_PIXIE) const
    {
        item::ItemProfile profile;
        profile.SetMisc(item::misc_type::Wand, IS_PIXIE, item::material::Wood);
        return itemFactory_.Make(profile);
    }

} // namespace creature
} // namespace heroespath
