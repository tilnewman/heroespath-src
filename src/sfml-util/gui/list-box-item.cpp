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

#include "creature/condition.hpp"
#include "creature/creature.hpp"
#include "creature/title.hpp"
#include "item/item.hpp"
#include "song/song.hpp"
#include "spell/spell.hpp"
#include "state/game-state.hpp"

#include <string>
#include <tuple>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        ListBoxItem::ListBoxItem(const std::string & NAME, const bool IS_VALID)
            : TextRegion(std::string(NAME).append("_ListBoxItemBase"))
            , CHARACTER_PTR_OPT(boost::none)
            , GAMESTATE_PTR_OPT(boost::none)
            , ITEM_PTR_OPT(boost::none)
            , COND_PTR_OPT(boost::none)
            , TITLE_PTR_OPT(boost::none)
            , SPELL_PTR_OPT(boost::none)
            , SONG_PTR_OPT(boost::none)
            , is_valid(IS_VALID)
        {}

        ListBoxItem::ListBoxItem(
            const std::string & NAME,
            const sfml_util::gui::TextInfo & TEXT_INFO,
            const bool IS_VALID)
            : TextRegion(
                  std::string(NAME).append("_ListBoxItemTextOnly"), TEXT_INFO, sf::FloatRect())
            , CHARACTER_PTR_OPT(boost::none)
            , GAMESTATE_PTR_OPT(boost::none)
            , ITEM_PTR_OPT(boost::none)
            , COND_PTR_OPT(boost::none)
            , TITLE_PTR_OPT(boost::none)
            , SPELL_PTR_OPT(boost::none)
            , SONG_PTR_OPT(boost::none)
            , is_valid(IS_VALID)
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
            , CHARACTER_PTR_OPT(CHARACTER_PTR)
            , GAMESTATE_PTR_OPT(boost::none)
            , ITEM_PTR_OPT(boost::none)
            , COND_PTR_OPT(boost::none)
            , TITLE_PTR_OPT(boost::none)
            , SPELL_PTR_OPT(boost::none)
            , SONG_PTR_OPT(boost::none)
            , is_valid(IS_VALID)
        {}

        ListBoxItem::ListBoxItem(
            const std::string & NAME,
            const sfml_util::gui::TextInfo & TEXT_INFO,
            const state::GameStatePtr_t GAMESTATE_PTR,
            const bool IS_VALID)
            : TextRegion(
                  std::string(NAME).append("_ListBoxItemGameState"), TEXT_INFO, sf::FloatRect())
            , CHARACTER_PTR_OPT(boost::none)
            , GAMESTATE_PTR_OPT(GAMESTATE_PTR)
            , ITEM_PTR_OPT(boost::none)
            , COND_PTR_OPT(boost::none)
            , TITLE_PTR_OPT(boost::none)
            , SPELL_PTR_OPT(boost::none)
            , SONG_PTR_OPT(boost::none)
            , is_valid(IS_VALID)
        {}

        ListBoxItem::ListBoxItem(
            const std::string & NAME,
            const sfml_util::gui::TextInfo & TEXT_INFO,
            const creature::ConditionPtr_t CONDITION_PTR_PARAM,
            const bool IS_VALID)
            : TextRegion(
                  std::string(NAME).append("_ListBoxItemCondition"), TEXT_INFO, sf::FloatRect())
            , CHARACTER_PTR_OPT(boost::none)
            , GAMESTATE_PTR_OPT(boost::none)
            , ITEM_PTR_OPT(boost::none)
            , COND_PTR_OPT(CONDITION_PTR_PARAM)
            , TITLE_PTR_OPT(boost::none)
            , SPELL_PTR_OPT(boost::none)
            , SONG_PTR_OPT(boost::none)
            , is_valid(IS_VALID)
        {}

        ListBoxItem::ListBoxItem(
            const std::string & NAME,
            const sfml_util::gui::TextInfo & TEXT_INFO,
            const item::ItemPtr_t ITEM_PTR_PARAM,
            const bool IS_VALID)
            : TextRegion(std::string(NAME).append("_ListBoxItemItem"), TEXT_INFO, sf::FloatRect())
            , CHARACTER_PTR_OPT(boost::none)
            , GAMESTATE_PTR_OPT(boost::none)
            , ITEM_PTR_OPT(ITEM_PTR_PARAM)
            , COND_PTR_OPT(boost::none)
            , TITLE_PTR_OPT(boost::none)
            , SPELL_PTR_OPT(boost::none)
            , SONG_PTR_OPT(boost::none)
            , is_valid(IS_VALID)
        {}

        ListBoxItem::ListBoxItem(
            const std::string & NAME,
            const sfml_util::gui::TextInfo & TEXT_INFO,
            const creature::TitlePtr_t & TITLE_PTR_PARAM,
            const bool IS_VALID)
            : TextRegion(std::string(NAME).append("_ListBoxItemTitle"), TEXT_INFO, sf::FloatRect())
            , CHARACTER_PTR_OPT(boost::none)
            , GAMESTATE_PTR_OPT(boost::none)
            , ITEM_PTR_OPT(boost::none)
            , COND_PTR_OPT(boost::none)
            , TITLE_PTR_OPT(TITLE_PTR_PARAM)
            , SPELL_PTR_OPT(boost::none)
            , SONG_PTR_OPT(boost::none)
            , is_valid(IS_VALID)
        {}

        ListBoxItem::ListBoxItem(
            const std::string & NAME,
            const sfml_util::gui::TextInfo & TEXT_INFO,
            const spell::SpellPtr_t SPELL_PTR_PARAM,
            const bool IS_VALID)
            : TextRegion(std::string(NAME).append("_ListBoxItemSpell"), TEXT_INFO, sf::FloatRect())
            , CHARACTER_PTR_OPT(boost::none)
            , GAMESTATE_PTR_OPT(boost::none)
            , ITEM_PTR_OPT(boost::none)
            , COND_PTR_OPT(boost::none)
            , TITLE_PTR_OPT(boost::none)
            , SPELL_PTR_OPT(SPELL_PTR_PARAM)
            , SONG_PTR_OPT(boost::none)
            , is_valid(IS_VALID)
        {}

        ListBoxItem::ListBoxItem(
            const std::string & NAME,
            const sfml_util::gui::TextInfo & TEXT_INFO,
            const song::SongPtr_t SONG_PTR_PARAM,
            const bool IS_VALID)
            : TextRegion(std::string(NAME).append("_ListBoxItemSong"), TEXT_INFO, sf::FloatRect())
            , CHARACTER_PTR_OPT(boost::none)
            , GAMESTATE_PTR_OPT(boost::none)
            , ITEM_PTR_OPT(boost::none)
            , COND_PTR_OPT(boost::none)
            , TITLE_PTR_OPT(boost::none)
            , SPELL_PTR_OPT(boost::none)
            , SONG_PTR_OPT(SONG_PTR_PARAM)
            , is_valid(IS_VALID)
        {}

        bool operator==(const ListBoxItem & L, const ListBoxItem & R)
        {
            return std::tie(
                       L.CHARACTER_PTR_OPT,
                       L.GAMESTATE_PTR_OPT,
                       L.ITEM_PTR_OPT,
                       L.COND_PTR_OPT,
                       L.TITLE_PTR_OPT,
                       L.SPELL_PTR_OPT,
                       L.SONG_PTR_OPT,
                       L.is_valid)
                == std::tie(
                       R.CHARACTER_PTR_OPT,
                       R.GAMESTATE_PTR_OPT,
                       R.ITEM_PTR_OPT,
                       R.COND_PTR_OPT,
                       R.TITLE_PTR_OPT,
                       R.SPELL_PTR_OPT,
                       R.SONG_PTR_OPT,
                       R.is_valid);
        }

        bool operator<(const ListBoxItem & L, const ListBoxItem & R)
        {
            return std::tie(
                       L.CHARACTER_PTR_OPT,
                       L.GAMESTATE_PTR_OPT,
                       L.ITEM_PTR_OPT,
                       L.COND_PTR_OPT,
                       L.TITLE_PTR_OPT,
                       L.SPELL_PTR_OPT,
                       L.SONG_PTR_OPT,
                       L.is_valid)
                < std::tie(
                       R.CHARACTER_PTR_OPT,
                       R.GAMESTATE_PTR_OPT,
                       R.ITEM_PTR_OPT,
                       R.COND_PTR_OPT,
                       R.TITLE_PTR_OPT,
                       R.SPELL_PTR_OPT,
                       R.SONG_PTR_OPT,
                       R.is_valid);
        }

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
