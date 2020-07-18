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
#include "gui/sfx-set-enum.hpp"
#include "gui/sound-effects-enum.hpp"
#include "item/material-enum.hpp"
#include "misc/not-null.hpp"
#include "song/song-enum.hpp"
#include "spell/spell-enum.hpp"

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

    class WeaponTemplate;

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
    struct CombatSoundEffects
    {
        struct Playlist
        {
            Playlist() = default;

            explicit Playlist(
                const gui::sound_effect::Enum SFX,
                const gui::sound_effect::Enum SFX_AFTER = gui::sound_effect::Count,
                const float SFX_AFTER_PRE_DELAY = 0.0f)
                : sfx_vec(1, SFX)
                , sfx_after(SFX_AFTER)
                , sfx_after_delay_sec(SFX_AFTER_PRE_DELAY)
            {}

            Playlist(const gui::SfxEnumVec_t & SFX_VEC)
                : sfx_vec(SFX_VEC)
            {}

            explicit Playlist(const gui::sound_effect_set::Enum SFX_SET)
                : sfx_set(SFX_SET)
            {}

            gui::SfxEnumVec_t sfx_vec {};
            gui::sound_effect_set::Enum sfx_set = gui::sound_effect_set::Count;
            gui::sound_effect::Enum sfx_after = gui::sound_effect::Count;
            float sfx_after_delay_sec = 0.0f;
        };

        CombatSoundEffects() = delete;

        static void PlayShoot(const item::ItemPtr_t);
        static void PlayHitOrMiss(const creature::CreaturePtr_t, const HitInfo &);
        static void PlaySpell(const spell::SpellPtr_t);
        static void PlaySong(const song::SongPtr_t);
        static void PlayRoar(const creature::CreaturePtr_t);
        static void PlayDeath(const creature::CreaturePtr_t);

    private:
        static bool Play(const Playlist &);

        static const Playlist RoarPlaylist(const creature::CreaturePtr_t);
        static const Playlist SpellPlaylist(const spell::Spells::Enum);
        static const Playlist SongPlaylist(const song::Songs::Enum);
        static const Playlist DeathPlaylist(const creature::CreaturePtr_t);

        static const Playlist HitPlaylist(const item::WeaponTemplate &, const item::Material::Enum);

        static const Playlist HitOfMaterialPlaylist(const item::Material::Enum);
        static const Playlist MissPlaylist(const item::WeaponTemplate &);
    };

} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_GAME_COMBAT_COMBATSOUNDEFFECTS_HPP_INCLUDED
