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
#include <exception>
#include <sstream>


namespace game
{
namespace creature
{

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

}
}
