// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// image-loaders.cpp
//
#include "image-loaders.hpp"

#include "avatar/portrait-factory.hpp"
#include "creature/condition.hpp"
#include "creature/creature.hpp"
#include "creature/title.hpp"
#include "game/game-state.hpp"
#include "item/item.hpp"
#include "sfml-util/combat-image-loader.hpp"
#include "sfml-util/condition-image-loader.hpp"
#include "sfml-util/creature-image-loader.hpp"
#include "sfml-util/item-image-loader.hpp"
#include "sfml-util/list-element.hpp"
#include "sfml-util/song-image-loader.hpp"
#include "sfml-util/spell-image-loader.hpp"
#include "sfml-util/text-region.hpp"
#include "sfml-util/title-image-loader.hpp"
#include "song/song.hpp"
#include "spell/spell.hpp"

namespace heroespath
{
namespace gui
{

    CachedTexture LoadAndCacheImage(
        const creature::CreaturePtr_t & CREATURE_PTR, const gui::ImageOptions & OPTIONS)
    {
        gui::CreatureImageLoader creatureImageLoader;

        auto imageOptions { OPTIONS };

        if (creatureImageLoader.WillHorizFlipToFaceRight(CREATURE_PTR))
        {
            imageOptions.option_enum |= ImageOpt::FlipHoriz;
        }

        return CachedTexture(PathWrapper(creatureImageLoader.Path(CREATURE_PTR)), imageOptions);
    }

    CachedTexture LoadAndCacheImage(
        const std::string & FAKE_PATH,
        const avatar::Avatar::Enum WHICH_AVATAR,
        const gui::ImageOptions & OPTIONS)
    {
        return avatar::PortraitFactory::Make(
            "FAKE_PATH_FOR_gui::image::Load(Avatar::Enum=" + avatar::Avatar::ToString(WHICH_AVATAR)
                + ", fake_path=" + FAKE_PATH + ")",
            WHICH_AVATAR,
            OPTIONS);
    }

    CachedTexture LoadAndCacheImage(
        const game::GameStatePtr_t & GAMESTATE_PTR, const gui::ImageOptions & OPTIONS)
    {
        return LoadAndCacheImage(
            "FAKE_PATH_FOR_gui::image::Load(game::GameStatePtr_t)",
            GAMESTATE_PTR->Party().Avatar(),
            OPTIONS);
    }

    CachedTexture LoadAndCacheImage(
        const creature::Conditions::Enum WHICH_CONDITION, const gui::ImageOptions & OPTIONS)
    {
        gui::ConditionImageLoader conditionImageLoader;

        return CachedTexture(PathWrapper(conditionImageLoader.Path(WHICH_CONDITION)), OPTIONS);
    }

    CachedTexture LoadAndCacheImage(
        const creature::ConditionPtr_t & CONDITION_PTR, const gui::ImageOptions & OPTIONS)
    {
        return LoadAndCacheImage(CONDITION_PTR->Which(), OPTIONS);
    }

    CachedTexture
        LoadAndCacheImage(const item::ItemPtr_t & ITEM_PTR, const gui::ImageOptions & OPTIONS)
    {
        gui::ItemImageLoader itemImageLoader;

        return CachedTexture(PathWrapper(itemImageLoader.Path(ITEM_PTR)), OPTIONS);
    }

    CachedTexture LoadAndCacheImage(
        const creature::Titles::Enum WHICH_TITLE, const gui::ImageOptions & OPTIONS)
    {
        gui::TitleImageLoader titleImageLoader;

        return CachedTexture(PathWrapper(titleImageLoader.Path(WHICH_TITLE)), OPTIONS);
    }

    CachedTexture
        LoadAndCacheImage(const creature::TitlePtr_t & TITLE_PTR, const gui::ImageOptions & OPTIONS)
    {
        return LoadAndCacheImage(TITLE_PTR->Which(), OPTIONS);
    }

    CachedTexture
        LoadAndCacheImage(const spell::Spells::Enum WHICH_SPELL, const gui::ImageOptions & OPTIONS)
    {
        gui::SpellImageLoader spellImageLoader;

        return CachedTexture(PathWrapper(spellImageLoader.Path(WHICH_SPELL)), OPTIONS);
    }

    CachedTexture
        LoadAndCacheImage(const spell::SpellPtr_t & SPELL_PTR, const gui::ImageOptions & OPTIONS)
    {
        return LoadAndCacheImage(SPELL_PTR->Which(), OPTIONS);
    }

    CachedTexture
        LoadAndCacheImage(const song::Songs::Enum WHICH_SONG, const gui::ImageOptions & OPTIONS)
    {
        gui::SongImageLoader songImageLoader;

        return CachedTexture(PathWrapper(songImageLoader.Path(WHICH_SONG)), OPTIONS);
    }

    CachedTexture
        LoadAndCacheImage(const song::SongPtr_t & SONG_PTR, const gui::ImageOptions & OPTIONS)
    {
        return LoadAndCacheImage(SONG_PTR->Which(), OPTIONS);
    }

    CachedTexture LoadAndCacheImage(
        const gui::CombatImageType::Enum WHICH_COMBAT_IMAGE, const gui::ImageOptions & OPTIONS)
    {
        gui::CombatImageLoader combatImageLoader;

        return CachedTexture(PathWrapper(combatImageLoader.Path(WHICH_COMBAT_IMAGE)), OPTIONS);
    }

} // namespace gui
} // namespace heroespath
