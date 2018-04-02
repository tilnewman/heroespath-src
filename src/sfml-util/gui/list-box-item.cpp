// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
