#ifndef GAME_COMBAT_COMBATSOUNDEFFECTS_HPP_INCLUDED
#define GAME_COMBAT_COMBATSOUNDEFFECTS_HPP_INCLUDED
//
// combat-sound-effects.hpp
//
#include "game/combat/fight-results.hpp"

#include <memory>


namespace game
{
namespace item
{
    class Item;
    using ItemSPtr_t = std::shared_ptr<Item>;
}
namespace combat
{

    //Stores combat sound effects in memory and provides a simple interface to play them.
    class CombatSoundEffects
    {
        //prevent copy assignment
        CombatSoundEffects & operator=(const CombatSoundEffects &) =delete;

        //prevent copy construction
        CombatSoundEffects(const CombatSoundEffects &) =delete;

    public:
        CombatSoundEffects();

        void PlayShoot(const item::ItemSPtr_t &);
        void PlayHitOrMiss(const HitInfo &);
    };

}
}

#endif //GAME_COMBAT_COMBATSOUNDEFFECTS_HPP_INCLUDED
