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
// item-type-enum.cpp
//
#include "item-type-enum.hpp"

#include "game/game-data-file.hpp"

#include <sstream>
#include <exception>


namespace game
{
namespace item
{

    const std::string category::ToString(const item::category::Enum E, const bool WILL_WRAP)
    {
        std::ostringstream ss;

        if (E == category::None)
        {
            return "";
        }
        else
        {
            if (E & category::Broken)       ss << "broken/useless";
            else if (E & category::Useable) ss << "useable";

            if (E & category::Weapon)       ss << ((ss.str().empty()) ? "" : ", ") << "weapon";
            if (E & category::Armor)        ss << ((ss.str().empty()) ? "" : ", ") << "armor";
            if (E & category::Equippable)   ss << ((ss.str().empty()) ? "" : ", ") << "equippable";
            if (E & category::BodyPart)     ss << ((ss.str().empty()) ? "" : ", ") << "bodypart";
            if (E & category::Wearable)     ss << ((ss.str().empty()) ? "" : ", ") << "wearable";
            if (E & category::OneHanded)    ss << ((ss.str().empty()) ? "" : ", ") << "one-handed";
            if (E & category::TwoHanded)    ss << ((ss.str().empty()) ? "" : ", ") << "two-handed";
            if (E & category::QuestItem)    ss << ((ss.str().empty()) ? "" : ", ") << "quest item";
            if (E & category::Blessed)      ss << ((ss.str().empty()) ? "" : ", ") << "blessed";
            if (E & category::Cursed)       ss << ((ss.str().empty()) ? "" : ", ") << "cursed";
            if (E & category::AllowsCasting)ss << ((ss.str().empty()) ? "" : ", ") << "allows casting";
            if (E & category::ConsumedOnUse)ss << ((ss.str().empty()) ? "" : ", ") << "consumed upon use";
        }

        if (ss.str().empty())
        {
            std::ostringstream ssErr;
            ssErr << "game::item::category::ToString(" << E << ")_InvalidValueError";
            throw std::range_error(ssErr.str());
        }

        if (WILL_WRAP)
        {
            return "(" + ss.str() + ")";
        }
        else
        {
            return ss.str();
        }
    }


    const std::string element_type::ToString(const element_type::Enum E, const bool WILL_WRAP)
    {
        std::ostringstream ss;

        if (E == element_type::None)
        {
            return "";
        }
        else
        {
            if (E & element_type::Fire)     ss << "Fire";
            if (E & element_type::Frost)    ss << ((ss.str().empty()) ? "" : ", ") << "Frost";
            if (E & element_type::Honor)    ss << ((ss.str().empty()) ? "" : ", ") << "Honor";
            if (E & element_type::Shadow)   ss << ((ss.str().empty()) ? "" : ", ") << "Shadow";
        }

        if (ss.str().empty())
        {
            std::ostringstream ssErr;
            ssErr << "game::item::element_type::ToString(" << E << ")_InvalidValueError";
            throw std::range_error(ssErr.str());
        }

        if (WILL_WRAP)
        {
            return "(" + ss.str() + ")";
        }
        else
        {
            return ss.str();
        }
    }


    const std::string element_type::Name(const element_type::Enum E, const bool WILL_WRAP)
    {
        std::ostringstream ss;

        if (E == element_type::None)
        {
            return "";
        }
        else
        {
            ss << "of ";

            if (E & element_type::Fire)     ss << "Fire";
            if (E & element_type::Frost)    ss << ((ss.str().empty()) ? "" : "and ") << "Frost";
            if (E & element_type::Honor)    ss << ((ss.str().empty()) ? "" : "and ") << "Honor";
            if (E & element_type::Shadow)   ss << ((ss.str().empty()) ? "" : "and ") << "Shadow";
        }

        if (ss.str().empty())
        {
            std::ostringstream ssErr;
            ssErr << "game::item::element_type::Name(" << E << ")_InvalidValueError";
            throw std::range_error(ssErr.str());
        }

        if (WILL_WRAP)
        {
            return "(" + ss.str() + ")";
        }
        else
        {
            return ss.str();
        }
    }


    const std::vector<element_type::Enum> element_type::Combinations(const element_type::Enum E)
    {
        std::vector<element_type::Enum> v;

        if (E & element_type::Fire)     { v.push_back(element_type::Fire); }
        if (E & element_type::Frost)    { v.push_back(element_type::Frost); }
        if (E & element_type::Honor)    { v.push_back(element_type::Honor); }
        if (E & element_type::Shadow)   { v.push_back(element_type::Shadow); }

        if ((E & element_type::Fire) && (E & element_type::Honor))
        {
            v.push_back( static_cast<element_type::Enum>(element_type::Fire |
                                                         element_type::Honor) );
        }

        if ((E & element_type::Fire) && (E & element_type::Shadow))
        {
            v.push_back( static_cast<element_type::Enum>(element_type::Fire |
                                                         element_type::Shadow) );
        }

        if ((E & element_type::Frost) && (E & element_type::Honor))
        {
            v.push_back(static_cast<element_type::Enum>(element_type::Frost |
                                                        element_type::Honor));
        }

        if ((E & element_type::Frost) && (E & element_type::Shadow))
        {
            v.push_back(static_cast<element_type::Enum>(element_type::Frost |
                                                        element_type::Shadow));
        }

        return v;
    }


    const std::string set_type::ToString(const set_type::Enum E)
    {
        switch (E)
        {
            case NotASet:               { return "NotASet"; }
            case TheAssassins:          { return "TheAssassins"; }
            case TheNeverwinter:        { return "TheNeverwinter"; }
            case TheTickler:            { return "TheTickler"; }
            case TheMagus:              { return "TheMagus"; }
            case TheNecromancers:       { return "TheNecromancers"; }
            case TheWarlocks:           { return "TheWarlocks"; }
            case TheLichKings:          { return "TheLichKings"; }
            case TheSages:              { return "TheSages"; }
            case TheShamans:            { return "TheShamans"; }
            case TheOracles:            { return "TheOracles"; }
            case TheAngelic:            { return "TheAngelic"; }
            case TheBalladeers:         { return "TheBalladeers"; }
            case TheTroubadours:        { return "TheTroubadours"; }
            case TheMuses:              { return "TheMuses"; }
            case TheCavaliers:          { return "TheCavaliers"; }
            case TheChampions:          { return "TheChampions"; }
            case ThePaladins:           { return "ThePaladins"; }
            case TheBerserkers:         { return "TheBerserkers"; }
            case TheRosewood:           { return "TheRosewood"; }
            case TheDragonslayers:      { return "TheDragonslayers"; }
            case TheEventideRider:      { return "TheEventideRider"; }
            case TheHunters:            { return "TheHunters"; }
            case TheSureShot:           { return "TheSureShot"; }
            case TheMarksmans:          { return "TheMarksmans"; }
            case TheDeadeye:            { return "TheDeadeye"; }
            case TheDuskRanger:         { return "TheDuskRanger"; }
            case TheVenomBow:           { return "TheVenomBow"; }
            case TheCritterChannelers:  { return "TheCritterChannelers"; }
            case TheMammalianHead:      { return "TheMammalianHead"; }
            case TheSavageTaskmasters:  { return "TheSavageTaskmasters"; }
            case TheMonsterOverseers:   { return "TheMonsterOverseers"; }
            case TheBeastRulers:        { return "TheBeastRulers"; }
            case Count:
            default:
            {
                std::ostringstream ssErr;
                ssErr << "game::item::set_type::ToString(" << E << ")_InvalidValueError";
                throw std::range_error(ssErr.str());
            }
        }
    }


    const std::string set_type::Name(const set_type::Enum E)
    {
        switch (E)
        {
            case NotASet:               { return "NotASet"; }
            case TheAssassins:          { return "The Assassin's"; }
            case TheNeverwinter:        { return "The Neverwinter"; }
            case TheTickler:            { return "The Tickler's"; }
            case TheMagus:              { return "The Magus'"; }
            case TheNecromancers:       { return "The Necromancer's"; }
            case TheWarlocks:           { return "The Warlock's"; }
            case TheLichKings:          { return "The Lich King's"; }
            case TheSages:              { return "The Sage's"; }
            case TheShamans:            { return "The Shaman's"; }
            case TheOracles:            { return "The Oracle's"; }
            case TheAngelic:            { return "The Angelic"; }
            case TheBalladeers:         { return "The Balladeer's"; }
            case TheTroubadours:        { return "The Troubadour's"; }
            case TheMuses:              { return "The Muses'"; }
            case TheCavaliers:          { return "The Cavalier's"; }
            case TheChampions:          { return "The Champion's"; }
            case ThePaladins:           { return "The Paladin's"; }
            case TheBerserkers:         { return "The Berserker's"; }
            case TheRosewood:           { return "The Rosewood"; }
            case TheDragonslayers:      { return "The Dragonslayer's"; }
            case TheEventideRider:      { return "The Eventide Rider's"; }
            case TheHunters:            { return "The Hunter's"; }
            case TheSureShot:           { return "The Sure Shot's"; }
            case TheMarksmans:          { return "The Marksman's"; }
            case TheDeadeye:            { return "The Deadeye's"; }
            case TheDuskRanger:         { return "The Dusk Ranger's"; }
            case TheVenomBow:           { return "The Venom Bow"; }
            case TheCritterChannelers:  { return "The Critter Channeler's"; }
            case TheMammalianHead:      { return "The Mammalian Head's"; }
            case TheSavageTaskmasters:  { return "The Savage Taskmaster's"; }
            case TheMonsterOverseers:   { return "The Monster Overseer's"; }
            case TheBeastRulers:        { return "The Beast Ruler's"; }
            case Count:
            default:
            {
                std::ostringstream ssErr;
                ssErr << "game::item::set_type::Name(" << E << ")_InvalidValueError";
                throw std::range_error(ssErr.str());
            }
        }
    }


    creature::role::Enum set_type::Role(const set_type::Enum E)
    {
        switch (E)
        {
            case NotASet:               { return creature::role::Count; }
            case TheAssassins:
            case TheTickler:
            case TheNeverwinter:        { return creature::role::Thief; }
            case TheMagus:
            case TheNecromancers:
            case TheWarlocks:
            case TheLichKings:          { return creature::role::Sorcerer; }
            case TheSages:
            case TheShamans:
            case TheOracles:
            case TheAngelic:            { return creature::role::Cleric; }
            case TheBalladeers:
            case TheTroubadours:
            case TheMuses:              { return creature::role::Bard; }
            case TheCavaliers:
            case TheChampions:
            case ThePaladins:
            case TheBerserkers:
            case TheRosewood:
            case TheDragonslayers:
            case TheEventideRider:      { return creature::role::Knight; }
            case TheHunters:
            case TheSureShot:
            case TheMarksmans:
            case TheDeadeye:
            case TheDuskRanger:
            case TheVenomBow:           { return creature::role::Archer; }
            case TheCritterChannelers:
            case TheMammalianHead:
            case TheSavageTaskmasters:
            case TheMonsterOverseers:
            case TheBeastRulers:        { return creature::role::Beastmaster; }
            case Count:
            default:
            {
                std::ostringstream ssErr;
                ssErr << "game::item::set_type::Role(" << E << ")_InvalidValueError";
                throw std::range_error(ssErr.str());
            }
        }
    }


    const std::string unique_type::ToString(const Enum E)
    {
        switch (E)
        {
            case NotUnique:                 { return "NotUnqiue"; }
            case BasiliskTonge:             { return "BasiliskTonge"; }
            case BerserkersBeard:           { return "BerserkersBeard"; }
            case BishopsHanky:              { return "BishopsHanky"; }
            case BleedingTrophy:            { return "BleedingTrophy"; }
            case BloodyDragonScale:         { return "BloodyDragonScale"; }
            case BottleOfBansheeScreams:    { return "BottleOfBansheeScreams"; }
            case BraceletCrown:             { return "BraceletCrown"; }
            case BraceletFeather:           { return "BraceletFeather"; }
            case BraceletFist:              { return "BraceletFist"; }
            case BraceletHourglass:         { return "BraceletHourglass"; }
            case BraceletKey:               { return "BraceletKey"; }
            case BraceletMask:              { return "BraceletMask"; }
            case BroochCrown:               { return "BroochCrown"; }
            case BroochFeather:             { return "BroochFeather"; }
            case BroochFist:                { return "BroochFist"; }
            case BroochHourglass:           { return "BroochHourglass"; }
            case BroochKey:                 { return "BroochKey"; }
            case BroochMask:                { return "BroochMask"; }
            case BurialShroud:              { return "BurialShroud"; }
            case CharmCrown:                { return "CharmCrown"; }
            case CharmFeather:              { return "CharmFeather"; }
            case CharmFist:                 { return "CharmFist"; }
            case CharmHourglass:            { return "CharmHourglass"; }
            case CharmKey:                  { return "CharmKey"; }
            case CharmMask:                 { return "CharmMask"; }
            case ChimeraBone:               { return "ChimeraBone"; }
            case CobraTooth:                { return "CobraTooth"; }
            case CommandersCape:            { return "CommandersCape"; }
            case CopperTroll:               { return "CopperTroll"; }
            case CrystalCat:                { return "CrystalCat"; }
            case CrystalChimes:             { return "CrystalChimes"; }
            case CyclopsEye:                { return "CyclopsEye"; }
            case DemonDiary:                { return "DemonDiary"; }
            case DoveBloodVial:             { return "DoveBloodVial"; }
            case DragonToothWhistle:        { return "DragonToothWhistle"; }
            case DriedFrog:                 { return "DriedFrog"; }
            case DriedGecko:                { return "DriedGecko"; }
            case DriedIguana:               { return "DriedIguana"; }
            case DriedLizard:               { return "DriedLizard"; }
            case DriedSalamander:           { return "DriedSalamander"; }
            case DriedSkink:                { return "DriedSkink"; }
            case DriedToad:                 { return "DriedToad"; }
            case DriedTurtle:               { return "DriedTurtle"; }
            case DruidLeaf:                 { return "DruidLeaf"; }
            case EvilRabbitsFoot:           { return "EvilRabbitsFoot"; }
            case ExoticGoldenGong:          { return "ExoticGoldenGong"; }
            case FanaticsFlag:              { return "FanaticsFlag"; }
            case FriarsChronicle:           { return "FriarsChronicle"; }
            case FuneralRecord:             { return "FuneralRecord"; }
            case GeneralsCape:              { return "GeneralsCape"; }
            case GhostSheet:                { return "GhostSheet"; }
            case GiantOwlEye:               { return "GiantOwlEye"; }
            case GriffinFeather:            { return "GriffinFeather"; }
            case HangmansNoose:             { return "HangmansNoose"; }
            case HawkEye:                   { return "HawkEye"; }
            case HobgoblinNose:             { return "HobgoblinNose"; }
            case HoboRing:                  { return "HoboRing"; }
            case HolyEpic:                  { return "HolyEpic"; }
            case HornOfTheHorde:            { return "HornOfTheHorde"; }
            case ImpTail:                   { return "ImpTail"; }
            case IslanderHeaddress:         { return "IslanderHeaddress"; }
            case JeweledAnkhNecklace:       { return "JeweledAnkhNecklace"; }
            case JeweledArmband:            { return "JeweledArmband"; }
            case JeweledHandbag:            { return "JeweledHandbag"; }
            case JeweledPrincessVeil:       { return "JeweledPrincessVeil"; }
            case KingsCape:                 { return "KingsCape"; }
            case LastRitesScroll:           { return "LastRitesScroll"; }
            case MacabreManuscript:         { return "MacabreManuscript"; }
            case MadRatJuju:                { return "MadRatJuju"; }
            case MagicHorseshoe:            { return "MagicHorseshoe"; }
            case MagnifyingGlass:           { return "MagnifyingGlass"; }
            case ManaAmulet:                { return "ManaAmulet"; }
            case MendicantRing:             { return "MendicantRing"; }
            case MinotaurHide:              { return "MinotaurHide"; }
            case MonkRing:                  { return "MonkRing"; }
            case MortuaryOrnament:          { return "MortuaryOrnament"; }
            case MournersMask:              { return "MournersMask"; }
            case PantherPaw:                { return "PantherPaw"; }
            case PinCrown:                  { return "PinCrown"; }
            case PinFeather:                { return "PinFeather"; }
            case PinFist:                   { return "PinFist"; }
            case PinHourglass:              { return "PinHourglass"; }
            case PinKey:                    { return "PinKey"; }
            case PinMask:                   { return "PinMask"; }
            case PixieBell:                 { return "PixieBell"; }
            case PriestRing:                { return "PriestRing"; }
            case RascalMask:                { return "RascalMask"; }
            case RattlesnakeTail:           { return "RattlesnakeTail"; }
            case RavenClaw:                 { return "RavenClaw"; }
            case RazorFingerclaw:           { return "RazorFingerclaw"; }
            case ReaperScythe:              { return "ReaperScythe"; }
            case RegalCaptainsFlag:         { return "RegalCaptainsFlag"; }
            case RequiemRegister:           { return "RequiemRegister"; }
            case RoyalScoutSpyglass:        { return "RoyalScoutSpyglass"; }
            case SaintCameoPin:             { return "SaintCameoPin"; }
            case SaintsJournal:             { return "SaintsJournal"; }
            case SanguineRelic:             { return "SanguineRelic"; }
            case ScorpionStingerFingerclaw: { return "ScorpionStingerFingerclaw"; }
            case ScoundrelSack:             { return "ScoundrelSack"; }
            case SepultureDecoration:       { return "SepultureDecoration"; }
            case ShadeCloak:                { return "ShadeCloak"; }
            case ShamanRainmaker:           { return "ShamanRainmaker"; }
            case SharkToothNecklace:        { return "SharkToothNecklace"; }
            case SignetCrown:               { return "SignetCrown"; }
            case SignetFeather:             { return "SignetFeather"; }
            case SignetFist:                { return "SignetFist"; }
            case SignetHourglass:           { return "SignetHourglass"; }
            case SignetKey:                 { return "SignetKey"; }
            case SignetMask:                { return "SignetMask"; }
            case SirenConch:                { return "SirenConch"; }
            case SpecterChains:             { return "SpecterChains"; }
            case SpecterRobe:               { return "SpecterRobe"; }
            case SprintersLegtie:           { return "SprintersLegtie"; }
            case SwindlersBag:              { return "SwindlersBag"; }
            case TribalFlag:                { return "TribalFlag"; }
            case TricksterPouch:            { return "TricksterPouch"; }
            case TuningFork:                { return "TuningFork"; }
            case TurtleShell:               { return "TurtleShell"; }
            case VampiresToothNecklace:     { return "VampiresToothNecklace"; }
            case ViperFangFingerclaw:       { return "ViperFangFingerclaw"; }
            case VultureGizzard:            { return "VultureGizzard"; }
            case WarhorseMarionette:        { return "WarhorseMarionette"; }
            case WarTrumpet:                { return "WarTrumpet"; }
            case WeaselTotem:               { return "WeaselTotem"; }
            case WolfenFur:                 { return "WolfenFur"; }
            case WraithTalisman:            { return "WraithTalisman"; }
            case ZombieSeeds:               { return "ZombieSeeds"; }
            case Count:
            default:
            {
                std::ostringstream ssErr;
                ssErr << "game::item::unique_type::ToString(" << E << ")_InvalidValueError";
                throw std::range_error(ssErr.str());
            }
        }
    }


    const std::string unique_type::Name(const Enum E)
    {
        switch (E)
        {
            case NotUnique:                 { return "NotUnqiue"; }
            case BasiliskTonge:             { return "Basilisk Tonge"; }
            case BerserkersBeard:           { return "Berserker's Beard"; }
            case BishopsHanky:              { return "Bishop's Hanky"; }
            case BleedingTrophy:            { return "Bleeding Trophy"; }
            case BloodyDragonScale:         { return "Bloody Dragon Scale"; }
            case BottleOfBansheeScreams:    { return "Bottle Of Banshee Screams"; }
            case BraceletCrown:             { return "Crown Bracelet"; }
            case BraceletFeather:           { return "Feather Bracelet Feather"; }
            case BraceletFist:              { return "Fist Bracelet"; }
            case BraceletHourglass:         { return "Hourglass Bracelet"; }
            case BraceletKey:               { return "Key Bracelet"; }
            case BraceletMask:              { return "Mask Bracelet"; }
            case BroochCrown:               { return "Brooch Crown"; }
            case BroochFeather:             { return "Brooch Feather"; }
            case BroochFist:                { return "Brooch Fist"; }
            case BroochHourglass:           { return "Brooch Hourglass"; }
            case BroochKey:                 { return "Brooch Key"; }
            case BroochMask:                { return "Brooch Mask"; }
            case BurialShroud:              { return "Burial Shroud"; }
            case CharmCrown:                { return "Crown Charm"; }
            case CharmFeather:              { return "Feather Charm"; }
            case CharmFist:                 { return "Fist Charm"; }
            case CharmHourglass:            { return "Hourglass Charm"; }
            case CharmKey:                  { return "Key Charm"; }
            case CharmMask:                 { return "Mask Charm"; }
            case ChimeraBone:               { return "Chimera Bone"; }
            case CobraTooth:                { return "Cobra Tooth"; }
            case CommandersCape:            { return "Commander's Cape"; }
            case CopperTroll:               { return "Copper Troll"; }
            case CrystalCat:                { return "Crystal Cat"; }
            case CrystalChimes:             { return "Crystal Chimes"; }
            case CyclopsEye:                { return "Cyclops Eye"; }
            case DemonDiary:                { return "Demon Diary"; }
            case DoveBloodVial:             { return "Dove Blood Vial"; }
            case DragonToothWhistle:        { return "Dragon Tooth Whistle"; }
            case DriedFrog:                 { return "Dried Frog"; }
            case DriedGecko:                { return "Dried Gecko"; }
            case DriedIguana:               { return "Dried Iguana"; }
            case DriedLizard:               { return "Dried Lizard"; }
            case DriedSalamander:           { return "Dried Salamander"; }
            case DriedSkink:                { return "Dried Skink"; }
            case DriedToad:                 { return "Dried Toad"; }
            case DriedTurtle:               { return "Dried Turtle"; }
            case DruidLeaf:                 { return "Druid Leaf"; }
            case EvilRabbitsFoot:           { return "Evil Rabbit's Foot"; }
            case ExoticGoldenGong:          { return "Exotic Golden Gong"; }
            case FanaticsFlag:              { return "Fanatic's Flag"; }
            case FriarsChronicle:           { return "Friar's Chronicle"; }
            case FuneralRecord:             { return "Funeral Record"; }
            case GeneralsCape:              { return "General's Cape"; }
            case GhostSheet:                { return "Ghost Sheet"; }
            case GiantOwlEye:               { return "Giant Owl Eye"; }
            case GriffinFeather:            { return "Griffin Feather"; }
            case HangmansNoose:             { return "Hangman's Noose"; }
            case HawkEye:                   { return "Hawk Eye"; }
            case HobgoblinNose:             { return "Hobgoblin Nose"; }
            case HoboRing:                  { return "Hobo Ring"; }
            case HolyEpic:                  { return "Holy Epic"; }
            case HornOfTheHorde:            { return "Horn Of The Horde"; }
            case ImpTail:                   { return "Imp Tail"; }
            case IslanderHeaddress:         { return "Islander Headdress"; }
            case JeweledAnkhNecklace:       { return "Jeweled Ankh Necklace"; }
            case JeweledArmband:            { return "Jeweled Armband"; }
            case JeweledHandbag:            { return "Jeweled Handbag"; }
            case JeweledPrincessVeil:       { return "Jeweled Princess Veil"; }
            case KingsCape:                 { return "King's Cape"; }
            case LastRitesScroll:           { return "Last Rites Scroll"; }
            case MacabreManuscript:         { return "Macabre Manuscript"; }
            case MadRatJuju:                { return "Mad Rat Juju"; }
            case MagicHorseshoe:            { return "Magic Horseshoe"; }
            case MagnifyingGlass:           { return "Magnifying Glass"; }
            case ManaAmulet:                { return "Mana Amulet"; }
            case MendicantRing:             { return "Mendicant Ring"; }
            case MinotaurHide:              { return "Minotaur Hide"; }
            case MonkRing:                  { return "Monk Ring"; }
            case MortuaryOrnament:          { return "Mortuary Ornament"; }
            case MournersMask:              { return "Mourner's Mask"; }
            case PantherPaw:                { return "Panther Paw"; }
            case PinCrown:                  { return "Crown Pin"; }
            case PinFeather:                { return "Feather Pin"; }
            case PinFist:                   { return "Fist Pin"; }
            case PinHourglass:              { return "Hourglass Pin"; }
            case PinKey:                    { return "Key Pin"; }
            case PinMask:                   { return "Mask Pin"; }
            case PixieBell:                 { return "Pixie Bell"; }
            case PriestRing:                { return "Priest Ring"; }
            case RascalMask:                { return "Rascal Mask"; }
            case RattlesnakeTail:           { return "Rattlesnake Tail"; }
            case RavenClaw:                 { return "Raven Claw"; }
            case RazorFingerclaw:           { return "Razor Fingerclaw"; }
            case ReaperScythe:              { return "Reaper Scythe"; }
            case RegalCaptainsFlag:         { return "Regal Captain's Flag"; }
            case RequiemRegister:           { return "Requiem Register"; }
            case RoyalScoutSpyglass:        { return "Royal Scout Spyglass"; }
            case SaintCameoPin:             { return "Saint Cameo Pin"; }
            case SaintsJournal:             { return "Saint's Journal"; }
            case SanguineRelic:             { return "Sanguine Relic"; }
            case ScorpionStingerFingerclaw: { return "Scorpion Stinger Fingerclaw"; }
            case ScoundrelSack:             { return "Scoundrel Sack"; }
            case SepultureDecoration:       { return "Sepulture Decoration"; }
            case ShadeCloak:                { return "Shade Cloak"; }
            case ShamanRainmaker:           { return "Shaman Rainmaker"; }
            case SharkToothNecklace:        { return "Shark Tooth Necklace"; }
            case SignetCrown:               { return "Crown Signet"; }
            case SignetFeather:             { return "Feather Signet"; }
            case SignetFist:                { return "Fist Signet"; }
            case SignetHourglass:           { return "Hourglass Signet"; }
            case SignetKey:                 { return "Key Signet"; }
            case SignetMask:                { return "Mask Signet"; }
            case SirenConch:                { return "Siren Conch"; }
            case SpecterChains:             { return "Specter Chains"; }
            case SpecterRobe:               { return "Specter Robe"; }
            case SprintersLegtie:           { return "Sprinter's Legtie"; }
            case SwindlersBag:              { return "Swindler's Bag"; }
            case TribalFlag:                { return "Tribal Flag"; }
            case TricksterPouch:            { return "Trickster Pouch"; }
            case TuningFork:                { return "Tuning Fork"; }
            case TurtleShell:               { return "Turtle Shell"; }
            case VampiresToothNecklace:     { return "Vampire's Tooth Necklace"; }
            case ViperFangFingerclaw:       { return "Viper Fang Fingerclaw"; }
            case VultureGizzard:            { return "Vulture Gizzard"; }
            case WarhorseMarionette:        { return "Warhorse Marionette"; }
            case WarTrumpet:                { return "War Trumpet"; }
            case WeaselTotem:               { return "Weasel Totem"; }
            case WolfenFur:                 { return "Wolfen Fur"; }
            case WraithTalisman:            { return "Wraith Talisman"; }
            case ZombieSeeds:               { return "Zombie Seeds"; }
            case Count:
            default:
            {
                std::ostringstream ssErr;
                ssErr << "game::item::unique_type::Name(" << E << ")_InvalidValueError";
                throw std::range_error(ssErr.str());
            }
        }
    }


    misc_type::Enum unique_type::MiscType(const Enum E)
    {
        switch (E)
        {
            case NotUnique:                 { return misc_type::NotMisc; }
            case BasiliskTonge:             { return misc_type::Tongue; }
            case BerserkersBeard:           { return misc_type::Beard; }
            case BishopsHanky:              { return misc_type::Hanky; }
            case BleedingTrophy:            { return misc_type::Trophy; }
            case BloodyDragonScale:         { return misc_type::Scale; }
            case BottleOfBansheeScreams:    { return misc_type::Potion; }
            case BraceletCrown:             { return misc_type::Bracelet_Crown; }
            case BraceletFeather:           { return misc_type::Bracelet_Feather; }
            case BraceletFist:              { return misc_type::Bracelet_Fist; }
            case BraceletHourglass:         { return misc_type::Bracelet_Hourglass; }
            case BraceletKey:               { return misc_type::Bracelet_Key; }
            case BraceletMask:              { return misc_type::Bracelet_Mask; }
            case BroochCrown:               { return misc_type::Brooch_Crown; }
            case BroochFeather:             { return misc_type::Brooch_Feather; }
            case BroochFist:                { return misc_type::Brooch_Fist; }
            case BroochHourglass:           { return misc_type::Brooch_Hourglass; }
            case BroochKey:                 { return misc_type::Brooch_Key; }
            case BroochMask:                { return misc_type::Brooch_Mask; }
            case BurialShroud:              { return misc_type::Shroud; }
            case CharmCrown:                { return misc_type::Charm_Crown; }
            case CharmFeather:              { return misc_type::Charm_Feather; }
            case CharmFist:                 { return misc_type::Charm_Fist; }
            case CharmHourglass:            { return misc_type::Charm_Hourglass; }
            case CharmKey:                  { return misc_type::Charm_Key; }
            case CharmMask:                 { return misc_type::Charm_Mask; }
            case ChimeraBone:               { return misc_type::Bone; }
            case CobraTooth:                { return misc_type::Tooth; }
            case CommandersCape:            { return misc_type::Cape; }
            case CopperTroll:               { return misc_type::Troll_Figure; }
            case CrystalCat:                { return misc_type::Cat; }
            case CrystalChimes:             { return misc_type::Chimes; }
            case CyclopsEye:                { return misc_type::Eye; }
            case DemonDiary:                { return misc_type::Tome; }
            case DoveBloodVial:             { return misc_type::Potion; }
            case DragonToothWhistle:        { return misc_type::Bone_Whistle; }
            case DriedFrog:                 { return misc_type::Frog; }
            case DriedGecko:                { return misc_type::Gecko; }
            case DriedIguana:               { return misc_type::Iguana; }
            case DriedLizard:               { return misc_type::Lizard; }
            case DriedSalamander:           { return misc_type::Salamander; }
            case DriedSkink:                { return misc_type::Skink; }
            case DriedToad:                 { return misc_type::Frog; }
            case DriedTurtle:               { return misc_type::Turtle_Shell; }
            case DruidLeaf:                 { return misc_type::Leaf; }
            case EvilRabbitsFoot:           { return misc_type::Rabbit_Foot; }
            case ExoticGoldenGong:          { return misc_type::Gong; }
            case FanaticsFlag:              { return misc_type::Flag; }
            case FriarsChronicle:           { return misc_type::Book; }
            case FuneralRecord:             { return misc_type::Scroll; }
            case GeneralsCape:              { return misc_type::Cape; }
            case GhostSheet:                { return misc_type::Ghost_Sheet; }
            case GiantOwlEye:               { return misc_type::Eye; }
            case GriffinFeather:            { return misc_type::Feather; }
            case HangmansNoose:             { return misc_type::Noose; }
            case HawkEye:                   { return misc_type::Eye; }
            case HobgoblinNose:             { return misc_type::Nose; }
            case HoboRing:                  { return misc_type::Ring; }
            case HolyEpic:                  { return misc_type::Book; }
            case HornOfTheHorde:            { return misc_type::Horn; }
            case ImpTail:                   { return misc_type::Imp_Tail; }
            case IslanderHeaddress:         { return misc_type::Headdress; }
            case JeweledAnkhNecklace:       { return misc_type::Amulet; }
            case JeweledArmband:            { return misc_type::Armband; }
            case JeweledHandbag:            { return misc_type::Handbag; }
            case JeweledPrincessVeil:       { return misc_type::Veil; }
            case KingsCape:                 { return misc_type::Cape; }
            case LastRitesScroll:           { return misc_type::Scroll; }
            case MacabreManuscript:         { return misc_type::Book; }
            case MadRatJuju:                { return misc_type::Rat_Juju; }
            case MagicHorseshoe:            { return misc_type::Horseshoe; }
            case MagnifyingGlass:           { return misc_type::Magnifying_Glass; }
            case ManaAmulet:                { return misc_type::Amulet; }
            case MendicantRing:             { return misc_type::Ring; }
            case MinotaurHide:              { return misc_type::Hide; }
            case MonkRing:                  { return misc_type::Ring; }
            case MortuaryOrnament:          { return misc_type::Grave_Ornament; }
            case MournersMask:              { return misc_type::Mask; }
            case PantherPaw:                { return misc_type::Paw; }
            case PinCrown:                  { return misc_type::Pin_Crown; }
            case PinFeather:                { return misc_type::Pin_Feather; }
            case PinFist:                   { return misc_type::Pin_Fist; }
            case PinHourglass:              { return misc_type::Pin_Hourglass; }
            case PinKey:                    { return misc_type::Pin_Key; }
            case PinMask:                   { return misc_type::Pin_Mask; }
            case PixieBell:                 { return misc_type::Bell; }
            case PriestRing:                { return misc_type::Ring; }
            case RascalMask:                { return misc_type::Mask; }
            case RattlesnakeTail:           { return misc_type::Rattlesnake_Tail; }
            case RavenClaw:                 { return misc_type::Bird_Claw; }
            case RazorFingerclaw:           { return misc_type::Fingerclaw; }
            case ReaperScythe:              { return misc_type::Scythe; }
            case RegalCaptainsFlag:         { return misc_type::Flag; }
            case RequiemRegister:           { return misc_type::Book; }
            case RoyalScoutSpyglass:        { return misc_type::Spyglass; }
            case SaintCameoPin:             { return misc_type::Cameo; }
            case SaintsJournal:             { return misc_type::Book; }
            case SanguineRelic:             { return misc_type::Relic; }
            case ScorpionStingerFingerclaw: { return misc_type::Fingerclaw; }
            case ScoundrelSack:             { return misc_type::Bag; }
            case SepultureDecoration:       { return misc_type::Grave_Ornament; }
            case ShadeCloak:                { return misc_type::Cloak; }
            case ShamanRainmaker:           { return misc_type::Rainmaker; }
            case SharkToothNecklace:        { return misc_type::Shark_Tooth_Necklace; }
            case SignetCrown:               { return misc_type::Signet_Crown; }
            case SignetFeather:             { return misc_type::Signet_Feather; }
            case SignetFist:                { return misc_type::Signet_Fist; }
            case SignetHourglass:           { return misc_type::Signet_Hourglass; }
            case SignetKey:                 { return misc_type::Signet_Key; }
            case SignetMask:                { return misc_type::Signet_Mask; }
            case SirenConch:                { return misc_type::Conch; }
            case SpecterChains:             { return misc_type::Chains; }
            case SpecterRobe:               { return misc_type::Robe; }
            case SprintersLegtie:           { return misc_type::Legtie; }
            case SwindlersBag:              { return misc_type::Bag; }
            case TribalFlag:                { return misc_type::Flag; }
            case TricksterPouch:            { return misc_type::Bag; }
            case TuningFork:                { return misc_type::Tuning_Fork; }
            case TurtleShell:               { return misc_type::Turtle_Shell; }
            case VampiresToothNecklace:     { return misc_type::Tooth_Necklace; }
            case ViperFangFingerclaw:       { return misc_type::Fingerclaw; }
            case VultureGizzard:            { return misc_type::Gizzard; }
            case WarhorseMarionette:        { return misc_type::Warhorse_Marionette; }
            case WarTrumpet:                { return misc_type::Horn; }
            case WeaselTotem:               { return misc_type::Weasel_Totem; }
            case WolfenFur:                 { return misc_type::Wolfen_Fur; }
            case WraithTalisman:            { return misc_type::Talisman; }
            case ZombieSeeds:               { return misc_type::Seeds; }
            case Count:
            default:
            {
                std::ostringstream ssErr;
                ssErr << "game::item::unique_type::MiscType(" << E << ")_InvalidValueError";
                throw std::range_error(ssErr.str());
            }
        }
    }


    element_type::Enum unique_type::ElementTypes(const unique_type::Enum E)
    {
        auto elementType{ element_type::None };

        if ((E == unique_type::RazorFingerclaw) ||
            (E == unique_type::ViperFangFingerclaw) ||
            (E == unique_type::ScorpionStingerFingerclaw))
        {
            elementType = static_cast<element_type::Enum>(element_type::Fire |
                                                          element_type::Frost |
                                                          element_type::Shadow);
        }
        else if ((E == unique_type::CommandersCape) ||
                 (E == unique_type::GeneralsCape) ||
                 (E == unique_type::KingsCape) ||
                 (E == unique_type::JeweledPrincessVeil))
        {
            elementType = element_type::Honor;
        }
        else if ((E == unique_type::JeweledArmband) ||
                 (E == unique_type::ManaAmulet))
        {
            elementType = static_cast<element_type::Enum>(element_type::Fire |
                                                          element_type::Frost |
                                                          element_type::Honor |
                                                          element_type::Shadow);
        }

        return elementType;
    }


    const MaterialVecPair_t unique_type::Materials(const unique_type::Enum E)
    {
        switch (E)
        {
            case BasiliskTonge:             { return MaterialVecPair_t({ material::Flesh }, {}); }
            case BerserkersBeard:           { return MaterialVecPair_t({ material::Hair }, {}); }
            case BishopsHanky:              { return MaterialVecPair_t({ material::Cloth }, {}); }
            case BleedingTrophy:            { return MaterialVecPair_t({ material::CorePrimary() }, { material::Blood }); }
            case BloodyDragonScale:         { return MaterialVecPair_t({ material::Scale }, { material::Blood }); }
            case BottleOfBansheeScreams:    { return MaterialVecPair_t({ material::Glass }, { material::Spirit }); }
            case BraceletCrown:
            case BraceletFeather:
            case BraceletFist:
            case BraceletHourglass:
            case BraceletKey:
            case BraceletMask:
            case BroochCrown:
            case BroochFeather:
            case BroochFist:
            case BroochHourglass:
            case BroochKey:
            case BroochMask:                { return MaterialVecPair_t({ material::CorePrimary() }, {}); }
            case BurialShroud:              { return MaterialVecPair_t({ material::Cloth }, {}); }
            case CharmCrown:
            case CharmFeather:
            case CharmFist:
            case CharmHourglass:
            case CharmKey:
            case CharmMask:                 { return MaterialVecPair_t({ material::CorePrimary() }, {}); }
            case ChimeraBone:               { return MaterialVecPair_t({ material::Bone }, {}); }
            case CobraTooth:                { return MaterialVecPair_t({ material::Tooth }, {}); }
            case CommandersCape:            { return MaterialVecPair_t({ material::Cloth }, { material::Steel }); }
            case CopperTroll:               { return MaterialVecPair_t({ material::Bronze }, {}); }
            case CrystalCat:                { return MaterialVecPair_t({ material::Glass }, {}); }
            case CrystalChimes:             { return MaterialVecPair_t({ material::Glass }, {}); }
            case CyclopsEye:
            {
                MaterialVec_t v{ material::CoreMetal() };
                material::AppendCoreJewel(v);
                return MaterialVecPair_t(v, {});
            }
            case DemonDiary:                { return MaterialVecPair_t({ material::Paper }, {}); }
            case DoveBloodVial:             { return MaterialVecPair_t({ material::Glass }, {}); }
            case DragonToothWhistle:        { return MaterialVecPair_t({ material::Tooth }, {}); }
            case DriedFrog:
            case DriedGecko:
            case DriedIguana:
            case DriedLizard:
            case DriedSalamander:
            case DriedSkink:
            case DriedToad:
            case DriedTurtle:               { return MaterialVecPair_t({ material::Flesh }, {}); }
            case DruidLeaf:
            {
                MaterialVec_t v{ material::CoreMetal() };
                material::AppendCoreJewel(v);
                return MaterialVecPair_t(v, {});
            }
            case EvilRabbitsFoot:           { return MaterialVecPair_t({ material::Flesh }, {}); }
            case ExoticGoldenGong:          { return MaterialVecPair_t({ material::Gold }, { material::Wood }); }
            case FanaticsFlag:              { return MaterialVecPair_t({ material::Cloth}, {}); }
            case FriarsChronicle:           { return MaterialVecPair_t({ material::Paper }, {}); }
            case FuneralRecord:             { return MaterialVecPair_t({ material::Paper }, {}); }
            case GeneralsCape:              { return MaterialVecPair_t({ material::Cloth}, { material::Silver }); }
            case GhostSheet:                { return MaterialVecPair_t({ material::Cloth }, {}); }
            case GiantOwlEye:               { return MaterialVecPair_t({ material::Flesh }, {}); }
            case GriffinFeather:            { return MaterialVecPair_t({ material::Feather }, {}); }
            case HangmansNoose:             { return MaterialVecPair_t({ material::Rope}, {}); }
            case HawkEye:                   { return MaterialVecPair_t({ material::Flesh }, {}); }
            case HobgoblinNose:             { return MaterialVecPair_t({ material::Flesh }, {}); }
            case HoboRing:                  { return MaterialVecPair_t({ material::CorePrimary() }, {}); }
            case HolyEpic:                  { return MaterialVecPair_t({ material::Paper }, {}); }
            case HornOfTheHorde:            { return MaterialVecPair_t({ material::Bronze }, {}); }
            case ImpTail:                   { return MaterialVecPair_t({ material::Flesh }, {}); }
            case IslanderHeaddress:         { return MaterialVecPair_t({ material::Wood }, { material::Feather }); }
            case JeweledAnkhNecklace:
            {
                MaterialVec_t v{ material::CoreMetal() };
                material::AppendCoreJewel(v);
                return MaterialVecPair_t(v, {});
            }
            case JeweledArmband:            { return MaterialVecPair_t({ material::CorePrimary() }, {}); }
            case JeweledHandbag:
            case JeweledPrincessVeil:       { return MaterialVecPair_t({ material::CoreJewel() }, { material::Cloth }); }
            case KingsCape:                 { return MaterialVecPair_t({ material::Cloth }, { material::Gold}); }
            case LastRitesScroll:           { return MaterialVecPair_t({ material::Paper }, {}); }
            case MacabreManuscript:         { return MaterialVecPair_t({ material::Paper }, {}); }
            case MadRatJuju:                { return MaterialVecPair_t({ material::Bronze }, { material::Pearl }); }
            case MagicHorseshoe:            { return MaterialVecPair_t({ material::Iron }, {}); }
            case MagnifyingGlass:           { return MaterialVecPair_t({ material::CorePrimary() }, {}); }
            case ManaAmulet:                { return MaterialVecPair_t({ material::CorePrimary() }, {}); }
            case MendicantRing:             { return MaterialVecPair_t({ material::Iron }, {}); }
            case MinotaurHide:              { return MaterialVecPair_t({ material::Hide }, {}); }
            case MonkRing:                  { return MaterialVecPair_t({ material::Gold }, {}); }
            case MortuaryOrnament:          { return MaterialVecPair_t({ material::Wood }, {}); }
            case MournersMask:              { return MaterialVecPair_t({ material::Wood }, {}); }
            case PantherPaw:                { return MaterialVecPair_t({ material::Flesh }, {}); }
            case PinCrown:
            case PinFeather:
            case PinFist:
            case PinHourglass:
            case PinKey:
            case PinMask:                   { return MaterialVecPair_t({ material::CorePrimary() }, {}); }
            case PixieBell:                 { return MaterialVecPair_t({ material::Steel }, {}); }
            case PriestRing:                { return MaterialVecPair_t({ material::Gold }, {}); }
            case RascalMask:                { return MaterialVecPair_t({ material::Wood }, {}); }
            case RattlesnakeTail:           { return MaterialVecPair_t({ material::Flesh }, {}); }
            case RavenClaw:                 { return MaterialVecPair_t({ material::Flesh }, {}); }
            case RazorFingerclaw:           { return MaterialVecPair_t({ material::Steel }, {}); }
            case ReaperScythe:              { return MaterialVecPair_t({ material::Wood }, { material::Steel }); }
            case RegalCaptainsFlag:         { return MaterialVecPair_t({ material::Cloth }, {}); }
            case RequiemRegister:           { return MaterialVecPair_t({ material::Paper }, {}); }
            case RoyalScoutSpyglass:        { return MaterialVecPair_t({ material::Steel }, { material::Glass }); }
            case SaintCameoPin:             { return MaterialVecPair_t({ material::CorePrimary() }, {}); }
            case SaintsJournal:             { return MaterialVecPair_t({ material::Paper }, {}); }
            case SanguineRelic:             { return MaterialVecPair_t({ material::CorePrimary() }, {}); }
            case ScorpionStingerFingerclaw: { return MaterialVecPair_t({ material::Steel }, {}); }
            case ScoundrelSack:             { return MaterialVecPair_t({ material::Cloth }, {}); }
            case SepultureDecoration:       { return MaterialVecPair_t({ material::Wood }, {}); }
            case ShadeCloak:                { return MaterialVecPair_t({ material::Cloth }, {}); }
            case ShamanRainmaker:           { return MaterialVecPair_t({ material::Wood }, {}); }
            case SharkToothNecklace:        { return MaterialVecPair_t({ material::Tooth }, {}); }
            case SignetCrown:
            case SignetFeather:
            case SignetFist:
            case SignetHourglass:
            case SignetKey:
            case SignetMask:                { return MaterialVecPair_t({ material::CorePrimary() }, {}); }
            case SirenConch:                { return MaterialVecPair_t({ material::Glass }, {}); }
            case SpecterChains:             { return MaterialVecPair_t({ material::Iron }, {}); }
            case SpecterRobe:               { return MaterialVecPair_t({ material::Wood }, {}); }
            case SprintersLegtie:           { return MaterialVecPair_t({ material::Cloth }, {}); }
            case SwindlersBag:              { return MaterialVecPair_t({ material::Cloth }, {}); }
            case TribalFlag:                { return MaterialVecPair_t({ material::Cloth }, {}); }
            case TricksterPouch:            { return MaterialVecPair_t({ material::Cloth }, {}); }
            case TuningFork:                { return MaterialVecPair_t({ material::CoreMetal() }, {}); }
            case TurtleShell:
            {
                MaterialVec_t v{ material::CoreMetal() };
                material::AppendCoreJewel(v);
                return MaterialVecPair_t(v, {});
            }
            case VampiresToothNecklace:     { return MaterialVecPair_t({ material::Tooth }, {}); }
            case ViperFangFingerclaw:       { return MaterialVecPair_t({ material::Steel }, {}); }
            case VultureGizzard:            { return MaterialVecPair_t({ material::Flesh }, {}); }
            case WarhorseMarionette:        { return MaterialVecPair_t({ material::Wood }, {}); }
            case WarTrumpet:                { return MaterialVecPair_t({ material::CoreMetal() }, {}); }
            case WeaselTotem:               { return MaterialVecPair_t({ material::Wood }, {}); }
            case WolfenFur:                 { return MaterialVecPair_t({ material::Fur }, {}); }
            case WraithTalisman:            { return MaterialVecPair_t({ material::Wood }, {}); }
            case ZombieSeeds:               { return MaterialVecPair_t({ material::Plant }, {}); }
            case NotUnique:
            case Count:
            default:                        { return MaterialVecPair_t({}, {}); }
        }
    }


    const std::string misc_type::ToString(const misc_type::Enum E)
    {
        switch (E)
        {
            case NotMisc:               { return "NotMisc"; }
            case Amulet:                { return "Amulet"; }
            case Ankh_Necklace:         { return "Ankh_Necklace"; }
            case Armband:               { return "Armband"; }
            case Bag:                   { return "Bag"; }
            case Balm_Pot:              { return "Balm_Pot"; }
            case Beard:                 { return "Beard"; }
            case Bell:                  { return "Bell"; }
            case Bird_Claw:             { return "Bird_Claw"; }
            case Bone:                  { return "Bone"; }
            case Bone_Whistle:          { return "Bone_Whistle"; }
            case Book:                  { return "Book"; }
            case Bracelet_Crown:        { return "Bracelet_Crown"; }
            case Bracelet_Feather:      { return "Bracelet_Feather"; }
            case Bracelet_Fist:         { return "Bracelet_Fist"; }
            case Bracelet_Hourglass:    { return "Bracelet_Hourglass"; }
            case Bracelet_Key:          { return "Bracelet_Key"; }
            case Bracelet_Mask:         { return "Bracelet_Mask"; }
            case Braid:                 { return "Braid"; }
            case Brooch_Crown:          { return "Brooch_Crown"; }
            case Brooch_Feather:        { return "Brooch_Feather"; }
            case Brooch_Fist:           { return "Brooch_Fist"; }
            case Brooch_Hourglass:      { return "Brooch_Hourglass"; }
            case Brooch_Key:            { return "Brooch_Key"; }
            case Brooch_Mask:           { return "Brooch_Mask"; }
            case Bust:                  { return "Bust"; }
            case Cameo:                 { return "Cameo"; }
            case Cape:                  { return "Cape"; }
            case Cat:                   { return "Cat"; }
            case Chains:                { return "Chains"; }
            case Charm_Crown:           { return "Charm_Crown"; }
            case Charm_Feather:         { return "Charm_Feather"; }
            case Charm_Fist:            { return "Charm_Fist"; }
            case Charm_Hourglass:       { return "Charm_Hourglass"; }
            case Charm_Key:             { return "Charm_Key"; }
            case Charm_Mask:            { return "Charm_Mask"; }
            case Chest:                 { return "Chest"; }
            case Chimes:                { return "Chimes"; }
            case Cloak:                 { return "Cloak"; }
            case Conch:                 { return "Conch"; }
            case Crown:                 { return "Crown"; }
            case Crumhorn:              { return "Crumhorn"; }
            case Devil_Horn:            { return "Devil_Horn"; }
            case Doll:                  { return "Doll"; }
            case Doll_Blessed:          { return "Doll_Blessed"; }
            case Doll_Cursed:           { return "Doll_Cursed"; }
            case Dried_Head:            { return "Dried_Head"; }
            case Drink:                 { return "Drink"; }
            case DrumLute:              { return "DrumLute"; }
            case Egg:                   { return "Egg"; }
            case Embryo:                { return "Embryo"; }
            case Eye:                   { return "Eye"; }
            case Feather:               { return "Feather"; }
            case Figurine_Blessed:      { return "Figurine_Blessed"; }
            case Figurine_Cursed:       { return "Figurine_Cursed"; }
            case Finger:                { return "Finger"; }
            case Fingerclaw:            { return "Fingerclaw"; }
            case Flag:                  { return "Flag"; }
            case Frog:                  { return "Frog"; }
            case Gecko:                 { return "Gecko"; }
            case Ghost_Sheet:           { return "Ghost_Sheet"; }
            case Gizzard:               { return "Gizzard"; }
            case Goblet:                { return "Goblet"; }
            case Gong:                  { return "Gong"; }
            case Grave_Ornament:        { return "Grave_Ornament"; }
            case Handbag:               { return "Handbag"; }
            case Hanky:                 { return "Hanky"; }
            case Herbs:                 { return "Herbs"; }
            case Headdress:             { return "Headdress"; }
            case Hide:                  { return "Hide"; }
            case Hourglass:             { return "Hourglass"; }
            case Horn:                  { return "Horn"; }
            case Horseshoe:             { return "Horseshoe"; }
            case Hurdy_Gurdy:           { return "Hurdy_Gurdy"; }
            case Icicle:                { return "Icicle"; }
            case Iguana:                { return "Iguana"; }
            case Imp_Tail:              { return "Imp_Tail"; }
            case Key:                   { return "Key"; }
            case Lantern:               { return "Lantern"; }
            case Leaf:                  { return "Leaf"; }
            case Legtie:                { return "Legtie"; }
            case Litch_Hand:            { return "Litch_Hand"; }
            case Lizard:                { return "Lizard"; }
            case Lockbox:               { return "Lockbox"; }
            case LockPicks:             { return "LockPicks"; }
            case Lyre:                  { return "Lyre"; }
            case Magnifying_Glass:      { return "Magnifying_Glass"; }
            case Mask:                  { return "Mask"; }
            case Medallion:             { return "Medallion"; }
            case Mirror:                { return "Mirror"; }
            case Mummy_Hand:            { return "Mummy_Hand"; }
            case Necklace:              { return "Necklace"; }
            case Noose:                 { return "Noose"; }
            case Nose:                  { return "Nose"; }
            case Orb:                   { return "Orb"; }
            case Paw:                   { return "Paw"; }
            case Pendant:               { return "Pendant"; }
            case Petrified_Snake:       { return "Petrified_Snake"; }
            case Pin_Crown:             { return "Pin_Crown"; }
            case Pin_Feather:           { return "Pin_Feather"; }
            case Pin_Fist:              { return "Pin_Fist"; }
            case Pin_Hourglass:         { return "Pin_Hourglass"; }
            case Pin_Key:               { return "Pin_Key"; }
            case Pin_Mask:              { return "Pin_Mask"; }
            case Pipe_And_Tabor:        { return "Pipe_And_Tabor"; }
            case Potion:                { return "Potion"; }
            case Puppet:                { return "Puppet"; }
            case Rabbit_Foot:           { return "Rabbit_Foot"; }
            case Rainmaker:             { return "Rainmaker"; }
            case Rat_Juju:              { return "Rat_Juju"; }
            case Rattlesnake_Tail:      { return "Rattlesnake_Tail"; }
            case Recorder:              { return "Recorder"; }
            case Relic:                 { return "Relic"; }
            case Ring:                  { return "Ring"; }
            case Robe:                  { return "Robe"; }
            case Salamander:            { return "Salamander"; }
            case Salve:                 { return "Salve"; }
            case Scale:                 { return "Scale"; }
            case Scepter:               { return "Scepter"; }
            case Scroll:                { return "Scroll"; }
            case Scythe:                { return "Scythe"; }
            case Seeds:                 { return "Seeds"; }
            case Shard:                 { return "Shard"; }
            case Shark_Tooth_Necklace:  { return "Shark_Tooth_Necklace"; }
            case Shroud:                { return "Shroud"; }
            case Signet_Crown:          { return "Signet_Crown"; }
            case Signet_Feather:        { return "Signet_Feather"; }
            case Signet_Fist:           { return "Signet_Fist"; }
            case Signet_Hourglass:      { return "Signet_Hourglass"; }
            case Signet_Key:            { return "Signet_Key"; }
            case Signet_Mask:           { return "Signet_Mask"; }
            case Skink:                 { return "Skink"; }
            case Skull:                 { return "Skull"; }
            case Spider_Eggs:           { return "Spider_Eggs"; }
            case Spyglass:              { return "Spyglass"; }
            case Talisman:              { return "Talisman"; }
            case Tome:                  { return "Tome"; }
            case Tongue:                { return "Tongue"; }
            case Tooth:                 { return "Tooth"; }
            case Tooth_Necklace:        { return "Tooth_Necklace"; }
            case Torch:                 { return "Torch"; }
            case Troll_Figure:          { return "Troll_Figure"; }
            case Trophy:                { return "Trophy"; }
            case Tuning_Fork:           { return "Tuning_Fork"; }
            case Turtle_Shell:          { return "Turtle_Shell"; }
            case Unicorn_Horn:          { return "Unicorn_Horn"; }
            case Veil:                  { return "Veil"; }
            case Viol:                  { return "Viol"; }
            case Wand:                  { return "Wand"; }
            case Warhorse_Marionette:   { return "Warhorse_Marionette"; }
            case Weasel_Totem:          { return "Weasel_Totem"; }
            case Wolfen_Fur:            { return "Wolfen_Fur"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::item::misc_type::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

    const std::string misc_type::Name(const misc_type::Enum E)
    {
        switch (E)
        {
            case NotMisc:
            case Ankh_Necklace:         { return "Ankh Necklace"; }
            case Balm_Pot:              { return "Pot of Balm"; }
            case Bird_Claw:             { return "Raven Claw"; }
            case Bone_Whistle:          { return "Dragon Bone Whistle"; }
            case Bracelet_Crown:        { return "Crown Bracelet"; }
            case Bracelet_Feather:      { return "Feather Bracelet"; }
            case Bracelet_Fist:         { return "Fist Bracelet"; }
            case Bracelet_Hourglass:    { return "Hourglass Bracelet"; }
            case Bracelet_Key:          { return "Key Bracelet"; }
            case Bracelet_Mask:         { return "Mask Bracelet"; }
            case Brooch_Crown:          { return "Crown Brooch"; }
            case Brooch_Feather:        { return "Feather Brooch"; }
            case Brooch_Fist:           { return "Fist Brooch"; }
            case Brooch_Hourglass:      { return "Hourglass Brooch"; }
            case Brooch_Key:            { return "Key Brooch"; }
            case Brooch_Mask:           { return "Mask Brooch"; }
            case Charm_Crown:           { return "Crown Charm"; }
            case Charm_Feather:         { return "Feather Charm"; }
            case Charm_Fist:            { return "Fist Charm"; }
            case Charm_Hourglass:       { return "Hourglass Charm"; }
            case Charm_Key:             { return "Key Charm"; }
            case Charm_Mask:            { return "Mask Charm"; }
            case Devil_Horn:            { return "Devil Horn"; }
            case Doll_Blessed:          { return "Doll"; }
            case Doll_Cursed:           { return "Doll"; }
            case Dried_Head:            { return "Dried Head"; }
            case Figurine_Blessed:      { return "Figurine"; }
            case Figurine_Cursed:       { return "Figurine"; }
            case Ghost_Sheet:           { return "Ghost Sheet"; }
            case Grave_Ornament:        { return "Grave Ornament"; }
            case Hurdy_Gurdy:           { return "Hurdy-Gurdy"; }
            case Imp_Tail:              { return "Imp Tail"; }
            case Litch_Hand:            { return "Litch Hand"; }
            case Magnifying_Glass:      { return "Magnifying Glass"; }
            case Petrified_Snake:       { return "Petrified Snake"; }
            case Pin_Crown:             { return "Crown Pin"; }
            case Pin_Feather:           { return "Feather Pin"; }
            case Pin_Fist:              { return "Fist Pin"; }
            case Pin_Hourglass:         { return "Hourglass Pin"; }
            case Pin_Key:               { return "Key Pin"; }
            case Pin_Mask:              { return "Mask Pin"; }
            case Pipe_And_Tabor:        { return "Pipe and Tabor"; }
            case Rabbit_Foot:           { return "Rabbit Foot"; }
            case Rat_Juju:              { return "Rat Juju"; }
            case Rattlesnake_Tail:      { return "Rattlesnake Tail"; }
            case Shark_Tooth_Necklace:  { return "Shark Tooth Necklace"; }
            case Signet_Crown:          { return "Crown Signet"; }
            case Signet_Feather:        { return "Feather Signet"; }
            case Signet_Fist:           { return "Fist Signet"; }
            case Signet_Hourglass:      { return "Hourglass Signet"; }
            case Signet_Key:            { return "Key Signet"; }
            case Signet_Mask:           { return "Mask Signet"; }
            case Spider_Eggs:           { return "Spider Eggs"; }
            case Tooth_Necklace:        { return "Vampire Tooth Necklace"; }
            case Troll_Figure:          { return "Troll Figure"; }
            case Tuning_Fork:           { return "Tuning Fork"; }
            case Turtle_Shell:          { return "Turtle Shell"; }
            case Unicorn_Horn:          { return "Unicorn Horn"; }
            case Warhorse_Marionette:   { return "Warhorse Marionette"; }
            case Weasel_Totem:          { return "Weasel Totem"; }
            case Wolfen_Fur:            { return "Wolfen Fur"; }
            case Amulet:
            case Armband:
            case Bag:
            case Beard:
            case Bell:
            case Bone:
            case Book:
            case Braid:
            case Bust:
            case Cameo:
            case Cape:
            case Cat:
            case Chains:
            case Chest:
            case Chimes:
            case Cloak:
            case Conch:
            case Crown:
            case Crumhorn:
            case Doll:
            case Drink:
            case DrumLute:
            case Embryo:
            case Egg:
            case Eye:
            case Feather:
            case Finger:
            case Fingerclaw:
            case Flag:
            case Frog:
            case Gecko:
            case Gizzard:
            case Goblet:
            case Gong:
            case Handbag:
            case Hanky:
            case Headdress:
            case Herbs:
            case Hide:
            case Horn:
            case Horseshoe:
            case Hourglass:
            case Icicle:
            case Iguana:
            case Key:
            case Lantern:
            case Leaf:
            case Legtie:
            case Lizard:
            case Lockbox:
            case LockPicks:
            case Lyre:
            case Mask:
            case Medallion:
            case Mirror:
            case Mummy_Hand:
            case Necklace:
            case Noose:
            case Nose:
            case Orb:
            case Paw:
            case Pendant:
            case Potion:
            case Puppet:
            case Rainmaker:
            case Recorder:
            case Relic:
            case Ring:
            case Robe:
            case Salamander:
            case Salve:
            case Scale:
            case Scepter:
            case Scroll:
            case Scythe:
            case Seeds:
            case Skink:
            case Skull:
            case Shard:
            case Shroud:
            case Spyglass:
            case Talisman:
            case Torch:
            case Tome:
            case Tongue:
            case Tooth:
            case Trophy:
            case Veil:
            case Viol:
            case Wand: { return misc_type::ToString(E); }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::item::misc_type::Name(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::string named_type::ToString(const named_type::Enum E)
    {
        switch (E)
        {
            case NotNamed:      { return "NotNamed"; }
            case Arctic:        { return "Arctic"; }
            case Army:          { return "Army"; }
            case Betrayer:      { return "Betrayer"; }
            case Burglar:       { return "Burglar"; }
            case Burning:       { return "Burning"; }
            case Charlatans:    { return "Charlatans"; }
            case Charred:       { return "Charred"; }
            case Chill:         { return "Chill"; }
            case Dancing:       { return "Dancing"; }
            case Daring:        { return "Daring"; }
            case Dark:          { return "Dark"; }
            case Dawn:          { return "Dawn"; }
            case Diabolic:      { return "Diabolic"; }
            case Dusk:          { return "Dusk"; }
            case Elite:         { return "Elite"; }
            case Fiendish:      { return "Fiendish"; }
            case Fiery:         { return "Fiery"; }
            case Focus:         { return "Focus"; }
            case Frigid:        { return "Frigid"; }
            case Frozen:        { return "Frozen"; }
            case Gladiator:     { return "Gladiator"; }
            case Gloom:         { return "Gloom"; }
            case Glory:         { return "Glory"; }
            case Heros:         { return "Heros"; }
            case Honest:        { return "Honest"; }
            case Icy:           { return "Icy"; }
            case Imposters:     { return "Imposters"; }
            case Infernal:      { return "Infernal"; }
            case Knaves:        { return "Knaves"; }
            case Light:         { return "Light"; }
            case Marauder:      { return "Marauder"; }
            case Misery:        { return "Misery"; }
            case Moon:          { return "Moon"; }
            case Mountebank:    { return "Mountebank"; }
            case Muggers:       { return "Muggers"; }
            case Nile:          { return "Nile"; }
            case Noble:         { return "Noble"; }
            case Pickpocket:    { return "Pickpocket"; }
            case Pirate:        { return "Pirate"; }
            case Princes:       { return "Princes"; }
            case Proud:         { return "Proud"; }
            case Pure:          { return "Pure"; }
            case Raging:        { return "Raging"; }
            case Ranger:        { return "Ranger"; }
            case Robbers:       { return "Robbers"; }
            case Samurai:       { return "Samurai"; }
            case Searing:       { return "Searing"; }
            case Soldiers:      { return "Soldiers"; }
            case Sorrow:        { return "Sorrow"; }
            case Sun:           { return "Sun"; }
            case Thief:         { return "Theif"; }
            case Thors:         { return "Thors"; }
            case Thugs:         { return "Thugs"; }
            case Twilight:      { return "Twilight"; }
            case Valiant:       { return "Valiant"; }
            case Wardens:       { return "Wardens"; }
            case Wicked:        { return "Wicked"; }
            case Winter:        { return "Winter"; }
            case Woe:           { return "Woe"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::item::named_type::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::string named_type::Name(const named_type::Enum E)
    {
        switch (E)
        {
            case Charlatans:    { return "Charlatan's"; }
            case Heros:         { return "Hero's"; }
            case Imposters:     { return "Imposter's"; }
            case Knaves:        { return "Knave's"; }
            case Muggers:       { return "Mugger's"; }
            case Princes:       { return "Prince's"; }
            case Robbers:       { return "Robber's"; }
            case Soldiers:      { return "Soldier's"; }
            case Thors:         { return "Thor's"; }
            case Thugs:         { return "Thug's"; }
            case Wardens:       { return "Warden's"; }
            case NotNamed:
            case Arctic:
            case Army:
            case Betrayer:
            case Burglar:
            case Burning:
            case Charred:
            case Chill:
            case Dancing:
            case Daring:
            case Dark:
            case Dawn:
            case Diabolic:
            case Dusk:
            case Elite:
            case Fiendish:
            case Fiery:
            case Focus:
            case Frigid:
            case Frozen:
            case Gladiator:
            case Gloom:
            case Glory:
            case Honest:
            case Icy:
            case Infernal:
            case Light:
            case Marauder:
            case Misery:
            case Moon:
            case Mountebank:
            case Nile:
            case Noble:
            case Pickpocket:
            case Pirate:
            case Proud:
            case Pure:
            case Searing:
            case Raging:
            case Ranger:
            case Samurai:
            case Sorrow:
            case Sun:
            case Thief:
            case Twilight:
            case Valiant:
            case Wicked:
            case Winter:
            case Woe:       { return named_type::ToString(E); }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::item::named_type::Name(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    element_type::Enum named_type::ElementTypes(const named_type::Enum E)
    {
        auto elementType{ element_type::None };

        if ((E == named_type::Dancing) ||
            (E == named_type::Army) ||
            (E == named_type::Gladiator) ||
            (E == named_type::Focus))
        {
            elementType = static_cast<element_type::Enum>(element_type::Fire |
                                                          element_type::Frost |
                                                          element_type::Honor |
                                                          element_type::Shadow);
        }
        else if ((E == named_type::Wicked) ||
                 (E == named_type::Fiendish) ||
                 (E == named_type::Infernal) ||
                 (E == named_type::Raging) ||
                 (E == named_type::Diabolic) ||
                 (E == named_type::Marauder) ||
                 (E == named_type::Imposters) ||
                 (E == named_type::Pickpocket) ||
                 (E == named_type::Burglar) ||
                 (E == named_type::Mountebank) ||
                 (E == named_type::Charlatans) ||
                 (E == named_type::Robbers) ||
                 (E == named_type::Thugs) ||
                 (E == named_type::Knaves) ||
                 (E == named_type::Muggers) ||
                 (E == named_type::Thief) ||
                 (E == named_type::Pirate))
        {
            elementType = static_cast<element_type::Enum>(element_type::Fire |
                                                          element_type::Frost |
                                                          element_type::Shadow);
        }
        else if ((E == named_type::Honest) ||
                 (E == named_type::Noble) ||
                 (E == named_type::Daring) ||
                 (E == named_type::Elite) ||
                 (E == named_type::Valiant) ||
                 (E == named_type::Heros) ||
                 (E == named_type::Soldiers))
        {
            elementType = static_cast<element_type::Enum>(element_type::Fire |
                                                          element_type::Frost);
        }

        return elementType;
    }


    const MaterialVecPair_t named_type::Materials(const named_type::Enum E)
    {
        if ((E == named_type::Robbers) ||
            (E == named_type::Thugs) ||
            (E == named_type::Knaves) ||
            (E == named_type::Muggers) ||
            (E == named_type::Thief) ||
            (E == named_type::Pirate))
        {
            MaterialVec_t v{ material::CoreMetal() };
            v.push_back(material::Bone);
            v.push_back(material::Stone);
            v.push_back(material::Obsidian);
            return MaterialVecPair_t(v, {});
        }
        else
        {
            return MaterialVecPair_t({}, {});
        }
    }


    bool misc_type::IsMusicalInstrument(const misc_type::Enum E)
    {
        return ((E == DrumLute) ||
                (E == Crumhorn) ||
                (E == Hurdy_Gurdy) ||
                (E == Lyre) ||
                (E == Pipe_And_Tabor) ||
                (E == Recorder) ||
                (E == Viol));
    }


    const MaterialVecPair_t misc_type::Materials(const misc_type::Enum E)
    {
        if (E == misc_type::LockPicks)
        {
            MaterialVec_t v{ material::CoreMetal() };
            material::AppendCoreJewel(v);
            v.push_back(material::Bone);
            v.push_back(material::Obsidian);
            return MaterialVecPair_t(v, {});
        }
        else if ((E == misc_type::Spider_Eggs) ||
                 (E == misc_type::Litch_Hand) ||
                 (E == item::misc_type::Egg) ||
                 (E == item::misc_type::Embryo) ||
                 (E == item::misc_type::Petrified_Snake))
        {
            return MaterialVecPair_t({ material::Flesh }, {});
        }
        else if (E == misc_type::Wand)
        {
            MaterialVec_t v{ material::Wood, material::Glass };
            material::AppendCorePrimaryNoPearl(v);
            return MaterialVecPair_t(v, material::CoreSecondary());
        }
        else if (E == misc_type::Braid)
        {
            return MaterialVecPair_t({ material::Hair }, {});
        }
        else if (E == misc_type::DrumLute)
        {
            return MaterialVecPair_t({ material::Wood }, material::CoreSecondary());
        }
        else if (E == item::misc_type::Figurine_Blessed)
        {
            return MaterialVecPair_t({ material::Stone,
                                       material::Wood,
                                       material::Glass }, material::CoreSecondary());
        }
        else if (E == item::misc_type::Figurine_Cursed)
        {
            return MaterialVecPair_t({ material::Stone,
                                       material::Wood,
                                       material::Bone,
                                       material::Obsidian }, material::CoreSecondary());
        }
        else if (E == item::misc_type::Doll_Blessed)
        {
            return MaterialVecPair_t({ material::Stone,
                                       material::Wood,
                                       material::Glass }, material::CoreSecondary());
        }
        else if (E == item::misc_type::Doll_Cursed)
        {
            return MaterialVecPair_t({ material::Stone,
                                       material::Wood,
                                       material::Bone,
                                       material::Obsidian }, material::CoreSecondary());
        }
        else if (E == item::misc_type::Bust)
        {
            return MaterialVecPair_t({ material::Stone,
                                       material::Wood,
                                       material::Bone,
                                       material::Obsidian,
                                       material::Glass }, material::CoreSecondary());
        }
        else if (E == item::misc_type::Puppet)
        {
            return MaterialVecPair_t({ material::Wood,
                                       material::Cloth }, material::CoreSecondary());
        }
        else if (E == item::misc_type::Dried_Head)
        {
            return MaterialVecPair_t({ material::Flesh }, material::CoreSecondary());
        }
        else if (E == item::misc_type::Goblet)
        {
            return MaterialVecPair_t({ material::Tin,
                                       material::Bronze,
                                       material::Silver,
                                       material::Gold }, material::CoreSecondary());
        }
        else if (E == item::misc_type::Balm_Pot)
        {
            return MaterialVecPair_t({ material::Bronze,
                                       material::Silver,
                                       material::Gold }, {});
        }
        else if (E == item::misc_type::Seeds)
        {
            return MaterialVecPair_t({ material::Plant }, {});
        }
        else if (E == item::misc_type::Mummy_Hand)
        {
            return MaterialVecPair_t({ material::Flesh }, { material::Cloth });
        }
        else if (E == item::misc_type::Shard)
        {
            return MaterialVecPair_t(material::CoreJewel(), {});
        }
        else if (E == item::misc_type::Orb)
        {
            return MaterialVecPair_t(material::CoreJewel(), { material::Wood });
        }
        else if (E == item::misc_type::Scepter)
        {
            return MaterialVecPair_t(material::CorePrimaryNoPearl(), material::CoreSecondary());
        }
        else if (E == item::misc_type::Icicle)
        {
            return MaterialVecPair_t({ material::Glass }, {});
        }
        else if (E == item::misc_type::Finger)
        {
            return MaterialVecPair_t({ material::Stone }, {});
        }
        else if (E == item::misc_type::Unicorn_Horn)
        {
            return MaterialVecPair_t({ material::Horn }, {});
        }
        else if (E == item::misc_type::Devil_Horn)
        {
            return MaterialVecPair_t({ material::Horn }, {});
        }
        else if ((E == item::misc_type::Recorder) ||
                 (E == item::misc_type::Viol) ||
                 (E == item::misc_type::Pipe_And_Tabor) ||
                 (E == item::misc_type::Lyre) ||
                 (E == item::misc_type::Hurdy_Gurdy))
        {
            return MaterialVecPair_t({ material::Wood }, material::CoreSecondary());
        }
        else
        {
            return MaterialVecPair_t({}, {});
        }
    }


    const std::string material::ToString(const item::material::Enum E)
    {
        switch (E)
        {
            case Nothing    : { return "Nothing"; }
            case Wood       : { return "Wood"; }
            case HardLeather: { return "HardLeather"; }
            case SoftLeather: { return "SoftLeather"; }
            case Bone       : { return "Bone"; }
            case Water      : { return "Water"; }
            case Dirt       : { return "Dirt"; }
            case Acid       : { return "Acid"; }
            case Stone      : { return "Stone"; }
            case Plant      : { return "Plant"; }
            case Flesh      : { return "Flesh"; }
            case Hide       : { return "Hide"; }
            case Feather    : { return "Feather"; }
            case Fur        : { return "Fur"; }
            case Hair       : { return "Hair"; }
            case Scale      : { return "Scale"; }
            case Obsidian   : { return "Obsidian"; }
            case Jade       : { return "Jade"; }
            case Amethyst   : { return "Amethyst"; }
            case Emerald    : { return "Emerald"; }
            case Pearl      : { return "Pearl"; }
            case Ruby       : { return "Ruby"; }
            case Lazuli     : { return "Lazuli"; }
            case Sapphire   : { return "Sapphire"; }
            case Diamond    : { return "Diamond"; }
            case Glass      : { return "Glass"; }
            case Gas        : { return "Gas"; }
            case Blood      : { return "Blood"; }
            case Paper      : { return "Paper"; }
            case Rope       : { return "Rope"; }
            case Cloth      : { return "Cloth"; }
            case Claw       : { return "Claw"; }
            case Horn       : { return "Horn"; }
            case Tooth      : { return "Tooth"; }
            case Spirit     : { return "Spirit"; }
            case Tin        : { return "Tin"; }
            case Gold       : { return "Gold"; }
            case Iron       : { return "Iron"; }
            case Bronze     : { return "Bronze"; }
            case Silver     : { return "Silver"; }
            case Steel      : { return "Steel"; }
            case Platinum   : { return "Platinum"; }
            case Count      :
            default:
            {
                std::ostringstream ss;
                ss << "game::item::material::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::string material::ToReadableString(const item::material::Enum E)
    {
        if (E == material::SoftLeather)
        {
            return "Soft-Leather";
        }
        else if (E == material::HardLeather)
        {
            return "Hard-Leather";
        }
        else
        {
            return ToString(E);
        }
    }


    stats::Trait_t material::ArmorRatingBonus(const material::Enum MATERIAL_PRI,
                                              const material::Enum MATERIAL_SEC)
    {
        return ArmorRatingBonusPri(MATERIAL_PRI) + ArmorRatingBonusSec(MATERIAL_SEC);
    }


    stats::Trait_t material::ArmorRatingBonusPri(const material::Enum MATERIAL_PRI)
    {
        switch (MATERIAL_PRI)
        {
            case Nothing    :
            case Spirit     :
            case Gas        :
            case Water      :
            case Blood      :
            case Acid       :
            case Dirt       :
            case Paper      :
            case Glass      :
            case Feather    :
            case Fur        :
            case Hair       : { return 0; }
            case Flesh      : { return 1; }
            case Rope       :
            case Cloth      : { return 2; }
            case Hide       : { return 5; }
            case SoftLeather: { return 6; }
            case HardLeather: { return 10; }
            case Plant      : { return 11; }
            case Claw       : { return 12; }
            case Scale      : { return 13; }
            case Horn       : { return 14; }
            case Bone       : { return 15; }
            case Tooth      : { return 16; }
            case Wood       : { return 17; }
            case Stone      : { return 20; }
            case Obsidian   : { return 21; }
            case Tin        : { return 25; }
            case Bronze     : { return 26; }
            case Iron       : { return 27; }
            case Steel      : { return 28; }
            case Jade       : { return 29; }
            case Amethyst   : { return 30; }
            case Emerald    : { return 31; }
            case Pearl      : { return 32; }
            case Ruby       : { return 33; }
            case Lazuli     : { return 34; }
            case Silver     : { return 35; }
            case Sapphire   : { return 36; }
            case Gold       : { return 40; }
            case Platinum   : { return 45; }
            case Diamond    : { return 50; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::item::material::ArmorRatingBonusPri(" << MATERIAL_PRI
                    << ")_InvalidValueError.";

                throw std::range_error(ss.str());
            }
        }
    }


    stats::Trait_t material::ArmorRatingBonusSec(const material::Enum MATERIAL_SEC)
    {
        auto const SEC_MATERIAL_ARMOR_ADJ_RATIO{ GameDataFile::Instance()->GetCopyFloat(
            "heroespath-item-secondary-material-armor-adj-ratio") };

        return static_cast<stats::Trait_t>(static_cast<float>(ArmorRatingBonusPri(MATERIAL_SEC)) *
            SEC_MATERIAL_ARMOR_ADJ_RATIO);
    }


    stats::Trait_t material::PriceAdj(const material::Enum MATERIAL_PRI,
                                    const material::Enum MATERIAL_SEC)
    {
        return PriceAdjPri(MATERIAL_PRI) + PriceAdjSec(MATERIAL_SEC);
    }


    stats::Trait_t material::PriceAdjPri(const material::Enum MATERIAL_PRI)
    {
        switch (MATERIAL_PRI)
        {
            case Nothing    : { return 0; }
            case Spirit     : { return 0; }
            case Gas        : { return 0; }
            case Water      : { return 0; }
            case Blood      : { return 0; }
            case Acid       : { return 100; }
            case Dirt       : { return 0; }
            case Paper      : { return 0; }
            case Glass      : { return 10; }
            case Feather    : { return 0; }
            case Fur        : { return 8; }
            case Hair       : { return 0; }
            case Flesh      : { return 0; }
            case Rope       : { return 0; }
            case Cloth      : { return 0; }
            case Hide       : { return 7; }
            case SoftLeather: { return 5; }
            case HardLeather: { return 10; }
            case Plant      : { return 0; }
            case Claw       : { return 4; }
            case Scale      : { return 1000; }
            case Horn       : { return 6; }
            case Bone       : { return 12; }
            case Tooth      : { return 5; }
            case Wood       : { return 0; }
            case Stone      : { return 0; }
            case Obsidian   : { return 20; }
            case Tin        : { return 30; }
            case Bronze     : { return 32; }
            case Iron       : { return 36; }
            case Steel      : { return 40; }
            case Jade       : { return 100; }
            case Amethyst   : { return 200; }
            case Emerald    : { return 300; }
            case Pearl      : { return 500; }
            case Ruby       : { return 600; }
            case Lazuli     : { return 700; }
            case Silver     : { return 1000; }
            case Sapphire   : { return 2000; }
            case Gold       : { return 5000; }
            case Platinum   : { return 7500; }
            case Diamond    : { return 10000; }
            case Count      :
            default         :
            {
                std::ostringstream ss;

                ss << "game::item::material::PriceAdjPri(" << MATERIAL_PRI
                    << ")_InvalidValueError.";

                throw std::range_error(ss.str());
            }
        }
    }


    stats::Trait_t material::PriceAdjSec(const material::Enum E)
    {
        return PriceAdjPri(E) / 10;
    }


    float material::WeightMult(const material::Enum MATERIAL_PRI,
                               const material::Enum MATERIAL_SEC)
    {
        return WeightMultPri(MATERIAL_PRI) + WeightMultSec(MATERIAL_SEC);
    }


    float material::WeightMultPri(const material::Enum MATERIAL_PRI)
    {
        switch (MATERIAL_PRI)
        {
            case Nothing    : { return 0.0f; }
            case Spirit     : { return 0.0f; }
            case Gas        : { return 0.001f; }
            case Plant      : { return 0.9f; }
            case Cloth      : { return 0.9f; }
            case SoftLeather: { return 1.0f; }
            case HardLeather: { return 1.1f; }
            case Rope       : { return 1.1f; }
            case Bone       : { return 1.15f; }
            case Paper      : { return 1.2f; }
            case Claw       : { return 1.2f; }
            case Horn       : { return 1.2f; }
            case Tooth      : { return 1.2f; }
            case Water      : { return 1.5f; }
            case Acid       : { return 1.5f; }
            case Blood      : { return 1.55f; }
            case Flesh      : { return 1.6f; }
            case Hide       : { return 1.35f; }
            case Feather    : { return 0.5f; }
            case Fur        : { return 0.75f; }
            case Hair       : { return 0.5f; }
            case Scale      : { return 0.9f; }
            case Glass      : { return 1.6f; }
            case Wood       : { return 1.85f; }
            case Dirt       : { return 1.9f; }
            case Obsidian   : { return 2.2f; }
            case Jade       : { return 2.1f; }
            case Amethyst   : { return 2.1f; }
            case Emerald    : { return 2.15f; }
            case Pearl      : { return 1.2f; }
            case Ruby       : { return 2.2f; }
            case Lazuli      : { return 2.5f; }
            case Sapphire   : { return 2.1f; }
            case Diamond    : { return 2.6f; }
            case Stone      : { return 3.0f; }
            case Tin        : { return 4.0f; } // 7.3 - density
            case Bronze     : { return 4.2f; } // 7.5
            case Iron       : { return 4.6f; } // 7.8
            case Steel      : { return 4.8f; } // 8.0
            case Silver     : { return 7.3f; } // 10.5
            case Gold       : { return 16.6f; }// 19.3
            case Platinum   : { return 18.8f; }// 21.5
            case Count:
            default:
            {
                std::ostringstream ss;

                ss << "game::item::material::WeightMultPri(" << MATERIAL_PRI
                    << ")_InvalidValueError.";

                throw std::range_error(ss.str());
            }
        }
    }


    float material::WeightMultSec(const material::Enum MATERIAL_SEC)
    {
        return WeightMultPri(MATERIAL_SEC) / 10.0f;
    }


    int material::Bonus(const material::Enum E)
    {
        switch (E)
        {
            case Nothing    :
            case Spirit     :
            case Gas        :
            case Blood      :
            case Water      :
            case Dirt       :
            case Acid       :
            case Paper      :
            case Glass      :
            case Feather    :
            case Fur        :
            case Hair       :
            case Flesh      :
            case Rope       :
            case Cloth      :
            case Hide       :
            case SoftLeather:
            case HardLeather:
            case Plant      :
            case Claw       :
            case Scale      :
            case Horn       : { return 0; }
            case Bone       : { return 1; }
            case Tooth      : { return 0; }
            case Wood       : { return 0; }
            case Stone      : { return 2; }
            case Obsidian   : { return 3; }
            case Tin        : { return 4; }
            case Bronze     : { return 5; }
            case Iron       : { return 6; }
            case Steel      : { return 7; }
            case Jade       : { return 8; }
            case Amethyst   : { return 9; }
            case Emerald    : { return 10; }
            case Pearl      : { return 11; }
            case Ruby       : { return 12; }
            case Lazuli     : { return 13; }
            case Silver     : { return 14; }
            case Sapphire   : { return 15; }
            case Gold       : { return 16; }
            case Platinum   : { return 17; }
            case Diamond    : { return 18; }
            case Count      :
            default         :
            {
                std::ostringstream ss;
                ss << "game::item::material::Bonus(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::vector<material::Enum> material::CoreMetal()
    {
        return { material::Tin,
                 material::Bronze,
                 material::Iron,
                 material::Steel,
                 material::Silver,
                 material::Gold,
                 material::Platinum };
    }


    const std::vector<material::Enum> material::CoreJewel()
    {
        return { material::Jade,
                 material::Amethyst,
                 material::Emerald,
                 material::Ruby,
                 material::Lazuli,
                 material::Sapphire,
                 material::Diamond };
    }


    const std::vector<material::Enum> material::CoreMisc()
    {
        return { material::Bone,
                 material::Stone,
                 material::Obsidian,
                 material::Pearl };
    }


    const std::vector<material::Enum> material::CorePrimary()
    {
        auto const CORE_METAL_VEC{ CoreMetal() };
        auto const CORE_JEWEL_VEC{ CoreJewel() };
        auto const CORE_MISC_VEC{ CoreMisc() };

        std::vector<material::Enum> v;
        v.reserve(CORE_METAL_VEC.size() + CORE_JEWEL_VEC.size() + CORE_MISC_VEC.size());

        std::copy(CORE_METAL_VEC.begin(), CORE_METAL_VEC.end(), std::back_inserter(v));
        std::copy(CORE_JEWEL_VEC.begin(), CORE_JEWEL_VEC.end(), std::back_inserter(v));
        std::copy(CORE_MISC_VEC.begin(), CORE_MISC_VEC.end(), std::back_inserter(v));

        return v;
    }


    const std::vector<material::Enum> material::CorePrimaryNoPearl()
    {
        auto const CORE_METAL_VEC{ CoreMetal() };
        auto const CORE_JEWEL_VEC{ CoreJewel() };

        std::vector<material::Enum> v;
        v.reserve(CORE_METAL_VEC.size() + CORE_JEWEL_VEC.size() + 3);

        v.push_back(material::Bone);
        v.push_back(material::Stone);
        v.push_back(material::Obsidian);

        std::copy(CORE_METAL_VEC.begin(), CORE_METAL_VEC.end(), std::back_inserter(v));
        std::copy(CORE_JEWEL_VEC.begin(), CORE_JEWEL_VEC.end(), std::back_inserter(v));

        return v;
    }


    const std::vector<material::Enum> material::CoreSecondary()
    {
        auto const CORE_JEWEL_VEC{ CoreJewel() };

        std::vector<material::Enum> v;
        v.reserve(CORE_JEWEL_VEC.size() + 6);

        std::copy(CORE_JEWEL_VEC.begin(), CORE_JEWEL_VEC.end(), std::back_inserter(v));

        v.push_back(material::Obsidian);
        v.push_back(material::Pearl);
        v.push_back(material::Silver);
        v.push_back(material::Gold);
        v.push_back(material::Platinum);
        v.push_back(material::Nothing);

        return v;
    }


    void material::AppendCoreMetal(std::vector<material::Enum> & v)
    {
        auto const CORE_METAL_VEC{ CoreMetal() };
        std::copy(CORE_METAL_VEC.begin(), CORE_METAL_VEC.end(), std::back_inserter(v));
    }


    void material::AppendCoreJewel(std::vector<material::Enum> & v)
    {
        auto const CORE_JEWEL_VEC{ CoreJewel() };
        std::copy(CORE_JEWEL_VEC.begin(), CORE_JEWEL_VEC.end(), std::back_inserter(v));
    }


    void material::AppendCoreMisc(std::vector<material::Enum> & v)
    {
        auto const CORE_MISC_VEC{ CoreMisc() };
        std::copy(CORE_MISC_VEC.begin(), CORE_MISC_VEC.end(), std::back_inserter(v));
    }


    void material::AppendCorePrimary(std::vector<material::Enum> & v)
    {
        auto const CORE_PRIMARY_VEC{ CorePrimary() };
        std::copy(CORE_PRIMARY_VEC.begin(), CORE_PRIMARY_VEC.end(), std::back_inserter(v));
    }


    void material::AppendCorePrimaryNoPearl(std::vector<material::Enum> & v)
    {
        auto const CORE_PRIMARY_NOPEARL_VEC{ CorePrimaryNoPearl() };

        std::copy(CORE_PRIMARY_NOPEARL_VEC.begin(),
                  CORE_PRIMARY_NOPEARL_VEC.end(),
                  std::back_inserter(v));
    }


    void material::AppendCoreSecondary(std::vector<material::Enum> & v)
    {
        auto const CORE_SEC_VEC{ CoreSecondary() };
        std::copy(CORE_SEC_VEC.begin(), CORE_SEC_VEC.end(), std::back_inserter(v));
    }


    bool material::IsMagical(const material::Enum PRI, const material::Enum SEC)
    {
        return ContainsSpirit(PRI, SEC);
    }


    bool material::IsLiquid(const material::Enum MATERIAL)
    {
        return ((MATERIAL == material::Acid)  ||
                (MATERIAL == material::Blood) ||
                (MATERIAL == material::Water));
    }


    bool material::IsSolid(const material::Enum MATERIAL)
    {
        return ((IsLiquid(MATERIAL) == false) &&
                (MATERIAL != material::Gas)   &&
                (MATERIAL != material::Spirit));
    }


    bool material::IsBendy(const material::Enum MATERIAL)
    {
        return ((IsSolid(MATERIAL) == false)        ||
                (MATERIAL == material::SoftLeather) ||
                (MATERIAL == material::Dirt)        ||
                (MATERIAL == material::Plant)       ||
                (MATERIAL == material::Flesh)       ||
                (MATERIAL == material::Hide)        ||
                (MATERIAL == material::Feather)     ||
                (MATERIAL == material::Fur)         ||
                (MATERIAL == material::Hair)        ||
                (MATERIAL == material::Paper)       ||
                (MATERIAL == material::Rope)        ||
                (MATERIAL == material::Cloth));
    }


    bool material::IsRigid(const material::Enum MATERIAL)
    {
        return ! IsBendy(MATERIAL);
    }


    bool material::ContainsSpirit(const material::Enum PRI, const material::Enum SEC)
    {
        return ((PRI == material::Spirit) || (SEC == material::Spirit));
    }


    bool material::IsBloody(const material::Enum PRI, const material::Enum SEC)
    {
        return ((PRI == material::Blood) || (SEC == material::Blood));
    }


    bool material::ContainsFlesh(const material::Enum PRI, const material::Enum SEC)
    {
        return ((PRI == material::Flesh) || (SEC == material::Flesh));
    }


    float material::FireDamageRatio(const material::Enum PRI, const material::Enum SEC)
    {
        float fireDamageRatio(0.0f);
        switch (PRI)
        {
            case material::Nothing    : { break; }
            case material::Wood       : { fireDamageRatio = 0.5f;   break; }
            case material::HardLeather: { fireDamageRatio = 0.7f;   break; }
            case material::SoftLeather: { fireDamageRatio = 0.75f;  break; }
            case material::Bone       : { fireDamageRatio = 0.2f;   break; }
            case material::Water      : { fireDamageRatio = 0.01f;  break; }
            case material::Dirt       : { fireDamageRatio = 0.01f;  break; }
            case material::Acid       : { fireDamageRatio = 0.01f;  break; }
            case material::Stone      : { fireDamageRatio = 0.001f; break; }
            case material::Plant      : { fireDamageRatio = 0.85f;  break; }
            case material::Flesh      : { fireDamageRatio = 0.9f;   break; }
            case material::Hide       : { fireDamageRatio = 0.77f;  break; }
            case material::Feather    : { fireDamageRatio = 1.0f;   break; }
            case material::Fur        : { fireDamageRatio = 1.0f;   break; }
            case material::Hair       : { fireDamageRatio = 2.0f;   break; }
            case material::Scale      : { fireDamageRatio = 0.1f;   break; }
            case material::Obsidian   : { fireDamageRatio = 0.0f;   break; }
            case material::Jade       : { fireDamageRatio = 0.0f;   break; }
            case material::Amethyst   : { fireDamageRatio = 0.0f;   break; }
            case material::Emerald    : { fireDamageRatio = 0.0f;   break; }
            case material::Pearl      : { fireDamageRatio = 0.01f;  break; }
            case material::Ruby       : { fireDamageRatio = 0.0f;   break; }
            case material::Lazuli     : { fireDamageRatio = 0.0f;   break; }
            case material::Sapphire   : { fireDamageRatio = 0.0f;   break; }
            case material::Diamond    : { fireDamageRatio = 0.0f;   break; }
            case material::Glass      : { fireDamageRatio = 0.9f;   break; }
            case material::Gas        : { fireDamageRatio = 0.0f;   break; }
            case material::Blood      : { fireDamageRatio = 0.2f;   break; }
            case material::Paper      : { fireDamageRatio = 0.99f;  break; }
            case material::Rope       : { fireDamageRatio = 0.85f;  break; }
            case material::Cloth      : { fireDamageRatio = 0.85f;  break; }
            case material::Claw       : { fireDamageRatio = 0.1f;   break; }
            case material::Horn       : { fireDamageRatio = 0.1f;   break; }
            case material::Tooth      : { fireDamageRatio = 0.05f;  break; }
            case material::Spirit     : { fireDamageRatio = 0.0f;   break; }
            case material::Gold       : { fireDamageRatio = 0.01f;  break; }
            case material::Iron       : { fireDamageRatio = 0.01f;  break; }
            case material::Tin        : { fireDamageRatio = 0.01f;  break; }
            case material::Bronze     : { fireDamageRatio = 0.01f;  break; }
            case material::Silver     : { fireDamageRatio = 0.01f;  break; }
            case material::Steel      : { fireDamageRatio = 0.01f;  break; }
            case material::Platinum   : { fireDamageRatio = 0.01f;  break; }
            case material::Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::item::FireDamageRatio(material_pri=" << PRI
                    << ", material_sec=...) -but that material_pri type is unknown.";

                throw std::range_error(ss.str());
            }
        }
        switch (SEC)
        {
            case material::Nothing    : { break; }
            case material::Wood       : { fireDamageRatio += 0.2f;   break; }
            case material::HardLeather: { fireDamageRatio += 0.1f;   break; }
            case material::SoftLeather: { fireDamageRatio += 0.1f;   break; }
            case material::Bone       : { fireDamageRatio += 0.05f;  break; }
            case material::Water      : { fireDamageRatio -= 0.75f;  break; }
            case material::Dirt       : { fireDamageRatio -= 0.6f;   break; }
            case material::Acid       : { fireDamageRatio -= 0.75f;  break; }
            case material::Stone      : { fireDamageRatio -= 0.5f;   break; }
            case material::Plant      : { fireDamageRatio += 0.4f;   break; }
            case material::Flesh      : { fireDamageRatio += 0.4f;   break; }
            case material::Hide       : { fireDamageRatio += 0.1f;   break; }
            case material::Feather    : { fireDamageRatio += 0.2f;   break; }
            case material::Fur        : { fireDamageRatio += 0.3f;   break; }
            case material::Hair       : { fireDamageRatio += 0.5f;   break; }
            case material::Scale      : { fireDamageRatio -= 0.75f;  break; }
            case material::Obsidian   : { fireDamageRatio -= 0.5f;   break; }
            case material::Jade       : { fireDamageRatio -= 0.5f;   break; }
            case material::Amethyst   : { fireDamageRatio -= 0.5f;   break; }
            case material::Emerald    : { fireDamageRatio -= 0.5f;   break; }
            case material::Pearl      : { fireDamageRatio -= 0.5f;   break; }
            case material::Ruby       : { fireDamageRatio -= 0.5f;   break; }
            case material::Lazuli     : { fireDamageRatio -= 0.5f;   break; }
            case material::Sapphire   : { fireDamageRatio -= 0.5f;   break; }
            case material::Diamond    : { fireDamageRatio -= 0.5f;   break; }
            case material::Glass      : { break; }
            case material::Gas        : { break; }
            case material::Blood      : { fireDamageRatio -= 0.75f;  break; }
            case material::Paper      : { fireDamageRatio += 0.5f;   break; }
            case material::Rope       : { fireDamageRatio += 0.25f;  break; }
            case material::Cloth      : { fireDamageRatio += 0.25f;  break; }
            case material::Claw       : { fireDamageRatio -= 0.1f;   break; }
            case material::Horn       : { fireDamageRatio -= 0.1f;   break; }
            case material::Tooth      : { fireDamageRatio -= 0.1f;   break; }
            case material::Spirit     : { break; }
            case material::Gold       : { fireDamageRatio -= 0.5f;   break; }
            case material::Iron       : { fireDamageRatio -= 0.5f;   break; }
            case material::Tin        : { fireDamageRatio -= 0.5f;   break; }
            case material::Bronze     : { fireDamageRatio -= 0.5f;   break; }
            case material::Silver     : { fireDamageRatio -= 0.5f;   break; }
            case material::Steel      : { fireDamageRatio -= 0.5f;   break; }
            case material::Platinum   : { fireDamageRatio -= 0.5f;   break; }
            case material::Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::item::FireDamageRatio(material_pri=" << material::ToString(PRI)
                    << ", material_sec=" << SEC << " -but that material_sec type is unknown.";

                throw std::range_error(ss.str());
            }
        }

        if (fireDamageRatio < 0.0f)
        {
            fireDamageRatio = 0.0f;
        }

        if (fireDamageRatio > 1.0f)
        {
            fireDamageRatio = 1.0f;
        }

        return fireDamageRatio;
    }


    bool material::IsMetal(const material::Enum MATERIAL)
    {
        return ((MATERIAL == material::Tin)    ||
                (MATERIAL == material::Gold)   ||
                (MATERIAL == material::Iron)   ||
                (MATERIAL == material::Bronze) ||
                (MATERIAL == material::Silver) ||
                (MATERIAL == material::Steel)  ||
                (MATERIAL == material::Platinum));
    }


    bool material::ContainsMetal(const material::Enum PRI, const material::Enum SEC)
    {
        return IsMetal(PRI) || IsMetal(SEC);
    }


    bool material::IsStone(const material::Enum PRI)
    {
        return ((PRI == material::Stone)    ||
                (PRI == material::Jade)     ||
                (PRI == material::Obsidian) ||
                (PRI == material::Amethyst) ||
                (PRI == material::Emerald)  ||
                (PRI == material::Ruby)     ||
                (PRI == material::Lazuli)   ||
                (PRI == material::Sapphire));
    }


    bool material::IsPrecious(const material::Enum MATERIAL)
    {
        return (IsJewel(MATERIAL)                ||
                (MATERIAL == material::Obsidian) ||
                (MATERIAL == material::Pearl)    ||
                (MATERIAL == material::Silver)   ||
                (MATERIAL == material::Gold)     ||
                (MATERIAL == material::Platinum));
    }


    bool material::ContiansPrecious(const material::Enum PRI, const material::Enum SEC)
    {
        return IsPrecious(PRI) || IsPrecious(SEC);
    }


    bool material::IsJewel(const material::Enum MATERIAL)
    {
        return ((MATERIAL == material::Amethyst) ||
                (MATERIAL == material::Jade)     ||
                (MATERIAL == material::Emerald)  ||
                (MATERIAL == material::Ruby)     ||
                (MATERIAL == material::Sapphire) ||
                (MATERIAL == material::Lazuli)   ||
                (MATERIAL == material::Diamond));
    }


    bool material::ContainsJewel(const material::Enum PRI, const material::Enum SEC)
    {
        return IsJewel(PRI) || IsJewel(SEC);
    }


    const std::string weapon_type::ToString(const item::weapon_type::Enum E,
                                            const bool WILL_WRAP)
    {
        std::ostringstream ss;

        if (E == weapon_type::NotAWeapon)
        {
            ss << "not a weapon";
        }
        else
        {
            if (E & weapon_type::Bladed)      ss << "bladed";
            if (E & weapon_type::Melee)       ss << ((ss.str().empty()) ? "" : "/") << "melee";
            if (E & weapon_type::Projectile)  ss << ((ss.str().empty()) ? "" : "/") << "projectile";
            if (E & weapon_type::Pointed)     ss << ((ss.str().empty()) ? "" : "/") << "pointed";
            if (E & weapon_type::Claws)       ss << ((ss.str().empty()) ? "" : "/") << "claws";
            if (E & weapon_type::Bite)        ss << ((ss.str().empty()) ? "" : "/") << "bite";
            if (E & weapon_type::Sword)       ss << ((ss.str().empty()) ? "" : "/") << "sword";
            if (E & weapon_type::Axe)         ss << ((ss.str().empty()) ? "" : "/") << "axe";
            if (E & weapon_type::Whip)        ss << ((ss.str().empty()) ? "" : "/") << "whip";
            if (E & weapon_type::Blowpipe)    ss << ((ss.str().empty()) ? "" : "/") << "blowpipe";
            if (E & weapon_type::Bow)         ss << ((ss.str().empty()) ? "" : "/") << "bow";
            if (E & weapon_type::Crossbow)    ss << ((ss.str().empty()) ? "" : "/") << "crossbow";
            if (E & weapon_type::Spear)       ss << ((ss.str().empty()) ? "" : "/") << "spear";
            if (E & weapon_type::Knife)       ss << ((ss.str().empty()) ? "" : "/") << "knife";
            if (E & weapon_type::Club)        ss << ((ss.str().empty()) ? "" : "/") << "club";
            if (E & weapon_type::Staff)       ss << ((ss.str().empty()) ? "" : "/") << "staff";
            if (E & weapon_type::Sling)       ss << ((ss.str().empty()) ? "" : "/") << "sling";
            if (E & weapon_type::BladedStaff) ss << ((ss.str().empty()) ? "" : "/") << "bladed staff";
            if (E & weapon_type::Fists)       ss << ((ss.str().empty()) ? "" : "/") << "fists";
            if (E & weapon_type::Tendrils)    ss << ((ss.str().empty()) ? "" : "/") << "tendrils";
            if (E & weapon_type::Breath)      ss << ((ss.str().empty()) ? "" : "/") << "breath";
        }

        if ((ss.str().empty()) || (ss.str() == "("))
        {
            std::ostringstream ssErr;
            ssErr << "game::item::weapon_type::ToString(" << E << ")_InvalidValueError";
            throw std::range_error(ssErr.str());
        }

        if (WILL_WRAP)
        {
            return "(" + ss.str() + ")";
        }
        else
        {
            return ss.str();
        }
    }

    weapon_type::Enum weapon_type::FromString(const std::string & S)
    {
        if      (S == "Melee")      return Melee;
        else if (S == "Projectile") return Projectile;
        else if (S == "Bladed")     return Bladed;
        else if (S == "Pointed")    return Pointed;
        else if (S == "Claws")      return Claws;
        else if (S == "Bite")       return Bite;
        else if (S == "Sword")      return Sword;
        else if (S == "Axe")        return Axe;
        else if (S == "Whip")       return Whip;
        else if (S == "Blowpipe")   return Blowpipe;
        else if (S == "Bow")        return Bow;
        else if (S == "Crossbow")   return Crossbow;
        else if (S == "Spear")      return Spear;
        else if (S == "Knife")      return Knife;
        else if (S == "Club")       return Club;
        else if (S == "Staff")      return Staff;
        else if (S == "Sling")      return Sling;
        else if (S == "BladedStaff")return BladedStaff;
        else if (S == "Fists")      return Fists;
        else if (S == "Tendrils")   return Tendrils;
        else if (S == "Breath")     return Breath;
        else
        {
            return NotAWeapon;
        }
    }


    const std::string armor_type::ToString(const item::armor_type::Enum E, const bool WILL_WRAP)
    {
        std::ostringstream ss;

        if (E == armor_type::NotArmor)
        {
            ss << "not armor";
        }
        else
        {
            if (E & armor_type::Sheild)      ss << "sheild";
            if (E & armor_type::Helm)        ss << ((ss.str().empty()) ? "" : "/") << "helm";
            if (E & armor_type::Gauntlets)   ss << ((ss.str().empty()) ? "" : "/") << "gauntlets";
            if (E & armor_type::Pants)       ss << ((ss.str().empty()) ? "" : "/") << "pants";
            if (E & armor_type::Boots)       ss << ((ss.str().empty()) ? "" : "/") << "boots";
            if (E & armor_type::Shirt)       ss << ((ss.str().empty()) ? "" : "/") << "shirt";
            if (E & armor_type::Bracer)      ss << ((ss.str().empty()) ? "" : "/") << "bracer";
            if (E & armor_type::Aventail)    ss << ((ss.str().empty()) ? "" : "/") << "aventail";
            if (E & armor_type::Skin)        ss << ((ss.str().empty()) ? "" : "/") << "skin";
            if (E & armor_type::Covering)    ss << ((ss.str().empty()) ? "" : "/") << "covering";
        }

        if ((ss.str().empty()) || (ss.str() == "("))
        {
            std::ostringstream ssErr;
            ssErr << "game::item::armor_type::ToString(" << E << ")_InvalidValueError";
            throw std::range_error(ssErr.str());
        }

        if (WILL_WRAP)
        {
            return "(" + ss.str() + ")";
        }
        else
        {
            return ss.str();
        }
    }

}
}
