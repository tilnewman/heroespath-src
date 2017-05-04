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
            case Count:        { return ""; }
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
            case Skeleton:  { return false; }
            case Demon:
            case Shade:
            case Griffin:
            case Werebat:
            case Wyvern:
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
                (E == race::Werebat));
    }

}
}
