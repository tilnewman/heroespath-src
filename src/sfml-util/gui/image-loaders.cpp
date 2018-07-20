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
#include "sfml-util/gui/condition-image-loader.hpp"
#include "sfml-util/gui/creature-image-loader.hpp"
#include "sfml-util/gui/item-image-loader.hpp"
#include "sfml-util/gui/list-element.hpp"
#include "sfml-util/gui/song-image-loader.hpp"
#include "sfml-util/gui/spell-image-loader.hpp"
#include "sfml-util/gui/text-region.hpp"
#include "sfml-util/gui/title-image-loader.hpp"
#include "sfml-util/texture-cache.hpp"
#include "song/song.hpp"
#include "spell/spell.hpp"

#include <boost/filesystem/path.hpp>

namespace heroespath
{

namespace sfml_util
{
    namespace gui
    {
        namespace image
        {

            CachedTexture Load(
                const creature::CreaturePtr_t & CREATURE_PTR,
                const sfml_util::ImageOptions & OPTIONS)
            {
                CreatureImageLoader creatureImageLoader;

                auto imageOptions { OPTIONS };

                if (creatureImageLoader.WillHorizFlipToFaceRight(CREATURE_PTR))
                {
                    imageOptions.option_enum |= ImageOpt::FlipHoriz;
                }

                return CachedTexture(
                    boost::filesystem::path(creatureImageLoader.Path(CREATURE_PTR)), imageOptions);
            }

            CachedTexture Load(
                const std::string & FAKE_PATH,
                const avatar::Avatar::Enum WHICH_AVATAR,
                const sfml_util::ImageOptions & OPTIONS)
            {
                return avatar::PortraitFactory::Make(
                    "FAKE_PATH_FOR_sfml_util::gui::image::Load(Avatar::Enum="
                        + avatar::Avatar::ToString(WHICH_AVATAR) + ", fake_path=" + FAKE_PATH + ")",
                    WHICH_AVATAR,
                    OPTIONS);
            }

            CachedTexture Load(
                const game::GameStatePtr_t & GAMESTATE_PTR, const sfml_util::ImageOptions & OPTIONS)
            {
                return Load(
                    "FAKE_PATH_FOR_sfml_util::gui::image::Load(game::GameStatePtr_t)",
                    GAMESTATE_PTR->Party().Avatar(),
                    OPTIONS);
            }

            CachedTexture Load(
                const creature::Conditions::Enum WHICH_CONDITION,
                const sfml_util::ImageOptions & OPTIONS)
            {
                ConditionImageLoader conditionImageLoader;

                return CachedTexture(
                    boost::filesystem::path(conditionImageLoader.Path(WHICH_CONDITION)), OPTIONS);
            }

            CachedTexture Load(
                const creature::ConditionPtr_t & CONDITION_PTR,
                const sfml_util::ImageOptions & OPTIONS)
            {
                return Load(CONDITION_PTR->Which(), OPTIONS);
            }

            CachedTexture
                Load(const item::ItemPtr_t & ITEM_PTR, const sfml_util::ImageOptions & OPTIONS)
            {
                ItemImageLoader itemImageLoader;

                return CachedTexture(
                    boost::filesystem::path(itemImageLoader.Path(ITEM_PTR)), OPTIONS);
            }

            CachedTexture Load(
                const creature::Titles::Enum WHICH_TITLE, const sfml_util::ImageOptions & OPTIONS)
            {
                TitleImageLoader titleImageLoader;

                return CachedTexture(
                    boost::filesystem::path(titleImageLoader.Path(WHICH_TITLE)), OPTIONS);
            }

            CachedTexture Load(
                const creature::TitlePtr_t & TITLE_PTR, const sfml_util::ImageOptions & OPTIONS)
            {
                return Load(TITLE_PTR->Which(), OPTIONS);
            }

            CachedTexture
                Load(const spell::Spells::Enum WHICH_SPELL, const sfml_util::ImageOptions & OPTIONS)
            {
                SpellImageLoader spellImageLoader;

                return CachedTexture(
                    boost::filesystem::path(spellImageLoader.Path(WHICH_SPELL)), OPTIONS);
            }

            CachedTexture
                Load(const spell::SpellPtr_t & SPELL_PTR, const sfml_util::ImageOptions & OPTIONS)
            {
                return Load(SPELL_PTR->Which(), OPTIONS);
            }

            CachedTexture
                Load(const song::Songs::Enum WHICH_SONG, const sfml_util::ImageOptions & OPTIONS)
            {
                SongImageLoader songImageLoader;

                return CachedTexture(
                    boost::filesystem::path(songImageLoader.Path(WHICH_SONG)), OPTIONS);
            }

            CachedTexture
                Load(const song::SongPtr_t & SONG_PTR, const sfml_util::ImageOptions & OPTIONS)
            {
                return Load(SONG_PTR->Which(), OPTIONS);
            }

        } // namespace image

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
