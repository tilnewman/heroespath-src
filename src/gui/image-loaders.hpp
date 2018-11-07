// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_IMAGE_LOADERS_HPP_INCLUDED
#define HEROESPATH_GUI_IMAGE_LOADERS_HPP_INCLUDED
//
// image-loaders.hpp
//
#include "avatar/avatar-enum.hpp"
#include "creature/condition-enum.hpp"
#include "creature/title-enum.hpp"
#include "gui/cached-texture.hpp"
#include "gui/combat-image-enum.hpp"
#include "gui/image-options.hpp"
#include "gui/image-util.hpp"
#include "misc/not-null.hpp"
#include "song/song-enum.hpp"
#include "spell/spell-enum.hpp"

#include <memory>
#include <string>

namespace heroespath
{
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
namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;

    class Title;
    using TitlePtr_t = misc::NotNull<Title *>;

    class Condition;
    using ConditionPtr_t = misc::NotNull<Condition *>;
} // namespace creature
namespace item
{
    class Item;
    using ItemPtr_t = misc::NotNull<Item *>;
} // namespace item
namespace game
{
    class GameState;
    using GameStatePtr_t = misc::NotNull<GameState *>;
} // namespace game

namespace gui
{

    CachedTexture LoadAndCacheImage(
        const creature::CreaturePtr_t & CREATURE_PTR,
        const gui::ImageOptions & OPTIONS = gui::ImageOptions());

    CachedTexture LoadAndCacheImage(
        const std::string & FAKE_PATH,
        const avatar::Avatar::Enum WHICH_AVATAR,
        const gui::ImageOptions & OPTIONS = gui::ImageOptions());

    CachedTexture LoadAndCacheImage(
        const game::GameStatePtr_t & GAMESTATE_PTR,
        const gui::ImageOptions & OPTIONS = gui::ImageOptions());

    CachedTexture LoadAndCacheImage(
        const creature::Conditions::Enum WHICH_CONDITION,
        const gui::ImageOptions & OPTIONS = gui::ImageOptions());

    CachedTexture LoadAndCacheImage(
        const creature::ConditionPtr_t & CONDITION_PTR,
        const gui::ImageOptions & OPTIONS = gui::ImageOptions());

    CachedTexture LoadAndCacheImage(
        const item::ItemPtr_t & ITEM_PTR, const gui::ImageOptions & OPTIONS = gui::ImageOptions());

    CachedTexture LoadAndCacheImage(
        const creature::Titles::Enum WHICH_TITLE,
        const gui::ImageOptions & OPTIONS = gui::ImageOptions());

    CachedTexture LoadAndCacheImage(
        const creature::TitlePtr_t & TITLE_PTR,
        const gui::ImageOptions & OPTIONS = gui::ImageOptions());

    CachedTexture LoadAndCacheImage(
        const spell::Spells::Enum WHICH_SPELL,
        const gui::ImageOptions & OPTIONS = gui::ImageOptions());

    CachedTexture LoadAndCacheImage(
        const spell::SpellPtr_t & SPELL_PTR,
        const gui::ImageOptions & OPTIONS = gui::ImageOptions());

    CachedTexture LoadAndCacheImage(
        const song::Songs::Enum WHICH_SONG,
        const gui::ImageOptions & OPTIONS = gui::ImageOptions());

    CachedTexture LoadAndCacheImage(
        const song::SongPtr_t & SONG_PTR, const gui::ImageOptions & OPTIONS = gui::ImageOptions());

    CachedTexture LoadAndCacheImage(
        const gui::CombatImageType::Enum WHICH_COMBAT_IMAGE,
        const gui::ImageOptions & OPTIONS = gui::ImageOptions());

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_IMAGE_LOADERS_HPP_INCLUDED
