// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_IMAGE_LOADERS_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_IMAGE_LOADERS_HPP_INCLUDED
//
// image-loaders.hpp
//
#include "avatar/avatar-enum.hpp"
#include "creature/condition-enum.hpp"
#include "creature/title-enum.hpp"
#include "sfml-util/cached-texture.hpp"
#include "sfml-util/combat-image-enum.hpp"
#include "sfml-util/image-options.hpp"
#include "sfml-util/image-util.hpp"
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

namespace sfml_util
{

    CachedTexture LoadAndCacheImage(
        const creature::CreaturePtr_t & CREATURE_PTR,
        const sfml_util::ImageOptions & OPTIONS = sfml_util::ImageOptions());

    CachedTexture LoadAndCacheImage(
        const std::string & FAKE_PATH,
        const avatar::Avatar::Enum WHICH_AVATAR,
        const sfml_util::ImageOptions & OPTIONS = sfml_util::ImageOptions());

    CachedTexture LoadAndCacheImage(
        const game::GameStatePtr_t & GAMESTATE_PTR,
        const sfml_util::ImageOptions & OPTIONS = sfml_util::ImageOptions());

    CachedTexture LoadAndCacheImage(
        const creature::Conditions::Enum WHICH_CONDITION,
        const sfml_util::ImageOptions & OPTIONS = sfml_util::ImageOptions());

    CachedTexture LoadAndCacheImage(
        const creature::ConditionPtr_t & CONDITION_PTR,
        const sfml_util::ImageOptions & OPTIONS = sfml_util::ImageOptions());

    CachedTexture LoadAndCacheImage(
        const item::ItemPtr_t & ITEM_PTR,
        const sfml_util::ImageOptions & OPTIONS = sfml_util::ImageOptions());

    CachedTexture LoadAndCacheImage(
        const creature::Titles::Enum WHICH_TITLE,
        const sfml_util::ImageOptions & OPTIONS = sfml_util::ImageOptions());

    CachedTexture LoadAndCacheImage(
        const creature::TitlePtr_t & TITLE_PTR,
        const sfml_util::ImageOptions & OPTIONS = sfml_util::ImageOptions());

    CachedTexture LoadAndCacheImage(
        const spell::Spells::Enum WHICH_SPELL,
        const sfml_util::ImageOptions & OPTIONS = sfml_util::ImageOptions());

    CachedTexture LoadAndCacheImage(
        const spell::SpellPtr_t & SPELL_PTR,
        const sfml_util::ImageOptions & OPTIONS = sfml_util::ImageOptions());

    CachedTexture LoadAndCacheImage(
        const song::Songs::Enum WHICH_SONG,
        const sfml_util::ImageOptions & OPTIONS = sfml_util::ImageOptions());

    CachedTexture LoadAndCacheImage(
        const song::SongPtr_t & SONG_PTR,
        const sfml_util::ImageOptions & OPTIONS = sfml_util::ImageOptions());

    CachedTexture LoadAndCacheImage(
        const sfml_util::CombatImageType::Enum WHICH_COMBAT_IMAGE,
        const sfml_util::ImageOptions & OPTIONS = sfml_util::ImageOptions());

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_IMAGE_LOADERS_HPP_INCLUDED
