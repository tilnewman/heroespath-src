#ifndef APPBASE_HEROESPATH_CREATURE_RACEENUM_INCLUDED
#define APPBASE_HEROESPATH_CREATURE_RACEENUM_INCLUDED
//
// race-enum.hpp
//  An enumeration of all Races.
//
#include <string>


namespace heroespath
{
namespace creature
{

    struct race
    {
        enum Enum
        {
            Human = 0,
            Gnome,
            Pixie,
            Wolfen,
            Dragon,
            Count_PlayerRaces,
            Goblin = Count_PlayerRaces,
            Troll,
            Orc,
            Newt,
            Spider,
            Bog,
            CaveCrawler,
            Hydra,
            LizardWalker,
            Minotaur,
            Ogre,
            Plant,
            Shade,
            Skeleton,
            Beetle,
            Boar,
            Demon,
            Griffin,
            Halfling,
            Lion,
            LionBoar,
            Naga,
            Ramonaut,
            Serpent,
            Cobra,
            Werebear,
            Wereboar,
            Werecat,
            Werewolf,
            Werebat,
            Pug,
            Wyvern,
            Giant,
            Ghoul,
            Count
        };

        static const std::string ToString(const race::Enum);
        inline static const std::string Name(const race::Enum E) { return ToString(E); }
        static const std::string Desc(const race::Enum);
        static const std::string Abbr(const race::Enum);
        static bool HasTorso(const race::Enum);
        static bool CanFly(const race::Enum);
        static bool WillInitiallyFly(const race::Enum);
    };

}
}
#endif //APPBASE_HEROESPATH_CREATURE_RACEENUM_INCLUDED
