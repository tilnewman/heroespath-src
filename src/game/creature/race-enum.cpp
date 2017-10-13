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
// race-enum.hpp
//
#include "race-enum.hpp"

#include "game/game-data-file.hpp"

#include <algorithm>
#include <exception>
#include <sstream>


namespace game
{
namespace creature
{

    const std::string origin_type::ToString(const origin_type::Enum E)
    {
        switch (E)
        {
            case Statue:    { return "Statue"; }
            case Egg:       { return "Egg"; }
            case Embryo:    { return "Embryo"; }
            case Seeds:     { return "Seeds"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::creature::origin_type::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    std::size_t origin_type::UseCount(const origin_type::Enum E)
    {
        switch (E)
        {
            case Statue:    { return 20; }
            case Seeds:     { return 10; }
            case Egg:
            case Embryo:    { return 1; }
            case Count:
            default:        { return 0; }
        }
    }


    const std::string race::ToString(const race::Enum E)
    {
        switch (E)
        {
            case Human:        { return "Human"; }
            case Gnome:        { return "Gnome"; }
            case Pixie:        { return "Pixie"; }
            case Wolfen:       { return "Wolfen"; }
            case Dragon:       { return "Dragon"; }
            case Goblin:       { return "Goblin"; }
            case Troll:        { return "Troll"; }
            case Orc:          { return "Orc"; }
            case Newt:         { return "Newt"; }
            case Spider:       { return "Spider"; }
            case Bog:          { return "Bog"; }
            case CaveCrawler:  { return "CaveCrawler"; }
            case Hydra:        { return "Hydra"; }
            case LizardWalker: { return "LizardWalker"; }
            case Minotaur:     { return "Minotaur"; }
            case Ogre:         { return "Ogre"; }
            case Plant:        { return "Plant"; }
            case Shade:        { return "Shade"; }
            case Skeleton:     { return "Skeleton"; }
            case Demon:        { return "Demon"; }
            case Griffin:      { return "Griffin"; }
            case Halfling:     { return "Halfling"; }
            case Lion:         { return "Lion"; }
            case LionBoar:     { return "LionBoar"; }
            case Naga:         { return "Naga"; }
            case Ramonaut:     { return "Ramonaut"; }
            case Serpent:      { return "Serpent"; }
            case Cobra:        { return "Cobra"; }
            case Werebear:     { return "Werebear"; }
            case Wereboar:     { return "Wereboar"; }
            case Werecat:      { return "Werecat"; }
            case Werewolf:     { return "Werewolf"; }
            case Werebat:      { return "Werebat"; }
            case Pug:          { return "Pug"; }
            case Wyvern:       { return "Wyvern"; }
            case Giant:        { return "Giant"; }
            case Ghoul:        { return "Ghoul"; }
            case Beetle:       { return "Beetle"; }
            case Boar:         { return "Boar"; }
            case Bat:               { return "Bat"; }
            case Witch:             { return "Witch"; }
            case Golem:             { return "Golem"; }
            case Harpy:             { return "Harpy"; }
            case ThreeHeadedHound:  { return "ThreeHeadedHound"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::creature::race::Name(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::string race::Name(const race::Enum E)
    {
        if (E == Spider)
        {
            return "Giant Spider";
        }
        else
        {
            return ToString(E);
        }
    }


    const std::string race::Desc(const race::Enum E)
    {
        std::ostringstream ss;
        ss << "heroespath-creature-race-desc-" << ToString(E);
        return GameDataFile::Instance()->GetCopyStr(ss.str());
    }


    const std::string race::Abbr(const race::Enum E)
    {
        if      (E == Wolfen)   { return "Wlf"; }
        else if (E == Dragon)   { return "Drg"; }
        else if (E == Werebat)  { return "Wba"; }
        else if (E == Werebear) { return "Wbe"; }
        else if (E == Wereboar) { return "Wbo"; }
        else if (E == Werecat)  { return "Wca"; }
        else if (E == Werewolf) { return "Wwf"; }
        else
            return Name(E).substr(0, 3);
    }


    bool race::HasTorso(const race::Enum E)
    {
        switch (E)
        {
            case Human:
            case Gnome:
            case Pixie:
            case Goblin:
            case Troll:
            case Orc:
            case Newt:
            case Bog:
            case LizardWalker:
            case Minotaur:
            case Ogre:
            case Shade:
            case Skeleton:
            case Demon:
            case Halfling:
            case Naga:
            case Pug:
            case Giant:
            case Witch:
            case Golem:
            case Harpy:
            case Ghoul: { return true; }
            case Wolfen:
            case Dragon:
            case Spider:
            case CaveCrawler:
            case Hydra:
            case Plant:
            case Griffin:
            case Lion:
            case LionBoar:
            case Ramonaut:
            case Serpent:
            case Cobra:
            case Werebear:
            case Wereboar:
            case Werecat:
            case Werewolf:
            case Werebat:
            case Wyvern:
            case Beetle:
            case Bat:
            case ThreeHeadedHound:
            case Boar: { return false; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::creature::race::HasTorso(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    bool race::CanFly(const race::Enum E)
    {
        switch (E)
        {
            case Human:
            case Gnome:
            case Pixie:
            case Goblin:
            case Troll:
            case Orc:
            case Newt:
            case Bog:
            case LizardWalker:
            case Minotaur:
            case Ogre:
            case Halfling:
            case Naga:
            case Pug:
            case Giant:
            case Ghoul:
            case Wolfen:
            case Spider:
            case CaveCrawler:
            case Hydra:
            case Plant:
            case Lion:
            case LionBoar:
            case Ramonaut:
            case Serpent:
            case Cobra:
            case Werebear:
            case Wereboar:
            case Werecat:
            case Werewolf:
            case Beetle:
            case Boar:
            case Witch:
            case Golem:
            case ThreeHeadedHound:
            case Skeleton:  { return false; }
            case Demon:
            case Shade:
            case Griffin:
            case Werebat:
            case Wyvern:
            case Bat:
            case Harpy:
            case Dragon:    { return true; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::creature::race::HasTorso(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    bool race::WillInitiallyFly(const race::Enum E)
    {
        return ((E == race::Pixie) ||
                (E == race::Shade) ||
                (E == race::Griffin) ||
                (E == race::Werebat) ||
                (E == race::Bat) ||
                (E == race::Harpy));
    }


    //At last count on 2017-8-3 there were 167 unique race/roles.
    const RoleVec_t race::Roles(const race::Enum E)
    {
        switch (E)
        {
            case Human:
            {
                return { role::Knight,
                         role::Archer,
                         role::Bard,
                         role::Beastmaster,
                         role::Cleric,
                         role::Knight,
                         role::Thief,
                         role::Sorcerer,
                         role::Ranger,
                         role::Trader,
                         role::Drunk,
                         role::Brute,
                         role::Smasher,
                         role::Soldier,
                         role::Blacksmith,
                         role::Shaman,
                         role::Grunt };
            }
            case Gnome:
            {
                return { role::Bard,
                         role::Cleric,
                         role::Sorcerer,
                         role::Knight,
                         role::Beastmaster,
                         role::Archer,
                         role::Thief,
                         role::Grunt };
            }
            case Pixie:
            {
                return { role::Beastmaster,
                         role::Sorcerer,
                         role::Cleric,
                         role::Bard };
            }
            case Goblin:
            {
                return { role::Thief,
                         role::Sorcerer,
                         role::Brute,
                         role::Archer,
                         role::Captain,
                         role::Chieftain,
                         role::Shaman,
                         role::Trader,
                         role::Grunt };
            }
            case Troll:
            {
                return { role::Berserker,
                         role::Brute,
                         role::Chieftain,
                         role::Grunt,
                         role::Mountain,
                         role::Shaman,
                         role::Smasher,
                         role::Strangler,
                         role::Warlord };
            }
            case Orc:
            {
                return { role::Berserker,
                         role::Chieftain,
                         role::Elder,
                         role::FourArmed,
                         role::Captain,
                         role::Mountain,
                         role::Spike,
                         role::Grunt,
                         role::Shaman,
                         role::Smasher,
                         role::Warlord };
            }
            case Newt:
            {
                return { role::Chieftain,
                         role::Cleric,
                         role::Elder,
                         role::Sorcerer,
                         role::Trader,
                         role::Grunt };
            }
            case Bog:
            {
                return { role::Chieftain,
                         role::Smasher,
                         role::Sorcerer,
                         role::Shaman,
                         role::Spike,
                         role::Tendrilus,
                         role::Wing,
                         role::Whelp,
                         role::Grunt };
            }
            case LizardWalker:
            {
                return { role::Whelp,
                         role::Sorcerer,
                         role::Captain,
                         role::Chieftain,
                         role::Spike,
                         role::Shaman,
                         role::Grunt };
            }
            case Minotaur:
            {
                return { role::Grunt,
                         role::Brute,
                         role::FourArmed,
                         role::Mountain,
                         role::Shaman,
                         role::Warlord,
                         role::Grunt };
            }
            case Ogre:
            {
                return { role::Berserker,
                         role::Brute,
                         role::Elder,
                         role::FourArmed,
                         role::Giant,
                         role::Mountain,
                         role::Warlord,
                         role::Captain,
                         role::Chieftain,
                         role::Shaman,
                         role::Spike,
                         role::Grunt };
            }
            case Halfling:
            {
                return { role::Shaman,
                         role::Chieftain,
                         role::Captain,
                         role::Grunt };
            }
            case Naga:
            {
                return { role::Grunt };
            }
            case Pug:
            {
                return { role::Archer,
                         role::Cleric,
                         role::Grunt };
            }
            case Giant:
            {
                return { role::Smasher,
                         role::Strangler,
                         role::Warlord,
                         role::Mountain,
                         role::Grunt,
                         role::Brute };
            }
            case Ghoul:
            {
                return { role::Strangler,
                         role::Spike,
                         role::Mountain,
                         role::Grunt };
            }
            case Wolfen:
            {
                return { role::Wolfen, role::TwoHeaded };
            }
            case Spider:
            {
                return { role::Spider };
            }
            case CaveCrawler:
            {
                return { role::Mountain };
            }
            case Hydra:
            {
                return { role::Wing };
            }
            case Plant:
            {
                return { role::Smasher,
                         role::Tendrilus,
                         role::Pod,
                         role::Strangler};
            }
            case Lion:
            case LionBoar:
            {
                return { role::Lion };
            }
            case Ramonaut:
            {
                return { role::Ramonaut };
            }
            case Serpent:
            {
                return { role::Serpent, role::Water };
            }
            case Cobra:
            {
                return { role::Serpent };
            }
            case Werebear:
            {
                return { role::Elder,
                         role::Grunt,
                         role::Mountain };
            }
            case Wereboar:
            {
                return { role::Boar };
            }
            case Werecat:
            {
                return { role::Cat };
            }
            case Werewolf:
            {
                return { role::Wolf };
            }
            case Beetle:
            {
                return { role::Beetle };
            }
            case Boar:
            {
                return { role::Boar };
            }
            case Witch:
            {
                return { role::Elder,
                         role::Ghost,
                         role::Strangler,
                         role::Mountain,
                         role::Sorcerer };
            }
            case Golem:
            {
                return { role::Brute };
            }
            case ThreeHeadedHound:
            {
                return { role::Mountain };
            }
            case Skeleton:
            {
                return { role::Chieftain,
                         role::FourArmed,
                         role::Mountain,
                         role::Grunt };
            }
            case Demon:
            {
                return { role::Skeleton,
                         role::Spike,
                         role::Strangler,
                         role::Whelp,
                         role::Wing,
                         role::Grunt };
            }
            case Shade:
            {
                return { role::Ghost };
            }
            case Griffin:
            {
                return { role::Whelp, role::Wing };
            }
            case Werebat:
            {
                return { role::Bat };
            }
            case Wyvern:
            {
                return { role::Whelp, role::Wing };
            }
            case Bat:
            {
                return { role::Bat };
            }
            case Harpy:
            {
                return { role::Wing };
            }
            case Dragon:
            {
                return { role::Firebrand, role::Sylavin };
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::creature::race::HasTorso(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    bool race::RaceRoleMatch(const race::Enum RACE_ENUM, const role::Enum ROLE_ENUM)
    {
        return (race::ToString(RACE_ENUM) == role::ToString(ROLE_ENUM));
    }


    const OriginTypeVec_t race::OriginTypes(const race::Enum RACE, const role::Enum ROLE)
    {
        OriginTypeVec_t v(1, origin_type::Statue);

        switch (RACE)
        {
            case race::Human:
            case race::Orc:
            case race::Minotaur:
            case race::Ogre:
            case race::Halfling:
            case race::Pug:
            case race::Giant:
            case race::Wolfen:
            case race::Lion:
            case race::LionBoar:
            case race::Ramonaut:
            case race::Boar:
            {
                v.push_back(origin_type::Embryo);
                break;
            }
            case race::Demon:
            case race::Gnome:
            case race::Newt:
            case race::Bog:
            case race::Plant:
            {
                v.push_back(origin_type::Seeds);
                break;
            }
            case race::Pixie:
            case race::Goblin:
            case race::Troll:
            case race::Hydra:
            {
                v.push_back(origin_type::Egg);
                v.push_back(origin_type::Seeds);
                break;
            }
            case race::LizardWalker:
            case race::Naga:
            case race::Harpy:
            case race::Dragon:
            case race::Griffin:
            case race::Serpent:
            case race::Cobra:
            case race::Wyvern:
            {
                v.push_back(origin_type::Egg);
                break;
            }
            case race::Shade:
            case race::Skeleton:
            case race::Witch:
            case race::Golem:
            case race::Ghoul:
            case race::Spider:
            case race::CaveCrawler:
            case race::Werebear:
            case race::Wereboar:
            case race::Werecat:
            case race::Werewolf:
            case race::Werebat:
            case race::Beetle:
            case race::Bat:
            case race::ThreeHeadedHound:
            case race::Count:
            default:
            {
                break;
            }
        }

        if (ROLE == role::Tendrilus)
        {
            v.push_back(origin_type::Seeds);
        }
        else if ((ROLE == role::Wing) ||
                 (ROLE == role::Whelp) ||
                 (ROLE == role::Tendrilus))
        {
            v.push_back(origin_type::Egg);
        }

        std::sort(v.begin(), v.end());
        v.erase(std::unique(v.begin(), v.end()), v.end());
        return v;
    }


    const item::TreasureInfo race::TreasureScore(const race::Enum RACE_ENUM,
                                                 const role::Enum ROLE_ENUM)
    {
        stats::Trait_t coin{ 0 }, gem{ 0 }, magic{ 0 }, rel{ 0 };

        //these numbers intentionally max at 350, 300, 300, 300 -see comment below
        switch (RACE_ENUM)
        {
            case race::Human:            { coin += 60;  gem += 50;  magic += 100; rel += 50;  break; }
            case race::Orc:              { coin += 30;  gem += 20;  magic += 60;  rel += 20;  break; }
            case race::Minotaur:         { coin += 20;  gem += 5;   magic += 50;  rel += 10;  break; }
            case race::Ogre:             { coin += 15;  gem += 5;   magic += 66;  rel += 25;  break; }
            case race::Halfling:         { coin += 80;  gem += 75;  magic += 90;  rel += 50;  break; }
            case race::Pug:              { coin += 45;  gem += 10;  magic += 20;  rel += 30;  break; }
            case race::Giant:            { coin += 30;  gem += 10;  magic += 65;  rel += 30;  break; }
            case race::Wolfen:           { coin += 0;   gem += 5;   magic += 10;  rel += 10;  break; }
            case race::Lion:             { coin += 0;   gem += 0;   magic += 0;   rel += 0;   break; }
            case race::LionBoar:         { coin += 0;   gem += 0;   magic += 0;   rel += 0;   break; }
            case race::Ramonaut:         { coin += 0;   gem += 0;   magic += 0;   rel += 0;   break; }
            case race::Boar:             { coin += 0;   gem += 0;   magic += 0;   rel += 0;   break; }
            case race::Demon:            { coin += 225; gem += 125; magic += 200; rel += 200; break; }
            case race::Gnome:            { coin += 175; gem += 75;  magic += 120; rel += 40;  break; }
            case race::Newt:             { coin += 30;  gem += 10;  magic += 25;  rel += 20;  break; }
            case race::Bog:              { coin += 10;  gem += 0;   magic += 20;  rel += 0;   break; }
            case race::Plant:            { coin += 0;   gem += 0;   magic += 5;   rel += 0;   break; }
            case race::Pixie:            { coin += 125; gem += 100; magic += 125; rel += 50;  break; }
            case race::Goblin:           { coin += 75;  gem += 12;  magic += 50;  rel += 25;  break; }
            case race::Troll:            { coin += 40;  gem += 25;  magic += 75;  rel += 30;  break; }
            case race::Hydra:            { coin += 200; gem += 110; magic += 180; rel += 100; break; }
            case race::LizardWalker:     { coin += 125; gem += 20;  magic += 100; rel += 40;  break; }
            case race::Naga:             { coin += 166; gem += 35;  magic += 133; rel += 60;  break; }
            case race::Harpy:            { coin += 125; gem += 18;  magic += 100; rel += 80;  break; }
            case race::Dragon:           { coin += 350; gem += 300; magic += 300; rel += 150; break; }
            case race::Griffin:          { coin += 95;  gem += 20;  magic += 75;  rel += 20;  break; }
            case race::Serpent:          { coin += 0;   gem += 0;   magic += 0;   rel += 30;  break; }
            case race::Cobra:            { coin += 0;   gem += 0;   magic += 0;   rel += 0;   break; }
            case race::Wyvern:           { coin += 100; gem += 35;  magic += 80;  rel += 50;  break; }
            case race::Shade:            { coin += 85;  gem += 150; magic += 150; rel += 100; break; }
            case race::Skeleton:         { coin += 120; gem += 35;  magic += 100; rel += 50;  break; }
            case race::Witch:            { coin += 105; gem += 175; magic += 180; rel += 300; break; }
            case race::Golem:            { coin += 55;  gem += 100; magic += 150; rel += 50;  break; }
            case race::Ghoul:            { coin += 30;  gem += 35;  magic += 60;  rel += 60;  break; }
            case race::Spider:           { coin += 0;   gem += 0;   magic += 0;   rel += 0;   break; }
            case race::CaveCrawler:      { coin += 10;  gem += 0;   magic += 0;   rel += 0;   break; }
            case race::Werebear:         { coin += 0;   gem += 0;   magic += 50;  rel += 0;   break; }
            case race::Wereboar:         { coin += 0;   gem += 0;   magic += 0;   rel += 0;   break; }
            case race::Werecat:          { coin += 0;   gem += 0;   magic += 0;   rel += 0;   break; }
            case race::Werewolf:         { coin += 0;   gem += 10;  magic += 100; rel += 50;  break; }
            case race::Werebat:          { coin += 0;   gem += 0;   magic += 0;   rel += 0;   break; }
            case race::Beetle:           { coin += 0;   gem += 0;   magic += 0;   rel += 0;   break; }
            case race::Bat:              { coin += 0;   gem += 0;   magic += 0;   rel += 0;   break; }
            case race::ThreeHeadedHound: { coin += 0;   gem += 0;   magic += 0;   rel += 0;   break; }
            case race::Count:
            default:                     { break; }
        }

        //These numbers intentionally max at 50, 100, 100, 100 -see comment above
        //These numbers (and those above) intentionally combine to a max of 400, 400, 400, 400.
        //See TreasureScoreMax() below.
        switch (ROLE_ENUM)
        {
            case role::Archer:        { coin += 0;  gem += 0;   magic += 0;   rel += 0;   break; }
            case role::Bard:          { coin += 20; gem += 20;  magic += 30;  rel += 10;  break; }
            case role::Beastmaster:   { coin += 0;  gem += 10;  magic += 20;  rel += 20;  break; }
            case role::Cleric:        { coin += 0;  gem += 60;  magic += 50;  rel += 50;  break; }
            case role::Knight:        { coin += 0;  gem += 0;   magic += 0;   rel += 0;   break; }
            case role::Sorcerer:      { coin += 0;  gem += 70;  magic += 50;  rel += 20;  break; }
            case role::Thief:         { coin += 0;  gem += 5;   magic += 20;  rel += 0;   break; }
            case role::Firebrand:     { coin += 0;  gem += 0;   magic += 0;   rel += 0;   break; }
            case role::Sylavin:       { coin += 0;  gem += 0;   magic += 0;   rel += 0;   break; }
            case role::Wolfen:        { coin += 0;  gem += 0;   magic += 0;   rel += 0;   break; }
            case role::Thug:          { coin += 15; gem += 2;   magic += 15;  rel += 0;   break; }
            case role::Mugger:        { coin += 25; gem += 3;   magic += 25;  rel += 0;   break; }
            case role::Drunk:         { coin += 0;  gem += 0;   magic += 0;   rel += 0;   break; }
            case role::Grunt:         { coin += 0;  gem += 0;   magic += 0;   rel += 0;   break; }
            case role::Brute:         { coin += 0;  gem += 0;   magic += 0;   rel += 0;   break; }
            case role::Berserker:     { coin += 0;  gem += 0;   magic += 0;   rel += 0;   break; }
            case role::Mountain:      { coin += 0;  gem += 0;   magic += 0;   rel += 0;   break; }
            case role::Captain:       { coin += 50; gem += 10;  magic += 20;  rel += 5;   break; }
            case role::Chieftain:     { coin += 50; gem += 20;  magic += 50;  rel += 20;  break; }
            case role::Trader:        { coin += 30; gem += 10;  magic += 20;  rel += 15;  break; }
            case role::Warlord:       { coin += 50; gem += 30;  magic += 30;  rel += 10;  break; }
            case role::Shaman:        { coin += 10; gem += 100; magic += 50;  rel += 100; break; }
            case role::Smasher:       { coin += 0;  gem += 0;   magic += 0;   rel += 0;   break; }
            case role::Strangler:     { coin += 0;  gem += 0;   magic += 0;   rel += 0;   break; }
            case role::Soldier:       { coin += 0;  gem += 0;   magic += 0;   rel += 0;   break; }
            case role::TwoHeaded:     { coin += 0;  gem += 0;   magic += 0;   rel += 0;   break; }
            case role::Giant:         { coin += 0;  gem += 0;   magic += 0;   rel += 0;   break; }
            case role::Elder:         { coin += 10; gem += 75;  magic += 100; rel += 50;  break; }
            case role::FourArmed:     { coin += 0;  gem += 0;   magic += 0;   rel += 0;   break; }
            case role::Tendrilus:     { coin += 0;  gem += 0;   magic += 0;   rel += 0;   break; }
            case role::Wing:          { coin += 0;  gem += 0;   magic += 0;   rel += 0;   break; }
            case role::Whelp:         { coin += 0;  gem += 0;   magic += 0;   rel += 0;   break; }
            case role::Pod:           { coin += 0;  gem += 0;   magic += 0;   rel += 0;   break; }
            case role::Spike:         { coin += 0;  gem += 0;   magic += 0;   rel += 0;   break; }
            case role::Skeleton:      { coin += 0;  gem += 0;   magic += 0;   rel += 0;   break; }
            case role::Ranger:        { coin += 0;  gem += 10;  magic += 0;   rel += 0;   break; }
            case role::Water:         { coin += 0;  gem += 0;   magic += 0;   rel += 0;   break; }
            case role::Blacksmith:    { coin += 0;  gem += 0;   magic += 10;  rel += 0;   break; }
            case role::Spider:        { coin += 0;  gem += 0;   magic += 0;   rel += 0;   break; }
            case role::Beetle:        { coin += 0;  gem += 0;   magic += 0;   rel += 0;   break; }
            case role::Boar:          { coin += 0;  gem += 0;   magic += 0;   rel += 0;   break; }
            case role::Lion:          { coin += 0;  gem += 0;   magic += 0;   rel += 0;   break; }
            case role::Ramonaut:      { coin += 0;  gem += 0;   magic += 0;   rel += 0;   break; }
            case role::Serpent:       { coin += 0;  gem += 0;   magic += 0;   rel += 0;   break; }
            case role::Bat:           { coin += 0;  gem += 0;   magic += 0;   rel += 0;   break; }
            case role::Ghost:         { coin += 0;  gem += 50;  magic += 0;   rel += 0;   break; }
            case role::Cat:           { coin += 0;  gem += 0;   magic += 0;   rel += 0;   break; }
            case role::Wolf:          { coin += 0;  gem += 0;   magic += 0;   rel += 0;   break; }
            case role::Count:         { coin += 0;  gem += 0;   magic += 0;   rel += 0;   break; }
            default: { break; }
        }

        return item::TreasureInfo(coin, gem, magic, rel);
    }


    const item::TreasureInfo race::TreasureScoreMax()
    {
        //see TreasureScore() function above for where these numbers come from
        return item::TreasureInfo(400, 400, 400, 400);
    }


    const std::string race::RaceRoleName(const race::Enum RACE_ENUM, const role::Enum ROLE_ENUM)
    {
        if (RACE_ENUM == race::ThreeHeadedHound)
        {
            return race::Name(RACE_ENUM);
        }

        auto const IS_ROLE_FIRST{ ((ROLE_ENUM == role::Firebrand) ||
                                   (ROLE_ENUM == role::FourArmed) ||
                                   (ROLE_ENUM == role::Giant) ||
                                   (ROLE_ENUM == role::Mountain) ||
                                   (ROLE_ENUM == role::Spike) ||
                                   (ROLE_ENUM == role::Sylavin) ||
                                   (ROLE_ENUM == role::Tendrilus) ||
                                   (ROLE_ENUM == role::TwoHeaded) ||
                                   (ROLE_ENUM == role::Water)) };

        std::ostringstream ss;

        if (IS_ROLE_FIRST)
        {
            ss << role::Name(ROLE_ENUM);
        }
        else
        {
            ss << race::Name(RACE_ENUM);
        }

        if (race::RaceRoleMatch(RACE_ENUM, ROLE_ENUM) == false)
        {
            switch (RACE_ENUM)
            {
                case Human:
                case Gnome:
                case Pixie:
                case Goblin:
                case Troll:
                case Orc:
                case Newt:
                case Bog:
                case LizardWalker:
                case Minotaur:
                case Ogre:
                case Halfling:
                case Pug:
                case Giant:
                case Ghoul:
                case Plant:
                case Wolfen:
                case Serpent:
                case Werebear:
                case Witch:
                case Skeleton:
                case Demon:
                case Griffin:
                case Wyvern:
                case Dragon:
                {
                    if (IS_ROLE_FIRST)
                    {
                        ss << " " << race::Name(RACE_ENUM);
                    }
                    else
                    {
                        ss << " " << role::Name(ROLE_ENUM);
                    }

                    break;
                }

                case ThreeHeadedHound:
                case Naga:
                case Spider:
                case CaveCrawler:
                case Hydra:
                case Lion:
                case LionBoar:
                case Ramonaut:
                case Cobra:
                case Wereboar:
                case Werecat:
                case Werewolf:
                case Beetle:
                case Boar:
                case Golem:
                case Shade:
                case Werebat:
                case Bat:
                case Harpy:
                case Count:
                default:
                {
                    break;
                }
            }
        }

        return ss.str();
    }


    const RankRange_t race::RaceRoleRanks(
        const race::Enum RACE_ENUM, const role::Enum ROLE_ENUM)
    {
        Rank_t min{ 1_rank };
        Rank_t max{ 1_rank };

        switch (RACE_ENUM)
        {
            case race::Human:            { min +=  0_rank; max += 99_rank; break; }
            case race::Orc:              { min +=  4_rank; max += 19_rank; break; }
            case race::Minotaur:         { min +=  9_rank; max += 99_rank; break; }
            case race::Ogre:             { min +=  5_rank; max += 99_rank; break; }
            case race::Halfling:         { min +=  0_rank; max += 19_rank; break; }
            case race::Pug:              { min +=  0_rank; max +=  9_rank; break; }
            case race::Giant:            { min += 11_rank; max += 99_rank; break; }
            case race::Wolfen:           { min += 14_rank; max += 99_rank; break; }
            case race::Lion:             { min +=  0_rank; max +=  9_rank; break; }
            case race::LionBoar:         { min +=  4_rank; max += 19_rank; break; }
            case race::Ramonaut:         { min +=  7_rank; max += 19_rank; break; }
            case race::Boar:             { min +=  0_rank; max +=  7_rank; break; }
            case race::Demon:            { min += 19_rank; max += 99_rank; break; }
            case race::Gnome:            { min +=  0_rank; max += 19_rank; break; }
            case race::Newt:             { min +=  0_rank; max += 19_rank; break; }
            case race::Bog:              { min +=  0_rank; max += 19_rank; break; }
            case race::Plant:            { min +=  2_rank; max += 19_rank; break; }
            case race::Pixie:            { min +=  1_rank; max += 49_rank; break; }
            case race::Goblin:           { min +=  0_rank; max += 19_rank; break; }
            case race::Troll:            { min +=  4_rank; max += 49_rank; break; }
            case race::Hydra:            { min += 19_rank; max += 99_rank; break; }
            case race::LizardWalker:     { min +=  2_rank; max += 49_rank; break; }
            case race::Naga:             { min +=  9_rank; max += 59_rank; break; }
            case race::Harpy:            { min +=  0_rank; max += 49_rank; break; }
            case race::Dragon:           { min += 49_rank; max += 99_rank; break; }
            case race::Griffin:          { min +=  9_rank; max += 39_rank; break; }
            case race::Serpent:          { min +=  9_rank; max += 49_rank; break; }
            case race::Cobra:            { min +=  0_rank; max +=  9_rank; break; }
            case race::Wyvern:           { min +=  9_rank; max += 49_rank; break; }
            case race::Shade:            { min += 11_rank; max += 99_rank; break; }
            case race::Skeleton:         { min +=  0_rank; max += 29_rank; break; }
            case race::Witch:            { min +=  9_rank; max += 49_rank; break; }
            case race::Golem:            { min += 29_rank; max += 99_rank; break; }
            case race::Ghoul:            { min +=  0_rank; max += 19_rank; break; }
            case race::Spider:           { min +=  0_rank; max +=  9_rank; break; }
            case race::CaveCrawler:      { min +=  4_rank; max += 24_rank; break; }
            case race::Werebear:         { min +=  9_rank; max += 49_rank; break; }
            case race::Wereboar:         { min +=  7_rank; max += 19_rank; break; }
            case race::Werecat:          { min +=  0_rank; max +=  9_rank; break; }
            case race::Werewolf:         { min +=  9_rank; max += 49_rank; break; }
            case race::Werebat:          { min +=  3_rank; max += 19_rank; break; }
            case race::Beetle:           { min +=  0_rank; max += 12_rank; break; }
            case race::Bat:              { min +=  0_rank; max +=  9_rank; break; }
            case race::ThreeHeadedHound: { min += 14_rank; max += 39_rank; break; }
            case race::Count:
            default:                     { break; }
        }

        switch (ROLE_ENUM)
        {
            case role::Archer:        { min +=  1_rank;  max +=  0_rank; break; }
            case role::Bard:          { min +=  0_rank;  max +=  0_rank; break; }
            case role::Beastmaster:   { min +=  0_rank;  max +=  0_rank; break; }
            case role::Cleric:        { min +=  2_rank;  max += 20_rank; break; }
            case role::Knight:        { min +=  2_rank;  max +=  5_rank; break; }
            case role::Sorcerer:      { min +=  2_rank;  max += 20_rank; break; }
            case role::Thief:         { min +=  0_rank;  max +=  0_rank; break; }
            case role::Firebrand:     { min +=  0_rank;  max +=  0_rank; break; }
            case role::Sylavin:       { min +=  0_rank;  max +=  0_rank; break; }
            case role::Wolfen:        { min +=  0_rank;  max +=  0_rank; break; }
            case role::Thug:          { min +=  0_rank;  max +=  3_rank; break; }
            case role::Mugger:        { min +=  1_rank;  max +=  6_rank; break; }
            case role::Drunk:         { min +=  0_rank;  max +=  0_rank; break; }
            case role::Grunt:         { min +=  1_rank;  max += 10_rank; break; }
            case role::Brute:         { min +=  3_rank;  max +=  0_rank; break; }
            case role::Berserker:     { min +=  4_rank;  max +=  0_rank; break; }
            case role::Mountain:      { min +=  4_rank;  max += 10_rank; break; }
            case role::Captain:       { min +=  3_rank;  max += 10_rank; break; }
            case role::Chieftain:     { min +=  6_rank;  max += 15_rank; break; }
            case role::Trader:        { min +=  0_rank;  max +=  0_rank; break; }
            case role::Warlord:       { min +=  9_rank;  max += 20_rank; break; }
            case role::Shaman:        { min +=  8_rank;  max += 10_rank; break; }
            case role::Smasher:       { min +=  2_rank;  max +=  2_rank; break; }
            case role::Strangler:     { min +=  1_rank;  max +=  0_rank; break; }
            case role::Soldier:       { min +=  2_rank;  max -=  5_rank; break; }
            case role::TwoHeaded:     { min +=  5_rank;  max +=  0_rank; break; }
            case role::Giant:         { min +=  1_rank;  max +=  0_rank; break; }
            case role::Elder:         { min += 11_rank;  max += 20_rank; break; }
            case role::FourArmed:     { min +=  4_rank;  max +=  0_rank; break; }
            case role::Tendrilus:     { min +=  0_rank;  max +=  0_rank; break; }
            case role::Wing:          { min -=  1_rank;  max -=  5_rank; break; }
            case role::Whelp:         { min -=  1_rank;  max -=  5_rank; break; }
            case role::Pod:           { min +=  0_rank;  max +=  0_rank; break; }
            case role::Spike:         { min +=  4_rank;  max +=  0_rank; break; }
            case role::Skeleton:      { min +=  0_rank;  max +=  0_rank; break; }
            case role::Ranger:        { min +=  4_rank;  max +=  0_rank; break; }
            case role::Water:         { min +=  0_rank;  max +=  0_rank; break; }
            case role::Blacksmith:    { min +=  0_rank;  max +=  0_rank; break; }
            case role::Spider:        { min +=  0_rank;  max +=  0_rank; break; }
            case role::Beetle:        { min +=  0_rank;  max +=  0_rank; break; }
            case role::Boar:          { min +=  0_rank;  max +=  0_rank; break; }
            case role::Lion:          { min +=  0_rank;  max +=  0_rank; break; }
            case role::Ramonaut:      { min +=  0_rank;  max +=  0_rank; break; }
            case role::Serpent:       { min +=  0_rank;  max +=  0_rank; break; }
            case role::Bat:           { min +=  0_rank;  max +=  0_rank; break; }
            case role::Ghost:         { min +=  5_rank;  max +=  0_rank; break; }
            case role::Cat:           { min +=  0_rank;  max +=  0_rank; break; }
            case role::Wolf:          { min +=  1_rank;  max +=  0_rank; break; }
            case role::Count:         { min +=  0_rank;  max +=  0_rank; break; }
            default: { break; }
        }

        if (min < 1_rank)
        {
            min = 1_rank;
        }

        if (min > 199_rank)
        {
            min = 199_rank;
        }

        if (max > 200_rank)
        {
            max = 200_rank;
        }

        if (max <= min)
        {
            max = min + 1_rank;
        }

        return RankRange_t(min, max);
    }


    const misc::StrVec_t race::CorpseImageKeys(const race::Enum E)
    {
        switch (E)
        {
            case race::Shade:
            case race::Ghoul:
            case race::Human:
            case race::Gnome:
            case race::Pixie:
            case race::Witch:
            case race::Halfling:
            case race::Skeleton:
            {
                return { "media-images-bones-skull-humaniod",
                         "media-images-bones-skull-humaniod-pile-1",
                         "media-images-bones-skull-humaniod-pile-2",
                         "media-images-bones-skull-humaniod-pile-3" };
            }

            case race::Wyvern:
            case race::Dragon:
            case race::Hydra:
            {
                return { "media-images-bones-skull-dragon-1",
                         "media-images-bones-skull-dragon-2",
                         "media-images-bones-skull-dragon-3",
                         "media-images-bones-skull-dragon-4" };
            }

            case race::Orc:
            case race::Goblin:
            {
                return { "media-images-bones-skull-goblin" };
            }

            case race::Pug:
            case race::Newt:
            {
                return { "media-images-bones-skull-animal-2" };
            }

            case race::Naga:
            case race::LizardWalker:
            {
                //the orc skull looked better as a naga/lizard skull
                return { "media-images-bones-skull-orc" };
            }

            case race::Bog:
            {
                return { "media-images-bones-skull-bog" };
            }

            case race::Spider:
            case race::CaveCrawler:
            {
                return { "media-images-bones-cave-crawler" };
            }

            case race::Minotaur:
            {
                return { "media-images-bones-skull-minotaur" };
            }

            case race::Plant:
            {
                return { "media-images-bones-skull-animal-1" };
            }

            case race::Beetle:
            {
                return { "media-images-bones-beetle" };
            }

            case race::Demon:
            {
                return { "media-images-bones-skull-demon" };
            }

            case race::Griffin:
            {
                return { "media-images-bones-griffin" };
            }

            case race::Boar:
            case race::LionBoar:
            case race::Ramonaut:
            case race::Wereboar:
            {
                return { "media-images-bones-skull-animal-3" };
            }

            case race::Wolfen:
            case race::Lion:
            case race::Werebear:
            case race::Werewolf:
            {
                return { "media-images-bones-wolfen" };
            }

            case race::Serpent:
            case race::Cobra:
            {
                return { "media-images-bones-skull-snake" };
            }

            case race::Werecat:
            {
                return { "media-images-bones-cat" };
            }

            case race::Ogre:
            case race::Golem:
            case race::Troll:
            case race::Giant:
            {
                return { "media-images-bones-skull-giant" };
            }

            case race::Bat:
            case race::Werebat:
            {
                return { "media-images-bones-bat" };
            }

            case race::Harpy:
            {
                return { "media-images-bones-harpy" };
            }

            case race::ThreeHeadedHound:
            {
                return { "media-images-bones-three-headed-hound" };
            }

            case race::Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::creature::race::CorpseImageKeys(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

}
}
