#ifndef HEROESPATH_COMBAT_COMBATSOUNDEFFECTS_HPP_INCLUDED
#define HEROESPATH_COMBAT_COMBATSOUNDEFFECTS_HPP_INCLUDED
//
// combat-sound-effects.hpp
//
#include "heroespath/combat/fight-results.hpp"

#include <memory>


namespace sfml_util
{
    class StaticSounds;
    using StaticSoundsSPtr_t = std::shared_ptr<StaticSounds>;
}
namespace heroespath
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
        virtual ~CombatSoundEffects();

        void PlayShoot(const item::ItemSPtr_t &);
        void PlayHitOrMiss(const HitInfo &);

    private:
        sfml_util::StaticSoundsSPtr_t blowpipeShootSoundsSPtr_;
        sfml_util::StaticSoundsSPtr_t blowpipeHitSoundsSPtr_;
        sfml_util::StaticSoundsSPtr_t arrowShootSoundsSPtr_;
        sfml_util::StaticSoundsSPtr_t arrowHitSoundsSPtr_;
        sfml_util::StaticSoundsSPtr_t projectileMissSoundsSPtr_;
        sfml_util::StaticSoundsSPtr_t meleeMissSoundsSPtr_;
        sfml_util::StaticSoundsSPtr_t fistHitSoundsSPtr_;
        sfml_util::StaticSoundsSPtr_t breathHitSoundsSPtr_;
        sfml_util::StaticSoundsSPtr_t tendrilHitSoundsSPtr_;
        sfml_util::StaticSoundsSPtr_t whipHitSoundsSPtr_;
        sfml_util::StaticSoundsSPtr_t whipMissSoundsSPtr_;
        sfml_util::StaticSoundsSPtr_t materialHitMetalSoundsSPtr_;
        sfml_util::StaticSoundsSPtr_t materialHitMiscSoundsSPtr_;
        sfml_util::StaticSoundsSPtr_t clawTearSoundsSPtr_;
    };

}
}

#endif //HEROESPATH_COMBAT_COMBATSOUNDEFFECTS_HPP_INCLUDED
