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
