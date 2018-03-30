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
#include "item/armor-factory.hpp"
#include "item/misc-item-factory.hpp"
#include "item/weapon-factory.hpp"
#include "log/log-macros.hpp"
#include "misc/random.hpp"
#include "sfml-util/gui/creature-image-manager.hpp"
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
        EnsureValidImageFilename(CREATURE_PTR);
    }

    void Initial::EnsureValidImageFilename(const creature::CreaturePtr_t CREATURE_PTR)
    {
        if (CREATURE_PTR->ImageFilename().empty())
        {
            CREATURE_PTR->ImageFilename(
                sfml_util::gui::CreatureImageManager::Instance()->GetFilename(
                    CREATURE_PTR->Race(), CREATURE_PTR->Role(), CREATURE_PTR->Sex(), true));
        }
    }

    void Initial::SetupInventory(const creature::CreaturePtr_t CREATURE_PTR)
    {
        EquipBodyParts(CREATURE_PTR);

        using namespace item;

        auto const ROLE_ENUM{ CREATURE_PTR->Role() };

        if (ROLE_ENUM == creature::role::Knight)
        {
            ItemPtr_t weaponPtr = [&]() {
                switch (misc::random::Int(2))
                {
                    case 1:
                    {
                        return weapon::WeaponFactory::Make_Sword(
                            weapon::sword_type::Shortsword, material::Steel, material::Wood);
                    }
                    case 2:
                    {
                        return weapon::WeaponFactory::Make_Sword(
                            weapon::sword_type::Cutlass, material::Steel, material::Wood);
                    }
                    default:
                    {
                        return weapon::WeaponFactory::Make_Axe(
                            weapon::axe_type::Sickle, material::Steel, material::Wood);
                    }
                }
            }();

            CREATURE_PTR->ItemAdd(weaponPtr);
            CREATURE_PTR->ItemEquip(weaponPtr);

            auto bootsPtr{ armor::ArmorFactory::Make_Boots(
                armor::base_type::Plain, HardOrSoftLeatherRand(), material::Nothing) };

            CREATURE_PTR->ItemAdd(bootsPtr);
            CREATURE_PTR->ItemEquip(bootsPtr);

            auto shirtPtr{ armor::ArmorFactory::Make_Shirt(
                armor::base_type::Plain, HardOrSoftLeatherRand()) };

            CREATURE_PTR->ItemAdd(shirtPtr);
            CREATURE_PTR->ItemEquip(shirtPtr);

            auto pantsPtr{ armor::ArmorFactory::Make_Pants(
                armor::base_type::Plain, HardOrSoftLeatherRand()) };

            CREATURE_PTR->ItemAdd(pantsPtr);
            CREATURE_PTR->ItemEquip(pantsPtr);

            auto helmPtr{ armor::ArmorFactory::Make_Helm(
                armor::helm_type::Great, HardOrSoftLeatherRand(), material::Nothing) };

            CREATURE_PTR->ItemAdd(helmPtr);
            CREATURE_PTR->ItemEquip(helmPtr);

            return;
        }

        if (ROLE_ENUM == creature::role::Beastmaster)
        {
            ItemPtr_t weaponPtr = [&]() {
                switch (misc::random::Int(2))
                {
                    case 1:
                    {
                        return weapon::WeaponFactory::Make_Sword(
                            weapon::sword_type::Shortsword, material::Steel, material::Wood);
                    }
                    case 2:
                    {
                        return weapon::WeaponFactory::Make_Sword(
                            weapon::sword_type::Cutlass, material::Steel, material::Wood);
                    }
                    default:
                    {
                        return weapon::WeaponFactory::Make_Axe(
                            weapon::axe_type::Sickle, material::Steel, material::Wood);
                    }
                }
            }();

            CREATURE_PTR->ItemAdd(weaponPtr);
            CREATURE_PTR->ItemEquip(weaponPtr);

            auto bootsPtr{ armor::ArmorFactory::Make_Boots(
                armor::base_type::Plain, HardOrSoftLeatherRand(), material::Nothing) };

            CREATURE_PTR->ItemAdd(bootsPtr);
            CREATURE_PTR->ItemEquip(bootsPtr);

            auto shirtPtr{ armor::ArmorFactory::Make_Shirt(
                armor::base_type::Plain, HardOrSoftLeatherRand()) };

            CREATURE_PTR->ItemAdd(shirtPtr);
            CREATURE_PTR->ItemEquip(shirtPtr);

            auto pantsPtr{ armor::ArmorFactory::Make_Pants(
                armor::base_type::Plain, HardOrSoftLeatherRand()) };

            CREATURE_PTR->ItemAdd(pantsPtr);
            CREATURE_PTR->ItemEquip(pantsPtr);

            auto wandPtr{ item::MiscItemFactory::Make_Wand(material::Wood, material::Nothing) };

            CREATURE_PTR->ItemAdd(wandPtr);
            CREATURE_PTR->ItemEquip(wandPtr);

            return;
        }

        if (ROLE_ENUM == creature::role::Archer)
        {
            auto shortbowPtr{ weapon::WeaponFactory::Make_Projectile(
                weapon::projectile_type::Shortbow, material::Wood, material::Nothing) };

            CREATURE_PTR->ItemAdd(shortbowPtr);
            CREATURE_PTR->ItemEquip(shortbowPtr);

            auto bootsPtr{ armor::ArmorFactory::Make_Boots(
                armor::base_type::Plain, HardOrSoftLeatherRand(), material::Nothing) };

            CREATURE_PTR->ItemAdd(bootsPtr);
            CREATURE_PTR->ItemEquip(bootsPtr);

            auto shirtPtr{ armor::ArmorFactory::Make_Shirt(
                armor::base_type::Plain, HardOrSoftLeatherRand()) };

            CREATURE_PTR->ItemAdd(shirtPtr);
            CREATURE_PTR->ItemEquip(shirtPtr);

            auto pantsPtr{ armor::ArmorFactory::Make_Pants(
                armor::base_type::Plain, HardOrSoftLeatherRand()) };

            CREATURE_PTR->ItemAdd(pantsPtr);
            CREATURE_PTR->ItemEquip(pantsPtr);

            return;
        }

        if (ROLE_ENUM == creature::role::Bard)
        {
            auto bootsPtr{ armor::ArmorFactory::Make_Boots(
                armor::base_type::Plain,
                HardOrSoftLeatherRand(),
                material::Nothing,
                CREATURE_PTR->IsPixie()) };

            CREATURE_PTR->ItemAdd(bootsPtr);
            CREATURE_PTR->ItemEquip(bootsPtr);

            auto shirtPtr{ armor::ArmorFactory::Make_Shirt(
                armor::base_type::Plain,
                material::Cloth,
                material::Nothing,
                CREATURE_PTR->IsPixie()) };

            CREATURE_PTR->ItemAdd(shirtPtr);
            CREATURE_PTR->ItemEquip(shirtPtr);

            auto pantsPtr{ armor::ArmorFactory::Make_Pants(
                armor::base_type::Plain,
                HardOrSoftLeatherRand(),
                material::Tin,
                CREATURE_PTR->IsPixie()) };

            CREATURE_PTR->ItemAdd(pantsPtr);
            CREATURE_PTR->ItemEquip(pantsPtr);

            auto drumLutePtr{ item::MiscItemFactory::Make_DrumLute(CREATURE_PTR->IsPixie()) };

            CREATURE_PTR->ItemAdd(drumLutePtr);
            CREATURE_PTR->ItemEquip(drumLutePtr);

            return;
        }

        if (ROLE_ENUM == creature::role::Thief)
        {
            auto daggerPtr{ weapon::WeaponFactory::Make_Knife(
                true,
                sfml_util::Size::Small,
                material::Steel,
                material::Wood,
                CREATURE_PTR->IsPixie()) };

            CREATURE_PTR->ItemAdd(daggerPtr);
            CREATURE_PTR->ItemEquip(daggerPtr);

            auto bootsPtr{ armor::ArmorFactory::Make_Boots(
                armor::base_type::Plain,
                HardOrSoftLeatherRand(),
                material::Nothing,
                CREATURE_PTR->IsPixie()) };

            CREATURE_PTR->ItemAdd(bootsPtr);
            CREATURE_PTR->ItemEquip(bootsPtr);

            auto shirtPtr{ armor::ArmorFactory::Make_Shirt(
                armor::base_type::Plain,
                material::Cloth,
                material::Nothing,
                CREATURE_PTR->IsPixie()) };

            CREATURE_PTR->ItemAdd(shirtPtr);
            CREATURE_PTR->ItemEquip(shirtPtr);

            auto pantsPtr{ armor::ArmorFactory::Make_Pants(
                armor::base_type::Plain,
                HardOrSoftLeatherRand(),
                material::Tin,
                CREATURE_PTR->IsPixie()) };

            CREATURE_PTR->ItemAdd(pantsPtr);
            CREATURE_PTR->ItemEquip(pantsPtr);

            for (int i(0); i < 9; ++i)
            {
                CREATURE_PTR->GetAchievements().Increment(creature::AchievementType::LocksPicked);
            }

            return;
        }

        if (ROLE_ENUM == creature::role::Cleric)
        {
            auto bootsPtr{ armor::ArmorFactory::Make_Boots(
                armor::base_type::Plain,
                HardOrSoftLeatherRand(),
                material::Nothing,
                CREATURE_PTR->IsPixie()) };

            CREATURE_PTR->ItemAdd(bootsPtr);
            CREATURE_PTR->ItemEquip(bootsPtr);

            auto shirtPtr{ armor::ArmorFactory::Make_Shirt(
                armor::base_type::Plain,
                material::Cloth,
                material::Nothing,
                CREATURE_PTR->IsPixie()) };

            CREATURE_PTR->ItemAdd(shirtPtr);
            CREATURE_PTR->ItemEquip(shirtPtr);

            auto pantsPtr{ armor::ArmorFactory::Make_Pants(
                armor::base_type::Plain,
                HardOrSoftLeatherRand(),
                material::Tin,
                CREATURE_PTR->IsPixie()) };

            CREATURE_PTR->ItemAdd(pantsPtr);
            CREATURE_PTR->ItemEquip(pantsPtr);

            auto wandPtr{ item::MiscItemFactory::Make_Wand(material::Glass, material::Nothing) };

            CREATURE_PTR->ItemAdd(wandPtr);
            CREATURE_PTR->ItemEquip(wandPtr);

            return;
        }

        if (ROLE_ENUM == creature::role::Sorcerer)
        {
            auto bootsPtr{ armor::ArmorFactory::Make_Boots(
                armor::base_type::Plain,
                HardOrSoftLeatherRand(),
                material::Nothing,
                CREATURE_PTR->IsPixie()) };

            CREATURE_PTR->ItemAdd(bootsPtr);
            CREATURE_PTR->ItemEquip(bootsPtr);

            auto shirtPtr{ armor::ArmorFactory::Make_Shirt(
                armor::base_type::Plain,
                material::Cloth,
                material::Nothing,
                CREATURE_PTR->IsPixie()) };

            CREATURE_PTR->ItemAdd(shirtPtr);
            CREATURE_PTR->ItemEquip(shirtPtr);

            auto pantsPtr{ armor::ArmorFactory::Make_Pants(
                armor::base_type::Plain,
                HardOrSoftLeatherRand(),
                material::Tin,
                CREATURE_PTR->IsPixie()) };

            CREATURE_PTR->ItemAdd(pantsPtr);
            CREATURE_PTR->ItemEquip(pantsPtr);

            auto wandPtr{ item::MiscItemFactory::Make_Wand(material::Wood, material::Nothing) };

            CREATURE_PTR->ItemAdd(wandPtr);
            CREATURE_PTR->ItemEquip(wandPtr);
            return;
        }

        if ((ROLE_ENUM == creature::role::Wolfen) || (ROLE_ENUM == creature::role::Sylavin)
            || (ROLE_ENUM == creature::role::Firebrand))
        {
            auto const SKIN_ITEM_PTR{ item::armor::ArmorFactory::Instance()->Make_Skin(
                ((ROLE_ENUM == creature::role::Wolfen) ? item::material::Hide
                                                       : item::material::Scale),
                1_rank,
                false) };

            CREATURE_PTR->ItemAdd(SKIN_ITEM_PTR);
            CREATURE_PTR->ItemEquip(SKIN_ITEM_PTR);
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
        auto const & BODY{ CREATURE_PTR->Body() };

        if (BODY.HasBreath())
        {
            auto const BREATH_WEAPON_ITEM_PTR{ item::weapon::WeaponFactory::Instance()->Make_Breath(
                CREATURE_PTR) };

            CREATURE_PTR->ItemAdd(BREATH_WEAPON_ITEM_PTR);
            CREATURE_PTR->ItemEquip(BREATH_WEAPON_ITEM_PTR);
        }

        if (BODY.HasClaws())
        {
            auto const CLAWS_WEAPON_ITEM_PTR{ item::weapon::WeaponFactory::Instance()->Make_Claws(
                CREATURE_PTR) };

            CREATURE_PTR->ItemAdd(CLAWS_WEAPON_ITEM_PTR);
            CREATURE_PTR->ItemEquip(CLAWS_WEAPON_ITEM_PTR);
        }

        if (BODY.HasBite())
        {
            auto const BITE_WEAPON_ITEM_PTR{ item::weapon::WeaponFactory::Instance()->Make_Bite(
                CREATURE_PTR) };

            CREATURE_PTR->ItemAdd(BITE_WEAPON_ITEM_PTR);
            CREATURE_PTR->ItemEquip(BITE_WEAPON_ITEM_PTR);
        }

        if ((BODY.IsHumanoid()) && (CREATURE_PTR->IsPixie() == false))
        {
            auto const FISTS_WEAPON_ITEM_PTR{
                item::weapon::WeaponFactory::Instance()->Make_Fists()
            };

            CREATURE_PTR->ItemAdd(FISTS_WEAPON_ITEM_PTR);
            CREATURE_PTR->ItemEquip(FISTS_WEAPON_ITEM_PTR);
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

    item::material::Enum Initial::HardOrSoftLeatherRand()
    {
        if (misc::random::Bool())
        {
            return item::material::SoftLeather;
        }
        else
        {
            return item::material::HardLeather;
        }
    }

} // namespace player
} // namespace heroespath
