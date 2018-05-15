// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GAME_COMBAT_COMBATSOUNDEFFECTS_HPP_INCLUDED
#define HEROESPATH_GAME_COMBAT_COMBATSOUNDEFFECTS_HPP_INCLUDED
//
// combat-sound-effects.hpp
//
#include "combat/fight-results.hpp"
#include "misc/not-null.hpp"
#include "sfml-util/sound-effects-enum.hpp"

#include <memory>

namespace heroespath
{
namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
} // namespace creature
namespace item
{
    class Item;
    using ItemPtr_t = misc::NotNull<Item *>;
} // namespace item
namespace spell
{
    class Spell;
    using SpellPtr_t = misc::NotNull<Spell *>;
} // namespace spell
namespace song
{
    class Song;
    using SongPtr_t = misc::NotNull<Song *>;
} // namespace song

namespace combat
{

    // Stores combat sound effects in memory and provides a simple
    // interface to play them.
    class CombatSoundEffects
    {
    public:
        CombatSoundEffects & operator=(const CombatSoundEffects &) = delete;
        CombatSoundEffects & operator=(CombatSoundEffects &&) = delete;
        CombatSoundEffects(const CombatSoundEffects &) = delete;
        CombatSoundEffects(CombatSoundEffects &&) = delete;

        CombatSoundEffects() = default;

        void PlayShoot(const item::ItemPtr_t) const;
        void PlayHitOrMiss(const creature::CreaturePtr_t, const HitInfo &) const;
        void PlaySpell(const spell::SpellPtr_t) const;
        void PlaySong(const song::SongPtr_t) const;
        void PlayRoar(const creature::CreaturePtr_t) const;
        void PlayDeath(const creature::CreaturePtr_t) const;

    private:
        void GetDeathSfx(const creature::CreaturePtr_t, sfml_util::SfxEnumVec_t &) const;
    };

} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_GAME_COMBAT_COMBATSOUNDEFFECTS_HPP_INCLUDED
