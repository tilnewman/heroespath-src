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

#include "sfml-util/gui/creature-image-manager.hpp"

#include "game/player/character.hpp"
#include "game/log-macros.hpp"
#include "game/game-data-file.hpp"
#include "game/item/weapon-factory.hpp"
#include "game/item/armor-factory.hpp"
#include "game/item/misc-item-factory.hpp"
#include "game/creature/creature.hpp"
#include "game/spell/spell-enum.hpp"

#include <exception>
#include <sstream>
#include <string>


namespace game
{
namespace player
{

    void Initial::Setup(CharacterPtrC_t characterPtrC)
    {
        SetupInventory(characterPtrC);
        characterPtrC->SetCurrentWeaponsToBest();
        SetStartingHealth(characterPtrC);
        SetStartingMana(characterPtrC);
        EnsureValidImageFilename(characterPtrC);
    }


    void Initial::EnsureValidImageFilename(CharacterPtrC_t characterPtrC)
    {
        if (characterPtrC->ImageFilename().empty())
        {
             const std::string FILENAME(sfml_util::gui::CreatureImageManager::Instance()->GetFilename(characterPtrC->Race().Which(),
                                                                                                      characterPtrC->Role().Which(),
                                                                                                      characterPtrC->Sex(),
                                                                                                      true));

             characterPtrC->ImageFilename(FILENAME);
        }
    }


    void Initial::SetupInventory(CharacterPtrC_t characterPtrC)
    {
        EquipBodyParts(characterPtrC);

        using namespace item;

        //TEMP TODO REMOVE -only added to test inventory
        if (characterPtrC->IsBeast() == false)
        {
            characterPtrC->CoinsAdj(100);
            characterPtrC->GemsAdj(100);
            characterPtrC->MeteorShardsAdj(100);
        }

        auto const ROLE_ENUM{ characterPtrC->Role().Which() };

        if (ROLE_ENUM == creature::role::Knight)
        {
            ItemPtr_t cutlassPtr(weapon::WeaponFactory::Make_Sword(
                weapon::sword_type::Cutlass, material::Steel, material::Wood));

            characterPtrC->ItemAdd(cutlassPtr);
            characterPtrC->ItemEquip(cutlassPtr);

            ItemPtr_t bootsPtr(armor::ArmorFactory::Make_Boots(
                armor::base_type::Plain, material::SoftLeather, material::Nothing));

            characterPtrC->ItemAdd(bootsPtr);
            characterPtrC->ItemEquip(bootsPtr);

            ItemPtr_t shirtPtr(armor::ArmorFactory::Make_Shirt(
                armor::base_type::Plain, material::HardLeather));

            characterPtrC->ItemAdd(shirtPtr);
            characterPtrC->ItemEquip(shirtPtr);

            ItemPtr_t pantsPtr(armor::ArmorFactory::Make_Pants(
                armor::base_type::Plain, material::SoftLeather));

            characterPtrC->ItemAdd(pantsPtr);
            characterPtrC->ItemEquip(pantsPtr);

            ItemPtr_t helmPtr(armor::ArmorFactory::Make_Helm(
                armor::helm_type::Great, material::HardLeather, material::Nothing));

            characterPtrC->ItemAdd(helmPtr);
            characterPtrC->ItemEquip(helmPtr);

            return;
        }

        if (ROLE_ENUM == creature::role::Beastmaster)
        {
            ItemPtr_t shortswordPtr(weapon::WeaponFactory::Make_Sword(
                weapon::sword_type::Shortsword, material::Steel, material::Wood));

            characterPtrC->ItemAdd(shortswordPtr);
            characterPtrC->ItemEquip(shortswordPtr);

            ItemPtr_t bootsPtr(armor::ArmorFactory::Make_Boots(
                armor::base_type::Plain, material::SoftLeather, material::Nothing));

            characterPtrC->ItemAdd(bootsPtr);
            characterPtrC->ItemEquip(bootsPtr);

            ItemPtr_t shirtPtr(armor::ArmorFactory::Make_Shirt(
                armor::base_type::Plain, material::HardLeather));

            characterPtrC->ItemAdd(shirtPtr);
            characterPtrC->ItemEquip(shirtPtr);

            ItemPtr_t pantsPtr(armor::ArmorFactory::Make_Pants(
                armor::base_type::Plain, material::SoftLeather));

            characterPtrC->ItemAdd(pantsPtr);
            characterPtrC->ItemEquip(pantsPtr);

            ItemPtr_t wandPtr(misc::ItemFactory::Make_Wand(
                material::Wood, material::Nothing, characterPtrC->IsPixie()));

            characterPtrC->ItemAdd(wandPtr);
            characterPtrC->ItemEquip(wandPtr);

            return;
        }

        if (ROLE_ENUM == creature::role::Archer)
        {
            ItemPtr_t longbowPtr(weapon::WeaponFactory::Make_Projectile(
                weapon::projectile_type::Longbow, material::Nothing));

            characterPtrC->ItemAdd(longbowPtr);
            characterPtrC->ItemEquip(longbowPtr);

            ItemPtr_t bootsPtr(armor::ArmorFactory::Make_Boots(
                armor::base_type::Plain, material::SoftLeather, material::Nothing));

            characterPtrC->ItemAdd(bootsPtr);
            characterPtrC->ItemEquip(bootsPtr);

            ItemPtr_t shirtPtr(armor::ArmorFactory::Make_Shirt(
                armor::base_type::Plain, material::SoftLeather));

            characterPtrC->ItemAdd(shirtPtr);
            characterPtrC->ItemEquip(shirtPtr);

            ItemPtr_t pantsPtr(armor::ArmorFactory::Make_Pants(
                armor::base_type::Plain, material::SoftLeather));

            characterPtrC->ItemAdd(pantsPtr);
            characterPtrC->ItemEquip(pantsPtr);

            return;
        }

        if (ROLE_ENUM == creature::role::Bard)
        {
            ItemPtr_t bootsPtr(armor::ArmorFactory::Make_Boots(armor::base_type::Plain,
                                                               material::SoftLeather,
                                                               material::Nothing,
                                                               characterPtrC->IsPixie()));

            characterPtrC->ItemAdd(bootsPtr);
            characterPtrC->ItemEquip(bootsPtr);

            ItemPtr_t shirtPtr(armor::ArmorFactory::Make_Shirt(
                armor::base_type::Plain, material::Cloth, characterPtrC->IsPixie()));

            characterPtrC->ItemAdd(shirtPtr);
            characterPtrC->ItemEquip(shirtPtr);

            ItemPtr_t pantsPtr(armor::ArmorFactory::Make_Pants(
                armor::base_type::Plain, material::SoftLeather, characterPtrC->IsPixie()));

            characterPtrC->ItemAdd(pantsPtr);
            characterPtrC->ItemEquip(pantsPtr);

            ItemPtr_t panflutePtr(misc::ItemFactory::Make_DrumLute(characterPtrC->IsPixie()));

            characterPtrC->ItemAdd(panflutePtr);
            characterPtrC->ItemEquip(panflutePtr);

            return;
        }

        if (ROLE_ENUM == creature::role::Thief)
        {
            ItemPtr_t daggerPtr(weapon::WeaponFactory::Make_Knife(
                true,
                sfml_util::Size::Small,
                material::Steel,
                material::Wood,
                characterPtrC->IsPixie()));

            characterPtrC->ItemAdd(daggerPtr);
            characterPtrC->ItemEquip(daggerPtr);

            ItemPtr_t bootsPtr(armor::ArmorFactory::Make_Boots(
                armor::base_type::Plain,
                material::SoftLeather,
                material::Nothing,
                characterPtrC->IsPixie()));

            characterPtrC->ItemAdd(bootsPtr);
            characterPtrC->ItemEquip(bootsPtr);

            ItemPtr_t shirtPtr(armor::ArmorFactory::Make_Shirt(
                armor::base_type::Plain, material::Cloth, characterPtrC->IsPixie()));
            characterPtrC->ItemAdd(shirtPtr);
            characterPtrC->ItemEquip(shirtPtr);

            ItemPtr_t pantsPtr(armor::ArmorFactory::Make_Pants(
                armor::base_type::Plain, material::SoftLeather, characterPtrC->IsPixie()));
            characterPtrC->ItemAdd(pantsPtr);
            characterPtrC->ItemEquip(pantsPtr);

            return;
        }

        if (ROLE_ENUM == creature::role::Cleric)
        {
            ItemPtr_t bootsPtr(armor::ArmorFactory::Make_Boots(
                armor::base_type::Plain,
                material::SoftLeather,
                material::Nothing,
                characterPtrC->IsPixie()));

            characterPtrC->ItemAdd(bootsPtr);
            characterPtrC->ItemEquip(bootsPtr);

            ItemPtr_t shirtPtr(armor::ArmorFactory::Make_Shirt(
                armor::base_type::Plain, material::Cloth, characterPtrC->IsPixie()));

            characterPtrC->ItemAdd(shirtPtr);
            characterPtrC->ItemEquip(shirtPtr);

            ItemPtr_t pantsPtr(armor::ArmorFactory::Make_Pants(
                armor::base_type::Plain, material::SoftLeather, characterPtrC->IsPixie()));

            characterPtrC->ItemAdd(pantsPtr);
            characterPtrC->ItemEquip(pantsPtr);

            ItemPtr_t wandPtr(misc::ItemFactory::Make_Wand(
                material::Glass, material::Nothing, characterPtrC->IsPixie()));

            characterPtrC->ItemAdd(wandPtr);
            characterPtrC->ItemEquip(wandPtr);

            characterPtrC->SpellAdd(spell::Spells::Awaken);
            characterPtrC->SpellAdd(spell::Spells::Bandage);
            characterPtrC->SpellAdd(spell::Spells::ClearMind);
            characterPtrC->SpellAdd(spell::Spells::Lift);
            characterPtrC->SpellAdd(spell::Spells::Sleep);
            characterPtrC->SpellAdd(spell::Spells::Antidote);

            return;
        }

        if (ROLE_ENUM == creature::role::Sorcerer)
        {
            ItemPtr_t bootsPtr(armor::ArmorFactory::Make_Boots(
                armor::base_type::Plain,
                material::SoftLeather,
                material::Nothing,
                characterPtrC->IsPixie()));

            characterPtrC->ItemAdd(bootsPtr);
            characterPtrC->ItemEquip(bootsPtr);

            ItemPtr_t shirtPtr(armor::ArmorFactory::Make_Shirt(
                armor::base_type::Plain, material::Cloth, characterPtrC->IsPixie()));

            characterPtrC->ItemAdd(shirtPtr);
            characterPtrC->ItemEquip(shirtPtr);

            ItemPtr_t pantsPtr(armor::ArmorFactory::Make_Pants(
                armor::base_type::Plain, material::SoftLeather, characterPtrC->IsPixie()));

            characterPtrC->ItemAdd(pantsPtr);
            characterPtrC->ItemEquip(pantsPtr);

            ItemPtr_t wandPtr(misc::ItemFactory::Make_Wand(
                material::Wood, material::Nothing, characterPtrC->IsPixie()));

            characterPtrC->ItemAdd(wandPtr);
            characterPtrC->ItemEquip(wandPtr);

            characterPtrC->SpellAdd(spell::Spells::Daze);
            characterPtrC->SpellAdd(spell::Spells::Panic);
            characterPtrC->SpellAdd(spell::Spells::Sleep);
            characterPtrC->SpellAdd(spell::Spells::Awaken);
            characterPtrC->SpellAdd(spell::Spells::Poison);
            characterPtrC->SpellAdd(spell::Spells::Sparks);
            characterPtrC->SpellAdd(spell::Spells::Trip);

            //TODO TEMP REMOVE -this line was only added so that mutli-creature
            //targeting spells could be tested
            characterPtrC->SpellAdd(spell::Spells::PoisonCloud);
            return;
        }

        if ((ROLE_ENUM == creature::role::Wolfen) ||
            (ROLE_ENUM == creature::role::Sylavin) ||
            (ROLE_ENUM == creature::role::Firebrand))
        {
            auto const SKIN_ITEM_PTR{ item::armor::ArmorFactory::Instance()->Make_Skin(
                ((ROLE_ENUM == creature::role::Wolfen) ?
                    item::material::Hide : item::material::Scale), 1, false) };

            characterPtrC->ItemAdd(SKIN_ITEM_PTR);
            characterPtrC->ItemEquip(SKIN_ITEM_PTR);
            return;
        }


        std::ostringstream ss;
        ss << "game::player::Initial::SetupInventory(\"" << characterPtrC->Name() << "\", race="
            << characterPtrC->Race().Name() << ", role=" << characterPtrC->Role().Name()
            << ")  failed to assign any items.";

        throw std::runtime_error(ss.str());
    }


    void Initial::EquipBodyParts(CharacterPtrC_t characterPtrC)
    {
        auto const & BODY{ characterPtrC->Body() };

        if (BODY.HasBreath())
        {
            auto const BREATH_WEAPON_ITEM_PTR(item::weapon::WeaponFactory::Instance()->Make_Breath(characterPtrC));
            characterPtrC->ItemAdd(BREATH_WEAPON_ITEM_PTR);
            characterPtrC->ItemEquip(BREATH_WEAPON_ITEM_PTR);
        }

        if (BODY.HasClaws())
        {
            auto const CLAWS_WEAPON_ITEM_PTR(item::weapon::WeaponFactory::Instance()->Make_Claws(characterPtrC));
            characterPtrC->ItemAdd(CLAWS_WEAPON_ITEM_PTR);
            characterPtrC->ItemEquip(CLAWS_WEAPON_ITEM_PTR);
        }

        if (BODY.HasBite())
        {
            auto const BITE_WEAPON_ITEM_PTR(item::weapon::WeaponFactory::Instance()->Make_Bite(characterPtrC));
            characterPtrC->ItemAdd(BITE_WEAPON_ITEM_PTR);
            characterPtrC->ItemEquip(BITE_WEAPON_ITEM_PTR);
        }

        if ((BODY.IsHumanoid()) && (characterPtrC->IsPixie() == false))
        {
            auto const FISTS_WEAPON_ITEM_PTR(item::weapon::WeaponFactory::Instance()->Make_Fists());
            characterPtrC->ItemAdd(FISTS_WEAPON_ITEM_PTR);
            characterPtrC->ItemEquip(FISTS_WEAPON_ITEM_PTR);
        }
    }


    stats::Health_t Initial::GetStartingHealth(CharacterCPtrC_t CHARACTER_CPTRC)
    {
        std::ostringstream ss;
        ss << "heroespath-player-race-health-initial-" << creature::race::ToString(CHARACTER_CPTRC->Race().Which());
        const stats::Health_t HEALTH_BASE( GameDataFile::Instance()->GetCopyInt(ss.str()) );

        ss.str("");
        ss << "heroespath-player-role-health-adjustment-initial-" << creature::role::ToString(CHARACTER_CPTRC->Role().Which());
        return HEALTH_BASE + GameDataFile::Instance()->GetCopyInt(ss.str());
    }


    void Initial::SetStartingHealth(CharacterPtrC_t characterPtrC)
    {
        const stats::Health_t STARTING_HEALTH(GetStartingHealth(characterPtrC));
        characterPtrC->HealthNormalSet(STARTING_HEALTH);
        characterPtrC->HealthCurrentSet(STARTING_HEALTH);
    }


    void Initial::SetStartingMana(CharacterPtrC_t characterPtrC)
    {
        if ((characterPtrC->Role().Which() == creature::role::Sorcerer) ||
            (characterPtrC->Role().Which() == creature::role::Cleric))
        {
            auto const STARTING_MANA{ characterPtrC->Stats().Int().Current() / 2 };
            characterPtrC->ManaNormalSet(STARTING_MANA);
            characterPtrC->ManaCurrentSet(STARTING_MANA);
        }
    }

}
}
