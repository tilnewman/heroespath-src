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

#include "game/log-macros.hpp"
#include "game/creature/enchantment-warehouse.hpp"
#include "game/item/item.hpp"

#include "misc/assertlogandthrow.hpp"


namespace game
{
namespace creature
{

    std::unique_ptr<EnchantmentFactory> EnchantmentFactory::instanceUPtr_{ nullptr };


    EnchantmentFactory::EnchantmentFactory()
    {
        M_HP_LOG_DBG("Singleton Construction: EnchantmentFactory");
    }


    EnchantmentFactory::~EnchantmentFactory()
    {
        M_HP_LOG_DBG("Singleton Destruction: EnchantmentFactory");
    }


    EnchantmentFactory * EnchantmentFactory::Instance()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            M_HP_LOG_WRN("Singleton Instance() before Acquire(): EnchantmentFactory");
            Acquire();
        }

        return instanceUPtr_.get();
    }


    void EnchantmentFactory::Acquire()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            instanceUPtr_.reset(new EnchantmentFactory);
        }
        else
        {
            M_HP_LOG_WRN("Singleton Acquire() after Construction: EnchantmentFactory");
        }
    }


    void EnchantmentFactory::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((instanceUPtr_.get() != nullptr),
            "game::creature::EnchantmentFactory::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }


    item::ItemPtr_t EnchantmentFactory::MakeStoreAttachReturn(item::ItemPtr_t itemPtr) const
    {
        M_ASSERT_OR_LOGANDTHROW_SS((itemPtr != nullptr),
            "game::creature::EnchantmentFactory::MakeStoreAttachReturn(itemPtr version)"
            << " was given a null itemPtr.");

        if ((itemPtr->UniqueType() != item::unique_type::NotUnique) &&
            (itemPtr->UniqueType() != item::unique_type::Count))
        {
            auto const MADE_ECHANTMENT_PTR{ MakeFromUniqueType(itemPtr->UniqueType(),
                                                               itemPtr->MaterialPrimary()) };
            if (MADE_ECHANTMENT_PTR != nullptr)
            {
                return MakeStoreAttachReturn(itemPtr, MADE_ECHANTMENT_PTR);
            }
        }

        if ((itemPtr->NamedType() != item::named_type::NotNamed) &&
            (itemPtr->NamedType() != item::named_type::Count))
        {
            auto const MADE_ECHANTMENT_PTR{ MakeFromNamedType(
                itemPtr->NamedType(),
                itemPtr->MaterialPrimary(),
                (itemPtr->Category() & item::category::Armor),
                (itemPtr->Category() & item::category::Weapon)) };

            if (MADE_ECHANTMENT_PTR != nullptr)
            {
                return MakeStoreAttachReturn(itemPtr, MADE_ECHANTMENT_PTR);
            }
        }

        if (itemPtr->SetType() != item::set_type::NotASet)
        {
            auto const MADE_ECHANTMENT_PTR{ MakeFromSetType(itemPtr->SetType()) };
            if (MADE_ECHANTMENT_PTR != nullptr)
            {
                return MakeStoreAttachReturn(itemPtr, MADE_ECHANTMENT_PTR);
            }
        }

        if (itemPtr->MiscType() != item::misc_type::NotMisc)
        {
            auto const MADE_ECHANTMENT_PTR{ MakeFromMiscType(itemPtr->MiscType()) };
            if (MADE_ECHANTMENT_PTR != nullptr)
            {
                return MakeStoreAttachReturn(itemPtr, MADE_ECHANTMENT_PTR);
            }
        }

        M_HP_LOG_ERR("game::creature::EnchantmentFactory::MakeStoreAttachReturn"
            << "(itemPtr version) with misc_type="
            << item::misc_type::ToString(itemPtr->MiscType())
            << " -unable to create an enchantment based on any of the item's types.");

        return itemPtr;
    }


    item::ItemPtr_t EnchantmentFactory::MakeStoreAttachReturn(
        item::ItemPtr_t             itemPtr,
        const EnchantmentType::Enum TYPE,
        const stats::TraitSet &     TRAIT_SET,
        const UseInfo &             USE_INFO,
        const SummonInfo &          SUMMON_INFO) const
    {
        M_ASSERT_OR_LOGANDTHROW_SS((itemPtr != nullptr),
            "game::creature::EnchantmentFactory::MakeStoreAttachReturn(itemPtr/details version)"
            << " was given a null itemPtr.");

        return MakeStoreAttachReturn(itemPtr, new Enchantment(TYPE,
                                                              TRAIT_SET,
                                                              USE_INFO,
                                                              SUMMON_INFO));
    }


    item::ItemPtr_t EnchantmentFactory::MakeStoreAttachReturn(
        item::ItemPtr_t     itemPtr,
        Enchantment * const enchantmentPtr) const
    {
        M_ASSERT_OR_LOGANDTHROW_SS((itemPtr != nullptr),
            "game::creature::EnchantmentFactory::MakeStoreAttachReturn(itemPtr/enchPtr version)"
            << " was given a null itemPtr.");

        M_ASSERT_OR_LOGANDTHROW_SS((enchantmentPtr != nullptr),
            "game::creature::EnchantmentFactory::MakeStoreAttachReturn(item=" << itemPtr->Name()
            << ") was given a null enchantmentPtr.");
        
        itemPtr->EnchantmentAdd( EnchantmentWarehouse::Instance()->Store(enchantmentPtr) );

        return itemPtr;
    }

/*
Basilisk Tonge 	            held:  8% magic cast, 13%/13% magic effect/resist, 	*** IF USED: turns creatures to stone, combat only, one use
Berserker’s Beard           held: -33% int, -16% cha, 25% melee damage, +8% encounter, 18% magic resist		
Bishop’s Hanky 	            held: 8% lck, 13% int, 18% cha, 8%/16%/16% magic cast/effect/resist, 16%/28% bless cast/effect, 22% curse resist	
Bleeding  Trophy 	        held: 3x material based -% lck, 3x material based +% curse cast,  5% + 4x material based +% curse effect, material based +% find magic, curse without items	
Bloody Dragon Scale 	    held: 50% animal resist, 10% find coin, 10% find magic, -26% cha 	
Bottle of Banshee Screams 	held: -20% cha, -20% int, -6% encounter, 	*** IF USED: panics enemies, ten uses
Bracelet Crown              equipped: 4x material based +% stat		
Bracelet Feather            equipped: 4x material based +% stat		
Bracelet Fist               equipped: 4x material based +% stat		
Bracelet Hourglass          equipped: 4x material based +% stat		
Bracelet Key                equipped: 4x material based +% stat		
Bracelet Mask               equipped: 4x material based +% stat		
Brooch Crown                equipped: 5x material based +% stat		
Brooch Feather              equipped: 5x material based +% stat		
Brooch Fist                 equipped: 5x material based +% stat		
Brooch Hourglass            equipped: 5x material based +% stat		
Brooch Key                  equipped: 5x material based +% stat		
Brooch Mask                 equipped: 5x material based +% stat		
Burial Shroud               equipped: -28% cha, -16% lck, 18%/50%/50% curse cast/effect/resist, 33% shadow resist, +18% find religious, 33% bless resist		
Charm Crown                 equipped: 3x material based +% stat		
Charm Feather               equipped: 3x material based +% stat		
Charm Fist                  equipped: 3x material based +% stat		
Charm Hourglass             equipped: 3x material based +% stat		
Charm Key                   equipped: 3x material based +% stat		
Charm Mask                  equipped: 3x material based +% stat		
Chimera Bone 	            held: 18% magic resist, -50% cha, 13% int, 8% find magic	
Cobra Tooth 	            held and NOT flying: 16% melee damage , 36% poison	
Commander’s Cape            equipped: 13% str, 13% armor, 13% curse resist	
Copper Troll 	            held:  13% str, -16% cha, -6% encounter	
Crystal Cat 	            held: 13% spd, 10% lck, -3% encounter	
Crystal Chimes 	            used: 15% mana, removes conditions dazed/asleep-natural/daunted	
Cyclops Eye 	            held: 1/2x material based +% critical, material based -% surprised, 	*** IF USED: reveals all info about an enemy creature, infinite uses
Demon Diary 	            held: 13% shadow damage, 33% shadow resist, 22%/33%/33% curse cast/effect/resist	
Dove Blood Vial 	        used: removes all poison/disease/dazed type conditions, ten uses	
Dragon Tooth Whistle 	    used: panics flying enemy creatures	
Dried Frog                  used -10% mana, cures poison, consumed upon use		
Dried Gecko                 used -10% mana, cures poison, consumed upon use		
Dried Iguana 	            used -10% mana, cures poison, consumed upon use	
Dried Lizard                used -10% mana, cures poison, consumed upon use		
Dried Salamander            used -10% mana, cures poison, consumed upon use		
Dried Skink                 used -10% mana, cures poison, consumed upon use		
Dried Toad                  used -10% mana, cures poison, consumed upon use		
Dried Turtle                used -10% mana, cures poison, consumed upon use		
Druid Leaf 	                equipped: 10% + 3x material based +% cha an int, 2x material based +% mana, 1/2x material based +% magic cast and find religious, material based +% magic effect, material based +% magic resist	
Evil Rabbit’s Foot 	        held: -36% lck, 4% surprised, 6% encounter, -13% find magic, -80%/-666%/666% bless cast/effect/resist, 80%/666%/666% curse cast/effect/resist, -100% animal resist	
Exotic Golden Gong	        used: 20% mana, removes conditions dazed/asleep-either/daunted/panic, heavy	
Fanatic’s Flag 	            held: 13% str, 8% spd, -20% cha, 6% encounter, 8% surprised	
Friar’s Chronicle	        held: 33% bless cast, 6% shadow resist, 8% cha, 8% mana, 6% find religious, bless without items	
Funeral Record 	            held: 33% curse cast, 4% shadow resist, -4% cha, 4% int, 4% find religious, 6% mana, curse without items	
General’s Cape  	        equipped: 18% str, 16% armor, 24% curse resist	
Ghost Sheet 	            equipped:  10% armor, 10% magic resist, -6% spd, -2% encounter	
Giant Owl Eye 	            held:  16% acc, 8% spd, 6% critical, +8% projectile damage, -8% surprise	
Griffin Feather 	        held: 16% spd, 10% str, 13% projectile damage, allows flying	
Hangman’s Noose 	        held: -13% cha, -28% lck, 22%/33%/33% curse cast/effect/resist, 18% shadow damage	
Hawk Eye 	                held: 16% acc, 10% spd, 6% critical, +10% projectile damage, -6% surprised	
Hobgoblin Nose 	            held: -26% cha, 13% lck, -12% surprised, 13% magic resist	
Hobo Ring 	                equipped: material based +% find coins, 1/4x material based +% find religious, 1/2x material based +% find magic	
Holy Epic	                held: 66%/66% bless cast/effect, 16% shadow resist, 18% cha, 18% mana, 16% find religious, bless without items	
Horn of the Horde 	        equipped:  +13% str, -8% encounter, -8% surprise, 	*** IF USED: Daunts or Panics enemies
Imp Tail                    equipped: 20% melee damage, 13% spd		
Islander Headdress 	        equipped: 16% cha, 13% mana, 8%/18% magic cast/resist, 18% curse resist, 13% find religious, -8% encounter, 20% shadow resist	
Jeweled Ankh Necklace 	    equipped: 1.5x material based +% find religious, material based +% find magic, 1/2x material based +% magic resist, material based +% curse cast, 13% + 2x material based +% curse effect and resist	
Jeweled Armband  	        equipped: 2x material based +% armor,	
Jeweled Handbag 	        equipped: 1/2x material based +% find magic and coins, 2x material based +% lck, 	
Jeweled Princess Veil  	    equipped: 4% + material based +% cha, 1/2x material based +% find magic	
King’s Cape  	            equipped: 25% str, 22% armor, 33% curse resist	
Last Rites Scroll	        held: 17%/17%/17% curse cast/effect/resist, 12% shadow resist, -22% cha, 22% int, 12% find religious, 16% mana, curse without items	
Macabre Manuscript 	        held: 33% curse resist, 8% shadow resist, -16% cha, 16% int, 8% find religious, 10% mana, curse without items	
Mad Rat Juju 	            held: 16% cha, 33% int, 33% spd, -18% animal resist	
Magic Horseshoe 	        held: 18% lck, 4% find religious, 8% find magic, 16% find coin	
Magnifying Glass 	        used: reveals all info about an enemy creature, combat only, material based use count	
Mana Amulet  	            equipped: 18% + material based +% mana, 1/2x material based +% magic effect	
Mendicant Ring 	            equipped: -16% cha, 13% find coins, 8% find magic, 18% find religious	
Minotaur Hide 	            held: 33% animal resist, 16% str, 6% power, -28% cha, -16% int	
Monk’s Ring 	            equipped: 6%/13% magic cast/resist, 18% int, 18% cha, 13% find coins, 28% curse resist	
Mortuary Ornament 	        held: 66% shadow resist, 13% find magic, 26% find religious	
Mourner’s Mask 	            equipped: -33% cha, 16%/33%/50% curse cast/effect/resist, 28% shadow resist	
Panther Paw 	            held: 18% spd, 13% str, 13% animal resist	
Pin Arcane Tome Pin 	    equipped: 22% + material based +% int	
Pin Hourglass Pin	        equipped: 22% + material based +% spd	
Pin Jeweled Nymph Pin 	    equipped: 22% + material based +% cha	
Pin King’s Quiver Pin 	    equipped: 22% + material based +% acc	
Pin Leprechaun Clover Pin   equipped: 22% + material based +% lck		
Pin Titan’s Foot Pin 	    equipped: 22% + material based +% str	
Pixie Bell 	                used: 10% mana, removes conditions dazed/asleep-natural	
Priest’s Ring 	            equipped:  8%/18% magic cast/resist, 18% cha, 18% mana, 20% str, 13% bless cast, 26% bless effect	
Rascal Mask 	            equipped: 22% lck, 18% cha, 8% find magic, 16% find coin	
Rattlesnake Tail 	        held: 16% spd, +8% encounter	
Raven Claw 	                held and flying: 13% str, 13% acc, 18% projectile damage	
Razor Fingerclaw  	        equipped: 2x material based +% melee damage	
Reaper Scythe               held: 33% curse effect/resist, if attack with succeeds health gain 1/4 damage		
Regal Captain’s Flag 	    held: 8% str, 8% cha, -6% encounter, +8% surprised, 8% magic resist	
Requiem Register	        held: 33% curse effect, 6% shadow resist, -8% cha, 8% int, 6% find religious, 8% mana, curse without items	   
Ring Brawn Ring  	        equipped: 14% + 2x material based +% str	
Ring Cat Ring  	            equipped: 14% + 2x material based +% lck	
Ring Dryad Ring  	        equipped: 14% + 2x material based +% cha	
Ring Ferret Ring  	        equipped: 14% + 2x material based +% spd	
Ring Sage Ring  	        equipped: 14% + 2x material based +% int	
Ring Target Ring  	        equipped: 14% + 2x material based +% acc	
Royal Scout Spyglass 	    held: 16% acc, -12% surprised	
Saint Cameo Pin 	        equipped: material based +% cha, material based +% find magic, 1/2x material based +% magic effect, material based +% bless cast, 10% + 2x material based +% bless effect	
Saint’s Journal	            held: 33% bless effect, 8% shadow resist, 13% cha, 13% mana, 8% find religious, bless without items	
Sanguine  Relic 	        held: 5x material based +% curse cast, 10% + 5x material based +% curse effect and resist, material based +% find religious, curse without items	
Scorpion Stinger Fingerclaw equipped: -18% cha, +1000% fists damage, 22% melee poison		
Scoundrel Sack 	            equipped: 13% find coin, 6% find magic, 15% lck, 6% backstab	
Sepulture Decoration        held: 33% shadow resist, 8% find magic, 13% find religious		
Shade Cloak 	            equipped: 33% armor, 26% magic resist, -20% spd, -6% encounter	
Shaman Rainmaker 	        held: 16% cha, +8% encounter, 13% find magic, 8%/18% magic cast/effect, 8% bless, 	*** IF USED: 10% mana, infinite uses
Shark Tooth Necklace 	    equipped: 13% str, 18% melee damage	
Siren’s Conch 	            held: 8%/13% magic cast/effect, 13% cha, 	*** IF USED: casts the “Drown” spell on target enemy
Specter Chains 	            held:  18% str, -6% encounter, 	*** IF USED:  panics enemies, infinite uses
Specter Robe 	            equipped: 20% armor, 18% magic resist, -13% spd, -4% encounter	
Sprinter’s Legtie 	        equipped: 22% spd, -8% str	
Swindler’s Bag 	            equipped: 18% find coin, 8% find magic, 20% lck, 4% critical	
Tribal Flag 	            held: -16% cha, +6% encounter, +8% surprised	
Trickster Pouch 	        equipped: 8% find coin, 4% find magic, 10% lck, 18% cha	
Tuning Fork 	            equipped: bard only, material based +% effect and cha	
Turtle Shell 	            held: 3x material based +% armor, very heavy	
Vampire’s Tooth Necklace 	equipped: 13% cha, health gain 1/4 all damage, 28% curse resist 	
Viper Fang Fingerclaw  	    equipped: -13% cha, +500% fists damage, 16% melee poison	
Vulture Gizzard 	        used: cures poison, three uses	
War Trumpet 	            used: 3% + material based +% Panic enemies, then if not,  13% + material based +% Daunt enemies	
Warhorse Marionette 	    held: 18% str, 20% armor, -13% spd, 8% power, -16% int, very heavy	
Weasel Totem 	            held: 28% spd, 18% lck, -6% encounter	
Wolfen Fur 	                held: 28% animal resist, 16% spd, -16% cha, -13% int	
Wraith Talisman 	        held: -16% cha, 8% find magic, 8%/13%/13% magic cast/effect/resist, 13%/33%/33% curse cast/effect/resist, 	*** IF USED: panics enemies, infinite uses
Zombie Seeds 	            used: summons a Ghoul creature to fight for the party, ten uses	
*/

    Enchantment * EnchantmentFactory::MakeFromUniqueType(
        const item::unique_type::Enum UNIQUE_ENUM,
        const item::material::Enum) const
    {
        switch (UNIQUE_ENUM)
        {
            case item::unique_type::BasiliskTonge:
            case item::unique_type::BerserkersBeard:
            case item::unique_type::BishopsHanky:
            case item::unique_type::BleedingTrophy:
            case item::unique_type::BloodyDragonScale:
            case item::unique_type::BottleOfBansheeScreams:
            case item::unique_type::BraceletCrown:
            case item::unique_type::BraceletFeather:
            case item::unique_type::BraceletFist:
            case item::unique_type::BraceletHourglass:
            case item::unique_type::BraceletKey:
            case item::unique_type::BraceletMask:
            case item::unique_type::BroochCrown:
            case item::unique_type::BroochFeather:
            case item::unique_type::BroochFist:
            case item::unique_type::BroochHourglass:
            case item::unique_type::BroochKey:
            case item::unique_type::BroochMask:
            case item::unique_type::BurialShroud:
            case item::unique_type::CharmCrown:
            case item::unique_type::CharmFeather:
            case item::unique_type::CharmFist:
            case item::unique_type::CharmHourglass:
            case item::unique_type::CharmKey:
            case item::unique_type::CharmMask:
            case item::unique_type::ChimeraBone:
            case item::unique_type::CobraTooth:
            case item::unique_type::CommandersCape:
            case item::unique_type::CopperTroll:
            case item::unique_type::CrystalCat:
            case item::unique_type::CrystalChimes:
            case item::unique_type::CyclopsEye:
            case item::unique_type::DemonDiary:
            case item::unique_type::DoveBloodVial:
            case item::unique_type::DragonToothWhistle:
            case item::unique_type::DriedFrog:
            case item::unique_type::DriedGecko:
            case item::unique_type::DriedIguana:
            case item::unique_type::DriedLizard:
            case item::unique_type::DriedSalamander:
            case item::unique_type::DriedSkink:
            case item::unique_type::DriedToad:
            case item::unique_type::DriedTurtle:
            case item::unique_type::DruidLeaf:
            case item::unique_type::EvilRabbitsFoot:
            case item::unique_type::ExoticGoldenGong:
            case item::unique_type::FanaticsFlag:
            case item::unique_type::FriarsChronicle:
            case item::unique_type::FuneralRecord:
            case item::unique_type::GeneralsCape:
            case item::unique_type::GhostSheet:
            case item::unique_type::GiantOwlEye:
            case item::unique_type::GriffinFeather:
            case item::unique_type::HangmansNoose:
            case item::unique_type::HawkEye:
            case item::unique_type::HobgoblinNose:
            case item::unique_type::HoboRing:
            case item::unique_type::HolyEpic:
            case item::unique_type::HornOfTheHorde:
            case item::unique_type::ImpTail:
            case item::unique_type::IslanderHeaddress:
            case item::unique_type::JeweledAnkhNecklace:
            case item::unique_type::JeweledHandbag:
            case item::unique_type::JeweledPrincessVeil:
            case item::unique_type::KingsCape:
            case item::unique_type::LastRitesScroll:
            case item::unique_type::MacabreManuscript:
            case item::unique_type::MadRatJuju:
            case item::unique_type::MagicHorseshoe:
            case item::unique_type::MagnifyingGlass:
            case item::unique_type::MendicantRing:
            case item::unique_type::MinotaurHide:
            case item::unique_type::MonkRing:
            case item::unique_type::MortuaryOrnament:
            case item::unique_type::MournersMask:
            case item::unique_type::PantherPaw:
            case item::unique_type::PinArcaneTome:
            case item::unique_type::PinHourglass:
            case item::unique_type::PinJeweledNymph:
            case item::unique_type::PinKingsQuiver:
            case item::unique_type::PinLeprechaunClover:
            case item::unique_type::PinTitansFoot:
            case item::unique_type::PixieBell:
            case item::unique_type::PriestRing:
            case item::unique_type::RascalMask:
            case item::unique_type::RattlesnakeTail:
            case item::unique_type::RavenClaw:
            case item::unique_type::ReaperScythe:
            case item::unique_type::RegalCaptainsFlag:
            case item::unique_type::RequiemRegister:
            case item::unique_type::RoyalScoutSpyglass:
            case item::unique_type::SaintCameoPin:
            case item::unique_type::SaintsJournal:
            case item::unique_type::SanguineRelic:
            case item::unique_type::ScorpionStingerFingerclaw:
            case item::unique_type::ScoundrelSack:
            case item::unique_type::SepultureDecoration:
            case item::unique_type::ShadeCloak:
            case item::unique_type::ShamanRainmaker:
            case item::unique_type::SharkToothNecklace:
            case item::unique_type::SirenConch:
            case item::unique_type::SpecterChains:
            case item::unique_type::SpecterRobe:
            case item::unique_type::SprintersLegtie:
            case item::unique_type::SwindlersBag:
            case item::unique_type::TribalFlag:
            case item::unique_type::TricksterPouch:
            case item::unique_type::TuningFork:
            case item::unique_type::TurtleShell:
            case item::unique_type::VampiresToothNecklace:
            case item::unique_type::ViperFangFingerclaw:
            case item::unique_type::VultureGizzard:
            case item::unique_type::WarhorseMarionette:
            case item::unique_type::WarTrumpet:
            case item::unique_type::WeaselTotem:
            case item::unique_type::WolfenFur:
            case item::unique_type::WraithTalisman:
            case item::unique_type::ZombieSeeds:
            case item::unique_type::NotUnique:
            case item::unique_type::Count:
            default:
            {
                M_HP_LOG_ERR("game::creature::EnchantmentFactory::MakeFromUniqueType"
                    << "(unique_type=" << item::unique_type::ToString(UNIQUE_ENUM)
                    << ") invalid unique_type.");

                return nullptr;
            }
        }
    }


    Enchantment * EnchantmentFactory::MakeFromMiscType(const item::misc_type::Enum E) const
    {
        if (E == item::misc_type::Spider_Eggs)
        {
            return new Enchantment(EnchantmentType::WhenUsed,
                                   {},
                                   UseInfo(10, Phase::Combat),
                                   SummonInfo(3, race::Spider, role::Spider));
        }
        else
        {
            return nullptr;
        }
    }


    Enchantment * EnchantmentFactory::MakeFromSetType(const item::set_type::Enum E) const
    {
        switch (E)
        {
            case item::set_type::TheAssassins:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Luck, 4),
                                            std::make_pair(stats::Traits::Speed, 4),
                                            std::make_pair(stats::Traits::Backstab, 2),
                                            std::make_pair(stats::Traits::FindMagic, 2) }) );
            }
            case item::set_type::TheNeverwinter:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Luck, 8),
                                            std::make_pair(stats::Traits::Speed, 6),
                                            std::make_pair(stats::Traits::Backstab, 4),
                                            std::make_pair(stats::Traits::FindMagic, 3) }) );
            }
            case item::set_type::TheTickler:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Luck, 13),
                                            std::make_pair(stats::Traits::Speed, 8),
                                            std::make_pair(stats::Traits::Backstab, 6),
                                            std::make_pair(stats::Traits::FindMagic, 4) }) );
            }
            case item::set_type::TheMagus:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Intelligence, 4),
                                            std::make_pair(stats::Traits::Mana, 4),
                                            std::make_pair(stats::Traits::MagicCast, 2),
                                            std::make_pair(stats::Traits::MagicEffect, 2),
                                            std::make_pair(stats::Traits::MagicResist, 2),
                                            std::make_pair(stats::Traits::ShadowDamage, 2) }) );
            }
            case item::set_type::TheNecromancers:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Intelligence, 8),
                                            std::make_pair(stats::Traits::Mana, 6),
                                            std::make_pair(stats::Traits::MagicCast, 4),
                                            std::make_pair(stats::Traits::MagicEffect, 4),
                                            std::make_pair(stats::Traits::MagicResist, 4),
                                            std::make_pair(stats::Traits::ShadowDamage, 4) }) );
            }
            case item::set_type::TheWarlocks:
                {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Intelligence, 16),
                                            std::make_pair(stats::Traits::Mana, 8),
                                            std::make_pair(stats::Traits::MagicCast, 8),
                                            std::make_pair(stats::Traits::MagicEffect, 8),
                                            std::make_pair(stats::Traits::MagicResist, 8),
                                            std::make_pair(stats::Traits::ShadowDamage, 8) }) );
            }
            case item::set_type::TheLichKings:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem |
                                            EnchantmentType::CurseWithoutItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Intelligence, 20),
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
                                            std::make_pair(stats::Traits::FindReligious, 5) }) );
            }
            case item::set_type::TheSages:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Charm, 4),
                                            std::make_pair(stats::Traits::Mana, 4),
                                            std::make_pair(stats::Traits::MagicCast, 2),
                                            std::make_pair(stats::Traits::MagicEffect, 2),
                                            std::make_pair(stats::Traits::MagicResist, 2),
                                            std::make_pair(stats::Traits::ShadowResist, 2) }) );
            }
            case item::set_type::TheShamans:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Charm, 8),
                                            std::make_pair(stats::Traits::Mana, 6),
                                            std::make_pair(stats::Traits::MagicCast, 4),
                                            std::make_pair(stats::Traits::MagicEffect, 4),
                                            std::make_pair(stats::Traits::MagicResist, 4),
                                            std::make_pair(stats::Traits::ShadowResist, 4) }) );
            }
            case item::set_type::TheOracles:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Charm, 16),
                                            std::make_pair(stats::Traits::Mana, 8),
                                            std::make_pair(stats::Traits::MagicCast, 8),
                                            std::make_pair(stats::Traits::MagicEffect, 8),
                                            std::make_pair(stats::Traits::MagicResist, 8),
                                            std::make_pair(stats::Traits::ShadowResist, 8) }) );
            }
            case item::set_type::TheAngelic:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem |
                                            EnchantmentType::BlessWithoutItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Charm, 20),
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
                                            std::make_pair(stats::Traits::ShadowResist, 13) }) );
            }
            case item::set_type::TheBalladeers:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Charm, 4),
                                            std::make_pair(stats::Traits::Intelligence, 4),
                                            std::make_pair(stats::Traits::Mana, 4),
                                            std::make_pair(stats::Traits::MagicCast, 2),
                                            std::make_pair(stats::Traits::MagicEffect, 2),
                                            std::make_pair(stats::Traits::MagicResist, 2) }) );
            }
            case item::set_type::TheTroubadours:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Charm, 8),
                                            std::make_pair(stats::Traits::Intelligence, 8),
                                            std::make_pair(stats::Traits::Mana, 6),
                                            std::make_pair(stats::Traits::MagicCast, 5),
                                            std::make_pair(stats::Traits::MagicEffect, 5),
                                            std::make_pair(stats::Traits::MagicResist, 5) }) );
            }
            case item::set_type::TheMuses:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Charm, 13),
                                            std::make_pair(stats::Traits::Intelligence, 13),
                                            std::make_pair(stats::Traits::Mana, 8),
                                            std::make_pair(stats::Traits::MagicCast, 10),
                                            std::make_pair(stats::Traits::MagicEffect, 10),
                                            std::make_pair(stats::Traits::MagicResist, 10) }) );
            }
            case item::set_type::TheCavaliers:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Strength, 2),
                                            std::make_pair(stats::Traits::Speed, 2),
                                            std::make_pair(stats::Traits::HitPower, 1) }) );
            }
            case item::set_type::TheChampions:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Strength, 4),
                                            std::make_pair(stats::Traits::Speed, 3),
                                            std::make_pair(stats::Traits::HitPower, 1),
                                            std::make_pair(stats::Traits::HitCritical, 1),
                                            std::make_pair(stats::Traits::MagicResist, 1) }) );
            }
            case item::set_type::ThePaladins:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Strength, 6),
                                            std::make_pair(stats::Traits::Speed, 4),
                                            std::make_pair(stats::Traits::HitPower, 2),
                                            std::make_pair(stats::Traits::HitCritical, 1),
                                            std::make_pair(stats::Traits::MagicResist, 2) }) );
            }
            case item::set_type::TheBerserkers:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Strength, 8),
                                            std::make_pair(stats::Traits::Speed, 5),
                                            std::make_pair(stats::Traits::HitPower, 2),
                                            std::make_pair(stats::Traits::HitCritical, 2),
                                            std::make_pair(stats::Traits::MagicResist, 3) }) );
            }
            case item::set_type::TheRosewood:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Strength, 10),
                                            std::make_pair(stats::Traits::Speed, 6),
                                            std::make_pair(stats::Traits::HitPower, 3),
                                            std::make_pair(stats::Traits::HitCritical, 2),
                                            std::make_pair(stats::Traits::MagicResist, 4) }) );
            }
            case item::set_type::TheDragonslayers:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Strength, 13),
                                            std::make_pair(stats::Traits::Speed, 8),
                                            std::make_pair(stats::Traits::HitPower, 3),
                                            std::make_pair(stats::Traits::HitCritical, 3),
                                            std::make_pair(stats::Traits::MagicResist, 5) }) );
            }
            case item::set_type::TheEventideRider:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Strength, 10),
                                            std::make_pair(stats::Traits::Speed, 5),
                                            std::make_pair(stats::Traits::HitPower, 4),
                                            std::make_pair(stats::Traits::HitCritical, 2),
                                            std::make_pair(stats::Traits::MagicResist, 3),
                                            std::make_pair(stats::Traits::CurseResist, 10),
                                            std::make_pair(stats::Traits::CurseOnDamage, 4),
                                            std::make_pair(stats::Traits::ShadowDamage, 2) }) );
            }
            case item::set_type::TheHunters:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Accuracy, 8),
                                            std::make_pair(stats::Traits::Strength, 4),
                                            std::make_pair(stats::Traits::Speed, 4),
                                            std::make_pair(stats::Traits::MagicResist, 1),
                                            std::make_pair(stats::Traits::HitCritical, 1) }) );
            }
            case item::set_type::TheSureShot:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Accuracy, 10),
                                            std::make_pair(stats::Traits::Strength, 6),
                                            std::make_pair(stats::Traits::Speed, 6),
                                            std::make_pair(stats::Traits::MagicResist, 2),
                                            std::make_pair(stats::Traits::HitCritical, 2) }) );
            }
            case item::set_type::TheMarksmans:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Accuracy, 13),
                                            std::make_pair(stats::Traits::Strength, 10),
                                            std::make_pair(stats::Traits::Speed, 10),
                                            std::make_pair(stats::Traits::MagicResist, 3),
                                            std::make_pair(stats::Traits::HitCritical, 3) }) );
            }
            case item::set_type::TheDeadeye:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Accuracy, 16),
                                            std::make_pair(stats::Traits::Strength, 13),
                                            std::make_pair(stats::Traits::Speed, 13),
                                            std::make_pair(stats::Traits::MagicResist, 4),
                                            std::make_pair(stats::Traits::HitCritical, 4) }) );
            }
            case item::set_type::TheDuskRanger:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Accuracy, 12),
                                            std::make_pair(stats::Traits::Strength, 6),
                                            std::make_pair(stats::Traits::Speed, 6),
                                            std::make_pair(stats::Traits::MagicResist, 1),
                                            std::make_pair(stats::Traits::HitCritical, 5),
                                            std::make_pair(stats::Traits::CurseResist, 8),
                                            std::make_pair(stats::Traits::CurseOnDamage, 8),
                                            std::make_pair(stats::Traits::ShadowDamage, 3) }) );
            }
            case item::set_type::TheVenomBow:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Accuracy, 12),
                                            std::make_pair(stats::Traits::Strength, 6),
                                            std::make_pair(stats::Traits::Speed, 6),
                                            std::make_pair(stats::Traits::MagicResist, 1),
                                            std::make_pair(stats::Traits::HitCritical, 3),
                                            std::make_pair(stats::Traits::PoisonOnAll, 8) }) );
            }
            case item::set_type::TheCritterChannelers:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Strength, 2),
                                            std::make_pair(stats::Traits::Accuracy, 1),
                                            std::make_pair(stats::Traits::Speed, 1),
                                            std::make_pair(stats::Traits::Intelligence, 2),
                                            std::make_pair(stats::Traits::AnimalResist, 5),
                                            std::make_pair(stats::Traits::MagicResist, 1) }) );
            }
            case item::set_type::TheMammalianHead:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Strength, 3),
                                            std::make_pair(stats::Traits::Accuracy, 2),
                                            std::make_pair(stats::Traits::Speed, 2),
                                            std::make_pair(stats::Traits::Intelligence, 3),
                                            std::make_pair(stats::Traits::AnimalResist, 6),
                                            std::make_pair(stats::Traits::MagicResist, 1) }) );
            }
            case item::set_type::TheSavageTaskmasters:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Strength, 4),
                                            std::make_pair(stats::Traits::Accuracy, 3),
                                            std::make_pair(stats::Traits::Speed, 3),
                                            std::make_pair(stats::Traits::Intelligence, 4),
                                            std::make_pair(stats::Traits::AnimalResist, 7),
                                            std::make_pair(stats::Traits::MagicResist, 2) }) );
            }
            case item::set_type::TheMonsterOverseers:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Strength, 5),
                                            std::make_pair(stats::Traits::Accuracy, 4),
                                            std::make_pair(stats::Traits::Speed, 4),
                                            std::make_pair(stats::Traits::Intelligence, 5),
                                            std::make_pair(stats::Traits::AnimalResist, 8),
                                            std::make_pair(stats::Traits::MagicResist, 3) }) );
            }
            case item::set_type::TheBeastRulers:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Strength, 6),
                                            std::make_pair(stats::Traits::Accuracy, 5),
                                            std::make_pair(stats::Traits::Speed, 5),
                                            std::make_pair(stats::Traits::Intelligence, 6),
                                            std::make_pair(stats::Traits::AnimalResist, 9),
                                            std::make_pair(stats::Traits::MagicResist, 4) }) );
            }
            case item::set_type::NotASet:
            case item::set_type::Count:
            default:
            {
                M_HP_LOG_ERR("game::creature::EnchantmentFactory::MakeFromSetType"
                    << "(set_type=" << item::set_type::ToString(E)
                    << ") invalid set_type.");

                return nullptr;
            }
        }
    }
    

    Enchantment * EnchantmentFactory::MakeFromSetCompleteType(const item::set_type::Enum E) const
    {
        switch (E)
        {
            case item::set_type::TheAssassins:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Luck, 20),
                                            std::make_pair(stats::Traits::Speed, 20),
                                            std::make_pair(stats::Traits::Backstab, 5),
                                            std::make_pair(stats::Traits::FindMagic, 10),
                                            std::make_pair(stats::Traits::FindCoinsAmount, 13) }) );
            }
            case item::set_type::TheNeverwinter:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Luck, 33),
                                            std::make_pair(stats::Traits::Speed, 33),
                                            std::make_pair(stats::Traits::Backstab, 10),
                                            std::make_pair(stats::Traits::FindMagic, 13),
                                            std::make_pair(stats::Traits::FindCoinsAmount, 18) }) );
            }
            case item::set_type::TheTickler:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Luck, 50),
                                            std::make_pair(stats::Traits::Speed, 50),
                                            std::make_pair(stats::Traits::Backstab, 15),
                                            std::make_pair(stats::Traits::FindMagic, 18),
                                            std::make_pair(stats::Traits::FindCoinsAmount, 24) }) );
            }
            case item::set_type::TheMagus:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Intelligence, 20),
                                            std::make_pair(stats::Traits::MagicCast, 8),
                                            std::make_pair(stats::Traits::MagicEffect, 8),
                                            std::make_pair(stats::Traits::MagicResist, 8),
                                            std::make_pair(stats::Traits::ShadowDamage, 10),
                                            std::make_pair(stats::Traits::Mana, 18) }) );
            }
            case item::set_type::TheNecromancers:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Intelligence, 33),
                                            std::make_pair(stats::Traits::MagicCast, 16),
                                            std::make_pair(stats::Traits::MagicEffect, 16),
                                            std::make_pair(stats::Traits::MagicResist, 16),
                                            std::make_pair(stats::Traits::ShadowDamage, 20),
                                            std::make_pair(stats::Traits::Mana, 24) }) );
            }
            case item::set_type::TheWarlocks:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Intelligence, 50),
                                            std::make_pair(stats::Traits::MagicCast, 33),
                                            std::make_pair(stats::Traits::MagicEffect, 33),
                                            std::make_pair(stats::Traits::MagicResist, 33),
                                            std::make_pair(stats::Traits::ShadowDamage, 16),
                                            std::make_pair(stats::Traits::Mana, 16),
                                            std::make_pair(stats::Traits::CurseCast, 28),
                                            std::make_pair(stats::Traits::CurseEffect, 28) }) );
            }
            case item::set_type::TheLichKings:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem |
                                            EnchantmentType::CurseWithoutItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Intelligence, 50),
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
                                            std::make_pair(stats::Traits::FindReligious, 10) }) );
            }
            case item::set_type::TheSages:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Charm, 20),
                                            std::make_pair(stats::Traits::MagicCast, 8),
                                            std::make_pair(stats::Traits::MagicEffect, 8),
                                            std::make_pair(stats::Traits::MagicResist, 8),
                                            std::make_pair(stats::Traits::BlessCast, 8),
                                            std::make_pair(stats::Traits::BlessEffect, 8),
                                            std::make_pair(stats::Traits::CurseResist, 8),
                                            std::make_pair(stats::Traits::Mana, 18),
                                            std::make_pair(stats::Traits::ShadowResist, 10) }) );
            }
            case item::set_type::TheShamans:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Charm, 33),
                                            std::make_pair(stats::Traits::MagicCast, 16),
                                            std::make_pair(stats::Traits::MagicEffect, 16),
                                            std::make_pair(stats::Traits::MagicResist, 16),
                                            std::make_pair(stats::Traits::BlessCast, 13),
                                            std::make_pair(stats::Traits::BlessEffect, 13),
                                            std::make_pair(stats::Traits::CurseResist, 16),
                                            std::make_pair(stats::Traits::Mana, 24),
                                            std::make_pair(stats::Traits::ShadowResist, 20) }) );
            }
            case item::set_type::TheOracles:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Charm, 50),
                                            std::make_pair(stats::Traits::MagicCast, 33),
                                            std::make_pair(stats::Traits::MagicEffect, 33),
                                            std::make_pair(stats::Traits::MagicResist, 33),
                                            std::make_pair(stats::Traits::BlessCast, 18),
                                            std::make_pair(stats::Traits::BlessEffect, 18),
                                            std::make_pair(stats::Traits::CurseResist, 33),
                                            std::make_pair(stats::Traits::Mana, 28),
                                            std::make_pair(stats::Traits::ShadowResist, 30) }) );
            }
            case item::set_type::TheAngelic:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem |
                                            EnchantmentType::BlessWithoutItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Charm, 50),
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
                                            std::make_pair(stats::Traits::ShadowResist, 20) }) );
            }
            case item::set_type::TheBalladeers:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Charm, 20),
                                            std::make_pair(stats::Traits::Intelligence, 20),
                                            std::make_pair(stats::Traits::MagicCast, 8),
                                            std::make_pair(stats::Traits::MagicEffect, 8),
                                            std::make_pair(stats::Traits::MagicResist, 8),
                                            std::make_pair(stats::Traits::Mana, 18) }) );
            }
            case item::set_type::TheTroubadours:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Charm, 33),
                                            std::make_pair(stats::Traits::Intelligence, 33),
                                            std::make_pair(stats::Traits::MagicCast, 16),
                                            std::make_pair(stats::Traits::MagicEffect, 16),
                                            std::make_pair(stats::Traits::MagicResist, 16),
                                            std::make_pair(stats::Traits::Mana, 24) }) );
            }
            case item::set_type::TheMuses:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Charm, 50),
                                            std::make_pair(stats::Traits::Intelligence, 50),
                                            std::make_pair(stats::Traits::MagicCast, 28),
                                            std::make_pair(stats::Traits::MagicEffect, 28),
                                            std::make_pair(stats::Traits::MagicResist, 28),
                                            std::make_pair(stats::Traits::Mana, 28) }) );
            }
            case item::set_type::TheCavaliers:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Strength, 20),
                                            std::make_pair(stats::Traits::Speed, 13),
                                            std::make_pair(stats::Traits::MagicResist, 6),
                                            std::make_pair(stats::Traits::CurseResist, 10),
                                            std::make_pair(stats::Traits::HitCritical, 2),
                                            std::make_pair(stats::Traits::HitPower, 5) }) );
            }
            case item::set_type::TheChampions:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Strength, 33),
                                            std::make_pair(stats::Traits::Speed, 18),
                                            std::make_pair(stats::Traits::MagicResist, 8),
                                            std::make_pair(stats::Traits::CurseResist, 20),
                                            std::make_pair(stats::Traits::HitCritical, 3),
                                            std::make_pair(stats::Traits::HitPower, 6) }) );
            }
            case item::set_type::ThePaladins:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Strength, 50),
                                            std::make_pair(stats::Traits::Speed, 26),
                                            std::make_pair(stats::Traits::MagicResist, 10),
                                            std::make_pair(stats::Traits::CurseResist, 30),
                                            std::make_pair(stats::Traits::HitCritical, 4),
                                            std::make_pair(stats::Traits::HitPower, 7) }) );
            }
            case item::set_type::TheBerserkers:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Strength, 70),
                                            std::make_pair(stats::Traits::Speed, 33),
                                            std::make_pair(stats::Traits::MagicResist, 12),
                                            std::make_pair(stats::Traits::CurseResist, 40),
                                            std::make_pair(stats::Traits::HitCritical, 5),
                                            std::make_pair(stats::Traits::HitPower, 8) }) );
            }
            case item::set_type::TheRosewood:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Strength, 100),
                                            std::make_pair(stats::Traits::Speed, 50),
                                            std::make_pair(stats::Traits::MagicResist, 14),
                                            std::make_pair(stats::Traits::CurseResist, 50),
                                            std::make_pair(stats::Traits::HitCritical, 6),
                                            std::make_pair(stats::Traits::HitPower, 9) }) );
            }
            case item::set_type::TheDragonslayers:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Strength, 133),
                                            std::make_pair(stats::Traits::Speed, 70),
                                            std::make_pair(stats::Traits::MagicResist, 16),
                                            std::make_pair(stats::Traits::CurseResist, 60),
                                            std::make_pair(stats::Traits::HitCritical, 7),
                                            std::make_pair(stats::Traits::HitPower, 10) }) );
            }
            case item::set_type::TheEventideRider:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Strength, 133),
                                            std::make_pair(stats::Traits::Speed, 70),
                                            std::make_pair(stats::Traits::MagicResist, 16),
                                            std::make_pair(stats::Traits::CurseResist, 20),
                                            std::make_pair(stats::Traits::HitCritical, 7),
                                            std::make_pair(stats::Traits::HitPower, 10),
                                            std::make_pair(stats::Traits::CurseOnDamage, 26),
                                            std::make_pair(stats::Traits::ShadowDamage, 14) }) );
            }
            case item::set_type::TheHunters:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Accuracy, 20),
                                            std::make_pair(stats::Traits::Strength, 15),
                                            std::make_pair(stats::Traits::Speed, 15),
                                            std::make_pair(stats::Traits::MagicResist, 2),
                                            std::make_pair(stats::Traits::HitCritical, 4),
                                            std::make_pair(stats::Traits::HitPower, 2) }) );
            }
            case item::set_type::TheSureShot:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Accuracy, 33),
                                            std::make_pair(stats::Traits::Strength, 28),
                                            std::make_pair(stats::Traits::Speed, 28),
                                            std::make_pair(stats::Traits::MagicResist, 4),
                                            std::make_pair(stats::Traits::HitCritical, 6),
                                            std::make_pair(stats::Traits::HitPower, 4) }) );
            }
            case item::set_type::TheMarksmans:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Accuracy, 50),
                                            std::make_pair(stats::Traits::Strength, 36),
                                            std::make_pair(stats::Traits::Speed, 36),
                                            std::make_pair(stats::Traits::MagicResist, 8),
                                            std::make_pair(stats::Traits::HitCritical, 8),
                                            std::make_pair(stats::Traits::HitPower, 6) }) );
            }
            case item::set_type::TheDeadeye:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Accuracy, 70),
                                            std::make_pair(stats::Traits::Strength, 50),
                                            std::make_pair(stats::Traits::Speed, 50),
                                            std::make_pair(stats::Traits::MagicResist, 16),
                                            std::make_pair(stats::Traits::HitCritical, 13),
                                            std::make_pair(stats::Traits::HitPower, 8) }) );
            }
            case item::set_type::TheDuskRanger:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Accuracy, 50),
                                            std::make_pair(stats::Traits::Strength, 50),
                                            std::make_pair(stats::Traits::Speed, 50),
                                            std::make_pair(stats::Traits::MagicResist, 8),
                                            std::make_pair(stats::Traits::HitCritical, 20),
                                            std::make_pair(stats::Traits::CurseResist, 50),
                                            std::make_pair(stats::Traits::CurseOnDamage, 33),
                                            std::make_pair(stats::Traits::ShadowDamage, 18) }) );
            }
            case item::set_type::TheVenomBow:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Accuracy, 50),
                                            std::make_pair(stats::Traits::Strength, 50),
                                            std::make_pair(stats::Traits::Speed, 50),
                                            std::make_pair(stats::Traits::MagicResist, 8),
                                            std::make_pair(stats::Traits::HitCritical, 20),
                                            std::make_pair(stats::Traits::PoisonOnAll, 33) }) );
            }
            case item::set_type::TheCritterChannelers:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Strength, 10),
                                            std::make_pair(stats::Traits::Accuracy, 8),
                                            std::make_pair(stats::Traits::Speed, 8),
                                            std::make_pair(stats::Traits::Intelligence, 8),
                                            std::make_pair(stats::Traits::AnimalResist, 8),
                                            std::make_pair(stats::Traits::MagicResist, 2) }) );
            }
            case item::set_type::TheMammalianHead:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Strength, 15),
                                            std::make_pair(stats::Traits::Accuracy, 10),
                                            std::make_pair(stats::Traits::Speed, 10),
                                            std::make_pair(stats::Traits::Intelligence, 10),
                                            std::make_pair(stats::Traits::AnimalResist, 13),
                                            std::make_pair(stats::Traits::MagicResist, 2) }) );
            }
            case item::set_type::TheSavageTaskmasters:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Strength, 20),
                                            std::make_pair(stats::Traits::Accuracy, 13),
                                            std::make_pair(stats::Traits::Speed, 13),
                                            std::make_pair(stats::Traits::Intelligence, 13),
                                            std::make_pair(stats::Traits::AnimalResist, 18),
                                            std::make_pair(stats::Traits::MagicResist, 4) }) );
            }
            case item::set_type::TheMonsterOverseers:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Strength, 25),
                                            std::make_pair(stats::Traits::Accuracy, 18),
                                            std::make_pair(stats::Traits::Speed, 18),
                                            std::make_pair(stats::Traits::Intelligence, 18),
                                            std::make_pair(stats::Traits::AnimalResist, 24),
                                            std::make_pair(stats::Traits::MagicResist, 8) }) );
            }
            case item::set_type::TheBeastRulers:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Strength, 30),
                                            std::make_pair(stats::Traits::Accuracy, 24),
                                            std::make_pair(stats::Traits::Speed, 24),
                                            std::make_pair(stats::Traits::Intelligence, 24),
                                            std::make_pair(stats::Traits::AnimalResist, 28),
                                            std::make_pair(stats::Traits::MagicResist, 16) }) );
            }
            case item::set_type::NotASet:
            case item::set_type::Count:
            default:
            {
                M_HP_LOG_ERR("game::creature::EnchantmentFactory::MakeFromSetCompleteType"
                    << "(set_type=" << item::set_type::ToString(E)
                    << ") invalid set_type.");

                return nullptr;
            }
        }
    }


    Enchantment * EnchantmentFactory::MakeFromNamedType(
        const item::named_type::Enum NAMED_ENUM,
        const item::material::Enum   MATERIAL_ENUM,
        const bool                   IS_WEAPON,
        const bool                   IS_ARMOR) const
    {
        switch (NAMED_ENUM)
        {
            case item::named_type::Wicked:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Strength, 13),
                                            std::make_pair(stats::Traits::Luck, -18),
                                            std::make_pair(stats::Traits::Charm, -13),
                                            std::make_pair(stats::Traits::Intelligence, -13),
                                            std::make_pair(stats::Traits::CurseOnDamage, 3),
                                            std::make_pair(stats::Traits::HitCritical, 2),
                                            std::make_pair(stats::Traits::CurseResist, -13),
                                            std::make_pair(stats::Traits::Encounter, 1),
                                            std::make_pair(stats::Traits::Surprise, 1) }) );
            }
            case item::named_type::Fiendish:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Strength, 16),
                                            std::make_pair(stats::Traits::Luck, -20),
                                            std::make_pair(stats::Traits::Charm, -18),
                                            std::make_pair(stats::Traits::Intelligence, -18),
                                            std::make_pair(stats::Traits::CurseOnDamage, 6),
                                            std::make_pair(stats::Traits::HitCritical, 4),
                                            std::make_pair(stats::Traits::CurseResist, -18),
                                            std::make_pair(stats::Traits::Encounter, 2),
                                            std::make_pair(stats::Traits::Surprise, 2) }) );
            }
            case item::named_type::Infernal:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Strength, 18),
                                            std::make_pair(stats::Traits::Luck, -22),
                                            std::make_pair(stats::Traits::Charm, -24),
                                            std::make_pair(stats::Traits::Intelligence, -24),
                                            std::make_pair(stats::Traits::CurseOnDamage, 8),
                                            std::make_pair(stats::Traits::HitCritical, 6),
                                            std::make_pair(stats::Traits::CurseResist, -26),
                                            std::make_pair(stats::Traits::Encounter, 3),
                                            std::make_pair(stats::Traits::Surprise, 3) }) );
            }
            case item::named_type::Raging:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Strength, 26),
                                            std::make_pair(stats::Traits::Luck, -24),
                                            std::make_pair(stats::Traits::Charm, -28),
                                            std::make_pair(stats::Traits::Intelligence, -28),
                                            std::make_pair(stats::Traits::CurseOnDamage, 10),
                                            std::make_pair(stats::Traits::HitCritical, 8),
                                            std::make_pair(stats::Traits::CurseResist, -33),
                                            std::make_pair(stats::Traits::Encounter, 4),
                                            std::make_pair(stats::Traits::Surprise, 4) }) );
            }
            case item::named_type::Diabolic:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Strength, 33),
                                            std::make_pair(stats::Traits::Luck, -26),
                                            std::make_pair(stats::Traits::Charm, -33),
                                            std::make_pair(stats::Traits::Intelligence, -33),
                                            std::make_pair(stats::Traits::CurseOnDamage, 13),
                                            std::make_pair(stats::Traits::HitCritical, 10),
                                            std::make_pair(stats::Traits::CurseResist, -100),
                                            std::make_pair(stats::Traits::Encounter, 5),
                                            std::make_pair(stats::Traits::Surprise, 5) }) );
            }
            case item::named_type::Dancing:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Strength, 18),
                                            std::make_pair(stats::Traits::Speed, 18),
                                            std::make_pair(stats::Traits::CurseEffect, 50),
                                            std::make_pair(stats::Traits::HitCritical, 8),
                                            std::make_pair(stats::Traits::ArmorRating, 16),
                                            std::make_pair(stats::Traits::Accuracy, 16) }) );
            }
            case item::named_type::Marauder:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Strength, 16),
                                            std::make_pair(stats::Traits::Accuracy, 13),
                                            std::make_pair(stats::Traits::HitCritical, 5) }) );
            }
            case item::named_type::Honest:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Accuracy, 1),
                                            std::make_pair(stats::Traits::ArmorRating, 1),
                                            std::make_pair(stats::Traits::BlessEffect, 4),
                                            std::make_pair(stats::Traits::ShadowResist, 8),
                                            std::make_pair(stats::Traits::HonorResist, 8) }) );
            }
            case item::named_type::Noble:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Accuracy, 2),
                                            std::make_pair(stats::Traits::ArmorRating, 2),
                                            std::make_pair(stats::Traits::BlessEffect, 6),
                                            std::make_pair(stats::Traits::Surprise, -2),
                                            std::make_pair(stats::Traits::ShadowResist, 13),
                                            std::make_pair(stats::Traits::HonorResist, 13) }) );
            }
            case item::named_type::Daring:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Accuracy, 3),
                                            std::make_pair(stats::Traits::ArmorRating, 3),
                                            std::make_pair(stats::Traits::BlessEffect, 8),
                                            std::make_pair(stats::Traits::Surprise, -2),
                                            std::make_pair(stats::Traits::Charm, 2),
                                            std::make_pair(stats::Traits::Intelligence, 2),
                                            std::make_pair(stats::Traits::ShadowResist, 18),
                                            std::make_pair(stats::Traits::HonorResist, 18) }) );
            }
            case item::named_type::Elite:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Accuracy, 4),
                                            std::make_pair(stats::Traits::ArmorRating, 4),
                                            std::make_pair(stats::Traits::BlessEffect, 13),
                                            std::make_pair(stats::Traits::Surprise, -2),
                                            std::make_pair(stats::Traits::Charm, 3),
                                            std::make_pair(stats::Traits::Intelligence, 3),
                                            std::make_pair(stats::Traits::CurseResist, 8),
                                            std::make_pair(stats::Traits::ShadowResist, 24),
                                            std::make_pair(stats::Traits::HonorResist, 24) }) );
            }
            case item::named_type::Valiant:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Accuracy, 5),
                                            std::make_pair(stats::Traits::ArmorRating, 5),
                                            std::make_pair(stats::Traits::BlessEffect, 16),
                                            std::make_pair(stats::Traits::Surprise, -2),
                                            std::make_pair(stats::Traits::Charm, 4),
                                            std::make_pair(stats::Traits::Intelligence, 4),
                                            std::make_pair(stats::Traits::CurseResist, 10),
                                            std::make_pair(stats::Traits::ShadowResist, 28),
                                            std::make_pair(stats::Traits::HonorResist, 28) }) );
            }
            case item::named_type::Heros:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Accuracy, 6),
                                            std::make_pair(stats::Traits::ArmorRating, 6),
                                            std::make_pair(stats::Traits::BlessEffect, 18),
                                            std::make_pair(stats::Traits::Surprise, -2),
                                            std::make_pair(stats::Traits::Charm, 5),
                                            std::make_pair(stats::Traits::Intelligence, 5),
                                            std::make_pair(stats::Traits::CurseResist, 13),
                                            std::make_pair(stats::Traits::MagicResist, 4),
                                            std::make_pair(stats::Traits::ShadowResist, 33),
                                            std::make_pair(stats::Traits::HonorResist, 33) }) );
            }
            case item::named_type::Army:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Strength, 3),
                                            std::make_pair(stats::Traits::Speed, 3),
                                            std::make_pair(stats::Traits::ArmorRating, 3) }) );
            }
            case item::named_type::Gladiator:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Strength, 8),
                                            std::make_pair(stats::Traits::Speed, 6) }) );
            }
            case item::named_type::Soldiers:
            {
                if (IS_WEAPON)
                {
                    return new Enchantment( static_cast<EnchantmentType::Enum>(
                                                EnchantmentType::WhenEquipped |
                                                EnchantmentType::BoundToItem),
                                            stats::TraitSet( {
                                                std::make_pair(stats::Traits::Strength, 18),
                                                std::make_pair(stats::Traits::Speed, 13),
                                                std::make_pair(stats::Traits::Charm, 8),
                                                std::make_pair(stats::Traits::HonorDamage, 22) }
                                            ) );
                }
                
                if (IS_ARMOR)
                {
                    return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::ArmorRating, 4) }) );
                }

                break;
            }
            case item::named_type::Wardens:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Strength, 5),
                                            std::make_pair(stats::Traits::ArmorRating, 5),
                                            std::make_pair(stats::Traits::Surprise, -3) }) );
            }
            case item::named_type::Princes:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Strength, 8),
                                            std::make_pair(stats::Traits::ArmorRating, 8) }) );
            }
            case item::named_type::Ranger:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Strength, 6),
                                            std::make_pair(stats::Traits::Accuracy, 8),
                                            std::make_pair(stats::Traits::ArmorRating, 4) }) );
            }
            case item::named_type::Samurai:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Strength, 10),
                                            std::make_pair(stats::Traits::Accuracy, 10),
                                            std::make_pair(stats::Traits::HitCritical, 2) }) );
            }
            case item::named_type::Thors:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                            EnchantmentType::WhenEquipped |
                                            EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                                            std::make_pair(stats::Traits::Strength, 12),
                                            std::make_pair(stats::Traits::ArmorRating, 12),
                                            std::make_pair(stats::Traits::MagicResist, 4),
                                            std::make_pair(stats::Traits::HitPower, 2) }) );
            }
            case item::named_type::Nile:
            {
                if (IS_WEAPON)
                {
                    return new Enchantment( static_cast<EnchantmentType::Enum>(
                                                EnchantmentType::WhenEquipped |
                                                EnchantmentType::BoundToItem),
                                            stats::TraitSet( {
                                                std::make_pair(stats::Traits::Strength, 12),
                                                std::make_pair(stats::Traits::Accuracy, 12),
                                                std::make_pair(stats::Traits::ArmorRating, 8),
                                                std::make_pair(stats::Traits::CurseResist, 16),
                                                std::make_pair(stats::Traits::HitCritical, 1),
                                                std::make_pair(stats::Traits::PoisonOnMelee, 20) }) );
                }

                if (IS_ARMOR)
                {
                    return new Enchantment(static_cast<EnchantmentType::Enum>(
                        EnchantmentType::WhenEquipped |
                        EnchantmentType::BoundToItem),
                        stats::TraitSet({
                        std::make_pair(stats::Traits::Strength, 12),
                        std::make_pair(stats::Traits::Accuracy, 12),
                        std::make_pair(stats::Traits::ArmorRating, 8),
                        std::make_pair(stats::Traits::CurseResist, 16),
                        std::make_pair(stats::Traits::HitCritical, 1) }));
                }

                break;
            }
            case item::named_type::Searing:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                                EnchantmentType::WhenEquipped |
                                                EnchantmentType::BoundToItem),
                                            stats::TraitSet( {
                                                std::make_pair(stats::Traits::FireDamage, 16) }) );
            }
            case item::named_type::Burning:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                                EnchantmentType::WhenEquipped |
                                                EnchantmentType::BoundToItem),
                                            stats::TraitSet( {
                                                std::make_pair(stats::Traits::FireDamage, 33) }) );
            }
            case item::named_type::Fiery:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                                EnchantmentType::WhenEquipped |
                                                EnchantmentType::BoundToItem),
                                            stats::TraitSet( {
                                                std::make_pair(stats::Traits::FireDamage, 66) }) );
            }
            case item::named_type::Charred:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                                EnchantmentType::WhenEquipped |
                                                EnchantmentType::BoundToItem),
                                            stats::TraitSet( {
                                                std::make_pair(stats::Traits::FireResist, 8) }) );
            }
            case item::named_type::Icy:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                                EnchantmentType::WhenEquipped |
                                                EnchantmentType::BoundToItem),
                                            stats::TraitSet( {
                                                std::make_pair(stats::Traits::FrostDamage, 16) }) );
            }
            case item::named_type::Winter:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                                EnchantmentType::WhenEquipped |
                                                EnchantmentType::BoundToItem),
                                            stats::TraitSet( {
                                                std::make_pair(stats::Traits::FrostDamage, 33) }) );
            }
            case item::named_type::Frigid:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                                EnchantmentType::WhenEquipped |
                                                EnchantmentType::BoundToItem),
                                            stats::TraitSet( {
                                                std::make_pair(stats::Traits::FrostDamage, 66) }) );
            }
            case item::named_type::Chill:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                                EnchantmentType::WhenEquipped |
                                                EnchantmentType::BoundToItem),
                                            stats::TraitSet( {
                                                std::make_pair(stats::Traits::FrostResist, 8) }) );
            }
            case item::named_type::Frozen:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                                EnchantmentType::WhenEquipped |
                                                EnchantmentType::BoundToItem),
                                            stats::TraitSet( {
                                                std::make_pair(stats::Traits::FrostResist, 13) }) );
            }
            case item::named_type::Arctic:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                                EnchantmentType::WhenEquipped |
                                                EnchantmentType::BoundToItem),
                                            stats::TraitSet( {
                                                std::make_pair(stats::Traits::FrostResist, 18) }) );
            }
            case item::named_type::Proud:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                                EnchantmentType::WhenEquipped |
                                                EnchantmentType::BoundToItem),
                                            stats::TraitSet( {
                                                std::make_pair(stats::Traits::HonorDamage, 16) }) );
            }
            case item::named_type::Glory:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                                EnchantmentType::WhenEquipped |
                                                EnchantmentType::BoundToItem),
                                            stats::TraitSet( {
                                                std::make_pair(stats::Traits::HonorDamage, 33) }) );
            }
            case item::named_type::Pure:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                                EnchantmentType::WhenEquipped |
                                                EnchantmentType::BoundToItem),
                                            stats::TraitSet( {
                                                std::make_pair(stats::Traits::HonorDamage, 66) }) );
            }
            case item::named_type::Light:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                                EnchantmentType::WhenEquipped |
                                                EnchantmentType::BoundToItem),
                                            stats::TraitSet( {
                                                std::make_pair(stats::Traits::HonorResist, 8) }) );
            }
            case item::named_type::Dawn:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                                EnchantmentType::WhenEquipped |
                                                EnchantmentType::BoundToItem),
                                            stats::TraitSet( {
                                                std::make_pair(stats::Traits::HonorResist, 13) }) );
            }
            case item::named_type::Sun:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                                EnchantmentType::WhenEquipped |
                                                EnchantmentType::BoundToItem),
                                            stats::TraitSet( {
                                                std::make_pair(stats::Traits::HonorResist, 18) }) );
            }
            case item::named_type::Gloom:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                                EnchantmentType::WhenEquipped |
                                                EnchantmentType::BoundToItem),
                                            stats::TraitSet( {
                                                std::make_pair(stats::Traits::ShadowDamage, 16) }) );
            }
            case item::named_type::Twilight:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                                EnchantmentType::WhenEquipped |
                                                EnchantmentType::BoundToItem),
                                            stats::TraitSet( {
                                                std::make_pair(stats::Traits::ShadowDamage, 33) }) );
            }
            case item::named_type::Dusk:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                                EnchantmentType::WhenEquipped |
                                                EnchantmentType::BoundToItem),
                                            stats::TraitSet( {
                                                std::make_pair(stats::Traits::ShadowDamage, 66) }) );
            }
            case item::named_type::Sorrow:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                                EnchantmentType::WhenEquipped |
                                                EnchantmentType::BoundToItem),
                                            stats::TraitSet( {
                                                std::make_pair(stats::Traits::ShadowResist, 20) }) );
            }
            case item::named_type::Woe:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                                EnchantmentType::WhenEquipped |
                                                EnchantmentType::BoundToItem),
                                            stats::TraitSet( {
                                                std::make_pair(stats::Traits::ShadowResist, 20) }) );
            }
            case item::named_type::Misery:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                                EnchantmentType::WhenEquipped |
                                                EnchantmentType::BoundToItem),
                                            stats::TraitSet( {
                                                std::make_pair(stats::Traits::ShadowResist, 30) }) );
            }
            case item::named_type::Moon:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                                EnchantmentType::WhenEquipped |
                                                EnchantmentType::BoundToItem),
                                            stats::TraitSet( {
                                                std::make_pair(stats::Traits::Charm, -18),
                                                std::make_pair(stats::Traits::ArmorRating, 18),
                                                std::make_pair(stats::Traits::ShadowResist, 33),
                                                std::make_pair(stats::Traits::CurseResist, 33) }) );
            }
            case item::named_type::Dark:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                                EnchantmentType::WhenEquipped |
                                                EnchantmentType::BoundToItem),
                                            stats::TraitSet( {
                                                std::make_pair(stats::Traits::ShadowDamage, 66),
                                                std::make_pair(stats::Traits::CurseOnDamage, 8),
                                                std::make_pair(stats::Traits::HitCritical, 4),
                                                std::make_pair(stats::Traits::Strength, 18) }) );
            }
            case item::named_type::Betrayer:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                                EnchantmentType::WhenEquipped |
                                                EnchantmentType::BoundToItem),
                                            stats::TraitSet( {
                                                std::make_pair(stats::Traits::ShadowDamage, 80),
                                                std::make_pair(stats::Traits::CurseOnDamage, 18),
                                                std::make_pair(stats::Traits::HitCritical, 8),
                                                std::make_pair(stats::Traits::Strength, 33),
                                                std::make_pair(stats::Traits::Luck, -33),
                                                std::make_pair(stats::Traits::CurseResist, 33), }) );
            }
            case item::named_type::Imposters:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                                EnchantmentType::WhenEquipped |
                                                EnchantmentType::BoundToItem),
                                            stats::TraitSet( {
                                                std::make_pair(stats::Traits::Luck, 10),
                                                std::make_pair(stats::Traits::Backstab, 3),
                                                std::make_pair(stats::Traits::FindCoinsAmount, 5),
                                                std::make_pair(stats::Traits::FindMagic, 5),
                                                std::make_pair(stats::Traits::Surprise, -5) }) );
            }
            case item::named_type::Pickpocket:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                                EnchantmentType::WhenEquipped |
                                                EnchantmentType::BoundToItem),
                                            stats::TraitSet( {
                                                std::make_pair(stats::Traits::Luck, 13),
                                                std::make_pair(stats::Traits::Backstab, 4),
                                                std::make_pair(stats::Traits::FindCoinsAmount, 8),
                                                std::make_pair(stats::Traits::FindMagic, 6),
                                                std::make_pair(stats::Traits::Surprise, -6) }) );
            }
            case item::named_type::Burglar:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                                EnchantmentType::WhenEquipped |
                                                EnchantmentType::BoundToItem),
                                            stats::TraitSet( {
                                                std::make_pair(stats::Traits::Luck, 18),
                                                std::make_pair(stats::Traits::Backstab, 5),
                                                std::make_pair(stats::Traits::FindCoinsAmount, 10),
                                                std::make_pair(stats::Traits::FindMagic, 8),
                                                std::make_pair(stats::Traits::Surprise, -8) }) );
            }
            case item::named_type::Mountebank:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                                EnchantmentType::WhenEquipped |
                                                EnchantmentType::BoundToItem),
                                            stats::TraitSet( {
                                                std::make_pair(stats::Traits::Luck, 21),
                                                std::make_pair(stats::Traits::Backstab, 6),
                                                std::make_pair(stats::Traits::FindCoinsAmount, 8),
                                                std::make_pair(stats::Traits::FindMagic, 4),
                                                std::make_pair(stats::Traits::Charm, 18) }) );
            }
            case item::named_type::Charlatans:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                                EnchantmentType::WhenEquipped |
                                                EnchantmentType::BoundToItem),
                                            stats::TraitSet( {
                                                std::make_pair(stats::Traits::Luck, 13),
                                                std::make_pair(stats::Traits::Backstab, 3),
                                                std::make_pair(stats::Traits::FindCoinsAmount, 6),
                                                std::make_pair(stats::Traits::FindMagic, 3),
                                                std::make_pair(stats::Traits::Charm, 24) }) );
            }
            case item::named_type::Focus:
            {
                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                                EnchantmentType::WhenEquipped |
                                                EnchantmentType::BoundToItem),
                                            stats::TraitSet( {
                                                std::make_pair(stats::Traits::Accuracy, 18),
                                                std::make_pair(stats::Traits::Strength, 13),
                                                std::make_pair(stats::Traits::DamageBonusProj, 16) }) );
            }
            case item::named_type::Robbers:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_ENUM) };

                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                                EnchantmentType::WhenEquipped |
                                                EnchantmentType::BoundToItem),
                    stats::TraitSet( { std::make_pair(stats::Traits::Luck, (MAT_BONUS / 2)) }) );
            }
            case item::named_type::Thugs:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_ENUM) };

                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                                EnchantmentType::WhenEquipped |
                                                EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                    std::make_pair(stats::Traits::Luck, 10 + (MAT_BONUS / 2)) }) );
            }
            case item::named_type::Knaves:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_ENUM) };

                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                                EnchantmentType::WhenEquipped |
                                                EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                    std::make_pair(stats::Traits::Luck, 20 + (MAT_BONUS / 2)) }) );
            }
            case item::named_type::Muggers:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_ENUM) };

                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                                EnchantmentType::WhenEquipped |
                                                EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                    std::make_pair(stats::Traits::Backstab, (MAT_BONUS / 2)) }) );
            }
            case item::named_type::Thief:
            {
                auto const MATERIAL_BASED_BONUS{ 5 + (item::material::Bonus(MATERIAL_ENUM) / 2) };

                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                                EnchantmentType::WhenEquipped |
                                                EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                    std::make_pair(stats::Traits::Luck, MATERIAL_BASED_BONUS),
                    std::make_pair(stats::Traits::Backstab, MATERIAL_BASED_BONUS) }) );
            }
            case item::named_type::Pirate:
            {
                auto const MAT_BONUS{ item::material::Bonus(MATERIAL_ENUM) };

                return new Enchantment( static_cast<EnchantmentType::Enum>(
                                                EnchantmentType::WhenEquipped |
                                                EnchantmentType::BoundToItem),
                                        stats::TraitSet( {
                    std::make_pair(stats::Traits::Backstab, 8 + (MAT_BONUS / 2)),
                    std::make_pair(stats::Traits::FindCoinsAmount, 8 + (MAT_BONUS / 2)),
                    std::make_pair(stats::Traits::Luck, (MAT_BONUS / 2)),
                    std::make_pair(stats::Traits::FindMagic, (MAT_BONUS / 2)) }) );
            }
            case item::named_type::NotNamed:
            case item::named_type::Count:
            default:
            {}
        }

        M_HP_LOG_ERR("game::creature::EnchantmentFactory::MakeFromNamedType"
            << "(named_type=" << item::named_type::ToString(NAMED_ENUM)
            << " invalid named_type.");

        return nullptr;
    }

}
}
