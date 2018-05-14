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
// initial.cpp
//
#include "initial.hpp"

#include "creature/creature.hpp"
#include "game/game-data-file.hpp"
#include "item/item-factory.hpp"
#include "item/item.hpp"
#include "item/weapon-factory.hpp"
#include "log/log-macros.hpp"
#include "misc/random.hpp"
#include "sfml-util/gui/creature-image-loader.hpp"
#include "song/song-enum.hpp"
#include "spell/spell-enum.hpp"

#include <exception>
#include <sstream>
#include <string>

namespace heroespath
{
namespace player
{

    void Initial::Setup(const creature::CreaturePtr_t CREATURE_PTR)
    {
        SetupInventory(CREATURE_PTR);
        SetupSpellsAndSongs(CREATURE_PTR);
        CREATURE_PTR->SetHeldWeaponsToBest();
        SetStartingHealth(CREATURE_PTR);
        SetStartingMana(CREATURE_PTR);
    }

    void Initial::SetupInventory(const creature::CreaturePtr_t CREATURE_PTR)
    {
        EquipBodyParts(CREATURE_PTR);

        using namespace item;

        auto const ROLE_ENUM{ CREATURE_PTR->Role() };

        if (ROLE_ENUM == creature::role::Knight)
        {
            ItemPtr_t weaponPtr{ [&]() {
                ItemProfile profile;
                switch (misc::random::Int(2))
                {
                    case 1:
                    {
                        profile.SetSword(
                            weapon::sword_type::Shortsword, material::Steel, material::Wood);

                        return item::ItemFactory::Make(profile);
                    }
                    case 2:
                    {
                        profile.SetSword(
                            weapon::sword_type::Cutlass, material::Steel, material::Wood);

                        return item::ItemFactory::Make(profile);
                    }
                    default:
                    {
                        profile.SetAxe(weapon::axe_type::Sickle, material::Steel, material::Wood);
                        return item::ItemFactory::Make(profile);
                    }
                }
            }() };

            EnsureItemAddedAndEquipped(CREATURE_PTR, weaponPtr);
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainBoots(CREATURE_PTR->IsPixie()));
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainShirt(CREATURE_PTR->IsPixie()));
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainPants(CREATURE_PTR->IsPixie()));

            {
                ItemProfile helmProfile;

                helmProfile.SetHelm(armor::helm_type::Great, material::Leather, material::Nothing);

                EnsureItemAddedAndEquipped(CREATURE_PTR, item::ItemFactory::Make(helmProfile));
            }

            return;
        }
        else if (ROLE_ENUM == creature::role::Beastmaster)
        {
            ItemPtr_t weaponPtr{ [&]() {
                ItemProfile profile;
                switch (misc::random::Int(2))
                {
                    case 1:
                    {
                        profile.SetSword(
                            weapon::sword_type::Shortsword, material::Steel, material::Wood);

                        return item::ItemFactory::Make(profile);
                    }
                    case 2:
                    {
                        profile.SetSword(
                            weapon::sword_type::Cutlass, material::Steel, material::Wood);

                        return item::ItemFactory::Make(profile);
                    }
                    default:
                    {
                        profile.SetAxe(weapon::axe_type::Sickle, material::Steel, material::Wood);
                        return item::ItemFactory::Make(profile);
                    }
                }
            }() };

            EnsureItemAddedAndEquipped(CREATURE_PTR, weaponPtr);
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainBoots(CREATURE_PTR->IsPixie()));
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainShirt(CREATURE_PTR->IsPixie()));
            EnsureItemAddedAndEquipped(CREATURE_PTR, MakePlainPants(CREATURE_PTR->IsPixie()));

            {
                ItemProfile wandProfile;
                wandProfile.SetMisc(misc_type::Wand, CREATURE_PTR->IsPixie(), material::Wood);
                EnsureItemAddedAndEquipped(CREATURE_PTR, item::ItemFactory::Make(wandProfile));
            }

            return;
        }
        else if (ROLE_ENUM == creature::role::Archer)
        {
            {
                ItemProfile bowProfile;
                bowProfile.SetProjectile(weapon::projectile_type::Shortbow, material::Wood);
                EnsureItemAddedAndEquipped(CREATURE_PTR, item::ItemFactory::Make(bowProfile));
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
                ItemProfile daggerProfile;

                daggerProfile.SetKnife(
                    material::Steel,
                    material::Wood,
                    named_type::Not,
                    set_type::Not,
                    element_type::None,
                    CREATURE_PTR->IsPixie());

                EnsureItemAddedAndEquipped(CREATURE_PTR, item::ItemFactory::Make(daggerProfile));
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
                CREATURE_PTR, ItemFactory::Make(body_part::Skin, CREATURE_PTR));
            return;
        }

        std::ostringstream ss;
        ss << "player::Initial::SetupInventory(\"" << CREATURE_PTR->Name()
           << "\", race=" << CREATURE_PTR->RaceName() << ", role=" << CREATURE_PTR->RoleName()
           << ")  failed to assign any items.";

        throw std::runtime_error(ss.str());
    }

    void Initial::SetupSpellsAndSongs(const creature::CreaturePtr_t CREATURE_PTR)
    {
        auto const ROLE_ENUM{ CREATURE_PTR->Role() };

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

    void Initial::EquipBodyParts(const creature::CreaturePtr_t CREATURE_PTR)
    {
        using namespace item;

        auto const BODY{ CREATURE_PTR->Body() };

        if (BODY.HasBreath()
            && ((CREATURE_PTR->Role() == creature::role::Sylavin)
                || (CREATURE_PTR->Role() == creature::role::Firebrand)))
        {
            EnsureItemAddedAndEquipped(
                CREATURE_PTR, ItemFactory::Make(body_part::Breath, CREATURE_PTR));
        }

        if (BODY.HasClaws())
        {
            EnsureItemAddedAndEquipped(
                CREATURE_PTR, ItemFactory::Make(body_part::Claws, CREATURE_PTR));
        }

        if (BODY.HasBite())
        {
            EnsureItemAddedAndEquipped(
                CREATURE_PTR, ItemFactory::Make(body_part::Bite, CREATURE_PTR));
        }

        if ((BODY.IsHumanoid()) && (CREATURE_PTR->IsPixie() == false))
        {
            EnsureItemAddedAndEquipped(
                CREATURE_PTR, ItemFactory::Make(body_part::Fists, CREATURE_PTR));
        }
    }

    Health_t Initial::GetStartingHealth(const creature::CreaturePtr_t CHARACTER_PTR)
    {
        std::ostringstream ss;
        ss << "heroespath-player-race-health-initial-"
           << creature::race::ToString(CHARACTER_PTR->Race());

        auto const HEALTH_BASE{ Health_t(game::GameDataFile::Instance()->GetCopyInt(ss.str())) };

        ss.str("");
        ss << "heroespath-player-role-health-adjustment-initial-"
           << creature::role::ToString(CHARACTER_PTR->Role());

        return HEALTH_BASE + Health_t(game::GameDataFile::Instance()->GetCopyInt(ss.str()));
    }

    void Initial::SetStartingHealth(const creature::CreaturePtr_t CREATURE_PTR)
    {
        auto const STARTING_HEALTH{ GetStartingHealth(CREATURE_PTR) };
        CREATURE_PTR->HealthNormalSet(STARTING_HEALTH);
        CREATURE_PTR->HealthCurrentSet(STARTING_HEALTH);
    }

    void Initial::SetStartingMana(const creature::CreaturePtr_t CREATURE_PTR)
    {
        auto const ROLE_ENUM{ CREATURE_PTR->Role() };

        if ((ROLE_ENUM == creature::role::Sorcerer) || (ROLE_ENUM == creature::role::Cleric))
        {
            auto const INITIAL_MANA{ CREATURE_PTR->TraitNormal(stats::Traits::Intelligence) / 2 };
            CREATURE_PTR->TraitNormalSet(stats::Traits::Mana, INITIAL_MANA);
            CREATURE_PTR->TraitCurrentSet(stats::Traits::Mana, INITIAL_MANA);
        }
        else if (ROLE_ENUM == creature::role::Bard)
        {
            auto const INITIAL_MANA{ (CREATURE_PTR->TraitNormal(stats::Traits::Intelligence)
                                      + CREATURE_PTR->TraitNormal(stats::Traits::Charm))
                                     / 4 };

            CREATURE_PTR->TraitNormalSet(stats::Traits::Mana, INITIAL_MANA);
            CREATURE_PTR->TraitCurrentSet(stats::Traits::Mana, INITIAL_MANA);
        }
    }

    void Initial::EnsureItemAddedAndEquipped(
        const creature::CreaturePtr_t CREATURE_PTR, const item::ItemPtr_t ITEM_PTR)
    {
        auto const ITEM_ADD_STR{ CREATURE_PTR->ItemAdd(ITEM_PTR) };

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_ADD_STR == creature::Creature::ITEM_ACTION_SUCCESS_STR_),
            "player::Initial::EnsureItemAddedAndEquipped() was unable to ADD item={"
                << ITEM_PTR->ToString() << "} to character={" << CREATURE_PTR->ToString()
                << "} for reason=" << ITEM_ADD_STR << ".");

        auto const ITEM_EQUIP_STR{ CREATURE_PTR->ItemEquip(ITEM_PTR) };

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ITEM_EQUIP_STR == creature::Creature::ITEM_ACTION_SUCCESS_STR_),
            "player::Initial::EnsureItemAddedAndEquipped() was unable to EQUIP item={"
                << ITEM_PTR->ToString() << "} to character={" << CREATURE_PTR->ToString()
                << "} for reason=" << ITEM_EQUIP_STR << ".");
    }

    const item::ItemPtr_t Initial::MakePlainBoots(const bool IS_PIXIE)
    {
        using namespace item;

        ItemProfile profile;

        profile.SetBoots(
            armor::base_type::Plain,
            material::Leather,
            material::Tin,
            named_type::Not,
            set_type::Not,
            element_type::None,
            IS_PIXIE);

        return ItemFactory::Make(profile);
    }

    const item::ItemPtr_t Initial::MakePlainShirt(const bool IS_PIXIE)
    {
        using namespace item;

        ItemProfile profile;

        profile.SetShirt(
            armor::base_type::Plain,
            material::Leather,
            material::Tin,
            named_type::Not,
            set_type::Not,
            element_type::None,
            IS_PIXIE);

        return ItemFactory::Make(profile);
    }

    const item::ItemPtr_t Initial::MakePlainPants(const bool IS_PIXIE)
    {
        using namespace item;

        ItemProfile profile;

        profile.SetPants(
            armor::base_type::Plain,
            material::Leather,
            material::Tin,
            named_type::Not,
            set_type::Not,
            element_type::None,
            IS_PIXIE);

        return ItemFactory::Make(profile);
    }

    const item::ItemPtr_t Initial::MakePlainWand(const bool IS_PIXIE)
    {
        using namespace item;

        ItemProfile profile;
        profile.SetMisc(misc_type::Wand, IS_PIXIE, material::Wood);
        return ItemFactory::Make(profile);
    }

} // namespace player
} // namespace heroespath
