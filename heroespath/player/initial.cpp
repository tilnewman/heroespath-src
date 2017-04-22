//
// initial.cpp
//
#include "initial.hpp"

#include "sfml-util/gui/creature-image-manager.hpp"

#include "heroespath/player/character.hpp"
#include "heroespath/log-macros.hpp"
#include "heroespath/game-data-file.hpp"
#include "heroespath/item/weapon-factory.hpp"
#include "heroespath/item/armor-factory.hpp"
#include "heroespath/item/misc-item-factory.hpp"
#include "heroespath/creature/creature.hpp"

#include <exception>
#include <sstream>
#include <string>


namespace heroespath
{
namespace player
{

    void Initial::Setup(CharacterPtrC_t characterPtrC)
    {
        SetupInventory(characterPtrC);
        characterPtrC->SetCurrentWeaponsToBest();
        SetStartingHealth(characterPtrC);
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

             std::ostringstream ss;
             ss << "heroespath::player::Initial::EnsureValidImageFilename() for \"" << characterPtrC->Name() << "\", race=" << characterPtrC->Race().Name() << ", role=" << characterPtrC->Role().Name() << ", sex=" << characterPtrC->SexName() << " found no image filename yet assigned.  Setting the randomly chosen image filename: \"" << FILENAME << "\"";
             M_HP_LOG(ss.str());
        }
    }


    void Initial::SetupInventory(CharacterPtrC_t characterPtrC)
    {
        EquipBodyParts(characterPtrC);

        using namespace item;

        if (characterPtrC->Role().Which() == creature::role::Knight)
        {
            ItemSPtr_t cutlassSPtr(weapon::WeaponFactory::Make_Sword(weapon::sword_type::Cutlass, material::Steel, material::Wood));
            characterPtrC->ItemAdd(cutlassSPtr);
            characterPtrC->ItemEquip(cutlassSPtr);

            ItemSPtr_t bootsSPtr(armor::ArmorFactory::Make_Boots(armor::base_type::Plain, material::SoftLeather, material::Nothing));
            characterPtrC->ItemAdd(bootsSPtr);
            characterPtrC->ItemEquip(bootsSPtr);

            ItemSPtr_t shirtSPtr(armor::ArmorFactory::Make_Shirt(armor::base_type::Plain, material::HardLeather));
            characterPtrC->ItemAdd(shirtSPtr);
            characterPtrC->ItemEquip(shirtSPtr);

            ItemSPtr_t pantsSPtr(armor::ArmorFactory::Make_Pants(armor::base_type::Plain, material::SoftLeather));
            characterPtrC->ItemAdd(pantsSPtr);
            characterPtrC->ItemEquip(pantsSPtr);

            ItemSPtr_t helmSPtr(armor::ArmorFactory::Make_Helm(armor::helm_type::Great, material::Tin, material::Nothing));
            characterPtrC->ItemAdd(helmSPtr);
            characterPtrC->ItemEquip(helmSPtr);

            return;
        }

        if (characterPtrC->Role().Which() == creature::role::Beastmaster)
        {
            ItemSPtr_t shortswordSPtr(weapon::WeaponFactory::Make_Sword(weapon::sword_type::Shortsword, material::Steel, material::Wood));
            characterPtrC->ItemAdd(shortswordSPtr);
            characterPtrC->ItemEquip(shortswordSPtr);

            ItemSPtr_t bootsSPtr(armor::ArmorFactory::Make_Boots(armor::base_type::Plain, material::SoftLeather, material::Nothing));
            characterPtrC->ItemAdd(bootsSPtr);
            characterPtrC->ItemEquip(bootsSPtr);

            ItemSPtr_t shirtSPtr(armor::ArmorFactory::Make_Shirt(armor::base_type::Plain, material::HardLeather));
            characterPtrC->ItemAdd(shirtSPtr);
            characterPtrC->ItemEquip(shirtSPtr);

            ItemSPtr_t pantsSPtr(armor::ArmorFactory::Make_Pants(armor::base_type::Plain, material::SoftLeather));
            characterPtrC->ItemAdd(pantsSPtr);
            characterPtrC->ItemEquip(pantsSPtr);

            ItemSPtr_t wandSPtr(misc::ItemFactory::Make_Wand(material::Wood, material::Nothing, characterPtrC->IsPixie()));
            characterPtrC->ItemAdd(wandSPtr);
            characterPtrC->ItemEquip(wandSPtr);

            return;
        }

        if (characterPtrC->Role().Which() == creature::role::Archer)
        {
            ItemSPtr_t longbowSPtr(weapon::WeaponFactory::Make_Projectile(weapon::projectile_type::Longbow, material::Nothing));
            characterPtrC->ItemAdd(longbowSPtr);
            characterPtrC->ItemEquip(longbowSPtr);

            ItemSPtr_t bootsSPtr(armor::ArmorFactory::Make_Boots(armor::base_type::Plain, material::SoftLeather, material::Nothing));
            characterPtrC->ItemAdd(bootsSPtr);
            characterPtrC->ItemEquip(bootsSPtr);

            ItemSPtr_t shirtSPtr(armor::ArmorFactory::Make_Shirt(armor::base_type::Plain, material::SoftLeather));
            characterPtrC->ItemAdd(shirtSPtr);
            characterPtrC->ItemEquip(shirtSPtr);

            ItemSPtr_t pantsSPtr(armor::ArmorFactory::Make_Pants(armor::base_type::Plain, material::SoftLeather));
            characterPtrC->ItemAdd(pantsSPtr);
            characterPtrC->ItemEquip(pantsSPtr);

            return;
        }

        if (characterPtrC->Role().Which() == creature::role::Bard)
        {
            ItemSPtr_t bootsSPtr(armor::ArmorFactory::Make_Boots(armor::base_type::Plain,
                                                                 material::SoftLeather,
                                                                 material::Nothing,
                                                                 characterPtrC->IsPixie()));

            characterPtrC->ItemAdd(bootsSPtr);
            characterPtrC->ItemEquip(bootsSPtr);

            ItemSPtr_t shirtSPtr(armor::ArmorFactory::Make_Shirt(armor::base_type::Plain, material::Cloth, characterPtrC->IsPixie()));
            characterPtrC->ItemAdd(shirtSPtr);
            characterPtrC->ItemEquip(shirtSPtr);

            ItemSPtr_t pantsSPtr(armor::ArmorFactory::Make_Pants(armor::base_type::Plain, material::SoftLeather, characterPtrC->IsPixie()));
            characterPtrC->ItemAdd(pantsSPtr);
            characterPtrC->ItemEquip(pantsSPtr);

            ItemSPtr_t panfluteSPtr(misc::ItemFactory::Make_Instrument(misc::instrument_type::Panflute, characterPtrC->IsPixie()));
            characterPtrC->ItemAdd(panfluteSPtr);
            characterPtrC->ItemEquip(panfluteSPtr);

            return;
        }

        if (characterPtrC->Role().Which() == creature::role::Thief)
        {
            ItemSPtr_t daggerSPtr(weapon::WeaponFactory::Make_Knife(true, sfml_util::Size::Small, material::Steel, material::Wood, characterPtrC->IsPixie()));
            characterPtrC->ItemAdd(daggerSPtr);
            characterPtrC->ItemEquip(daggerSPtr);

            ItemSPtr_t bootsSPtr(armor::ArmorFactory::Make_Boots(armor::base_type::Plain, material::SoftLeather, material::Nothing, characterPtrC->IsPixie()));
            characterPtrC->ItemAdd(bootsSPtr);
            characterPtrC->ItemEquip(bootsSPtr);

            ItemSPtr_t shirtSPtr(armor::ArmorFactory::Make_Shirt(armor::base_type::Plain, material::Cloth, characterPtrC->IsPixie()));
            characterPtrC->ItemAdd(shirtSPtr);
            characterPtrC->ItemEquip(shirtSPtr);

            ItemSPtr_t pantsSPtr(armor::ArmorFactory::Make_Pants(armor::base_type::Plain, material::SoftLeather, characterPtrC->IsPixie()));
            characterPtrC->ItemAdd(pantsSPtr);
            characterPtrC->ItemEquip(pantsSPtr);

            return;
        }

        if (characterPtrC->Role().Which() == creature::role::Cleric)
        {
            ItemSPtr_t bootsSPtr(armor::ArmorFactory::Make_Boots(armor::base_type::Plain, material::SoftLeather, material::Nothing, characterPtrC->IsPixie()));
            characterPtrC->ItemAdd(bootsSPtr);
            characterPtrC->ItemEquip(bootsSPtr);

            ItemSPtr_t shirtSPtr(armor::ArmorFactory::Make_Shirt(armor::base_type::Plain, material::Cloth, characterPtrC->IsPixie()));
            characterPtrC->ItemAdd(shirtSPtr);
            characterPtrC->ItemEquip(shirtSPtr);

            ItemSPtr_t pantsSPtr(armor::ArmorFactory::Make_Pants(armor::base_type::Plain, material::SoftLeather, characterPtrC->IsPixie()));
            characterPtrC->ItemAdd(pantsSPtr);
            characterPtrC->ItemEquip(pantsSPtr);

            ItemSPtr_t wandSPtr(misc::ItemFactory::Make_Wand(material::Glass, material::Nothing, characterPtrC->IsPixie()));
            characterPtrC->ItemAdd(wandSPtr);
            characterPtrC->ItemEquip(wandSPtr);

            return;
        }

        if (characterPtrC->Role().Which() == creature::role::Sorcerer)
        {
            ItemSPtr_t bootsSPtr(armor::ArmorFactory::Make_Boots(armor::base_type::Plain, material::SoftLeather, material::Nothing, characterPtrC->IsPixie()));
            characterPtrC->ItemAdd(bootsSPtr);
            characterPtrC->ItemEquip(bootsSPtr);

            ItemSPtr_t shirtSPtr(armor::ArmorFactory::Make_Shirt(armor::base_type::Plain, material::Cloth, characterPtrC->IsPixie()));
            characterPtrC->ItemAdd(shirtSPtr);
            characterPtrC->ItemEquip(shirtSPtr);

            ItemSPtr_t pantsSPtr(armor::ArmorFactory::Make_Pants(armor::base_type::Plain, material::SoftLeather, characterPtrC->IsPixie()));
            characterPtrC->ItemAdd(pantsSPtr);
            characterPtrC->ItemEquip(pantsSPtr);

            ItemSPtr_t wandSPtr(misc::ItemFactory::Make_Wand(material::Wood, material::Nothing, characterPtrC->IsPixie()));
            characterPtrC->ItemAdd(wandSPtr);
            characterPtrC->ItemEquip(wandSPtr);

            return;
        }

        if ((characterPtrC->Role().Which() == creature::role::Wolfen) ||
            (characterPtrC->Role().Which() == creature::role::Sylavin) ||
            (characterPtrC->Role().Which() == creature::role::Firebrand))
        {
            auto const SKIN_ITEM_SPTR{ item::armor::ArmorFactory::Instance()->Make_Skin(((characterPtrC->Role().Which() == creature::role::Wolfen) ? item::material::Hide : item::material::Scale), 1, false) };
            characterPtrC->ItemAdd(SKIN_ITEM_SPTR);
            characterPtrC->ItemEquip(SKIN_ITEM_SPTR);
            return;
        }


        std::ostringstream ss;
        ss << "heroespath::player::Initial::SetupInventory(\"" << characterPtrC->Name() << "\", race=" << characterPtrC->Race().Name() << ", role=" << characterPtrC->Role().Name() << ")  failed to assign any items.";
        throw std::runtime_error(ss.str());
    }


    void Initial::EquipBodyParts(CharacterPtrC_t characterPtrC)
    {
        if (characterPtrC->Body().HasBreath())
        {
            auto const BREATH_WEAPON_ITEM_SPTR(item::weapon::WeaponFactory::Instance()->Make_Breath(characterPtrC));
            characterPtrC->ItemAdd(BREATH_WEAPON_ITEM_SPTR);
            characterPtrC->ItemEquip(BREATH_WEAPON_ITEM_SPTR);
        }

        if (characterPtrC->Body().HasClaws())
        {
            auto const CLAWS_WEAPON_ITEM_SPTR(item::weapon::WeaponFactory::Instance()->Make_Claws(characterPtrC));
            characterPtrC->ItemAdd(CLAWS_WEAPON_ITEM_SPTR);
            characterPtrC->ItemEquip(CLAWS_WEAPON_ITEM_SPTR);
        }

        if (characterPtrC->Body().HasBite())
        {
            auto const BITE_WEAPON_ITEM_SPTR(item::weapon::WeaponFactory::Instance()->Make_Bite(characterPtrC));
            characterPtrC->ItemAdd(BITE_WEAPON_ITEM_SPTR);
            characterPtrC->ItemEquip(BITE_WEAPON_ITEM_SPTR);
        }

        if ((characterPtrC->Body().IsHumanoid()) && (characterPtrC->IsPixie() == false))
        {
            auto const FISTS_WEAPON_ITEM_SPTR(item::weapon::WeaponFactory::Instance()->Make_Fists());
            characterPtrC->ItemAdd(FISTS_WEAPON_ITEM_SPTR);
            characterPtrC->ItemEquip(FISTS_WEAPON_ITEM_SPTR);
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
}
}
