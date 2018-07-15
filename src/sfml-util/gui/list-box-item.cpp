// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// list-box-item.cpp
//
#include "list-box-item.hpp"

#include "avatar/portrait-factory.hpp"
#include "creature/condition.hpp"
#include "creature/creature.hpp"
#include "creature/title.hpp"
#include "game/game-state.hpp"
#include "item/item.hpp"
#include "misc/enum-util.hpp"
#include "sfml-util/gui/condition-image-loader.hpp"
#include "sfml-util/gui/creature-image-loader.hpp"
#include "sfml-util/gui/item-image-loader.hpp"
#include "sfml-util/gui/list-box-item.hpp"
#include "sfml-util/gui/song-image-loader.hpp"
#include "sfml-util/gui/spell-image-loader.hpp"
#include "sfml-util/gui/title-image-loader.hpp"
#include "sfml-util/texture-cache.hpp"
#include "song/song.hpp"
#include "spell/spell.hpp"

#include <boost/filesystem/path.hpp>

#include <tuple>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        ListBoxItem::ListBoxItem(
            const std::string & NAME,
            const sfml_util::gui::TextInfo & TEXT_INFO,
            const bool IS_VALID)
            : TextRegion(
                  std::string(NAME).append("_ListBoxItemTextOnly"), TEXT_INFO, sf::FloatRect())
            , creaturePtrOpt_(boost::none)
            , gameStatePtrOpt_(boost::none)
            , itemPtrOpt_(boost::none)
            , conditionPtrOpt_(boost::none)
            , titlePtrOpt_(boost::none)
            , spellPtrOpt_(boost::none)
            , songPtrOpt_(boost::none)
            , isValid_(IS_VALID)
            , cachedTextureOpt_(boost::none)
            , sprite_()
        {}

        ListBoxItem::ListBoxItem(
            const std::string & NAME,
            const sfml_util::gui::TextInfo & TEXT_INFO,
            const creature::CreaturePtr_t CHARACTER_PTR,
            const bool IS_VALID)
            : TextRegion(
                  std::string(NAME).append("_ListBoxItemPlayerCharacter"),
                  TEXT_INFO,
                  sf::FloatRect())
            , creaturePtrOpt_(CHARACTER_PTR)
            , gameStatePtrOpt_(boost::none)
            , itemPtrOpt_(boost::none)
            , conditionPtrOpt_(boost::none)
            , titlePtrOpt_(boost::none)
            , spellPtrOpt_(boost::none)
            , songPtrOpt_(boost::none)
            , isValid_(IS_VALID)
            , cachedTextureOpt_(boost::none)
            , sprite_()
        {
            sfml_util::gui::CreatureImageLoader creatureImageLoader;

            auto imageOptions { TextureOpt::Default };

            if (creatureImageLoader.WillHorizFlipToFaceRight(CHARACTER_PTR))
            {
                imageOptions |= TextureOpt::FlipHoriz;
            }

            cachedTextureOpt_ = CachedTextureOpt_t(CachedTexture(
                boost::filesystem::path(creatureImageLoader.Path(CHARACTER_PTR)), imageOptions));

            sprite_.setTexture(cachedTextureOpt_.value().Get(), true);
        }

        ListBoxItem::ListBoxItem(
            const std::string & NAME,
            const sfml_util::gui::TextInfo & TEXT_INFO,
            const game::GameStatePtr_t GAMESTATE_PTR,
            const bool IS_VALID)
            : TextRegion(
                  std::string(NAME).append("_ListBoxItemGameState"), TEXT_INFO, sf::FloatRect())
            , creaturePtrOpt_(boost::none)
            , gameStatePtrOpt_(GAMESTATE_PTR)
            , itemPtrOpt_(boost::none)
            , conditionPtrOpt_(boost::none)
            , titlePtrOpt_(boost::none)
            , spellPtrOpt_(boost::none)
            , songPtrOpt_(boost::none)
            , isValid_(IS_VALID)
            , cachedTextureOpt_(boost::none)
            , sprite_()
        {
            {
                sf::Texture texture;

                auto const AVATAR_ENUM { GAMESTATE_PTR->Party().Avatar() };
                avatar::PortraitFactory::Make(AVATAR_ENUM, texture);

                std::ostringstream ss;
                ss << "FAKE_PATH_FOR_LISTBOX_ITEM_GAME_STATE_PARTY_AVATAR_"
                   << avatar::Avatar::ToString(AVATAR_ENUM);

                cachedTextureOpt_ = CachedTextureOpt_t(CachedTexture(ss.str(), texture));
            }

            sprite_.setTexture(cachedTextureOpt_.value().Get(), true);
        }

        ListBoxItem::ListBoxItem(
            const std::string & NAME,
            const sfml_util::gui::TextInfo & TEXT_INFO,
            const creature::ConditionPtr_t CONDITION_PTR,
            const bool IS_VALID)
            : TextRegion(
                  std::string(NAME).append("_ListBoxItemCondition"), TEXT_INFO, sf::FloatRect())
            , creaturePtrOpt_(boost::none)
            , gameStatePtrOpt_(boost::none)
            , itemPtrOpt_(boost::none)
            , conditionPtrOpt_(CONDITION_PTR)
            , titlePtrOpt_(boost::none)
            , spellPtrOpt_(boost::none)
            , songPtrOpt_(boost::none)
            , isValid_(IS_VALID)
            , cachedTextureOpt_(boost::none)
            , sprite_()
        {
            sfml_util::gui::ConditionImageLoader conditionImageLoader;

            cachedTextureOpt_ = CachedTextureOpt_t(CachedTexture(
                boost::filesystem::path(conditionImageLoader.Path(CONDITION_PTR->Which()))));

            sprite_.setTexture(cachedTextureOpt_.value().Get(), true);
        }

        ListBoxItem::ListBoxItem(
            const std::string & NAME,
            const sfml_util::gui::TextInfo & TEXT_INFO,
            const item::ItemPtr_t ITEM_PTR,
            const bool IS_VALID)
            : TextRegion(std::string(NAME).append("_ListBoxItemItem"), TEXT_INFO, sf::FloatRect())
            , creaturePtrOpt_(boost::none)
            , gameStatePtrOpt_(boost::none)
            , itemPtrOpt_(ITEM_PTR)
            , conditionPtrOpt_(boost::none)
            , titlePtrOpt_(boost::none)
            , spellPtrOpt_(boost::none)
            , songPtrOpt_(boost::none)
            , isValid_(IS_VALID)
            , cachedTextureOpt_(boost::none)
            , sprite_()
        {
            sfml_util::gui::ItemImageLoader itemImageLoader;

            cachedTextureOpt_ = CachedTextureOpt_t(
                CachedTexture(boost::filesystem::path(itemImageLoader.Path(ITEM_PTR))));

            sprite_.setTexture(cachedTextureOpt_.value().Get(), true);
        }

        ListBoxItem::ListBoxItem(
            const std::string & NAME,
            const sfml_util::gui::TextInfo & TEXT_INFO,
            const creature::TitlePtr_t & TITLE_PTR,
            const bool IS_VALID)
            : TextRegion(std::string(NAME).append("_ListBoxItemTitle"), TEXT_INFO, sf::FloatRect())
            , creaturePtrOpt_(boost::none)
            , gameStatePtrOpt_(boost::none)
            , itemPtrOpt_(boost::none)
            , conditionPtrOpt_(boost::none)
            , titlePtrOpt_(TITLE_PTR)
            , spellPtrOpt_(boost::none)
            , songPtrOpt_(boost::none)
            , isValid_(IS_VALID)
            , cachedTextureOpt_(boost::none)
            , sprite_()
        {
            sfml_util::gui::TitleImageLoader titleImageLoader;

            cachedTextureOpt_ = CachedTextureOpt_t(
                CachedTexture(boost::filesystem::path(titleImageLoader.Path(TITLE_PTR->Which()))));

            sprite_.setTexture(cachedTextureOpt_.value().Get(), true);
        }

        ListBoxItem::ListBoxItem(
            const std::string & NAME,
            const sfml_util::gui::TextInfo & TEXT_INFO,
            const spell::SpellPtr_t SPELL_PTR,
            const bool IS_VALID)
            : TextRegion(std::string(NAME).append("_ListBoxItemSpell"), TEXT_INFO, sf::FloatRect())
            , creaturePtrOpt_(boost::none)
            , gameStatePtrOpt_(boost::none)
            , itemPtrOpt_(boost::none)
            , conditionPtrOpt_(boost::none)
            , titlePtrOpt_(boost::none)
            , spellPtrOpt_(SPELL_PTR)
            , songPtrOpt_(boost::none)
            , isValid_(IS_VALID)
            , cachedTextureOpt_(boost::none)
            , sprite_()
        {
            sfml_util::gui::SpellImageLoader spellImageLoader;

            cachedTextureOpt_ = CachedTextureOpt_t(
                CachedTexture(boost::filesystem::path(spellImageLoader.Path(SPELL_PTR->Which()))));

            sprite_.setTexture(cachedTextureOpt_.value().Get(), true);
        }

        ListBoxItem::ListBoxItem(
            const std::string & NAME,
            const sfml_util::gui::TextInfo & TEXT_INFO,
            const song::SongPtr_t SONG_PTR,
            const bool IS_VALID)
            : TextRegion(std::string(NAME).append("_ListBoxItemSong"), TEXT_INFO, sf::FloatRect())
            , creaturePtrOpt_(boost::none)
            , gameStatePtrOpt_(boost::none)
            , itemPtrOpt_(boost::none)
            , conditionPtrOpt_(boost::none)
            , titlePtrOpt_(boost::none)
            , spellPtrOpt_(boost::none)
            , songPtrOpt_(SONG_PTR)
            , isValid_(IS_VALID)
            , cachedTextureOpt_(boost::none)
            , sprite_()
        {
            sfml_util::gui::SongImageLoader songImageLoader;

            cachedTextureOpt_ = CachedTextureOpt_t(
                CachedTexture(boost::filesystem::path(songImageLoader.Path(SONG_PTR->Which()))));

            sprite_.setTexture(cachedTextureOpt_.value().Get(), true);
        }

        ListBoxItem::~ListBoxItem() {}

        void ListBoxItem::draw(sf::RenderTarget & target, sf::RenderStates states) const
        {
            TextRegion::draw(target, states);

            if (cachedTextureOpt_)
            {
                target.draw(sprite_, states);
            }
        }

        void ListBoxItem::SetEntityPos(const float POS_LEFT, const float POS_TOP)
        {
            MoveEntityPos(POS_LEFT - entityRegion_.left, POS_TOP - entityRegion_.top);
        }

        void ListBoxItem::MoveEntityPos(const float HORIZ, const float VERT)
        {
            TextRegion::MoveEntityPos(HORIZ, VERT);
            sprite_.move(HORIZ, VERT);
        }

        bool operator==(const ListBoxItem & L, const ListBoxItem & R)
        {
            // cachedTexture_ and sprite_ are intentionally excluded
            return std::tie(
                       L.creaturePtrOpt_,
                       L.gameStatePtrOpt_,
                       L.itemPtrOpt_,
                       L.conditionPtrOpt_,
                       L.titlePtrOpt_,
                       L.spellPtrOpt_,
                       L.songPtrOpt_,
                       L.isValid_)
                == std::tie(
                       R.creaturePtrOpt_,
                       R.gameStatePtrOpt_,
                       R.itemPtrOpt_,
                       R.conditionPtrOpt_,
                       R.titlePtrOpt_,
                       R.spellPtrOpt_,
                       R.songPtrOpt_,
                       R.isValid_);
        }

        bool operator<(const ListBoxItem & L, const ListBoxItem & R)
        {
            // cachedTexture_ and sprite_ are intentionally excluded
            return std::tie(
                       L.creaturePtrOpt_,
                       L.gameStatePtrOpt_,
                       L.itemPtrOpt_,
                       L.conditionPtrOpt_,
                       L.titlePtrOpt_,
                       L.spellPtrOpt_,
                       L.songPtrOpt_,
                       L.isValid_)
                < std::tie(
                       R.creaturePtrOpt_,
                       R.gameStatePtrOpt_,
                       R.itemPtrOpt_,
                       R.conditionPtrOpt_,
                       R.titlePtrOpt_,
                       R.spellPtrOpt_,
                       R.songPtrOpt_,
                       R.isValid_);
        }

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
